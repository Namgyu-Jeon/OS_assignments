# 사용자 안내

[과제 1 보고서로 돌아가기](../README.md)

## 1. 준비 환경

- Windows 10 또는 Windows 11
- Desktop development with C++ 워크로드가 설치된 Visual Studio Community 2026
- 별도 외부 라이브러리는 필요하지 않습니다.

GitHub 저장소에는 소스와 프로젝트 설정만 포함하며 실행 파일은 포함하지 않습니다. 아래 절차로 직접 Release/x64 실행 파일을 생성합니다.

## 2. Visual Studio에서 빌드

1. 저장소에서 `assignments\assignment-01\project` 폴더로 이동합니다.
2. [multi.sln](../project/multi.sln)을 더블클릭해 Visual Studio로 엽니다.
3. 상단의 솔루션 구성은 `Release`, 플랫폼은 `x64`로 선택합니다.
4. `빌드 > 솔루션 빌드`를 누르거나 `Ctrl+Shift+B`를 입력합니다.
5. 빌드가 성공하면 다음 위치에 실행 파일이 생성됩니다.

```text
project\build\x64\Release\multi.exe
```

소스 코드를 수정하고 다시 빌드하면 같은 위치의 실행 파일이 새 결과로 갱신됩니다.

## 3. PowerShell에서 실행

`project` 폴더에서 PowerShell을 열고 다음과 같이 실행합니다.

```powershell
.\build\x64\Release\multi.exe 15
```

PowerShell은 현재 폴더의 실행 파일을 자동으로 찾지 않으므로 경로 앞의 `.\`이 필요합니다.

## 4. 명령 프롬프트에서 실행

`project` 폴더에서 명령 프롬프트(cmd)를 열고 다음과 같이 실행합니다.

```bat
build\x64\Release\multi.exe 15
```

## 5. 입력 규칙

명령행에는 1 이상의 정수 하나만 전달합니다. 다음 값은 잘못된 입력입니다.

```text
인수 없음, 인수 둘 이상, abc, 8a, 8.0, ., 8,, 0, -1, 공백 포함 값, int 범위 초과
```

잘못 입력하면 프로그램은 다음과 같은 오류 기준과 예시를 보여 주고 다시 입력받습니다.

```text
오류: 최대 단수는 1 이상의 정수 하나만 입력할 수 있습니다.
예: multi.exe 8

다시 입력할 최대 단수 (종료: q):
```

재입력에서도 숫자만 허용하며 `q` 또는 `Q`를 입력하면 정상 종료합니다.

## 6. 콘솔 창이 바로 닫히는 경우

파일 탐색기에서 `multi.exe`를 더블클릭하면 별도의 콘솔 창이 열립니다. 유효한 값을 입력해 출력이 끝나면 프로그램도 정상 종료되므로 그 창이 바로 닫힐 수 있습니다. 결과를 계속 확인하려면 먼저 PowerShell이나 명령 프롬프트를 열고 위 명령으로 실행합니다.
