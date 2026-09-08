# Quality Gates

Replace placeholders during project initialization. Commands must be non-interactive and deterministic by default.

## Focused verification

```text
[FOCUSED_LINT_COMMAND]
[FOCUSED_TEST_COMMAND]
```

Run focused checks while implementing. Include the directly changed behavior and the nearest regression boundary.

## Full verification

Run once after the approved implementation scope is complete:

```text
[FULL_LINT_COMMAND]
[FULL_TEST_COMMAND]
[DEPENDENCY_HEALTH_COMMAND]
git diff --check
```

Also verify:

- UTF-8 and repository line-ending policy
- internal documentation links
- secrets, tokens, personal paths, and private identifiers
- ignored caches, settings, databases, media, partial files, build output, and local tools
- dependency locks and notices when dependencies changed
- platform-specific regression coverage when shared code changed

## Staged and publication gate

Before an approved commit or push:

```text
git status --short
git diff --cached --name-status
git diff --cached --check
```

Review the entire staged diff. Confirm only approved files are staged and the base branch did not change.

## Evidence language

- Report exact commands, pass/fail, and test counts.
- Distinguish “not run,” “automated,” “manually verified,” and “clean environment verified.”
- A previous result is reusable only when no relevant code, configuration, lock, or environment changed afterward.
- Do not convert absence of evidence into a compatibility or security claim.
