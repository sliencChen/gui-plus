/*
 * gui_interface.c
 *
 *  Created on: 2017-12-27
 *      Author: Brant
 */

#include <gdi/gui_interface.h>


#if ENABLE_DEBUG_MEMORY
typedef struct _gui_memory_info_
{
	char *invoker;
	void *addr;
	unsigned int size;
	struct _gui_memory_info_ *next;
} GuiMemoryInfo;

static GuiMemoryInfo gui_memory_info_head;

static void set_memory_info_invoker(GuiMemoryInfo *info, const char *invoker)
{
	info->invoker = (char *)malloc(strlen(invoker) + 1);
	if (info->invoker)
	{
		strcpy(info->invoker, invoker);
	}
}

static void add_gui_memory_info(const char *invoker, void *addr, unsigned int size)
{
	GuiMemoryInfo *node;
	GuiMemoryInfo *tail;

	node = (GuiMemoryInfo *)malloc(sizeof(GuiMemoryInfo));
	if (node == NULL)
	{
		return;
	}
	memset(node, 0, sizeof(GuiMemoryInfo));
	set_memory_info_invoker(node, invoker);
	node->addr = addr;
	node->size = size;
	tail = &gui_memory_info_head;
	while (tail->next)
	{
		tail = tail->next;
	}
	tail->next = node;
}

static void destroy_memory_info_node(GuiMemoryInfo *info)
{
	if (info)
	{
		if (info->invoker)
		{
			free(info->invoker);
		}
		free(info);
	}
}

static void remove_gui_memory_info(void *addr)
{
	GuiMemoryInfo *node, *next;

	node = &gui_memory_info_head;
	next = node->next;
	while (next)
	{
		if (next->addr == addr)
		{
			node->next = next->next;
			destroy_memory_info_node(next);
			break;
		}
		node = next;
		next = node->next;
	}
}

void* gui_malloc_internal(const char *invoker, unsigned int size)
{
	void *addr;

	addr = malloc(size);
	add_gui_memory_info(invoker, addr, size);

	return addr;
}

void gui_free_internal(void *addr)
{
	remove_gui_memory_info(addr);

	return free(addr);
}

void print_memory_use(void)
{
	GuiMemoryInfo *node;

	node = gui_memory_info_head.next;
	while (node)
	{
		printf("%s, size %d\n", node->invoker, node->size);
		node = node->next;
	}
}
#endif


int gui_mutex_init(int *handle, char *name)
{
    return 0;
//	return GSOS_MutexCreate((GSOS_Mutex_t *)handle, name);
}

int gui_mutex_delete(int *handle)
{
    return 0;
//	return GSOS_MutexDelete((GSOS_Mutex_t *)handle);
}

int gui_mutex_lock(int *handle, U32 timeout)
{
    return 0;
//	return GSOS_MutexLock((GSOS_Mutex_t *)handle, timeout);
}

int gui_mutex_unlock(int *handle)
{
    return 0;
//	return GSOS_MutexUnlock((GSOS_Mutex_t *)handle);
}


int gui_message_send(U32 msg_id, U32 param1, U32 param2, U32 param3)
{
    return 0;
//	return SendMessageMain(msg_id, param1, param2, param3);
}

int gui_message_receive(U32 *msg_id, U32 *param1, U32 *param2, U32 *param3, U32 timeout)
{
    return 0;
//	MW_MSG msg;
//	int res = 0;
//
//	if (timeout == GUI_MSG_WAIT_FOREVER)
//	{
//		do
//		{
//			res = ReceiveMessageMain(&msg, 10);
//		} while (res != 0);
//	}
//	else
//	{
//		res = ReceiveMessageMain(&msg, timeout);
//	}
//	if (res == 0)
//	{
//		if (msg_id) *msg_id = msg.message_id;
//		if (param1) *param1 = msg.param1;
//		if (param2) *param2 = msg.param2;
//		if (param3) *param3 = msg.param3;
//	}
//
//	return res;
}
