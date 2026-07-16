# `.github/` — tests-linux Leaf Repository Automation

> GitHub automation directory for the **tests-linux** leaf repository of
> [agentrt-linux (AirymaxOS)](https://atomgit.com/openairymax/agentrt-linux).

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.

---

## Positioning

This directory hosts tests-linux-specific GitHub automation. The test suites
(KUnit, kselftest, LTP, PCTS, formal/soak/chaos) are invoked by the management
repository's `ci-kernel.yml` and `nightly.yml` workflows; this leaf repository
hosts language-level CI for the test harnesses, TAP parsers and fixtures.

## Directory Contents

```
tests-linux/.github/
└── README.md    # This file
```

## Applicable Management-Repository Workflows

| Workflow | Jobs | Relevance to tests-linux |
|----------|------|---------------------------|
| `ci-kernel.yml` | `kernel-build` + `kernel-verify` | `kernel-verify` runs KUnit on UML (OS-TEST-001~012) and kselftest on QEMU (OS-TEST-013~022); TAP output parsed by `tools/kunit-tap-diff.py` and `tools/parse-tap.py` |
| `nightly.yml` | `nightly-test-suite` + `nightly-revert-or-budget` | seL4-style formal verification, 72h soak (`soak-runner.py`), chaos (`chaos-runner.py`); auto-revert via `auto-bisect.py`; 60-min CI budget check (OS-STD-TEST-011) |
| `mgmt-orchestrator.yml` | `file-integrity` + `orchestrate-leaf-ci` | Verifies the `tests-linux/` submodule dir exists; aggregates this repo's CI status |
| `release.yml` | `build-and-sign` + `publish-release` | `syft tests-linux/` SBOM fragment |

## Development Guide

- Add leaf-local workflows for TAP parser unit tests, fixture validation, and LTP /
  PCTS runner smoke checks; keep each workflow ≤ 2 jobs.
- All test suites must emit TAP for machine parsing by the management CI tools.
- Every OS-STD-TEST-* rule change must add or update a corresponding test case.
- Test harness code: Python (PEP 8 / `ruff`), shell (`shellcheck`), C test code
  (tab-8, 80 cols).

## License

Dual-licensed under **AGPL v3 + Apache 2.0** (SPDX: `AGPL-3.0-or-later OR Apache-2.0`).
See the repository root [LICENSE](../LICENSE) and [NOTICE](../NOTICE).

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.
