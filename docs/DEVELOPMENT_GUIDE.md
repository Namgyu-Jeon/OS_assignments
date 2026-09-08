# Development Guide

## Delivery model

Work in small milestones. Each milestone must state its goal, acceptance gate, exclusions, tests, manual checks, and Git authority. A milestone is not complete merely because code was written.

## Architecture expectations

- Separate user interface, application orchestration, domain rules, and infrastructure integrations when the project benefits from these boundaries.
- Keep long-running or external operations away from interactive UI threads.
- Exchange structured domain models across boundaries rather than raw third-party responses.
- Centralize platform and environment differences behind testable interfaces.
- Keep persistence migrations additive, testable, and failure-safe.

Record the actual project design in [ARCHITECTURE.md](ARCHITECTURE.md) rather than forcing these suggestions onto every project.

## Branch and review policy

1. Synchronize and verify the approved base branch.
2. Create one clearly named feature, fix, build, or docs branch.
3. Preserve unrelated working-tree changes.
4. Implement and verify one coherent scope.
5. Review staged files and staged diff before committing.
6. Use normal push; never force-push unless separately authorized for an exact recovery case.
7. Prefer Draft PR while manual or external validation remains.
8. After merge, fast-forward local main and rerun the agreed post-merge gate.

## Change control

New production dependencies, external binaries, migrations, platform permissions, release publication, and repository visibility changes require explicit review. Use the matching playbook under `docs/ai/playbooks/`.

## Documentation duties

- Update `DECISIONS.md` for durable policy or architecture choices.
- Update `CHANGELOG.md` for completed milestones and user-visible changes.
- Update `TESTING.md` when commands, fixtures, or manual procedures change.
- Update `CURRENT_STATE.md` so a new session can resume without relying on chat history.
