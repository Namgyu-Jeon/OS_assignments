"""설치된 Visual Studio로 스레드·큰 정수 검사를 빌드하고 실행한다. 추가 패키지는 없다."""

import argparse
import json
import os
from pathlib import Path
import random
import subprocess


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--debug", action="store_true")
    args = parser.parse_args()
    finder = Path(os.environ["ProgramFiles(x86)"]) / "Microsoft Visual Studio/Installer/vswhere.exe"
    installation = subprocess.check_output(
        [str(finder), "-latest", "-products", "*", "-requires",
         "Microsoft.VisualStudio.Component.VC.Tools.x86.x64", "-property", "installationPath"],
        text=True).strip()
    if not installation:
        parser.error("Install the Visual Studio C++ desktop tools before running these checks.")
    environment = Path(installation) / "VC/Auxiliary/Build/vcvars64.bat"
    configuration = "Debug" if args.debug else "Release"
    folder = ROOT / "tmp/native" / configuration
    folder.mkdir(parents=True, exist_ok=True)
    options = "/Od /MTd /D_DEBUG" if args.debug else "/O2 /MT /DNDEBUG"
    relative = folder.relative_to(ROOT)
    lines = ["@echo off", f'call "{environment}" >nul', "if errorlevel 1 exit /b 1"]
    for name in ("thread_contract", "value_arithmetic"):
        lines.extend([
            f'cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 /DWIN32_LEAN_AND_MEAN /DNOMINMAX '
            f'{options} /Fe:{relative / name}.exe /Fo:{relative}\\ '
            f'tests\\{name}.cpp src\\MatrixValue.cpp',
            "if errorlevel 1 exit /b 1",
        ])
    script = folder / "build.cmd"
    script.write_text("\n".join(lines) + "\n", encoding="utf-8")
    subprocess.run(["cmd.exe", "/d", "/c", str(script)], cwd=ROOT, check=True)
    thread = subprocess.run([str(folder / "thread_contract.exe")], cwd=folder,
                            capture_output=True, text=True, timeout=180, check=True)
    print(thread.stdout, end="", flush=True)

    # 같은 변수에 더하기, 반대 부호, 9자리 경계의 올림·빌림을 독립 정답과 대조한다.
    values = [i % 7 - 3 for i in range(32)]
    operations = []
    expected = []
    generator = random.Random(20261029)

    def add(destination, source, factor):
        values[destination] += values[source] * factor
        assert abs(values[destination]) < 10**24, "Test generator exceeded its numeric bound."
        operations.append(f"{destination} {source} {factor}")
        expected.append(str(values[destination]))

    # 작은 수에서 시작해 최대 범위 부근까지 의도적으로 키운다.
    for _ in range(36):
        for destination in range(32):
            if abs(values[destination] * 4) < 10**24:
                add(destination, destination, 3)
    for _ in range(6000):
        destination = generator.randrange(32)
        source = generator.randrange(32)
        factor = generator.randint(-3, 3)
        if abs(values[destination] + values[source] * factor) >= 10**24:
            add(destination, destination, -1)
            if abs(values[source] * factor) >= 10**24:
                factor = 0
        add(destination, source, factor)
    process = subprocess.run([str(folder / "value_arithmetic.exe")],
                             input="\n".join(operations) + "\n", capture_output=True,
                             text=True, timeout=60, check=True)
    assert not process.stderr, process.stderr
    actual = process.stdout.splitlines()
    assert len(actual) == len(expected), "Arithmetic driver returned an unexpected result count."
    for index, (received, answer) in enumerate(zip(actual, expected)):
        assert received == answer, (index, operations[index], received, answer)
    count = thread.stdout.count("PASS ")
    print(f"PASS arithmetic: {len(expected)} independent exact-integer comparisons", flush=True)
    (folder / "summary.json").write_text(json.dumps({
        "configuration": configuration, "thread_checks_passed": count,
        "arithmetic_comparisons_passed": len(expected), "status": "PASS",
        "thread_results": thread.stdout.splitlines(),
    }, indent=2), encoding="utf-8")


if __name__ == "__main__":
    main()
