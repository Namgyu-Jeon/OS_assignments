# Packaging and Release Module

- Pin builder versions and document reproducible commands.
- Keep package output outside source commits and inspect the final file inventory.
- Bundle required runtime, notices, version metadata, and user-facing executable only.
- Test start, version, tool resolution, normal exit, install/upgrade/uninstall, and user-data preservation as applicable.
- Generate hashes after the final artifact and verify them again before publication.
- Use least-privilege workflows, explicit triggers, short artifact retention, and no secrets in logs or artifacts.
- Treat code signing, notarization, store submission, tag creation, and release publication as separate gates.
- State unsigned, prerelease, architecture, OS version, and unsupported features precisely.
