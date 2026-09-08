---
paths:
  - "src/ui/**/*"
  - "src/components/**/*"
  - "app/**/*.{css,html,tsx,jsx,vue,svelte}"
---

# UI rules

- Present the user flow and visible difference before a material redesign.
- Preserve keyboard access, focus visibility, readable contrast, scaling, and important loading/empty/error states.
- Keep long-running work away from the interactive thread.
- Verify representative rendered states; functional tests alone do not establish visual acceptance.
