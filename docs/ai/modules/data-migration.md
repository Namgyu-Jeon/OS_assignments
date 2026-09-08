# Data Migration Module

- Test a fresh database, upgrade from every supported prior schema, and migration re-execution.
- Preserve unrelated and nullable legacy records.
- Run tests only against temporary copies or fixtures, never a real user database.
- Use transactions or an equivalent safe boundary; failure must not delete or overwrite the only copy.
- Define backup, rollback, forward-fix, disk-space, interruption, and version-skew behavior.
- Restored display state must not silently re-run historical operations.
