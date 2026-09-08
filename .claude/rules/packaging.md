---
paths:
  - "packaging/**/*"
  - ".github/workflows/**/*"
  - "Dockerfile*"
  - "**/*.spec"
---

# Packaging and workflow rules

- Pin build inputs and use official sources with integrity or signature verification where available.
- Use least permissions, explicit triggers, and bounded artifact retention.
- Keep secrets, user data, local settings, caches, databases, media, and development tools out of artifacts.
- Verify architecture, runtime dependencies, notices, hashes, startup, rollback, and clean-environment limitations before release claims.
