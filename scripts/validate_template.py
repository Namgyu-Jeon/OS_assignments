from __future__ import annotations

import re
import sys
from pathlib import Path
from urllib.parse import unquote


ROOT = Path(__file__).resolve().parents[1]
REQUIRED = {
    "AGENTS.md",
    "CLAUDE.md",
    "PROJECT_INIT.md",
    "README.md",
    "docs/PRD.md",
    "docs/DEVELOPMENT_GUIDE.md",
    "docs/ai/CURRENT_STATE.md",
    "docs/ai/APPROVAL_MODEL.md",
    "docs/ai/QUALITY_GATES.md",
}
FORBIDDEN_SUFFIXES = {
    ".7z",
    ".db",
    ".dmg",
    ".exe",
    ".gz",
    ".jpeg",
    ".jpg",
    ".mov",
    ".mp3",
    ".mp4",
    ".png",
    ".sqlite",
    ".sqlite3",
    ".tar",
    ".zip",
}
FORBIDDEN_PARTS = {
    ".local-tools",
    ".pytest_cache",
    ".ruff_cache",
    ".venv",
    "__pycache__",
    "build",
    "dist",
    "node_modules",
}
SECRET_PATTERNS = {
    "GitHub token": re.compile(r"\b(?:ghp|github_pat)_[A-Za-z0-9_]{20,}\b"),
    "AWS access key": re.compile(r"\bAKIA[0-9A-Z]{16}\b"),
    "private key": re.compile(r"-----BEGIN (?:RSA |EC |OPENSSH )?PRIVATE KEY-----"),
    "personal Windows path": re.compile(r"[A-Za-z]:\\Users\\[^\\\s]+", re.IGNORECASE),
    "personal macOS path": re.compile("/" + r"Users/[^/\s]+"),
}
MARKDOWN_LINK = re.compile(r"!?\[[^\]]*\]\(([^)]+)\)")


def text_files() -> list[Path]:
    suffixes = {".css", ".html", ".js", ".json", ".md", ".py", ".sh", ".txt", ".yml", ".yaml"}
    return [
        path
        for path in ROOT.rglob("*")
        if path.is_file()
        and ".git" not in path.relative_to(ROOT).parts
        and path.suffix.lower() in suffixes
    ]


def internal_target(source: Path, raw_target: str) -> Path | None:
    target = raw_target.strip().split("#", 1)[0]
    if not target or target.startswith(("http://", "https://", "mailto:", "#")):
        return None
    if " " in target and not target.startswith("<"):
        target = target.split(" ", 1)[0]
    target = unquote(target.strip("<>"))
    return (source.parent / target).resolve()


def main() -> int:
    errors: list[str] = []

    for relative in sorted(REQUIRED):
        if not (ROOT / relative).is_file():
            errors.append(f"missing required file: {relative}")

    for path in ROOT.rglob("*"):
        if ".git" in path.relative_to(ROOT).parts:
            continue
        if any(part in FORBIDDEN_PARTS for part in path.relative_to(ROOT).parts):
            errors.append(f"forbidden generated path: {path.relative_to(ROOT)}")
        if path.is_file() and path.suffix.lower() in FORBIDDEN_SUFFIXES:
            errors.append(f"forbidden binary/archive: {path.relative_to(ROOT)}")

    for relative in ("AGENTS.md", "CLAUDE.md"):
        path = ROOT / relative
        if path.exists() and len(path.read_text(encoding="utf-8").splitlines()) > 200:
            errors.append(f"root instruction exceeds 200 lines: {relative}")

    claude = ROOT / "CLAUDE.md"
    if claude.exists() and not claude.read_text(encoding="utf-8").lstrip().startswith("@AGENTS.md"):
        errors.append("CLAUDE.md must import AGENTS.md first")

    for rule in (ROOT / ".claude" / "rules").glob("*.md"):
        if not rule.read_text(encoding="utf-8").startswith("---\npaths:"):
            errors.append(f"Claude rule is not path-scoped: {rule.relative_to(ROOT)}")

    for path in text_files():
        raw = path.read_bytes()
        if raw.startswith(b"\xef\xbb\xbf"):
            errors.append(f"UTF-8 BOM is not allowed: {path.relative_to(ROOT)}")
        if b"\r\n" in raw:
            errors.append(f"CRLF found: {path.relative_to(ROOT)}")
        try:
            content = raw.decode("utf-8")
        except UnicodeDecodeError:
            errors.append(f"not valid UTF-8: {path.relative_to(ROOT)}")
            continue
        for label, pattern in SECRET_PATTERNS.items():
            if pattern.search(content):
                errors.append(f"{label} pattern in {path.relative_to(ROOT)}")
        if path.suffix.lower() == ".md":
            for match in MARKDOWN_LINK.finditer(content):
                target = internal_target(path, match.group(1))
                if target is not None and not target.exists():
                    errors.append(
                        f"broken link in {path.relative_to(ROOT)}: {match.group(1)}"
                    )

    if errors:
        print("Template validation failed:")
        for error in sorted(set(errors)):
            print(f"- {error}")
        return 1

    print(f"Template validation passed ({len(text_files())} text files checked).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
