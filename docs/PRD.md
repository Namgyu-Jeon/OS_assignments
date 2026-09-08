# Product Requirements Document

Status: Draft

## Product definition

- **Project name:** `[PROJECT_NAME]`
- **One-sentence purpose:** `[PRODUCT_PURPOSE]`
- **Primary users:** `[TARGET_USERS]`
- **Problem:** `[USER_PROBLEM]`
- **Why now:** `[RATIONALE]`

## Safety and scope

### Supported use

- `[AUTHORIZED_USE_CASE]`

### Explicitly excluded

- `[NON_GOAL_OR_UNSAFE_USE]`

### Data and privacy

- Data collected:
- Local or remote storage:
- Retention and deletion:
- Secrets and authentication:

## Success flow

Describe the first end-to-end outcome a real user must complete without mocks:

1. `[ENTRY]`
2. `[ACTION]`
3. `[RESULT]`
4. `[RECOVERY_OR_ERROR]`

## Functional requirements

| ID | Requirement | Priority | Acceptance evidence |
|---|---|---|---|
| FR-001 | `[REQUIREMENT]` | Must | `[TEST_OR_MANUAL_EVIDENCE]` |

## Quality requirements

| Area | Requirement |
|---|---|
| Reliability | `[FAILURE_AND_RECOVERY_EXPECTATION]` |
| Performance | `[MEASURABLE_BOUNDARY]` |
| Accessibility | `[KEYBOARD_SCREEN_READER_SCALING]` |
| Security | `[INPUT_PROCESS_DATA_BOUNDARY]` |
| Privacy | `[MINIMIZATION_AND_RETENTION]` |
| Compatibility | `[SUPPORTED_PLATFORMS]` |

## Milestones

| Milestone | Goal | Acceptance gate | Exclusions |
|---|---|---|---|
| M0 | Product definition and architecture | Decisions approved and reproducible setup documented | Feature implementation |
| M1 | Smallest real end-to-end flow | Real user flow works and is verified | Secondary features |
| M2 | Product quality and persistence | Recovery, settings, UX, and regression gates pass | Broad expansion |
| M3+ | Bounded feature expansions | Each sub-milestone has independent evidence | Silent scope growth |
| Release | Packaging and clean-environment validation | Artifact, license, integrity, and manual gates pass | Unsupported claims |

## Release blockers

- Crash, frozen critical UI, corrupt or invalid output
- Silent overwrite, deletion, fallback, or data migration failure
- Required test, security, privacy, or license check failure
- Missing human acceptance for material UI or real-world behavior
- Claims for platforms or environments that were not tested
