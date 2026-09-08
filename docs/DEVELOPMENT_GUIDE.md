# Development Guide

## 과제 추가 규칙

새 과제는 `assignments/assignment-NN` 폴더에 추가합니다. 과제 폴더의 `README.md`는 표지와 목차 역할을 하며, 상세 보고서는 `docs`, 빌드 가능한 소스는 `project`에 둡니다.

```text
assignments/assignment-NN/
├─ README.md
├─ docs/
│  ├─ ASSIGNMENT_REQUIREMENTS.md
│  ├─ SYSTEM_DESCRIPTION.md
│  ├─ TEST_DESCRIPTION.md
│  ├─ USER_GUIDE.md
│  ├─ SELF_EVALUATION.md
│  └─ images/
└─ project/
```

실행 파일, ZIP, `.vs`, `build`, Debug·Release 폴더는 저장소에 추가하지 않습니다. 테스트 통과 여부와 성능 수치는 같은 버전의 소스를 실제로 실행했을 때만 갱신합니다.

## 변경 절차

1. 과제별 브랜치에서 문서와 소스를 작성합니다.
2. 상대 링크, 이미지, Mermaid 블록, UTF-8을 검사합니다.
3. 해당 Visual Studio 솔루션을 Release/x64로 새로 빌드합니다.
4. 정상·오류·경계 입력을 실행하고 문서 수치와 대조합니다.
5. 전체 diff와 공개 제외 파일을 확인한 뒤 리뷰를 거쳐 `main`에 반영합니다.
