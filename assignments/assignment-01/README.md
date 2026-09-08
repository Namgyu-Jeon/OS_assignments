# Assignment 01 — Multiplication Table

> 명령행에서 최대 단수를 받아 삼각형 형태의 구구단을 5단씩 정렬해 출력하는 C++17 콘솔 프로그램

| 구분 | 내용 |
|---|---|
| 과목 | DCCS301 Operating System |
| 과제 | Programming Assignment 01 |
| 작성자 | 전남규 |
| 학번 | 2020271319 |
| 언어 | C++17 |
| 환경 | Windows, Visual Studio Community 2026, Release/x64 |
| 실행 파일 | `multi.exe` |

## 목차

1. [과제 개요](#과제-개요)
2. [구현 결과](#구현-결과)
3. [문서](#문서)
4. [핵심 설계](#핵심-설계)
5. [프로젝트 구성](#프로젝트-구성)
6. [빠른 빌드와 실행](#빠른-빌드와-실행)

## 과제 개요

`multi.exe N`처럼 양의 정수 하나를 명령행 인수로 전달하면 1단부터 N단까지 출력합니다. 단 `d`는 `d x 1`부터 `d x d`까지만 표시하며, 다섯 단을 한 묶음으로 가로 배치합니다. 숫자의 자릿수가 커져도 전체 최대 단수를 기준으로 공통 열 폭을 계산하기 때문에 위아래 묶음이 동일한 세로 격자를 유지합니다.

명령행 인수가 잘못되면 프로그램이 끝나지 않고 오류 이유와 예시를 보여 준 뒤 다시 입력받습니다. 재입력에서도 같은 엄격한 검사를 사용하며 `q` 또는 `Q`로 정상 종료할 수 있습니다.

## 구현 결과

아래 이미지는 최종 프로그램에 `15`를 전달해 실제로 실행한 PowerShell 화면입니다.

![15단 실제 실행 결과](docs/images/multi-15.png)

## 문서

| 문서 | 주요 내용 |
|---|---|
| [과제 요구사항](docs/ASSIGNMENT_REQUIREMENTS.md) | 강의에서 안내한 기능, 제출 형식, 채점 기준 |
| [시스템 설명](docs/SYSTEM_DESCRIPTION.md) | 데이터 흐름도, 순서도, 모듈과 함수, 알고리즘과 효율성 |
| [테스트 설명](docs/TEST_DESCRIPTION.md) | 검증 방법, 실제 결과, 3·5·8·15단 실행 화면 |
| [사용자 안내](docs/USER_GUIDE.md) | Visual Studio 빌드와 PowerShell·명령 프롬프트 실행 방법 |
| [자기 평가](docs/SELF_EVALUATION.md) | 독창성 85/100의 근거와 학습 내용, 도구 사용 범위 |

## 핵심 설계

- 명령행과 재입력에 동일한 `PositiveIntegerParser`를 적용해 부분 숫자 변환을 허용하지 않았습니다.
- 5단 묶음은 이름 있는 상수 `kTablesPerGroup`으로 관리합니다.
- N과 `N × N`의 자릿수로 공통 열 너비를 계산해 모든 묶음의 시작점과 결과 위치를 맞춥니다.
- 끝난 단은 같은 폭의 공백으로 유지하여 오른쪽 단이 왼쪽으로 이동하지 않게 합니다.
- 한 행만 문자열로 만든 뒤 한 번에 기록하여 전체 표를 메모리에 저장하지 않습니다.

## 프로젝트 구성

```text
project/
├─ multi.sln
├─ multi.vcxproj
├─ multi.vcxproj.filters
└─ src/
   ├─ main.cpp
   ├─ app/
   │  ├─ MultiplicationTableApp.h
   │  └─ MultiplicationTableApp.cpp
   ├─ input/
   │  ├─ PositiveIntegerParser.h
   │  └─ PositiveIntegerParser.cpp
   └─ output/
      ├─ TableRenderer.h
      └─ TableRenderer.cpp
```

소스는 [project/src](project/src/)에서 직접 확인할 수 있습니다.

## 빠른 빌드와 실행

1. [multi.sln](project/multi.sln)을 Visual Studio Community 2026에서 엽니다.
2. 구성은 `Release`, 플랫폼은 `x64`를 선택합니다.
3. `Ctrl+Shift+B`로 솔루션을 빌드합니다.
4. `project` 폴더에서 PowerShell을 열고 실행합니다.

```powershell
.\build\x64\Release\multi.exe 15
```

전체 절차와 명령 프롬프트 사용법은 [사용자 안내](docs/USER_GUIDE.md)에 정리했습니다.
