# Claude Code and Codex Instruction Architecture

## Design goal

Give each agent the smallest relevant context while keeping high-risk rules visible in every task. Avoid duplicated instruction sources that drift over time.

## Two layers

### Personal global instructions

Install once per machine:

- Claude Code: `~/.claude/CLAUDE.md`
- Codex: `~/.codex/AGENTS.md`

Use the examples under `global/`. Keep language preference and universal safety here; never put project paths, secrets, or technology-specific rules in global instructions.

### Repository instructions

- `AGENTS.md` is the shared core and Codex entry point.
- `CLAUDE.md` imports `AGENTS.md` and contains only Claude-specific behavior.
- Stable product truth belongs in `docs/`.
- Task procedures belong in `docs/ai/playbooks/` and are read explicitly by routing rule.

## Scoped rules

Claude Code supports path-scoped `.claude/rules/*.md`. Rules without `paths` frontmatter load every session, so every rule in this template is path-scoped.

Codex resolves instructions by directory. Add a concise `AGENTS.md` close to specialized code, for example:

```text
src/ui/AGENTS.md
src/infrastructure/AGENTS.md
packaging/AGENTS.md
```

Place only the differences from the root agreement in nested files. Use `AGENTS.override.md` only when a closer scope must replace an inherited rule.

## Context budget rules

- Target fewer than 200 lines for each root instruction file.
- Put critical safety and routing in root; do not make root files pointer-only.
- Do not import a large documentation tree into `CLAUDE.md`: imports load at startup.
- Prefer path-scoped rules or explicit task routing for real progressive disclosure.
- Remove obsolete and contradictory rules during milestone closeout.
- Put machine-enforceable requirements in tests, hooks, CI, permissions, or scripts rather than repeating prose.

## Verification

- Claude Code: use `/memory` to inspect loaded instruction and rule files.
- Codex: ask it to list and summarize the active instruction sources for the current directory.
- Run scenario checks from the companion guide to ensure both agents stop, ask, test, and report consistently.

Official references:

- [Claude Code memory and CLAUDE.md](https://code.claude.com/docs/en/memory)
- [Codex custom instructions with AGENTS.md](https://learn.chatgpt.com/docs/agent-configuration/agents-md)
