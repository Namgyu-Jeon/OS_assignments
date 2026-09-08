# Testing Strategy

## Deterministic default suite

Default tests must not depend on real user data, credentials, production databases, paid services, live media, or mutable third-party content. Use temporary directories, isolated settings, fakes, and explicit platform fixtures.

## Test layers

| Layer | Purpose | Command |
|---|---|---|
| Static | Formatting, lint, types, schemas | `[STATIC_COMMAND]` |
| Unit | Domain success, failure, and boundaries | `[UNIT_COMMAND]` |
| Integration | Owned adapters and temporary persistence | `[INTEGRATION_COMMAND]` |
| UI | Critical interaction, keyboard, focus, rendering | `[UI_COMMAND]` |
| Full | Deterministic repository gate | `[FULL_COMMAND]` |

Put exact runnable commands in [ai/QUALITY_GATES.md](ai/QUALITY_GATES.md).

## Manual verification

Manual evidence is required for behavior automation cannot establish reliably, such as real operating-system integration, installation, hardware, external-service behavior, visual quality, or clean-machine compatibility.

Record:

- environment and version;
- exact scenario without private data;
- expected and actual result;
- screenshots or logs after redaction;
- limitations not tested.

## Test isolation checklist

- [ ] Temporary settings and data directories
- [ ] No real user database or downloads
- [ ] No hidden dependence on machine locale, path, or installed tools
- [ ] Explicit platform fixture for platform-specific expectations
- [ ] Late responses and cancellation covered where applicable
- [ ] Existing state preserved on failure
