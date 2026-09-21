# Validation results

The runner creates a new UTC timestamped folder on every invocation. Commit the
summary JSON/Markdown, stage logs, tool versions, benchmark JSON when available,
and completed manual checks. Preserve failed runs as well as successful reruns.

Keep a `HANDOFF.md` here describing the latest result, fix commits, remaining
failures and the next command for the Linux session. Scratch directories are
ignored. Do not add compiler binaries, object files, PDBs, SDKs or dump files.
Review logs for credentials or unrelated workplace data before committing them;
redact only that data, preserving commands, diagnostics and test outcomes.
