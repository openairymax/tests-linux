// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.
 *
 * test_superv_uapi.c — 验证 [SC] superv.h 的故障通道与裁决契约
 *
 * macro_d 通过 /dev/airy_superv 收故障、通过 io_uring 下发裁决；内核侧按
 * 同一份头编译。设备路径、事件定长布局、裁决枚举与 ioctl 编码一旦分叉，
 * 守护要么打不开通道，要么把明细读成错位字段。这里把契约钉在用户态。
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <linux/airymax/error.h>
#include <linux/airymax/ipc.h>
#include <linux/airymax/superv.h>

#include "airy_test.h"

static void test_superv_device_abi(void)
{
	AIRY_TEST(AIRY_SUPERV_ABI == 1, "AIRY_SUPERV_ABI == 1");
	AIRY_TEST(strcmp(AIRY_SUPERV_DEV_PATH, "/dev/airy_superv") == 0,
		  "AIRY_SUPERV_DEV_PATH == /dev/airy_superv");
}

static void test_superv_command_layout(void)
{
	AIRY_TEST(AIRY_SUPERV_CMD_SIZE == 16, "AIRY_SUPERV_CMD_SIZE == 16");
	AIRY_TEST(sizeof(struct airy_superv_cmd) == AIRY_SUPERV_CMD_SIZE,
		  "sizeof(airy_superv_cmd) == AIRY_SUPERV_CMD_SIZE");
	AIRY_TEST(offsetof(struct airy_superv_cmd, opcode) == 0,
		  "cmd.opcode offset == 0");
	AIRY_TEST(offsetof(struct airy_superv_cmd, agent_id) == 4,
		  "cmd.agent_id offset == 4");
	AIRY_TEST(offsetof(struct airy_superv_cmd, adjud) == 8,
		  "cmd.adjud offset == 8");
	AIRY_TEST(offsetof(struct airy_superv_cmd, reason) == 12,
		  "cmd.reason offset == 12");
}

static void test_superv_opcodes(void)
{
	AIRY_TEST(AIRY_IPC_OP_ADJUDICATE == 0x0020,
		  "AIRY_IPC_OP_ADJUDICATE == 0x0020");
	AIRY_TEST(AIRY_IPC_OP_UNFREEZE == 0x0021,
		  "AIRY_IPC_OP_UNFREEZE == 0x0021");
	AIRY_TEST(AIRY_IPC_OP_HEARTBEAT == 0x0022,
		  "AIRY_IPC_OP_HEARTBEAT == 0x0022");

	/* 监督面必须留在 agent 可达的 [SC] IPC 面之外 */
	AIRY_TEST(AIRY_IPC_OP_ADJUDICATE != AIRY_IPC_OP_SEND,
		  "ADJUDICATE 与 SEND 不重合");
	AIRY_TEST(AIRY_IPC_OP_UNFREEZE != AIRY_IPC_OP_FREEZE,
		  "UNFREEZE 与 FREEZE 不重合");
	AIRY_TEST(AIRY_IPC_OP_HEARTBEAT != AIRY_IPC_OP_CAP_REQUEST,
		  "HEARTBEAT 与 CAP_REQUEST 不重合");
	AIRY_TEST(AIRY_IPC_OP_HEARTBEAT != AIRY_IPC_OP_CAP_RESPONSE,
		  "HEARTBEAT 与 CAP_RESPONSE 不重合");
}

static void test_adjud_actions(void)
{
	AIRY_TEST(AIRY_ADJUD_WARN == 0, "AIRY_ADJUD_WARN == 0");
	AIRY_TEST(AIRY_ADJUD_DEGRADE == 1, "AIRY_ADJUD_DEGRADE == 1");
	AIRY_TEST(AIRY_ADJUD_PAUSE == 2, "AIRY_ADJUD_PAUSE == 2");
	AIRY_TEST(AIRY_ADJUD_TERMINATE == 3, "AIRY_ADJUD_TERMINATE == 3");
	AIRY_TEST(AIRY_ADJUD_RESUME == 4, "AIRY_ADJUD_RESUME == 4");
	AIRY_TEST(AIRY_ADJUD_NR == 5, "AIRY_ADJUD_NR == 5");
}

static void test_fault_event_layout(void)
{
	AIRY_TEST(AIRY_FAULT_EVENT_SIZE == 24, "AIRY_FAULT_EVENT_SIZE == 24");
	AIRY_TEST(sizeof(struct airy_fault_event) == AIRY_FAULT_EVENT_SIZE,
		  "sizeof(airy_fault_event) == AIRY_FAULT_EVENT_SIZE");
	AIRY_TEST(offsetof(struct airy_fault_event, ring_id) == 0,
		  "event.ring_id offset == 0");
	AIRY_TEST(offsetof(struct airy_fault_event, fault_code) == 4,
		  "event.fault_code offset == 4");
	AIRY_TEST(offsetof(struct airy_fault_event, timestamp_ns) == 8,
		  "event.timestamp_ns offset == 8");
	AIRY_TEST(offsetof(struct airy_fault_event, agent_id) == 16,
		  "event.agent_id offset == 16");
	AIRY_TEST(offsetof(struct airy_fault_event, pad) == 20,
		  "event.pad offset == 20");
}

static void test_superv_ioctl_values(void)
{
	AIRY_TEST(AIRY_SUPERV_IOC_BIND_EFD == 0x40044160u,
		  "AIRY_SUPERV_IOC_BIND_EFD == 0x40044160");
	AIRY_TEST(AIRY_SUPERV_IOC_UNBIND == 0x4161u,
		  "AIRY_SUPERV_IOC_UNBIND == 0x4161");
	AIRY_TEST(AIRY_SUPERV_IOC_LOST == 0x80044162u,
		  "AIRY_SUPERV_IOC_LOST == 0x80044162");
}

static void test_superv_command_roundtrip(void)
{
	struct airy_superv_cmd cmd;
	struct airy_fault_event ev;

	memset(&cmd, 0, sizeof(cmd));
	cmd.opcode = AIRY_IPC_OP_ADJUDICATE;
	cmd.agent_id = 7;
	cmd.adjud = AIRY_ADJUD_PAUSE;
	cmd.reason = AIRY_FAULT_RING_CORRUPT;

	memset(&ev, 0, sizeof(ev));
	ev.ring_id = 3;
	ev.fault_code = AIRY_FAULT_RING_CORRUPT;
	ev.timestamp_ns = 1784562248123456789ULL;
	ev.agent_id = 7;

	AIRY_TEST(cmd.opcode == AIRY_IPC_OP_ADJUDICATE, "命令 opcode 可回读");
	AIRY_TEST(cmd.adjud == AIRY_ADJUD_PAUSE, "命令 adjud 可回读");
	AIRY_TEST(cmd.reason == AIRY_FAULT_RING_CORRUPT, "命令 reason 可回读");
	AIRY_TEST(ev.fault_code == cmd.reason, "事件故障码与裁决依据一致");
	AIRY_TEST(ev.agent_id == cmd.agent_id, "事件与命令指向同一 agent");
}

int main(void)
{
	printf("=== AirymaxOS [SC] 监督故障通道契约测试 ===\n");

	test_superv_device_abi();
	test_superv_command_layout();
	test_superv_opcodes();
	test_adjud_actions();
	test_fault_event_layout();
	test_superv_ioctl_values();
	test_superv_command_roundtrip();

	airy_test_report("test_superv_uapi");
	return EXIT_SUCCESS;
}
