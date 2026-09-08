# Post-Merge Playbook

1. Confirm the working tree is clean and inspect `origin/main` read-only.
2. Switch to local main.
3. Fetch and run `git pull --ff-only origin main`.
4. Confirm local and remote main resolve to the same full commit.
5. Run the agreed post-merge lint, full tests, and dependency health checks once.
6. Inspect recent history and report the merge commit.
7. Preserve feature branches unless deletion was explicitly approved.

Do not edit code, create a corrective commit, or push from this stage. If verification fails, preserve the failure and start a separately approved fix branch.
