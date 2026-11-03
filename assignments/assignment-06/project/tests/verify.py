"""실행 파일을 검사한다. 표준 라이브러리만 사용하며 입력 원본은 수정하지 않는다."""

import argparse
from collections import deque
from pathlib import Path
import random
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]

# 제공된 results.docx의 네 화면에서 확인한 프로세스별 정답이다.
EXPECTED = {
    "input1.txt": [6, 21, 7, 0, 2],
    "input2.txt": [21, 11, 2, 4, 3],
    "input3.txt": [119, 26, 67, 97, 3, 4, 2, 35, 77, 70],
    "input4.txt": [166, 68, 182, 149, 247, 238, 96, 81, 51, 130],
}


def parse(text):
    """샘플의 이름, 도착 시각, CPU/I/O 시간을 읽는다."""
    result = []
    for line in text.lstrip("\ufeff").splitlines():
        fields = line.split()
        if fields:
            values = [int(field.split(":")[1]) for field in fields[1:]]
            result.append((fields[0], values[0], values[1:]))
    return result


def expected_screen(processes, waits):
    """예제의 탭, 세 자리 대기 시간, 다섯 자리 평균과 앞뒤 빈 줄을 비교한다."""
    lines = [""]
    for name, arrival, bursts in processes:
        fields = [name, f"ARRIVAL:{arrival}"]
        fields.extend(f"{'CPU' if i % 2 == 0 else 'I/O'}:{value}"
                      for i, value in enumerate(bursts))
        lines.append("\t".join(fields))
    lines.extend(["", "Process : Waiting Time", "-" * 30])
    lines.extend(f"{p[0]}\t: {wait:3d}" for p, wait in zip(processes, waits))
    lines.extend(["-" * 30, f"Average Waiting Time  = {sum(waits) / len(waits):5.2f}"])
    return "\n".join(lines) + "\n\n"


