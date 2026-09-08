# Bug-Fix Playbook

## Diagnose first

1. Preserve the exact error, environment, and state after redacting private data.
2. Reproduce with the smallest deterministic test or fixture.
3. Trace actual code paths, state transitions, indices, platform policies, and late responses.
4. State the exact cause and evidence. Distinguish cause from symptoms and incidental log messages.
5. Propose the minimum fix, regression tests, exclusions, and rollback; obtain approval before editing.

## Fix rules

- Fix the violated invariant rather than hiding the error or weakening verification.
- Do not change production behavior merely to satisfy an incorrect platform-specific test.
- Keep security, integrity, license, and data-preservation checks at least as strong.
- If the failure reveals a material architecture or dependency change, stop for new approval.

## Verify

Run the original failing case, nearest regression suite, full gate, and any required real-environment reproduction.
