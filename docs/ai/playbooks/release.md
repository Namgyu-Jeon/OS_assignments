# Release Playbook

## Release authorization

Tagging, publishing artifacts, deployment, repository visibility, signing, notarization, and store submission are separate external actions. Confirm the exact approved subset.

## Candidate gate

- [ ] Main and origin/main match the approved full commit
- [ ] Working tree is clean
- [ ] Full automated gate passes
- [ ] Required clean-environment and real-device checks pass
- [ ] Build inputs are pinned and provenance verified
- [ ] Artifacts contain only approved runtime files and notices
- [ ] Install, upgrade, uninstall, rollback, and user-data preservation are tested as required
- [ ] SHA-256 or stronger approved integrity list is regenerated and rechecked
- [ ] Release notes state exact support and limitations without overclaiming
- [ ] Existing tags, releases, and platform assets remain unchanged unless explicitly in scope

Create the tag and release only after every blocking gate passes. If any check fails, do not create or rewrite the tag; preserve logs and report options.
