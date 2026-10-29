"""독립적인 Python 정수 계산으로 실행 파일의 행렬 결과와 오류 처리를 검사한다."""

import argparse
import ctypes
import json
from pathlib import Path
import random
import re
import subprocess
import tempfile
import time


ROOT = Path(__file__).resolve().parents[1]


def read_matrix(path):
    return [[int(value) for value in line.split()]
            for line in path.read_text(encoding="utf-8-sig").splitlines() if line.strip()]


def multiply(left, right):
    # Python 정수는 자릿수 제한이 없어 C++ 구현과 다른 방식으로 정답을 계산한다.
    columns = list(zip(*right))
    return [[sum(a * b for a, b in zip(row, column)) for column in columns]
            for row in left]


def write_matrix(path, matrix):
    path.write_text("\n".join(" ".join(map(str, row)) for row in matrix) + "\n",
                    encoding="utf-8")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--exe", type=Path, default=ROOT / "bin/x64/Release/mm.exe")
    parser.add_argument("--samples", type=Path, default=ROOT / "test-data")
    parser.add_argument("--stress", action="store_true",
                        help="100x100 행렬 10개를 곱하는 최대 범위 검사도 실행")
    args = parser.parse_args()
    exe = args.exe.resolve()
    if not exe.is_file():
        parser.error("Build mm.exe before running these checks.")

    temporary = ROOT / "tmp/tests"
    temporary.mkdir(parents=True, exist_ok=True)
    run_root = Path(tempfile.mkdtemp(prefix="run-", dir=temporary))
    records = []

    def success(name, matrices, expected=None, filenames=None, raw=None):
        folder = run_root / name
        folder.mkdir()
        paths = []
        for i, matrix in enumerate(matrices):
            path = folder / (filenames[i] if filenames else f"input-{i}.txt")
            write_matrix(path, matrix)
            paths.append(path)
        if raw is not None:
            paths[0].write_bytes(raw)
        if expected is None:
            expected = matrices[0]
            for matrix in matrices[1:]:
                expected = multiply(expected, matrix)

        start = time.perf_counter()
        process = subprocess.run([str(exe), *map(str, paths)], cwd=folder,
                                 capture_output=True, text=True, timeout=300)
        wall = time.perf_counter() - start
        assert process.returncode == 0, (name, process.stderr)
        assert not process.stderr, (name, process.stderr)
        result = read_matrix(folder / "result.txt")
        assert result == expected, f"{name}: result.txt differs from the independent answer"
        preceding_text, time_text = process.stdout.rsplit("Processing Time:", 1)
        assert preceding_text.startswith("\n"), f"{name}: missing initial blank line"
        step_text, matrix_text = preceding_text[1:].split("\n\n", 1)
        expected_steps = []
        for i in range(1, len(matrices)):
            left_rows = len(matrices[0])
            left_columns = len(matrices[i - 1][0])
            right_rows = len(matrices[i])
            right_columns = len(matrices[i][0])
            expected_steps.append(
                f"({left_rows}x{left_columns}) x ({right_rows}x{right_columns}) = "
                f"({left_rows}x{right_columns})")
        assert step_text.splitlines() == expected_steps, f"{name}: multiplication steps differ"
        console = [[int(value) for value in line.split()]
                   for line in matrix_text.splitlines() if line.strip()]
        assert console == expected, f"{name}: console differs from result.txt"
        assert re.fullmatch(r" \d+\.\d{3} sec\n\n", time_text), time_text
        file_text = (folder / "result.txt").read_text(encoding="utf-8")
        assert matrix_text == file_text + "\n", f"{name}: unexpected blank-line layout"
        assert not file_text.startswith("\n") and not file_text.endswith("\n\n"), name
        assert not list(folder.glob("mm*.tmp")), f"{name}: a temporary output file was left behind"
        seconds = float(time_text.split()[0])
        # 1초 대기를 생략하지 않았는지, 곱셈 단계 수에 따른 시간 하한을 확인한다.
        assert seconds >= len(matrices) - 1 - 0.05, (name, seconds)
        assert seconds <= wall + 0.1, (name, seconds, wall)
        records.append({"name": name, "status": "PASS", "seconds": seconds,
                        "rows": len(result), "columns": len(result[0])})
        print(f"PASS {name}: {seconds:.3f} sec", flush=True)

    def failure(name, contents, expected_message, count=None, protect=False,
                block_output=False, read_only=False, hard_link=False, locked=False):
        folder = run_root / name
        folder.mkdir()
        paths = []
        for i, content in enumerate(contents):
            path = folder / f"input-{i}.txt"
            if content is not None:
                path.write_text(content, encoding="utf-8")
            paths.append(path)
        result = folder / "result.txt"
        original = b"previous result must survive\n"
        if block_output:
            result.mkdir()
        else:
            result.write_bytes(original)
        if protect:
            result.write_text("1\n", encoding="utf-8")
            original = result.read_bytes()
            paths[0] = result
        if hard_link:
            result.unlink()
            result.hardlink_to(paths[0])
            original = result.read_bytes()
        if read_only:
            result.chmod(0o444)
        handle = None
        if locked:
            # 다른 작업이 결과 파일을 열고 있을 때도 이전 내용을 훼손하지 않는지 확인한다.
            kernel = ctypes.WinDLL("kernel32", use_last_error=True)
            kernel.CreateFileW.restype = ctypes.c_void_p
            kernel.CreateFileW.argtypes = [ctypes.c_wchar_p, ctypes.c_uint32, ctypes.c_uint32,
                                          ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32,
                                          ctypes.c_void_p]
            kernel.CloseHandle.argtypes = [ctypes.c_void_p]
            handle = kernel.CreateFileW(str(result), 0x80000000, 1, None, 3, 0x80, None)
            assert handle not in (None, ctypes.c_void_p(-1).value), ctypes.get_last_error()
        if count is not None:
            paths = [paths[0]] * count if paths else []
        try:
            process = subprocess.run([str(exe), *map(str, paths)], cwd=folder,
                                     capture_output=True, text=True, timeout=60)
        finally:
            if handle is not None:
                kernel.CloseHandle(handle)
            if read_only:
                result.chmod(0o666)
        assert process.returncode != 0, name
        assert expected_message in process.stderr, (name, process.stderr)
        assert not process.stdout, (name, process.stdout)
        if not block_output:
            assert result.read_bytes() == original, f"{name}: existing output changed"
        assert not list(folder.glob("mm*.tmp")), f"{name}: a temporary output file was left behind"
        records.append({"name": name, "status": "PASS"})
        print(f"PASS {name}", flush=True)

    failure("no-arguments", [], "Provide 2 to 10")
    failure("one-argument", ["1\n"], "Provide 2 to 10")
    failure("eleven-operands", ["1\n"], "Provide 2 to 10", count=11)
    failure("missing-file", [None, "1\n"], "Cannot open")
    failure("empty-file", ["", "1\n"], "empty")
    failure("ragged-rows", ["1 2\n3\n", "1\n"], "same number")
    failure("out-of-range", ["4\n", "1\n"], "-3 to 3")
    failure("invalid-token", ["1.0\n", "1\n"], "integers")
    failure("huge-token", ["999999999999999999999999\n", "1\n"], "integers")
    failure("too-many-rows", ["1\n" * 101, "1\n"], "100 rows")
    failure("too-many-columns", ["1 " * 101, "1\n"], "100 columns")
    failure("incompatible", ["1 2\n", "1\n"], "dimensions do not match")
    failure("late-incompatible", ["1\n", "1\n", "1\n1\n"], "dimensions do not match")
    failure("protect-input", ["1\n", "1\n"], "must not also be an input", protect=True)
    failure("hardlink-input", ["1\n", "1\n"], "must not also be an input", hard_link=True)
    failure("output-failure", ["1\n", "1\n"], "Cannot replace result.txt", block_output=True)
    failure("readonly-result", ["1\n", "1\n"], "Cannot replace result.txt", read_only=True)
    failure("locked-result", ["1\n", "1\n"], "Cannot replace result.txt", locked=True)

    success("lecture-example", [[[1, 2, 3], [1, 0, 1]], [[1, 2], [0, 0], [1, 0]]],
            [[4, 2], [2, 2]])
    success("rectangular-example", [[[0, 2, -3, 2, 3, 3, 0, -1, 2, -2],
                             [-3, 3, 0, -1, -3, 3, 1, 1, 1, 0]],
                            [[1], [0], [-1], [2], [-3], [1], [0], [1], [2], [-2]]])
    success("zero-and-negative", [[[0, -3], [3, 0]], [[0, 3], [-3, 0]]])
    success("cancellation", [[[3, -3]], [[3], [3]]], [[0]])
    success("ten-operands", [[[-3]]] * 10, [[59049]])
    success("single-row-column", [[[3, -2, 1]], [[-3], [2], [-1]]])
    success("unicode-paths", [[[1]], [[-3]]], filenames=["첫 행렬.txt", "두 번째 행렬.txt"])
    success("whitespace-bom", [[[1, -2], [3, 0]], [[1], [-1]]],
            raw=b"\xef\xbb\xbf\t1\t-2\r\n\r\n 3 0")
    # 여러 직사각형 행렬을 섞고, 난수 시작값을 고정해서 다시 검사할 수 있게 한다.
    generator = random.Random(20261029)
    for case in range(8):
        sizes = [generator.randint(1, 6) for _ in range(generator.randint(3, 5))]
        matrices = [[[generator.randint(-3, 3) for _ in range(columns)]
                     for _ in range(rows)] for rows, columns in zip(sizes, sizes[1:])]
        success(f"random-{case+1:02}", matrices)

    cases = [("matrix_1", ["20x40", "40x60", "60x80", "80x100", "100x100"]),
             ("matrix_2", ["4x10", "10x50", "50x6"]),
             ("matrix_3", ["100x100", "100x90", "90x80", "80x100"])]
    for name, names in cases:
        folder = args.samples / name
        matrices = [read_matrix(folder / entry) for entry in names]
        answer = matrices[0]
        for matrix in matrices[1:]:
            answer = multiply(answer, matrix)
        supplied = folder / "expected.txt"
        if not supplied.exists():
            supplied = folder / "result.txt"
        assert read_matrix(supplied) == answer, f"{name}: supplied answer mismatch"
        success(f"provided-{name}", matrices, answer)

    if args.stress:
        # 최종 값은 64비트 정수의 한계를 넘는다. 매 단계마다 스레드 10,000개를 만든다.
        positive = [[3] * 100 for _ in range(100)]
        negative = [[-3] * 100 for _ in range(100)]
        maximum = 3**10 * 100**9
        success("maximum-ten-positive", [positive] * 10, [[maximum] * 100 for _ in range(100)])
        success("maximum-ten-negative", [negative] + [positive] * 9,
                [[-maximum] * 100 for _ in range(100)])
        # 큰 수끼리 더하고 뺄 때 자리 빌림과 상쇄를 확인하고, 음수 0도 검출한다.
        mixed = [[3 if i < 49 + j % 3 else -3 for j in range(100)] for i in range(100)]
        answer_row = [3**10 * 100**8 * (2 * (49 + j % 3) - 100) for j in range(100)]
        success("large-sign-and-cancellation", [positive] * 9 + [mixed],
                [answer_row[:] for _ in range(100)])

    summary = {"passed": len(records), "stress": args.stress,
               "configuration": exe.parent.name, "cases": records}
    (run_root / "summary.json").write_text(json.dumps(summary, ensure_ascii=False, indent=2),
                                           encoding="utf-8")
    print(f"All {len(records)} checks passed. Evidence: tmp/tests/{run_root.name}/summary.json")


if __name__ == "__main__":
    main()
