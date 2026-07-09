**语言:** [English](README.md) | 简体中文

# agentrt-linux 测试（AirymaxOS Tests）

[![Version](https://img.shields.io/badge/version-0.1.1-5a6b7e)](https://atomgit.com/openairymax/airymaxos-tests)
[![License](https://img.shields.io/badge/license-AGPL--3.0+Apache--2.0-4a90d9)](LICENSE)

> [agentrt-linux（AirymaxOS）](https://atomgit.com/openairymax/agentrt-linux)（智能体操作系统）的测试子系统。
> 由 [agentrt-linux](https://atomgit.com/openairymax/agentrt-linux) 管理仓聚合的叶子仓之一。
> 覆盖所有 agentrt-linux 子系统的全模块测试。

---

## 概述

**agentrt-linux 测试（AirymaxOS Tests）**（`airymaxos-tests`）是 agentrt-linux（AirymaxOS）（智能体操作系统）的测试框架子系统。它提供单元测试、集成测试、形式化验证（seL4 风格）、Soak 测试与混沌测试，覆盖每一个 agentrt-linux 子系统——是整个操作系统的质量门。

在 agentrt-linux 0.1.1 中，本仓库为**文档体系完成**，仅包含设计文档、参考发行版规范及架构草案。实际的内核与 OS 开发在 1.0.1 版本进行。

### 核心技术

- **单元测试** 覆盖内核、服务、安全、内存、认知与系统组件
- **集成测试** 对齐 Euler 标准测试方法论
- **形式化验证**（seL4 风格）覆盖安全与内存关键路径
- **Soak 测试** 用于长时运行稳定性与资源泄漏检测
- **混沌测试** 用于故障注入、故障恢复与韧性验证

### 与 Airymax 全模块测试的关系

agentrt-linux 测试框架复用了 Airymax 运行时平台各模块的测试约定、测试桩与夹具。测试分类法、集成套件与形式化验证测试桩在用户态运行时（agentrt）与 OS 级测试框架（agentrt-linux）之间共享，确保架构同源、无适配层。

## 仓库结构（0.1.1（文档体系完成））

```
airymaxos-tests/
├── README.md           # 本文件（英文）
├── README_zh.md        # 中文翻译
├── LICENSE             # AGPL-3.0 + Apache-2.0 双许可证
├── NOTICE              # 版权、商标与第三方声明
└── .gitignore
```

设计文档与 参考发行版规范维护在伞仓的 `docs/AirymaxAgentOS/` 目录。

## 上下游依赖

### 上游

- **所有 agentrt-linux 子系统** — 内核、服务、安全、内存、认知与系统提供被测代码
- **Euler 标准测试框架** — 参考测试方法论与集成测试套件格式
- **seL4 验证方法论** — 形式化验证测试桩的参考

### 下游

- **CI/CD pipeline** — 将测试套件作为每次变更的质量门
- **QA team** — 运行 Soak 与混沌套件进行发布签收

## 分支策略

本叶子仓在 **`feature/official-hubs-01`** 分支上开发。聚合管理仓 `agentrt-linux` 保持在 `main` 分支。

## 许可证

采用 **AGPL v3 + Apache 2.0** 双许可证（SPDX：`AGPL-3.0-or-later OR Apache-2.0`）。完整文本见 [LICENSE](LICENSE)。

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.
