---
name: Working environment preference
description: User prefers Claude Code desktop/web app over terminal for all work
type: feedback
---

User prefers to work exclusively from Claude Code (desktop or web app), not from the terminal directly.

**Why:** Comfort and workflow preference — terminal-only operations that break the session are disruptive.

**How to apply:** When tasks require shell operations (builds, renames, etc.), either do them via Bash tool inside the session, or warn the user clearly before suggesting they run something in a separate terminal. Never assume the user will switch to terminal as a primary workflow.
