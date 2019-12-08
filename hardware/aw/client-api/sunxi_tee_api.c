//#define LOG_NDEBUG 0
#define LOG_TAG "sunxi_tee_api"
#include <utils/Log.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

#include "tee_protocol.h"
#include "tee_types.h"
#include "sunxi_tee_api.h"
#include "sw_list.h"

#ifndef ATTRIBUTE_UNUSED
#define ATTRIBUTE_UNUSED __attribute__ ((__unused__))
#endif
#define OTZ_CLIENT_FULL_PATH_DEV_NAME "/dev/te_device"

TEEC_Result TEEC_InitializeContext(const char* name, TEEC_Context* context) {
	int ret = 0;
	char temp_name[256];

	if (context == NULL) {
		ALOGE("TEEC_InitializeContext : Context is null");
		return TEEC_ERROR_BAD_PARAMETERS;
	}
	if (name == NULL) {
		//ALOGE("%s is assigned as default context", OTZ_CLIENT_FULL_PATH_DEV_NAME );
		strcpy(temp_name, OTZ_CLIENT_FULL_PATH_DEV_NAME);
	} else {
		strcpy(temp_name, name);
	}
	ret = open(temp_name, O_RDWR);

	if (ret == -1) {
		ALOGE("TEEC_InitializeContext : device open failed %s",
				strerror(errno));
		context->s_errno = errno;
		return TEEC_ERROR_GENERIC;
	} else {
		context->fd = ret;
		context->session_count = 0;
		context->shared_mem_cnt = 0;
		INIT_LIST_HEAD(&context->shared_mem_list);
	}
	return TEEC_SUCCESS;

}

void TEEC_FinalizeContext(TEEC_Context* context) {
	if (!context) {
		ALOGE("context is NULL");
		return;
	}
	if (context->session_count != 0) {
		ALOGW("warning: pending open sessions %d", context->session_count);
	}

	if (context->shared_mem_cnt != 0) {
		ALOGW("warning: unreleased shared memory blocks %d",
				context->shared_mem_cnt);
	}
	if ((context->session_count == 0) && (context->shared_mem_cnt == 0)) {
		ALOGV("device closed ");
		close(context->fd);
		context->fd = 0;
	}
	return;
}

TEEC_Result TEEC_AllocateSharedMemory(
	TEEC_Context*      context,
	TEEC_SharedMemory* sharedMem)
{
	int mmap_flags;
	if (context == NULL || sharedMem == NULL) {
		ALOGE("TEEC_AllocateSharedMemory : Error Illegal argument");
		return TEEC_ERROR_BAD_PARAMETERS;
	}
	if((sharedMem->size == 0) ||
		((sharedMem->flags != TEEC_MEM_INPUT) &&
		(sharedMem->flags != TEEC_MEM_OUTPUT) &&
		(sharedMem->flags != (TEEC_MEM_INPUT | TEEC_MEM_OUTPUT)))) {

		ALOGE("TEEC_AllocateSharedMemory : Error Illegal argument");
		return TEEC_ERROR_BAD_PARAMETERS;
	}

	sharedMem->buffer = NULL;
	mmap_flags = PROT_READ | PROT_WRITE;
	sharedMem->buffer = mmap(0, sharedMem->size, mmap_flags, MAP_SHARED,
			context->fd, 0);

	if (sharedMem->buffer == MAP_FAILED) {
		ALOGE("TEEC_AllocateSharedMemory - mmap failed");
		sharedMem->s_errno = errno;
		sharedMem->buffer = NULL;
		return TEEC_ERROR_OUT_OF_MEMORY;
	}

	sharedMem->allocated = 1;
	sharedMem->context = context;
	sharedMem->operation_count = 0;
	return TEEC_SUCCESS;

}
void TEEC_ReleaseSharedMemory(TEEC_SharedMemory* sharedMem) {

	struct list *l;
	TEEC_SharedMemory* tempSharedMem;
	int found = 0;
	unsigned int ret;

	if (sharedMem == NULL) {
		return;
	}
	if (sharedMem->operation_count != 0) {
		ALOGE("TEEC_ReleaseSharedMemory - pending operations count %d",
				sharedMem->operation_count);
		return;
	}

	if(sharedMem->allocated) {
		ret = ioctl(sharedMem->context->fd, TE_IOCTL_SHARED_MEM_FREE_REQUEST, sharedMem->buffer);
		if(ret != 0){
			ALOGE("TEEC_ReleaseShaarddMemory - release kernel sharedmemory list failed! return value: %x", ret);
		}
		munmap(sharedMem->buffer, sharedMem->size);
	}

	sharedMem->buffer = NULL;
	sharedMem->size = 0;
	sharedMem->context = NULL;
}

