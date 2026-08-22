**Language:** English | [简体中文](README_zh.md)

# tests-linux — agent-linux (AirymaxOS) Tests

[![Version](https://img.shields.io/badge/version-0.1.1-5a6b7e)](https://atomgit.com/openairymax/tests-linux)
[![License](https://img.shields.io/badge/license-AGPL--3.0+Apache--2.0-4a90d9)](LICENSE)

> Tests subsystem of [agent-linux (AirymaxOS)](https://atomgit.com/openairymax/agent-linux) — the AI Agent Operating System.
> One of the 8 leaf repositories aggregated by the [agent-linux](https://atomgit.com/openairymax/agent-linux) management repo.
> Covers full-module testing for every agent-linux subsystem.

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.

---

## Positioning

The **tests-linux** leaf repository is the testing framework subsystem of
agent-linux (AirymaxOS). It aggregates the KUnit, kselftest, LTP and PCTS test
suites and adds integration, formal verification (seL4-style), soak and chaos
harnesses across every agent-linux subsystem — the quality gate for the whole
operating system.

## Core Responsibilities

- **KUnit** — in-kernel unit tests run on UML (rules OS-TEST-001~012); TAP case-count monotonicity enforced (OS-TEST-012).
- **kselftest** — kernel selftests run on QEMU (rules OS-TEST-013~022).
- **LTP (Linux Test Project)** — POSIX/system-call conformance and regression coverage.
- **PCTS** — compatibility test suite aligned with the reference distribution standard.
- **Integration testing** aligned with Euler-standard test methodology.
- **Formal verification** (seL4-style) for security- and memory-critical paths.
- **Soak testing** for long-running stability and resource-leak detection (72h).
- **Chaos testing** for fault injection (CPU hotplug / mem hotremove / I/O error / net partition), failure recovery and resilience validation.

## Relationship with Airymax full-module testing

The tests-linux leaf repo reuses the testing conventions, harnesses and fixtures
from across the Airymax runtime platform modules. The test taxonomy, integration
suites and formal-verification harnesses are shared between the user-space
runtime (`agentrt`) and the OS-level test framework, ensuring architectural
homology with no adaptation layer.

## Document & File List

```
tests-linux/
├── README.md           # This file (English)
├── README_zh.md        # Chinese translation
├── LICENSE             # AGPL-3.0 + Apache-2.0 dual license
├── NOTICE              # Copyright, trademark and third-party notices
├── .gitignore
└── .github/
    └── README.md       # GitHub automation for this leaf repo
```

Design documents and reference distribution specifications are maintained in the
`docs/AirymaxOS/` directory of the umbrella documentation repository.

## CI Status

The test suites are consumed by management-repository workflows (each ≤ 2 jobs):

| Workflow | Jobs | Consumes tests-linux via |
|----------|------|----------------------------|
| `ci-kernel.yml` | `kernel-build` + `kernel-verify` | `kernel-verify` runs KUnit on UML (OS-TEST-001~012) and kselftest on QEMU (OS-TEST-013~022); TAP parsed by `tools/kunit-tap-diff.py` and `tools/parse-tap.py` |
| `nightly.yml` | `nightly-test-suite` (seL4-style formal verification + 72h soak + chaos) + `nightly-revert-or-budget` (auto-revert or 60-min CI budget check, OS-STD-TEST-011) | Nightly cron |
| `mgmt-orchestrator.yml` | `file-integrity` + `orchestrate-leaf-ci` | Verifies the `tests-linux/` submodule dir; aggregates this repo's CI status |
| `release.yml` | `build-and-sign` (SBOM scan of `tests-linux/`) + `publish-release` | Release tag |

Language-level CI (test harness lint, TAP parsers, fixture validation) is
delegated to this leaf repository's own `.github/workflows/`.

## Development Guide

- **Branch**: `feature/official-hubs-01` (the management repo stays on `main`).
- **DCO**: every commit must be `Signed-off-by` (`git commit -s`).
- **Commit prefix**: `tests-linux:`.
- **Code style**: Python — PEP 8 (`ruff`); shell — `shellcheck`; C test code — tab-8, 80 cols.
- **Adding tests**: every OS-STD-TEST-* rule change must add or update a corresponding test case; reference the rule ID in the test.
- **TAP output**: all suites must emit TAP for machine parsing by the management CI tools.

## Upstream & Downstream

- **Upstream** — all agent-linux subsystems (`kernel`, `services`, `security`, `memory`, `cognition`, `cloudnative`, `system`) provide the code under test; Euler-standard test framework; seL4 verification methodology; KUnit / kselftest / LTP / PCTS upstreams.
- **Downstream** — CI/CD pipeline (consumes the suites as the quality gate for every change); QA (runs soak and chaos suites for release sign-off).

## License

Dual-licensed under **AGPL v3 + Apache 2.0** (SPDX: `AGPL-3.0-or-later OR Apache-2.0`).
See [LICENSE](LICENSE) for the full text.

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.
