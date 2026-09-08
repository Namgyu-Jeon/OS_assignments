# Approval and Authority Model

## Why stages are separate

Editing a local test and publishing a release have different impact and reversibility. Authorization is therefore attached to a concrete stage and scope, not to the entire conversation.

## Authority matrix

| Action | Default | Required authority |
|---|---|---|
| Read files, inspect Git, explain | Allowed when relevant | User request or task relevance |
| Edit approved local files | Not assumed | Approved implementation scope |
| Install or upgrade production dependencies | Blocked | Explicit dependency approval |
| Delete, overwrite, migrate, or reset data | Blocked | Exact destructive-scope approval |
| Commit | Blocked | Explicit commit approval |
| Push or open PR | Blocked | Explicit external-action approval |
| Merge, deploy, tag, release, publish | Blocked | Separate explicit approval |
| Change repository visibility or settings | Blocked | Exact setting-change approval |

## Approval request template

Every proposal for a mutating stage states:

1. **Outcome:** exact completion condition
2. **Reason:** why the change is necessary now
3. **Scope:** files and systems that change
4. **Difference:** observable change from current behavior
5. **Verification:** focused, full, and manual checks
6. **Rollback:** verified baseline and recovery method
7. **Exclusions:** work deliberately not included
8. **Stop conditions:** risks that require a new decision

## Safe in-scope corrections

Once a stage is approved, ordinary formatting, lint, import, deterministic test, copy, and narrowly necessary compatibility fixes may proceed if they do not change product scope, architecture, dependencies, user data, permissions, or external state. Report them at completion.

## Stop rather than improvise

Pause and preserve evidence if resolution requires a new dependency, broader refactor, elevated privilege, unsafe workaround, destructive recovery, license assumption, secret, external publication, or material behavior choice absent from the approved requirements.
