# Environment Recovery Playbook

## Guardrails

Environment repair must preserve uncommitted code, user settings, databases, downloads, local tools, and Git configuration.

## Procedure

1. Inspect the required version, actual executable, virtual environment, lock files, and Git diff read-only.
2. Identify the smallest replaceable environment directory.
3. For installers or binaries, use the approved official source and verify published integrity and platform signature before execution.
4. Stop if the required version is unavailable, integrity fails, administrator access is required unexpectedly, or an unapproved fallback would alter dependencies.
5. Recreate only the approved environment and install strictly from existing lock or pinned dependency files.
6. Confirm versions and dependency health.
7. Run focused and full verification without modifying product or lock files.

Do not disguise a product failure by changing the toolchain version outside policy.