def tick_reference(processes):
    """이벤트 단위 C++ 구현과 별개로, 매 시간 대기한 작업을 직접 세는 기준 계산이다."""
    count = len(processes)
    status = ["new"] * count
    positions = [0] * count
    remaining = [p[2][0] for p in processes]
    waits = [0] * count
    ready = deque()
    io = []
    running = None
    time = 0
    completed = 0
    while completed < count:
        for i, (_, arrival, _) in enumerate(processes):
            if status[i] == "new" and arrival == time:
                ready.append(i)
                status[i] = "ready"
        for i, wake in io:
            if wake == time:
                ready.append(i)
                status[i] = "ready"
        io = [(i, wake) for i, wake in io if wake > time]
        if (running is not None and ready
                and min(remaining[i] for i in ready) < remaining[running]):
            ready.append(running)
            status[running] = "ready"
            running = None
        if running is None and ready:
            running = min(ready, key=lambda i: remaining[i])
            ready.remove(running)
            status[running] = "running"
        for i in ready:
            waits[i] += 1
        time += 1
        if running is not None:
            remaining[running] -= 1
            if remaining[running] == 0:
                i = running
                positions[i] += 1
                if positions[i] == len(processes[i][2]):
                    status[i] = "done"
                    completed += 1
                    # 종료-도착-CPU-I/O로도 대기 시간이 같아야 한다.
                    assert waits[i] == time - processes[i][1] - sum(processes[i][2])
                else:
                    io.append((i, time + processes[i][2][positions[i]]))
                    positions[i] += 1
                    remaining[i] = processes[i][2][positions[i]]
                    status[i] = "io"
                running = None
        assert time < 10000, "Reference simulation did not finish."
    return waits


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--exe", type=Path,
                        default=ROOT / "bin/x64/Release/sjf.exe")
    args = parser.parse_args()
    executable = args.exe.resolve()
    checked = 0

    def run(arguments):
        return subprocess.run([str(executable), *map(str, arguments)],
                              capture_output=True, text=True, encoding="utf-8", timeout=5)

    def check(path, waits):
        nonlocal checked
        processes = parse(path.read_text(encoding="utf-8-sig"))
        result = run([path])
        assert result.returncode == 0, (path.name, result.stderr)
        assert result.stderr == "", (path.name, result.stderr)
        assert result.stdout == expected_screen(processes, waits), (path.name, result.stdout, waits)
        checked += 1

    for filename, waits in EXPECTED.items():
        path = ROOT / "test-data" / filename
        assert tick_reference(parse(path.read_text())) == waits, filename
        check(path, waits)
    print("PASS: all 4 provided examples, individual waits, averages, and exact output spacing")

    # 알려진 답으로 선점 동률, 세 진입 경로, 병렬 I/O, 큰 시간 범위를 검사한다.
    cases = [
        ("strict-tie", "A ARRIVAL:0 CPU:4\nB ARRIVAL:2 CPU:2\n", [0, 2]),
        ("file-order", "Z ARRIVAL:1 CPU:2\nA ARRIVAL:1 CPU:2\n", [0, 2]),
        ("new-before-io", "R ARRIVAL:0 CPU:8\nW ARRIVAL:0 CPU:1 I/O:1 CPU:5\nN ARRIVAL:2 CPU:5\n", [11, 5, 0]),
        ("io-before-running", "R ARRIVAL:0 CPU:8\nW ARRIVAL:0 CPU:1 I/O:1 CPU:7\nN ARRIVAL:2 CPU:1\n", [9, 1, 0]),
        ("parallel-io", "A ARRIVAL:0 CPU:1 I/O:5 CPU:4\nB ARRIVAL:0 CPU:1 I/O:2 CPU:1\nC ARRIVAL:4 CPU:3\n", [2, 1, 1]),
        ("idle-io", "A ARRIVAL:10 CPU:2 I/O:10 CPU:3\n", [0]),
        ("large-arrival", "A ARRIVAL:1000000000000 CPU:1\n", [0]),
        ("large-wait", "A ARRIVAL:0 CPU:5000000000\nB ARRIVAL:0 CPU:6000000000\n", [0, 5000000000]),
        ("zero-duration", "A ARRIVAL:0 CPU:0 I/O:0 CPU:1\nB ARRIVAL:0 CPU:2\n", [0, 1]),
        ("bom-and-blanks", "\ufeff\nA ARRIVAL:0 CPU:1\n\n", [0]),
    ]
    (ROOT / "tmp").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="sjf-", dir=ROOT / "tmp") as folder:
        folder = Path(folder)
        for name, text, waits in cases:
            # 한글·공백 경로를 실제로 전달한다. 경로를 출력하거나 소스에 고정하지 않는다.
            path = folder / f"검사 입력 {name}.txt"
            path.write_text(text, encoding="utf-8")
            check(path, waits)
        print(f"PASS: {len(cases)} targeted scheduling and input cases")

        generator = random.Random(20261103)
        for _ in range(120):
            processes = []
            for i in range(generator.randint(1, 8)):
                bursts = [generator.randint(1, 9)
                          for _ in range(2 * generator.randint(1, 5) - 1)]
                processes.append((f"P{i:02}", generator.randint(0, 12), bursts))
            path = folder / "random.txt"
            path.write_text("\n".join(
                " ".join([name, f"ARRIVAL:{arrival}"] +
                         [f"{'CPU' if j % 2 == 0 else 'I/O'}:{v}"
                          for j, v in enumerate(bursts)])
                for name, arrival, bursts in processes), encoding="utf-8")
            check(path, tick_reference(processes))
        print("PASS: 120 seeded random inputs against independent per-time-unit simulation")

        invalid = ["", "P ARRIVAL:0", "P ARRIVAL:0 CPU:1 I/O:2",
                   "P ARRIVAL:0 I/O:1", "P ARRIVAL:-1 CPU:1",
                   "P ARRIVAL:0 CPU:1x", "P ARRIVAL:0 CPU:9223372036854775808",
                   "P ARRIVAL:9223372036854775807 CPU:1"]
        for text in invalid:
            path = folder / "invalid.txt"
            path.write_text(text, encoding="utf-8")
            result = run([path])
            assert result.returncode != 0 and not result.stdout and "Error:" in result.stderr
            checked += 1
        for arguments in [[], [folder / "missing.txt"], ["one", "two"]]:
            result = run(arguments)
            assert result.returncode != 0 and not result.stdout and result.stderr
            checked += 1
        print("PASS: 11 failure cases return a failing exit code without partial results")

    print(f"PASS: {checked} checks ({executable.name})")


if __name__ == "__main__":
    main()
