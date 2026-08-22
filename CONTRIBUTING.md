# 贡献指南 - tests-linux 子仓（测试套件）

> **治理依据**：[`docs/AirymaxOS/50-engineering-standards/07-maintainers-and-governance.md`](https://github.com/openairymax/docs/blob/main/AirymaxOS/50-engineering-standards/07-maintainers-and-governance.md)
> **开发流程**：[`docs/AirymaxOS/50-engineering-standards/05-development-process.md`](https://github.com/openairymax/docs/blob/main/AirymaxOS/50-engineering-standards/05-development-process.md)

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.

License: GPL-2.0-only

---

## 1. 子仓职责

tests-linux 子仓负责 AirymaxOS 的全部测试，包括：

- **unit**（`unit/`）：单元测试（[SC] 契约校验、纯 C 断言）
- **integration**（`integration/`）：集成测试（内核模块 + 用户态）
- **fuzz**（`fuzz/`）：模糊测试（libFuzzer）
- **perf**（`perf/`）：性能测试与基准
- **ci**（`ci/`）：CI 流水线脚本

### IRON-9 主层

- `[IND]`：全部测试均为 agent-linux 专属实现
- `[SC]`：测试通过 [SC] 头文件验证契约一致性

## 2. 开发环境要求

- **C 标准**：GNU C11（用户态测试）
- **编译器**：gcc ≥ 11 或 clang ≥ 14
- **构建系统**：CMake ≥ 3.20（含 `enable_testing()`）
- **测试运行器**：CTest（CMake 内置）
- **内核头**：通过 `-I../kernel/include` 引用 [SC] 头文件

### [SC] 头文件引用约束（OS-IRON-014）

- [SC] 头文件**唯一物理宿主**：`../kernel/include/uapi/linux/airymax/`
- 本子仓通过 `-I` 引用，**禁止物理副本**
- CMakeLists.txt 配置：
  ```cmake
  include_directories(${CMAKE_SOURCE_DIR}/../kernel/include)
  include_directories(${CMAKE_SOURCE_DIR}/../kernel/include/uapi/linux)
  ```

## 3. 构建与运行

### 3.1 构建并运行全部测试

```bash
cd tests-linux
mkdir build && cd build
cmake ..
make
ctest --output-on-failure
```

### 3.2 仅运行单个测试

```bash
./unit/test_ipc_magic
```

### 3.3 CTest 输出示例

```
Test project /home/spharx/SpharxWorks/airymaxhub/agent-linux/tests-linux/build
    Start 1: test_ipc_magic
1/1 Test #1: test_ipc_magic ....................   Passed    0.00 sec

100% tests passed, 1 tests passed out of 1
```

## 4. 代码规范

### 4.1 C 代码风格

- **OS-STD-FMT-001**：Tab-8 缩进（由 `.clang-format` 强制）
- **OS-STD-FMT-002**：80 列硬限制
- **GPL-2.0-only**：所有 `.c` / `.h` 文件必须包含 `SPDX-License-Identifier: GPL-2.0-only`
- **版权头**：`Copyright (c) 2025-2026 SPHARX Ltd.`

### 4.2 测试断言风格

- 使用标准 C `<assert.h>` 进行基本断言
- 复杂断言使用自定义宏：
  ```c
  AIRY_TEST_EQ(actual, expected, "描述");
  AIRY_TEST_NE(actual, unexpected, "描述");
  AIRY_TEST_OK(cond, "描述");
  ```

### 4.3 测试命名约定

| 类型 | 命名 | 示例 |
|------|------|------|
| 单元测试 | `test_<scope>.c` | `test_ipc_magic.c` |
| 集成测试 | `itest_<scope>.c` | `itest_lsm_register.c` |
| 模糊测试 | `fuzz_<scope>.c` | `fuzz_ipc_parser.c` |
| 性能测试 | `bench_<scope>.c` | `bench_mr_l1_alloc.c` |

## 5. 提交规范

### 5.1 DCO 签名

```bash
git commit -s
```

### 5.2 提交信息格式

```
tests-linux: 简短描述（≤72 字符）

详细说明 what 和 why，72 字符换行。

Signed-off-by: Your Name <your.email@example.com>
```

### 5.3 子系统前缀

| 前缀 | 范围 |
|------|------|
| `tests-linux:` | tests-linux/ 子仓整体 |
| `tests-linux: unit:` | 单元测试 |
| `tests-linux: itest:` | 集成测试 |
| `tests-linux: fuzz:` | 模糊测试 |
| `tests-linux: bench:` | 性能测试 |

## 6. 分支策略

- **开发分支**：`feature/official-hubs-01`
- PR 目标分支：`feature/official-hubs-01`

## 7. CI 集成

CI 流水线（`.github/workflows/`）将在 PR 时自动执行：

1. CMake 构建（含 `enable_testing()`）
2. CTest 全部测试
3. 覆盖率统计（lcov / gcov）
4. 内存检查（valgrind，仅 unit 测试）

任何测试失败将阻止 PR 合并。

## 8. 审查流程

1. 向 `feature/official-hubs-01` 提交 PR
2. CI 运行：SSoT 校验 + CMake 构建 + CTest
3. 至少一名维护者审批
4. Squash-merge

## 9. 报告问题

- **Bug**：在 [tests-linux issues](https://github.com/openairymax/tests-linux/issues) 提交
- **设计讨论**：使用管理仓的 GitHub Discussions