TEEC_Result TEEC_OpenSession (
		TEEC_Context*    context,
		TEEC_Session*    session,
		const TEEC_UUID* destination,
		uint32_t         connectionMethod,
		const void*      connectionData,
		TEEC_Operation* operation ATTRIBUTE_UNUSED,
		uint32_t*        returnOrigin)
{
	int ret,i=0;
	union te_cmd *cmd;
	struct te_answer answer;
	struct te_oper_param *te_param;

	cmd = (union te_cmd *) malloc(sizeof(union te_cmd));
	memset(cmd, 0, sizeof(union te_cmd));

	if ((context == NULL) || (session == NULL) || (destination == NULL)) {
		ALOGE("TEEC_OpenSession : Error Illegal argument");
		return TEEC_ERROR_BAD_PARAMETERS;
	}

	switch (connectionMethod) {
	case TEEC_LOGIN_PUBLIC: {
		if (connectionData != NULL) {
			ALOGE("TEEC_OpenSession : connection method requires empty connection data");
			return TEEC_ERROR_BAD_PARAMETERS;
		}
		break;
	}
	case TEEC_LOGIN_USER:
	case TEEC_LOGIN_APPLICATION:
	case TEEC_LOGIN_USER_APPLICATION: {
		if (connectionData != NULL) {
			ALOGE("TEEC_OpenSession : connection method requires empty connection data");
			return TEEC_ERROR_BAD_PARAMETERS;
		}
		ALOGV("TEEC_OpenSession : connection method is not implemented ");
		return TEEC_ERROR_NOT_IMPLEMENTED;
		break;
	}
	case TEEC_LOGIN_GROUP:
	case TEEC_LOGIN_GROUP_APPLICATION: {
		if (connectionData == NULL) {
			ALOGE("TEEC_OpenSession : connection method requires valid connection data");
			return TEEC_ERROR_BAD_PARAMETERS;
		}
		return TEEC_ERROR_NOT_IMPLEMENTED;
		break;
	}
	}

	/*encode te_cmd*/
	memcpy(&cmd->opensession.dest_uuid, destination, sizeof(cmd->opensession.dest_uuid));
	ret = ioctl(context->fd, TE_IOCTL_OPEN_CLIENT_SESSION, cmd);
	if (ret < 0) {
		if (returnOrigin) {
			*returnOrigin = TEEC_ORIGIN_API;
		}
		context->s_errno = errno;

		ALOGE("TEEC_OpenSession: Session client open request failed");
		if (ret == -ENOMEM) {
			return TEEC_ERROR_OUT_OF_MEMORY;
		}
		if (ret == -EFAULT) {
			return TEEC_ERROR_ACCESS_DENIED;
		}
		if (ret == -EINVAL) {
			return TEEC_ERROR_BAD_PARAMETERS;
		}

		return TEEC_ERROR_GENERIC;
	} else if (ret > 0) {
		if (returnOrigin) {
			*returnOrigin = TEEC_ORIGIN_TRUSTED_APP;
		}
		ALOGE("TEEC_OpenSession: service return error");
		return ret;
	}
	answer.session_id = cmd->opensession.answer.session_id;
	context->session_count++;
	session->operation_cnt = 0;
	session->session_id = answer.session_id;
	memcpy(&session->service_id, destination, sizeof(cmd->opensession.dest_uuid));
	session->device = context;
	free(cmd);
	return TEEC_SUCCESS;
}

