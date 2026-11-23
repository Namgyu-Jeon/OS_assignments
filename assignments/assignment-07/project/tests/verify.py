"""정답 화면, PDF 예제, 완전 탐색 기준 계산으로 실행 파일을 검증한다."""

import argparse
from functools import lru_cache
from pathlib import Path
import random
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def run(executable, arguments):
    """표준 출력과 오류를 분리하고, 멈추지 않는 실행은 제한 시간으로 검출한다."""
    return subprocess.run([str(executable), *map(str, arguments)],
                          capture_output=True, text=True, encoding="ascii", timeout=10)


def read_state(text):
    """출력 비교와 별개로 입력의 숫자를 읽어 기준 계산에 전달한다."""
    fields = iter(text.lstrip("\ufeff").split())
    m, n = int(next(fields)), int(next(fields))
    assert next(fields) == "Allocation"
    resources = [next(fields) for _ in range(m)]
    names, allocation, maximum = [], [], []
    for _ in range(n):
        names.append(next(fields))
        allocation.append([int(next(fields)) for _ in range(m)])
    assert next(fields) == "Max"
    assert [next(fields) for _ in range(m)] == resources
    for i in range(n):
        assert next(fields) == names[i]
        maximum.append([int(next(fields)) for _ in range(m)])
    assert next(fields) == "Available"
    assert [next(fields) for _ in range(m)] == resources
    available = [int(next(fields)) for _ in range(m)]
    assert next(fields, None) is None
    return names, allocation, maximum, available


def result_line(screen):
    lines = [line for line in screen.splitlines() if line.startswith("Result: ")]
    assert len(lines) == 1, "Expected exactly one result line"
    status, names = lines[0][len("Result: "):].split(" --> ")
    assert status in ("Safe", "UnSafe")
    return status == "Safe", names.split()


def reachable_completion(names, allocation, maximum, available):
    """프로세스 선택의 모든 분기를 탐색해 완료 가능한 집합을 구한다.

    C++의 첫 번째 후보 선택과 달리 모든 후보를 방문하므로 잘못된 탐색 종료도 검출한다.
    """
    n, m = len(names), len(available)

    @lru_cache(None)
    def visit(mask):
        work = [available[j] + sum(allocation[i][j] for i in range(n)
                                  if mask & (1 << i)) for j in range(m)]
        reachable = mask
        for i in range(n):
            if not mask & (1 << i) and all(
                    maximum[i][j] - allocation[i][j] <= work[j] for j in range(m)):
                reachable |= visit(mask | (1 << i))
        return reachable

    return visit(0)


def verify_semantics(screen, state):
    """안전 순서는 매 단계의 자원으로, 불안전 목록은 완전 탐색 결과로 확인한다."""
    names, allocation, maximum, available = state
    is_safe, listed = result_line(screen)
    reached = reachable_completion(*state)
    assert is_safe == (reached == (1 << len(names)) - 1)
    if is_safe:
        assert len(listed) == len(names) and set(listed) == set(names)
        work = available.copy()
        for name in listed:
            i = names.index(name)
            assert all(maximum[i][j] - allocation[i][j] <= work[j]
                       for j in range(len(work)))
            work = [value + allocation[i][j] for j, value in enumerate(work)]
    else:
        assert listed == [name for i, name in enumerate(names) if not reached & (1 << i)]


def make_input(allocation, maximum, available, names=None):
    """추가 사례도 실제 명령행 입력 파일과 같은 형식으로 만든다."""
    m, n = len(available), len(allocation)
    names = names if names is not None else [f"P_{i:02d}" for i in range(n)]
    resources = [chr(ord("A") + j) if j < 26 else f"R{j}" for j in range(m)]
    lines = [f"{m} {n}", "", "Allocation " + " ".join(resources)]
    lines.extend(name + " " + " ".join(map(str, row))
                 for name, row in zip(names, allocation))
    lines.extend(["", "Max " + " ".join(resources)])
    lines.extend(name + " " + " ".join(map(str, row))
                 for name, row in zip(names, maximum))
    lines.extend(["", "Available " + " ".join(resources), " ".join(map(str, available))])
    return "\n".join(lines) + "\n"


