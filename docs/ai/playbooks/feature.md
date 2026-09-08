# Feature Playbook

## Before implementation

- Verify branch, baseline commit, working tree, active milestone, and preserved user changes.
- Read PRD acceptance criteria and architecture boundaries.
- State included behavior, exclusions, changed files, tests, manual steps, rollback, and stop conditions.
- Obtain approval for the implementation stage.

## Implementation loop

1. Add a focused reproduction or acceptance test where practical.
2. Make the smallest coherent change within the approved boundary.
3. Run focused tests and lint.
4. Correct ordinary in-scope failures without broad refactoring.
5. Preserve compatibility and existing user data.
6. Update documentation that became factually outdated.

## Completion gate

Run the full gate once, prepare manual verification steps, and report Git actions as not performed unless separately approved.
