# Testing Strategy

## 자동 확인

- 저장소 필수 파일과 이미지 존재 여부
- Markdown 상대 링크와 이미지 경로
- Mermaid 코드 블록의 시작 선언과 닫힘
- 실행 파일, ZIP, 빌드 폴더 등 공개 제외 항목
- Visual Studio Community 2026 Release/x64 빌드
- 정상 입력의 전체 출력 문자열 및 공통 열 좌표
- 오류 입력, 재입력 성공, `q/Q` 종료

문서 구조 검사는 다음 명령으로 실행합니다.

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\validate_repository.ps1
```

프로그램 테스트의 실제 결과와 범위는 [Assignment 01 테스트 설명](../assignments/assignment-01/docs/TEST_DESCRIPTION.md)에 기록합니다.

## 수동 확인

- GitHub 공개 화면에서 README, 표, Mermaid 다이어그램이 잘리지 않고 표시되는지 확인합니다.
- 네 장의 콘솔 스크린샷이 명령어와 출력 결과를 읽을 수 있는 크기로 표시되는지 확인합니다.
- 제출자는 공개 링크를 로그아웃 상태 또는 시크릿 창에서 열어 접근 가능 여부를 최종 확인합니다.
