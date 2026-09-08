# Cross-Platform Playbook

## Policy

Define shared invariants first, then isolate operating-system names, paths, process behavior, UI conventions, packaging, and limits behind explicit adapters or fixtures.

## Checklist

- [ ] Platform is detected or injected through one testable boundary
- [ ] Executable names and bundle locations follow the target platform
- [ ] Path and filename rules test both shared safety and platform-specific restrictions
- [ ] Process cancellation covers child processes and late completion signals
- [ ] Native window behavior is allowed where forcing another platform's style is unsafe
- [ ] Shared-code changes retain other-platform regression tests
- [ ] Packaging fixtures match the actual frozen/bundle structure
- [ ] Claims name the exact OS version and architecture actually tested

Never make one platform imitate another only to preserve an incorrect test expectation.
