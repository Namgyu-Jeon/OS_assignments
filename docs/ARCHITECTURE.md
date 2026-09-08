# Repository Architecture

## 정보 구조

```mermaid
flowchart TD
    ROOT[저장소 첫 화면] --> INDEX[과제 목록]
    INDEX --> A1[Assignment 01 첫 화면]
    A1 --> REQ[요구사항]
    A1 --> SYS[시스템 설명]
    A1 --> TEST[테스트와 실제 화면]
    A1 --> USER[빌드·실행 안내]
    A1 --> SELF[자기 평가]
    A1 --> SOURCE[Visual Studio 소스]
```

최상위 README는 과목 전체의 인덱스만 담당합니다. 과제의 상세 내용은 과제 폴더 안에서 완결되며, 이후 과제도 같은 구조를 반복해 링크가 서로 섞이지 않게 합니다.

Assignment 01 프로그램 자체의 데이터 흐름과 알고리즘은 [시스템 설명](../assignments/assignment-01/docs/SYSTEM_DESCRIPTION.md)에 기록합니다.
