# Decision Log

Record durable decisions, not every conversation. Do not rewrite historical entries; add a superseding decision.

## ADR-000: Use staged human approval for mutating AI work

- **Status:** Accepted
- **Context:** AI coding tools can inspect, edit, run commands, and interact with external systems. These actions have different risk and reversibility.
- **Decision:** Separate discovery, implementation, Git publication, merge, deployment, and release authority. Require explicit scope and evidence at each material boundary.
- **Consequences:** Work may pause for high-impact decisions, while ordinary in-scope lint and test corrections can proceed without repeated approval.

## ADR template

### ADR-NNN: `[TITLE]`

- **Date:** `YYYY-MM-DD`
- **Status:** Proposed | Accepted | Superseded
- **Context:** `[PROBLEM_AND_CONSTRAINTS]`
- **Options:** `[OPTIONS_CONSIDERED]`
- **Decision:** `[SELECTED_OPTION]`
- **Consequences:** `[BENEFITS_COSTS_AND_FOLLOW_UP]`
- **Evidence:** `[PR_TEST_DOC_OR_RELEASE_REFERENCE]`
