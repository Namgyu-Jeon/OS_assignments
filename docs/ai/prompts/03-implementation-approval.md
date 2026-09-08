# Implementation approval

```text
제시한 [MILESTONE_ID] 구현 계획을 승인한다.

승인 범위:
- [파일·기능·테스트]

보존 대상:
- [미커밋 변경·사용자 데이터·기존 기능]

금지:
- 새 production dependency
- 승인 범위 밖 리팩터링
- 사용자 설정·DB·출력 파일 변경 또는 삭제
- commit·push·PR·merge·release

관련 테스트 중심으로 구현하고 일반적인 lint·format·deterministic test 오류는 승인 범위 안에서 수정해라.
중대한 데이터 손실, 권한, 보안, 라이선스 또는 범위 확장 문제가 없다면 중간 상태 보고로 종료하지 말고 구현과 자동 검증까지 완료해라.

완료 시 변경 파일, 사용자 흐름, 테스트 결과, 수동 확인 절차, 제한사항, 현재 Git 상태를 보고해라.
```
