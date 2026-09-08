# AI-Assisted Project Workflow Template

A reusable, tool-neutral operating system for developing software with **Claude Code or Codex**. It emphasizes deliberate planning, explicit approval boundaries, small milestones, evidence-based verification, safe Git operations, and durable handoff documentation.

This repository is a project template, not a production framework or a substitute for human review.

## Start a new project

1. Select **Use this template** on GitHub and create a new repository.
2. Clone the new repository and open its root directory in Claude Code or Codex.
3. Send this first request:

   > `PROJECT_INIT.md`를 읽고 프로젝트 초기화를 시작해줘. 아직 파일을 변경하지 말고 현재 상태를 읽기 전용으로 확인한 뒤 필요한 질문과 계획을 제시해줘.

4. Answer the initialization questions and review the proposed milestones.
5. Approve only the first clearly bounded stage.

Detailed Korean instructions and copy-ready prompts are available in the companion guide: **[AI-Assisted Project Workflow Guide](https://Namgyu-Jeon.github.io/ai-assisted-project-workflow-guide/)**.

## How both agents use the repository

| Agent | Automatic project entry | Scoped instructions |
|---|---|---|
| Codex | `AGENTS.md` | nearest nested `AGENTS.md` or `AGENTS.override.md` |
| Claude Code | `CLAUDE.md`, which imports `AGENTS.md` | nested `CLAUDE.md` and path-scoped `.claude/rules/*.md` |

`AGENTS.md` contains the shared non-negotiable rules. `CLAUDE.md` remains a thin adapter so the two files do not drift.

## Repository map

```text
.
├── AGENTS.md                    # Shared project operating rules
├── CLAUDE.md                    # Claude Code adapter
├── PROJECT_INIT.md              # First-session interview and setup gate
├── .claude/rules/               # Claude path-scoped rules
├── docs/
│   ├── PRD.md                   # Product scope and acceptance gates
│   ├── DEVELOPMENT_GUIDE.md     # Architecture and delivery policy
│   ├── ARCHITECTURE.md          # Current system design
│   ├── TESTING.md               # Test strategy and evidence
│   ├── SECURITY.md              # Threat boundaries and data policy
│   ├── DECISIONS.md             # Decision log
│   ├── CHANGELOG.md             # Milestone history
│   └── ai/
│       ├── CURRENT_STATE.md      # Compact resumable state
│       ├── QUALITY_GATES.md      # Exact verification commands
│       ├── playbooks/            # Task-specific operating procedures
│       ├── prompts/              # Copy-ready user instructions
│       └── modules/              # Optional project-type rules
└── scripts/validate_template.py # Standard-library repository validator
```

## Core principles

- Context is layered: keep root instructions short and load detail by task or path.
- Permission is staged: implementation does not imply commit, push, merge, release, or publication.
- Evidence beats confidence: tests, diffs, logs, hashes, and human checks support completion claims.
- User data and existing changes are preserved by default.
- Automated tests remain deterministic; live external services are separate manual or opt-in checks.
- Documentation records decisions and current truth, not raw private conversations.

## Validate the template

The validator uses only the Python standard library:

```bash
python scripts/validate_template.py
git diff --check
```

## Customize without creating instruction bloat

1. Put universal, high-risk rules in root `AGENTS.md`.
2. Put stable project facts in `docs/`.
3. Put task procedures in `docs/ai/playbooks/`.
4. Put Claude path-specific rules in `.claude/rules/`.
5. Put Codex path-specific rules in an `AGENTS.md` close to the relevant code.
6. Do not import every detailed document from `CLAUDE.md`; imports load at session start.

See [Instruction architecture](docs/ai/INSTRUCTION_ARCHITECTURE.md) for concrete examples.

## License

MIT. See [LICENSE](LICENSE).
