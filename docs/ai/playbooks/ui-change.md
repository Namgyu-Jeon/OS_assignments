# UI Change Playbook

## Before editing

Describe the current problem, intended user flow, visible before/after difference, representative states, keyboard behavior, scaling, and responsive limits. Use a wireframe when relationships are not clear in prose. Obtain design-scope approval.

## Implementation requirements

- Preserve architecture: UI does not directly perform long-running infrastructure work.
- Cover loading, empty, success, error, disabled, focus, hover, and cancellation states as relevant.
- Prefer model/view or virtualized structures for large lists; avoid a widget per row without a measured need.
- Ensure useful click targets, keyboard access, visible focus, readable contrast, and safe default actions.
- Support approved themes, locale, high-DPI scaling, and minimum window sizes.

## Acceptance

Run interaction tests and provide a rendered preview or screenshots at representative sizes. Functional and design acceptance are separate when the visual change is material.
