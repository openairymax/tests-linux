**Language:** English | [简体中文](README_zh.md)

# AirymaxOS Tests

[![Version](https://img.shields.io/badge/version-0.1.1-5a6b7e)](https://atomgit.com/openairymax/airymaxos-tests)
[![License](https://img.shields.io/badge/license-AGPL--3.0+Apache--2.0-4a90d9)](LICENSE)

> Tests subsystem of [AirymaxOS](https://atomgit.com/openairymax/agentrt-linux) — the AI Agent Operating System.
> One of the leaf repositories aggregated by the [agentrt-linux](https://atomgit.com/openairymax/agentrt-linux) management repo.
> Covers full-module testing for every AirymaxOS subsystem.

---

## Overview

The **AirymaxOS Tests** (`airymaxos-tests`) is the testing framework subsystem of AirymaxOS, the AI Agent Operating System. It provides unit tests, integration tests (mugen-style), formal verification (seL4-style), soak tests, and chaos tests across every AirymaxOS subsystem — the quality gate for the whole operating system.

In Airymax 0.1.1, this repository is a **placeholder** containing only design documents, openEuler reference specifications, and architectural drafts. Actual kernel and OS development takes place in version 1.0.1.

### Core Technologies

- **Unit testing** for kernel, services, security, memory, cognition and system components
- **Integration testing** (mugen-style) aligned with the openEuler test methodology
- **Formal verification** (seL4-style) for security- and memory-critical paths
- **Soak testing** for long-running stability and resource-leak detection
- **Chaos testing** for fault injection, failure recovery and resilience validation

### Relationship with Airymax full-module testing

The AirymaxOS Tests reuses the testing conventions, harnesses and fixtures from across the Airymax runtime platform modules. The test taxonomy, mugen-style integration suites and formal-verification harnesses are shared between the user-space runtime (agentrt) and the OS-level test framework (AirymaxOS), ensuring architectural homology with no adaptation layer.

## Repository Structure (0.1.1 Placeholder)

```
airymaxos-tests/
├── README.md           # This file (English)
├── README_zh.md        # Chinese translation
├── LICENSE             # AGPL-3.0 + Apache-2.0 dual license
├── NOTICE              # Copyright, trademark and third-party notices
└── .gitignore
```

Design documents and openEuler reference specifications are maintained in the `docs/AirymaxAgentOS/` directory of the umbrella repository.

## Upstream & Downstream Dependencies

### Upstream

- **All AirymaxOS subsystems** — kernel, services, security, memory, cognition and system provide the code under test
- **openEuler mugen** — reference test methodology and integration test suite format
- **seL4 verification methodology** — reference for the formal-verification harnesses

### Downstream

- **CI/CD pipeline** — consumes the test suites as the quality gate for every change
- **QA team** — runs soak and chaos suites for release sign-off

## Branch Strategy

This leaf repository is developed on **`feature/official-hubs-01`**. The aggregating `agentrt-linux` management repo stays on `main`.

## License

Dual-licensed under **AGPL v3 + Apache 2.0** (SPDX: `AGPL-3.0-or-later OR Apache-2.0`). See [LICENSE](LICENSE) for the full text.

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.
