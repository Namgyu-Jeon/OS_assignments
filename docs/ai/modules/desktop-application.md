# Desktop Application Module

- Keep analysis, network, subprocess, database, and media work off the GUI thread.
- Represent queued, active, completed, failed, and cancelled lifecycle explicitly when long-running work exists.
- Use platform-standard user data locations; isolate them in automated tests.
- Treat file open, folder open, clipboard, drag-and-drop, and external tools as failure-prone OS boundaries.
- Preserve settings, history, and user files across restart, failed migration, update, and uninstall according to documented policy.
- Verify keyboard access, focus, high-DPI scaling, themes, minimum window size, safe close, and real OS integration manually.
