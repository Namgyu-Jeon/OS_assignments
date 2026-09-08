# Quality Gates

## 문서 구조 검사

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\validate_repository.ps1
```

## Assignment 01 Release/x64 빌드

Visual Studio Developer Command Prompt에서 실행합니다.

```bat
msbuild assignments\assignment-01\project\multi.sln /t:Rebuild /m "/p:Configuration=Release;Platform=x64"
```

## Git 게시 전 검사

```powershell
git diff --check
git status --short
git diff --cached --name-status
git diff --cached --check
```

추가 확인 항목:

- README와 상세 문서의 상대 링크가 모두 존재하는가
- Mermaid 블록이 `flowchart` 또는 지원되는 다이어그램 선언으로 시작하는가
- 실제 이미지 4개가 존재하고 빈 파일이 아닌가
- `.exe`, `.zip`, `build`, `.vs`, `*.vcxproj.user`가 추적되지 않는가
- 정상·오류·경계 입력 결과가 테스트 문서와 일치하는가
- 공개 페이지에서 표, 이미지, Mermaid가 정상 렌더링되는가
