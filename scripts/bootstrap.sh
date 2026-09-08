#!/usr/bin/env bash
# Apply this template's Git/GitHub rules to a freshly created repository.
#
# Run once, from the repository root, right after:
#
#     gh repo create <name> --private \
#       --template Namgyu-Jeon/ai-assisted-project-workflow-template --clone
#     cd <name>
#     bash scripts/bootstrap.sh
#
# Idempotent: safe to run again after settings drift.
set -euo pipefail

echo "== git hooks =="
git config core.hooksPath .githooks
echo "core.hooksPath -> $(git config core.hooksPath)"

repo=$(gh repo view --json nameWithOwner --jq .nameWithOwner)

echo
echo "== repository merge settings ($repo) =="
gh repo edit "$repo" \
  --enable-squash-merge \
  --enable-merge-commit=false \
  --enable-rebase-merge=false \
  --delete-branch-on-merge

echo
echo "== branch protection ruleset on main =="
ruleset_json=$(mktemp)
trap 'rm -f "$ruleset_json"' EXIT

cat > "$ruleset_json" <<'EOF'
{
  "name": "protect-main",
  "target": "branch",
  "enforcement": "active",
  "conditions": {"ref_name": {"include": ["refs/heads/main"], "exclude": []}},
  "rules": [
    {"type": "deletion"},
    {"type": "non_fast_forward"},
    {"type": "pull_request", "parameters": {
      "required_approving_review_count": 0,
      "dismiss_stale_reviews_on_push": true,
      "require_code_owner_review": false,
      "require_last_push_approval": false,
      "required_review_thread_resolution": false,
      "allowed_merge_methods": ["squash"]
    }}
  ]
}
EOF

if gh api -X POST "repos/$repo/rulesets" --input "$ruleset_json" >/tmp/ruleset-response.json 2>&1; then
  echo "적용됨: GitHub이 main 직접 push·강제 push·삭제를 거부하고 PR을 요구합니다."
else
  if grep -q '"status":"403"' /tmp/ruleset-response.json 2>/dev/null; then
    echo "건너뜀 (403): 이 플랜은 private 저장소 룰셋을 지원하지 않습니다."
    echo "          .githooks/pre-commit이 로컬 가드로 남습니다 — --no-verify로 우회 가능하니 참고."
  else
    echo "룰셋 요청이 예상과 다르게 응답했습니다. 아래 원문을 확인하세요:"
    cat /tmp/ruleset-response.json
  fi
fi
rm -f /tmp/ruleset-response.json

echo
echo "== 확인 =="
gh repo view "$repo" \
  --json visibility,defaultBranchRef,squashMergeAllowed,mergeCommitAllowed,rebaseMergeAllowed,deleteBranchOnMerge \
  --jq '"공개범위: \(.visibility) / 기본브랜치: \(.defaultBranchRef.name) / squash전용: \(.squashMergeAllowed and (.mergeCommitAllowed|not) and (.rebaseMergeAllowed|not)) / 병합후브랜치삭제: \(.deleteBranchOnMerge)"'

echo
echo "다음: PROJECT_INIT.md를 따라 이 프로젝트를 정의하세요."
