/*
 * linkedlist.h
 *
 *  Created on: Aug 27, 2019
 *      Author: juha
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stdlib.h>

typedef enum {
	CREATE = 1,
	UPDATE = 2,
	DELETE = 3,
	PRINT = 4,
	CREATE_MAC = 5,
	DELETE_MAC = 6,
	PRINT_MAC = 7,
	GETPID = 8
}OPCODE;

typedef struct _msg_body {
	char destination[16];
	char mask;
	char gateway_ip[16];
	char oif[32];
} msg_body_t;

typedef struct _sync_msg {
	OPCODE  op_code;
	msg_body_t msg_body;
}sync_msg_t;

typedef struct _node {
	msg_body_t msg_body;
	struct _node * next;
} node_t;

node_t * create_table ();

void push(node_t * head, msg_body_t msg);

int pop(node_t ** head);

int remove_by_destmsk(node_t ** head, char* dest, char msk);

int update_by_destmsk(node_t ** head, msg_body_t msg);

void print_table(node_t * head);

#endif /* LINKEDLIST_H_ */
