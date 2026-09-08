# Security and Privacy

## Product boundary

Describe authorized use and explicitly exclude abuse, credential collection, access-control bypass, and unsupported privileged behavior.

## Development rules

- Do not place secrets in prompts, source, screenshots, tests, logs, documentation, commits, issues, or pull requests.
- Use placeholders in examples and environment variables or an approved secret store at runtime.
- Redact tokens, URLs containing secrets, personal paths, identifiers, and user content before sharing diagnostics.
- Treat external output and imported data as untrusted.
- Use least privilege for CI, apps, tokens, file access, and network access.
- Pin dependencies and verify official integrity or signatures where available.
- Preserve user data on failed migration, uninstall, cancellation, and partial operation.

## Required stop conditions

Stop before implementation or publication when:

- provenance, license, or redistribution rights cannot be established;
- an action needs unapproved administrator or security changes;
- a secret or private user artifact may be exposed;
- safe rollback or exact destructive scope cannot be established;
- a production dependency or external binary is unreviewed.

## Reporting vulnerabilities

Define a private contact method before public use. Do not ask reporters to include credentials, private data, or production database copies.
