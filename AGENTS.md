# AI Project Working Agreement

This file is the project entry point for Codex and the shared instruction source for Claude Code.
Keep it concise. Put detailed procedures in `docs/ai/` and load only the playbook relevant to the task.

## Start every task

1. Read `docs/ai/CURRENT_STATE.md` and the project documents relevant to the request.
2. Inspect the current branch, working tree, recent commits, and existing user changes before planning edits.
3. Classify the request as read-only investigation, implementation, Git publication, or release.
4. Read the matching playbook from the routing table below.
5. Never infer permission for a later lifecycle stage from permission for an earlier one.

If `PROJECT_INIT.md` is not marked `Initialized`, follow it before implementing product features.

## Non-negotiable safety rules

- Preserve unrelated and uncommitted user changes. Never reset, overwrite, or discard them.
- Never request, expose, store, log, or commit passwords, tokens, cookies, private keys, personal paths, or private user data.
- Treat file paths, URLs, filenames, metadata, logs, imported data, and subprocess output as untrusted input.
- Do not delete user data, databases, settings, media, downloads, or broad directories without explicit approval for the exact targets.
- Do not use destructive Git recovery, force-push, amend existing commits, change repository visibility, or modify Git configuration without explicit approval.
- Do not add or upgrade a production dependency without explaining the need and receiving approval.
- Do not commit generated output, caches, local tools, credentials, databases, media, build artifacts, or partial files.
- Use argument arrays and `shell=false` for untrusted subprocess input. Do not interpolate untrusted values into shell commands.
- Stop when a verified security, legal, licensing, authorization, data-loss, or privilege boundary cannot be resolved safely.

## Approval boundary

Read-only inspection and explanation are allowed when relevant. Before a mutating stage, present an approval request containing:

1. exact objective and completion conditions;
2. reason the change is needed;
3. files, repositories, services, or systems that will change;
4. visible and behavioral difference from the current state;
5. focused and full verification plan;
6. rollback point and recovery method;
7. explicit exclusions and stop conditions.

An approved implementation scope includes ordinary fixes to lint, formatting, imports, deterministic tests, and copy needed to meet its completion conditions. Stop for scope expansion, a new production dependency, elevated privileges, destructive recovery, sensitive-data risk, material architecture change, or an unapproved external action.

Commit, push, pull request, merge, tag, release, deployment, publication, and repository visibility are separate external actions unless the user explicitly approves them together.

## Standard lifecycle

`DISCOVERY → PLANNED → APPROVED → IMPLEMENTING → FOCUSED_VERIFIED → FULL_VERIFIED → MANUAL_VERIFIED → GIT_APPROVED → PR_OPEN → MERGED → POST_MERGE_VERIFIED → RELEASED`

- Record the current stage in `docs/ai/CURRENT_STATE.md` when the stage materially changes.
- Do not skip a stage whose acceptance evidence is required by the project.
- Do not mark manual verification complete unless a human performed it.
- Do not claim a clean-machine, production, platform, or release result that was not actually tested.

## Playbook routing

| Situation | Read before acting |
|---|---|
| New project or major direction | `PROJECT_INIT.md`, `docs/ai/playbooks/discovery.md` |
| New feature or milestone | `docs/ai/playbooks/feature.md` |
| Bug report or failure diagnosis | `docs/ai/playbooks/bugfix.md` |
| CI-only or cross-platform failure | `docs/ai/playbooks/ci-failure.md`, `docs/ai/playbooks/cross-platform.md` |
| Broken local development environment | `docs/ai/playbooks/environment-recovery.md` |
| UI or interaction change | `docs/ai/playbooks/ui-change.md` |
| Dependency, binary, license, or supply chain | `docs/ai/playbooks/dependency-change.md` |
| Commit, push, or pull request | `docs/ai/playbooks/commit-pr.md` |
| Merge completed | `docs/ai/playbooks/post-merge.md` |
| Package, deploy, tag, or release | `docs/ai/playbooks/release.md` |
| Pause, resume, or handoff | `docs/ai/playbooks/handoff.md` |

## Implementation quality

- Keep architecture boundaries explicit and avoid unrelated refactors.
- Prefer small, reviewable milestones with observable acceptance criteria.
- Add success, failure, and boundary tests for changed behavior where practical.
- Run focused tests during implementation; run the full project gate once after the approved scope is complete.
- Keep live external-service checks out of deterministic tests unless explicitly marked and approved.
- For UI changes, verify keyboard access, focus, scaling, important states, and a representative rendered result.
- For platform changes, use explicit platform fixtures and preserve other-platform regression coverage.
- Update project decisions, changelog, testing instructions, and current state when behavior or policy changes.

## Completion report

Lead with the outcome and include:

- implemented or diagnosed result;
- exact changed files and why;
- focused and full verification results with counts;
- manual verification still required;
- known limitations and risks;
- current branch and working-tree state;
- Git or external actions performed and actions explicitly not performed;
- next recommended milestone.

Use clear language. Distinguish verified facts, evidence-based inference, and recommendations.
