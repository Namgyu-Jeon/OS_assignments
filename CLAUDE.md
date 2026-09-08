@AGENTS.md

# Claude Code adapter

- Use this file only for Claude Code-specific behavior; shared project rules belong in `AGENTS.md`.
- Use `.claude/rules/` for path-scoped instructions so unrelated rules do not consume every task's context.
- Confirm loaded instruction sources with `/memory` when instruction routing is in doubt.
- Imports are organizational, not lazy loading: imported files enter startup context.
- Keep this adapter short and avoid duplicating `AGENTS.md`.