void TEEC_CloseSession(TEEC_Session* session) {

	int ret = 0;
	union te_cmd *cmd;
	cmd = (union te_cmd *) malloc(sizeof(union te_cmd));
	memset(cmd, 0, sizeof(union te_cmd));

	if (session == NULL) {
		ALOGE("TEEC_CloseSession: Warning: Session pointer is NULL");
		return;
	}
	if (session->operation_cnt) {
		ALOGE("TEEC_CloseSession: Warning: Pending operations %d",
				session->operation_cnt);
		return;
	}
	cmd->closesession.session_id = session->session_id;
	memcpy(&cmd->closesession.service_id, &session->service_id, sizeof(session->service_id));
	ret = ioctl(session->device->fd, TE_IOCTL_CLOSE_CLIENT_SESSION, cmd);

	if (ret == 0) {
		session->device->session_count--;
		session->device = NULL;
		session->session_id = -1;
	} else {
		ALOGE("TEEC_CloseSession: Session client close request failed");
	}
	free(cmd);
}
TEEC_Result TEEC_InvokeCommand(
		TEEC_Session*     session,
		uint32_t          commandID,
		TEEC_Operation*   operation,
		uint32_t*         returnOrigin)
{
	int ret = TEEC_SUCCESS;
	int rel_ret;
	int i;
	unsigned char inout = 0;
	uint32_t param_types[4], param_count;
	union te_cmd *cmd;
	struct te_oper_param *sunxi_tee_param;
	struct te_oper_param *decode_param, *temp_param;

	cmd = (union te_cmd *) malloc(sizeof(union te_cmd));
	memset(cmd, 0, sizeof(union te_cmd));

	sunxi_tee_param = (struct te_oper_param *)malloc(4 * sizeof(struct te_oper_param));
	memset(sunxi_tee_param,0,4*sizeof(struct te_oper_param));

	if (session == NULL) {
		ALOGE("TEEC_InvokeCommand : Illegal argument");
		return TEEC_ERROR_BAD_PARAMETERS;
	}

	cmd->launchop.session_id = session->session_id ;
	memcpy(&cmd->closesession.service_id, &session->service_id, sizeof(session->service_id));
	cmd->launchop.operation.command = commandID;
	/* Need to support cancellation in future releases */
	if (operation && !operation->started) {
		ALOGE("TEEC_InvokeCommand : cancellation support not yet implemented");
		return TEEC_ERROR_NOT_IMPLEMENTED;
	}

	session->operation_cnt++;
	if (operation->paramTypes != 0) {
		param_types[0] = operation->paramTypes & 0xf;
		param_types[1] = (operation->paramTypes >> 4) & 0xf;
		param_types[2] = (operation->paramTypes >> 8) & 0xf;
		param_types[3] = (operation->paramTypes >> 12) & 0xf;

		for(param_count = 0; param_count < 4; param_count++){
			if((param_types[param_count] == TEEC_VALUE_INPUT) ||
					(param_types[param_count] == TEEC_VALUE_OUTPUT) ||
					(param_types[param_count] == TEEC_VALUE_INOUT)){

				if( (param_types[param_count] == TEEC_VALUE_INPUT)){
					sunxi_tee_param[param_count].type = TE_PARAM_TYPE_INT_RO;

					sunxi_tee_param[param_count].u.Int.val_a = operation->params[param_count].value.a;
					sunxi_tee_param[param_count].u.Int.val_b = operation->params[param_count].value.b;
					sunxi_tee_param[param_count].index = 0xffffffff;
				}
				if((param_types[param_count] == TEEC_VALUE_OUTPUT) ||
						(param_types[param_count] == TEEC_VALUE_INOUT)){

					sunxi_tee_param[param_count].type = TE_PARAM_TYPE_INT_RW;
					sunxi_tee_param[param_count].u.Int.val_a = operation->params[param_count].value.a;
					sunxi_tee_param[param_count].u.Int.val_b = operation->params[param_count].value.b;
					sunxi_tee_param[param_count].index = 0xffffffff;
				}
			}else if( (param_types[param_count] == TEEC_MEMREF_WHOLE) ||
					(param_types[param_count] == TEEC_MEMREF_PARTIAL_INPUT) ||
					(param_types[param_count] == TEEC_MEMREF_PARTIAL_INOUT) ||
					(param_types[param_count] == TEEC_MEMREF_PARTIAL_OUTPUT)) {

				if(!operation->params[param_count].memref.parent) {
					if(returnOrigin){
						*returnOrigin = TEEC_ORIGIN_API;
					}
					ret = TEEC_ERROR_NO_DATA;
					ALOGE("TEEC_InvokeCommand: memory reference parent is NULL");
					break;
				}
				if (!operation->params[param_count].memref.parent->buffer) {
					if (returnOrigin) {
						*returnOrigin = TEEC_ORIGIN_API;
					}
					ret = TEEC_ERROR_NO_DATA;
					ALOGE("TEEC_InvokeCommand: memory reference parent data is NULL");
					break;
				}
				if(param_types[param_count] == TEEC_MEMREF_PARTIAL_INPUT) {
					if(!(operation->params[param_count].memref.parent->flags & TEEC_MEM_INPUT)) {
						if(returnOrigin){
							*returnOrigin = TEEC_ORIGIN_API;
						}
						ret = TEEC_ERROR_BAD_FORMAT;
						ALOGE("TEEC_InvokeCommand: memory reference direction is invalid");
						break;
					}
				}
				if(param_types[param_count] == TEEC_MEMREF_PARTIAL_OUTPUT) {
					if(!(operation->params[param_count].memref.parent->flags & TEEC_MEM_OUTPUT)) {
						if(returnOrigin){
							*returnOrigin = TEEC_ORIGIN_API;
						}
						ret = TEEC_ERROR_BAD_FORMAT;
						ALOGE("TEEC_InvokeCommand: memory reference direction is invalid");
						break;
					}
				}
				if(param_types[param_count] == TEEC_MEMREF_PARTIAL_INOUT) {
					if(!(operation->params[param_count].memref.parent->flags & TEEC_MEM_INPUT)) {
						if(returnOrigin){
							*returnOrigin = TEEC_ORIGIN_API;
						}
						ret = TEEC_ERROR_BAD_FORMAT;
						ALOGE("TEEC_InvokeCommand: memory reference direction is invalid");
						break;
					}
					if(!(operation->params[param_count].memref.parent->flags & TEEC_MEM_OUTPUT)) {
						if(returnOrigin){
							*returnOrigin = TEEC_ORIGIN_API;
						}
						ret = TEEC_ERROR_BAD_FORMAT;
						ALOGE("TEEC_InvokeCommand: memory reference direction is invalid");
						break;
					}
				}
				if((param_types[param_count] == TEEC_MEMREF_PARTIAL_INPUT) ||
						(param_types[param_count] == TEEC_MEMREF_PARTIAL_INOUT) ||
						(param_types[param_count] == TEEC_MEMREF_PARTIAL_OUTPUT)) {
					if((operation->params[param_count].memref.offset + operation->params[param_count].memref.size >
								operation->params[param_count].memref.parent->size) ) {
						if(returnOrigin){
							*returnOrigin = TEEC_ORIGIN_API;
						}
						ret = TEEC_ERROR_EXCESS_DATA;
						ALOGE("TEEC_InvokeCommand:memory reference offset + size is greater than the actual memory size");
						break;
					}
				}
				if (param_types[param_count] == TEEC_MEMREF_PARTIAL_INPUT) {

					sunxi_tee_param[param_count].type = TE_PARAM_TYPE_MEM_RO;
					sunxi_tee_param[param_count].u.Mem.base = (void*) ((uintptr_t)operation->params[param_count].memref.parent->buffer +
							(uint32_t)operation->params[param_count].memref.offset);
					sunxi_tee_param[param_count].u.Mem.len = operation->params[param_count].memref.parent->size;
					sunxi_tee_param[param_count].index = (uintptr_t)operation->params[param_count].memref.parent->buffer;
				}else if((param_types[param_count] == TEEC_MEMREF_PARTIAL_OUTPUT) ||
						(param_types[param_count] == TEEC_MEMREF_PARTIAL_INOUT)){

					sunxi_tee_param[param_count].type = TE_PARAM_TYPE_MEM_RW;
					sunxi_tee_param[param_count].u.Mem.base = (void*) ((uintptr_t)operation->params[param_count].memref.parent->buffer +
							(uint32_t)operation->params[param_count].memref.offset);
					sunxi_tee_param[param_count].u.Mem.len = operation->params[param_count].memref.parent->size;
					sunxi_tee_param[param_count].index = (uintptr_t)operation->params[param_count].memref.parent->buffer;

				}else if((param_types[param_count] == TEEC_MEMREF_WHOLE)){
					if(operation->params[param_count].memref.parent->flags == TEEC_MEM_INPUT){
						sunxi_tee_param[param_count].type = TE_PARAM_TYPE_MEM_RO;
						sunxi_tee_param[param_count].u.Mem.base = (void*) ((uintptr_t)operation->params[param_count].memref.parent->buffer);
						sunxi_tee_param[param_count].u.Mem.len = operation->params[param_count].memref.parent->size;
						sunxi_tee_param[param_count].index = (uintptr_t)operation->params[param_count].memref.parent->buffer;
					}
					if((operation->params[param_count].memref.parent->flags == TEEC_MEM_OUTPUT) ||
							(operation->params[param_count].memref.parent->flags == (TEEC_MEM_INPUT|TEEC_MEM_OUTPUT ))){
						sunxi_tee_param[param_count].type = TE_PARAM_TYPE_MEM_RW;
						sunxi_tee_param[param_count].u.Mem.base = (void*) ((uintptr_t)operation->params[param_count].memref.parent->buffer);
						sunxi_tee_param[param_count].u.Mem.len = operation->params[param_count].memref.parent->size;
						sunxi_tee_param[param_count].index = (uintptr_t)operation->params[param_count].memref.parent->buffer;
					}
				}
			}else if(param_types[param_count] == TEEC_NONE){
				sunxi_tee_param[param_count].type = TE_PARAM_TYPE_NONE;
			}
		}
		for (param_count = 0; param_count < 4; param_count++) {
			if (cmd->launchop.operation.list_count == 0) {
				cmd->launchop.operation.list_head = sunxi_tee_param + param_count;
				ALOGV("list_head:%p", sunxi_tee_param + param_count);
				cmd->launchop.operation.list_tail = sunxi_tee_param + param_count;
				cmd->launchop.operation.list_count++;
				sunxi_tee_param[param_count].next_ptr_user = NULL;
			} else {
				temp_param = cmd->launchop.operation.list_tail;
				temp_param->next_ptr_user = sunxi_tee_param + param_count;
				cmd->launchop.operation.list_tail = sunxi_tee_param + param_count;
				cmd->launchop.operation.list_count++;
				sunxi_tee_param[param_count].next_ptr_user = NULL;
			}
		}
	}
	if (ret) {
		ALOGE("error in encoding the data");
		goto operation_release;
	}

	/* Invoke the command */
	ret = ioctl(session->device->fd, TE_IOCTL_LAUNCH_OPERATION, cmd);
	if (ret < 0) {
		if (returnOrigin) {
			*returnOrigin = TEEC_ORIGIN_API;
		}
		session->s_errno = errno;
		if (ret == -EFAULT) {
			ret = TEEC_ERROR_ACCESS_DENIED;
		}
		if (ret == -EINVAL) {
			ret = TEEC_ERROR_BAD_PARAMETERS;
		}
		ALOGV("TEEC_InvokeCommand:command submission in client driver failed");
	} else if (ret > 0) {
		if (returnOrigin) {
			*returnOrigin = TEEC_ORIGIN_TRUSTED_APP;
		}
		ALOGV("TEEC_InvokeCommand:command submission failed in trusted application");
		//TEEC_GetError(ret, TEEC_ORIGIN_TRUSTED_APP));
	}
	//if (ret != 0) {
	//	goto operation_release;
	//}

	/*decode cmd*/
	if (operation->paramTypes != 0) {
		for (param_count = 0; param_count < 4; param_count++) {
			if((param_types[param_count] == TEEC_VALUE_INOUT) ||(param_types[param_count] == TEEC_VALUE_OUTPUT)) {
				decode_param = cmd->launchop.operation.list_head;
				operation->params[param_count].value.a = (decode_param + param_count)->u.Int.val_a;
				operation->params[param_count].value.b = (decode_param + param_count)->u.Int.val_b;

			}else if( (param_types[param_count] == TEEC_MEMREF_WHOLE) ||
					(param_types[param_count] == TEEC_MEMREF_PARTIAL_INOUT) ||
					(param_types[param_count] == TEEC_MEMREF_PARTIAL_OUTPUT)){
				decode_param = cmd->launchop.operation.list_head;
				operation->params[param_count].memref.size = (decode_param + param_count)->u.Mem.len;
			}
		}
	}
operation_release:
	session->operation_cnt--;
	free(sunxi_tee_param);
	free(cmd);
	return ret;
}
