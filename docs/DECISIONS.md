# Decision Log

## ADR-001: 과목당 하나의 공개 저장소 사용

- **Date:** 2026-09-08
- **Status:** Accepted
- **Decision:** `OS_assignments` 하나에서 과제별 폴더를 분리하고 각 과제의 직접 링크를 제출합니다.
- **Reason:** 저장소 설정을 반복하지 않으면서도 과제별 보고서를 독립적으로 열 수 있습니다.

## ADR-002: GitHub Markdown 보고서 사용

- **Date:** 2026-09-08
- **Status:** Accepted
- **Decision:** 허가된 GitHub 문서를 PPT 대신 사용하고 표지·목차·시스템·테스트·사용자·자기 평가 항목을 Markdown으로 제공합니다.
- **Reason:** 상대 링크와 목차를 통해 필요한 증거에 빠르게 접근할 수 있습니다.

## ADR-003: 코드 기반 Mermaid 다이어그램 사용

- **Date:** 2026-09-08
- **Status:** Accepted
- **Decision:** 구조도는 임의 이미지가 아니라 최종 코드의 모듈과 분기를 직접 표현한 Mermaid로 작성합니다.
- **Reason:** GitHub에서 선명하게 렌더링되고 문서와 함께 변경 이력을 관리할 수 있습니다.

## ADR-004: 소스 포함, 바이너리 제외

- **Date:** 2026-09-08
- **Status:** Accepted
- **Decision:** 검토 가능한 C++ 소스와 Visual Studio 프로젝트는 포함하고 `multi.exe`, ZIP, 빌드 결과는 제외합니다.
- **Reason:** 보고서의 근거를 확인할 수 있으면서 불필요한 생성 파일과 배포 위험을 줄입니다.
