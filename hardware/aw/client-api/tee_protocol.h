/*
 * Copyright (c) 2013 NVIDIA Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __TEE_PROTOCOL_H__
#define __TEE_PROTOCOL_H__

#include "tee_types.h"


#define TE_IOCTL_MAGIC_NUMBER ('t')
#define TE_IOCTL_OPEN_CLIENT_SESSION \
	_IOWR(TE_IOCTL_MAGIC_NUMBER, 0x10, union te_cmd)
#define TE_IOCTL_CLOSE_CLIENT_SESSION \
	_IOWR(TE_IOCTL_MAGIC_NUMBER, 0x11, union te_cmd)
#define TE_IOCTL_LAUNCH_OPERATION \
	_IOWR(TE_IOCTL_MAGIC_NUMBER, 0x14, union te_cmd)
#define TE_IOCTL_SHARED_MEM_FREE_REQUEST \
	_IOWR(TE_IOCTL_MAGIC_NUMBER, 0x15, union te_cmd)

#define TE_IOCTL_MIN_NR	_IOC_NR(TE_IOCTL_OPEN_CLIENT_SESSION)
#define TE_IOCTL_MAX_NR	_IOC_NR(TE_IOCTL_FILE_REQ_COMPLETE)

/* shared buffer is 2 pages: 1st are requests, 2nd are params */
#define TE_CMD_DESC_MAX	(PAGE_SIZE / sizeof(struct te_request))
#define TE_PARAM_MAX	(PAGE_SIZE / sizeof(struct te_oper_param))

#define MAX_EXT_SMC_ARGS	12



enum {
	TE_PARAM_TYPE_NONE	= 0,
	TE_PARAM_TYPE_INT_RO    = 1,
	TE_PARAM_TYPE_INT_RW    = 2,
	TE_PARAM_TYPE_MEM_RO    = 3,
	TE_PARAM_TYPE_MEM_RW    = 4,
};

struct te_oper_param {
	uint32_t index;
	uint32_t type;
	union {
		struct {
			uint32_t val_a;
			uint32_t val_b;
		} Int;
		struct {
			void  *base;
			void  *phys;
			uint32_t len;
		} Mem;
	} u;
	void *next_ptr_user;
};

struct te_operation {
	uint32_t command;
	struct te_oper_param *list_head;
	/* Maintain a pointer to tail of list to easily add new param node */
	struct te_oper_param *list_tail;
	uint32_t list_count;
	uint32_t status;
	uint32_t iterface_side;
};

struct te_service_id {
	uint32_t time_low;
	uint16_t time_mid;
	uint16_t time_hi_and_version;
	uint8_t clock_seq_and_node[8];
};
struct te_answer {
	uint32_t	result;
	uint32_t	session_id;
	uint32_t	result_origin;
};

/*
 * OpenSession
 */
struct te_opensession {
	struct te_service_id dest_uuid;
	struct te_operation operation;
	struct te_answer answer;
};

/*
 * CloseSession
 */
struct te_closesession {
	struct te_service_id service_id;
	struct te_answer	answer;
	uint32_t	session_id;
};

/*
 * LaunchOperation
 */
struct te_launchop {
	struct te_service_id service_id;
	uint32_t		session_id;
	struct te_operation	operation;
	struct te_answer		answer;
};

union te_cmd {
	struct te_opensession	opensession;
	struct te_closesession	closesession;
	struct te_launchop	launchop;
};

struct te_request {
	uint32_t		type;
	uint32_t		session_id;
	uint32_t		command_id;
	struct te_oper_param	*params;
	uint32_t		params_size;
	uint32_t		dest_uuid[4];
	uint32_t		result;
	uint32_t		result_origin;
};

#endif