def check_success(executable, path):
    completed = run(executable, [path])
    assert completed.returncode == 0 and completed.stderr == "", "Execution failed"
    assert completed.stdout.startswith("\n") and not completed.stdout.startswith("\n\n")
    assert completed.stdout.endswith("\n\n") and not completed.stdout.endswith("\n\n\n")
    return completed.stdout


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("executable", nargs="?", type=Path,
                        default=ROOT / "bin/x64/Release/safety.exe")
    executable = parser.parse_args().executable.resolve()

    for n in range(1, 10):
        path = ROOT / f"test-data/input/input{n}.txt"
        expected = (ROOT / f"test-data/expected/ret-input{n}.txt").read_text(encoding="ascii")
        screen = check_success(executable, path)
        # 운영체제 줄바꿈만 통일한다. 표의 공백과 줄 끝 공백도 빠짐없이 비교한다.
        assert screen == "\n" + expected.rstrip("\n") + "\n\n", f"Sample {n} differs"
        verify_semantics(screen, read_state(path.read_text(encoding="ascii")))
    print("PASS: all 9 sample screens, including exact spacing and process order")

    pdf_input = ROOT / "test-data/pdf-safe.txt"
    screen = check_success(executable, pdf_input)
    assert result_line(screen) == (True, ["P_07", "P_09", "P_00", "P_01", "P_02",
                                         "P_03", "P_04", "P_05", "P_06", "P_08"])
    verify_semantics(screen, read_state(pdf_input.read_text(encoding="ascii")))
    print("PASS: safe sequence shown on PDF page 5")

    with tempfile.TemporaryDirectory(prefix="safety-", dir=ROOT / "tmp") as directory:
        test_path = Path(directory) / "한글 경로 입력.txt"
        # 자원 반납 후 앞선 프로세스를 다시 찾는 경우, 부분 완료, Need=Work,
        # Need=0, 할당량이 32비트 범위를 넘는 경우를 각각 확인한다.
        cases = [
            make_input([[1], [1], [1]], [[3], [2], [1]], [0]),
            make_input([[1], [0]], [[1], [2]], [0]),
            make_input([[0, 0]], [[2, 3]], [2, 3]),
            make_input([[0, 0]], [[0, 0]], [0, 0]),
            make_input([[3_000_000_000], [0]], [[3_000_000_000], [3_000_000_000]], [0]),
            make_input([[1], [1]], [[2], [2]], [0]),
            make_input([[0]], [[1]], [0], ["Worker"]),
        ]
        rng = random.Random(20261123)
        for _ in range(200):
            n, m = rng.randint(1, 7), rng.randint(1, 5)
            allocation = [[rng.randint(0, 3) for _ in range(m)] for _ in range(n)]
            maximum = [[value + rng.randint(0, 5) for value in row] for row in allocation]
            cases.append(make_input(allocation, maximum, [rng.randint(0, 4) for _ in range(m)]))
        for content in cases:
            test_path.write_text(content, encoding="ascii")
            screen = check_success(executable, test_path)
            verify_semantics(screen, read_state(content))
        print(f"PASS: {len(cases)} boundary/random cases checked against exhaustive search")

        content = make_input([[0] * 25 for _ in range(120)],
                             [[0] * 25 for _ in range(120)], [0] * 25)
        test_path.write_text(content, encoding="ascii")
        assert result_line(check_success(executable, test_path)) == (
            True, [f"P_{i:02d}" for i in range(120)])
        print("PASS: 25 resource types and 120 processes without fixed-size limits")

        original = (ROOT / "test-data/input/input1.txt").read_text(encoding="ascii")
        expected = check_success(executable, ROOT / "test-data/input/input1.txt")
        for variant in [original.replace(" ", "\t"), "\ufeff" + original,
                        original.replace("\n", "\r\n"), "\n\n" + original + "\n\n"]:
            test_path.write_bytes(variant.encode("utf-8"))
            assert check_success(executable, test_path) == expected
        print("PASS: Korean/spaced paths, tabs, UTF-8 BOM, CRLF, and blank lines")

        for arguments in [[], [test_path, test_path], [Path(directory) / "missing.txt"]]:
            failed = run(executable, arguments)
            assert failed.returncode != 0 and failed.stdout == "" and failed.stderr
        bad_inputs = ["", "1 1\nAllocation A\nP_00", original + "extra",
                      make_input([[2]], [[1]], [0]), make_input([[0]], [[0]], [-1]),
                      make_input([[1]], [[1]], [9_223_372_036_854_775_807])]
        for content in bad_inputs:
            test_path.write_text(content, encoding="ascii")
            failed = run(executable, [test_path])
            assert failed.returncode != 0 and failed.stdout == "" and failed.stderr
        print("PASS: 9 invalid invocation/input cases, including overflow")

    print("All checks passed.")


if __name__ == "__main__":
    (ROOT / "tmp").mkdir(exist_ok=True)
    main()
