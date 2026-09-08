# Post-merge verification

```text
PR #[NUMBER]를 main에 merge했다.

다음만 수행해라.
1. 현재 branch·working tree·origin/main을 읽기 전용으로 확인
2. local main으로 전환
3. git pull --ff-only origin main
4. lint·전체 test·dependency health check를 한 번 실행
5. local main과 origin/main 일치, clean working tree, 최근 이력을 보고

코드 수정·새 commit·push·branch delete·release는 하지 마라.
검증 실패 시 수정하지 말고 실패 증거와 별도 fix 단계 필요성을 보고해라.
```
