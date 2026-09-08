# Commit, push, and Draft PR

```text
수동 검증을 완료했다. 현재 [BRANCH]의 승인된 변경만 최종 검토한 뒤 commit·일반 push·main 대상 Draft PR 생성까지 진행해라.

기준 main:
- [FULL_SHA]

커밋 메시지:
- [MESSAGE]

PR 제목:
- [TITLE]

PR 본문:
- 변경 사항, 검증, 수동 확인, 알려진 제한, 제외 범위를 사실대로 작성

먼저 branch·working tree·main 대비 diff·추적 예정 파일을 확인해라.
설정·DB·media·partial·cache·build/dist·local tools·secret·개인 경로가 포함되지 않는지 검사해라.
승인된 파일만 stage하고 staged file list, staged diff, git diff --cached --check를 확인해라.

force push·amend·main merge·branch delete·release는 하지 마라.
완료 후 commit SHA, PR 링크, 로컬/원격 branch 일치, main 미변경, working tree 상태를 보고해라.
```
