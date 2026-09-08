# Commit, Push, and Pull Request Playbook

Proceed only with explicit Git publication approval.

## Preflight

1. Verify current branch and approved base commit.
2. Confirm the working tree contains only the intended scope.
3. Check ignored and untracked files for categories, not private contents, unless a concrete risk requires inspection.
4. Scan the diff for secrets, personal paths, real identifiers, generated output, and unrelated changes.
5. Stage only approved files.
6. Review staged file list and full staged diff.
7. Run `git diff --cached --check` and any publication gate not already valid.

## Publication

- Create one coherent commit with the approved message.
- Push the current feature branch normally; never force-push.
- Open a Draft PR to the approved base with scope, exclusions, evidence, manual status, and rollback notes.
- Verify local HEAD equals the remote branch and main did not change.

Do not merge merely because the PR was created.
