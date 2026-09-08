# Release approval

```text
[VERSION] Release 생성을 승인한다.

기준 commit:
- [FULL_SHA]

지원 범위:
- [PLATFORM_AND_FEATURES]

필수 artifact:
- [FILES]

먼저 main 동기화, clean tree, full quality gate, 공식 build, artifact 내용·license notices·SHA-256·provenance·clean-environment evidence를 확인해라.
모든 blocking gate가 통과한 경우에만 annotated tag과 [private/public] Release를 생성해라.

기존 tag·Release·다른 platform asset·repository visibility는 변경하지 마라.
실패하면 tag과 Release를 만들지 말고 로그와 artifact를 보존해 보고해라.
```
