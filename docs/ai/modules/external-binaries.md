# External Binary Module

- Use a fixed official source and exact version; avoid mutable “latest” URLs.
- Verify checksum and signature/fingerprint trust paths when available before execution or bundling.
- Record architecture, build flags, license profile, transitive libraries, dynamic dependencies, and source-provision obligations.
- Build in an isolated staging directory and keep archives, keys, caches, and output ignored unless an approved provenance artifact requires them.
- Frozen applications resolve only audited bundle paths; development fallback must be explicit and testable.
- Preflight must block the operation safely when the tool is missing or incompatible.
- Do not weaken verification to accommodate unexpected upstream output; adjust semantic validation only with exact evidence and regression tests.
