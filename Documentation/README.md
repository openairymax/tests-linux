# tests-linux 子仓文档索引

> **子仓定位**：AirymaxOS 测试套件 —— unit + integration + fuzz + perf + CI。

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.

License: GPL-2.0-only

---

## 文档清单

### 已有文档

| 文档 | 说明 |
|------|------|
| [unit-tests.md](unit-tests.md) | 单元测试设计 |
| [integration.md](integration.md) | 集成测试设计 |

### 待编写文档（0.1.1 → 1.0.1）

| 文档 | 说明 | 计划版本 |
|------|------|---------|
| `fuzz.md` | 模糊测试设计 | 1.0.1 |
| `perf.md` | 性能测试与基准设计 | 1.0.1 |
| `ci.md` | CI 流水线设计 | 1.0.1 |
| `coverage.md` | 代码覆盖率统计 | 1.0.1 |

## 测试清单（0.1.1）

| 测试 | 类型 | 入口 | 状态 |
|------|------|------|------|
| `test_ipc_magic` | unit | `unit/test_ipc_magic.c` | ✅ 可编译 |

## 测试清单（1.0.1 计划）

| 测试 | 类型 | 入口 | 计划 |
|------|------|------|------|
| `test_error_codes` | unit | `unit/test_error_codes.c` | 1.0.1 |
| `test_cognition_types` | unit | `unit/test_cognition_types.c` | 1.0.1 |
| `test_memory_types` | unit | `unit/test_memory_types.c` | 1.0.1 |
| `test_lsm_types` | unit | `unit/test_lsm_types.c` | 1.0.1 |
| `test_badge_layout` | unit | `unit/test_badge_layout.c` | 1.0.1 |
| `itest_lsm_register` | integration | `integration/itest_lsm_register.c` | 1.0.1 |
| `itest_clt_kthread` | integration | `integration/itest_clt_kthread.c` | 1.0.1 |
| `fuzz_ipc_parser` | fuzz | `fuzz/fuzz_ipc_parser.c` | 1.0.1 |
| `bench_mr_l1_alloc` | perf | `perf/bench_mr_l1_alloc.c` | 1.0.1 |

## 相关文档

- [07-directory-structure.md](../../../docs/AirymaxOS/10-architecture/07-directory-structure.md) §4.8 — tests-linux 子仓完整目录结构
- [ipc.h](../../kernel/include/uapi/linux/airymax/ipc.h) — [SC] A-IPC 消息头契约
- 顶层 [CONTRIBUTING.md](../CONTRIBUTING.md) — tests-linux 子仓贡献指南
