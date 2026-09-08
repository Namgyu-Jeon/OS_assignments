---
paths:
  - "tests/**/*"
  - "**/*.{test,spec}.*"
---

# Test rules

- Isolate settings, databases, filesystem output, time, network, and platform assumptions.
- Use explicit temporary directories and platform fixtures.
- Cover success, failure, boundary, cancellation, and state-preservation behavior where relevant.
- Never use real credentials, user data, production services, or mutable live content in the deterministic suite.
