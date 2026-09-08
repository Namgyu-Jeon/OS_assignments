# Project initialization

Status: `Not initialized`

This file turns the generic repository into a specific project. The AI must begin with read-only discovery and must not invent product decisions.

## Initialization interview

Collect and confirm:

1. Project name and one-sentence purpose
2. Target users and their primary problem
3. One end-to-end success flow for the first milestone
4. Explicit non-goals and legal/security boundaries
5. Supported platforms and environments
6. Technology constraints and approved dependencies
7. Data storage, privacy, backup, migration, and deletion expectations
8. UI, accessibility, performance, and localization expectations
9. Test levels, deterministic quality commands, and manual checks
10. Packaging, deployment, release, and repository visibility policy
11. Git workflow and which external actions require separate approval
12. Highest-risk assumptions that need research or a prototype

## Required proposal before writing

Present:

- the interpreted product scope;
- open decisions and recommended defaults;
- milestone map with acceptance gates;
- proposed architecture and repository layout;
- files to create or update;
- verification plan;
- rollback baseline;
- approval request following `docs/ai/APPROVAL_MODEL.md`.

## After approval

Populate at minimum:

- `README.md`
- `docs/PRD.md`
- `docs/DEVELOPMENT_GUIDE.md`
- `docs/ARCHITECTURE.md`
- `docs/TESTING.md`
- `docs/SECURITY.md`
- `docs/DECISIONS.md`
- `docs/CHANGELOG.md`
- `docs/ai/CURRENT_STATE.md`
- project-specific test and validation commands in `docs/ai/QUALITY_GATES.md`

Replace this status with `Initialized: YYYY-MM-DD` only after the user approves the resulting project definition. Remove unused optional modules rather than leaving misleading rules active.
