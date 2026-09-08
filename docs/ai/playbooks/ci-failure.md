# CI Failure Playbook

## Read-only triage

1. Identify the exact workflow, run, commit, runner, failed step, command, and first meaningful error.
2. Confirm whether local code matches the tested revision.
3. Separate the primary failure from downstream skips and broken-pipe or cleanup noise.
4. Compare runner OS, shell, architecture, tool version, filesystem, locale, and permissions with local assumptions.
5. Reproduce with deterministic fixtures when possible; do not trigger another paid or remote run during diagnosis unless approved.

## Fix policy

- Prefer explicit platform fixtures and shared policy helpers over hard-coded test expectations.
- Preserve stronger integrity and safety checks; correct the faulty assumption or verifier semantics.
- Pin versions and inspect official compatibility when runner tools differ.
- Add a regression test for the exact cause.

## Report

Include the first failing line, exact cause, affected files, local evidence, and whether a workflow rerun is still required.
