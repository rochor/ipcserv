/*
 * linkedlist_mac.h
 *
 *  Created on: Sep 5, 2019
 *      Author: juha
 */

#ifndef LINKEDLIST_MAC_H_
#define LINKEDLIST_MAC_H_

#include <stdlib.h>
#include "linkedlist.h"


typedef struct _sync_msg_mac {
	OPCODE  op_code;
	char mac[17];
}sync_msg_mac_t;

typedef struct _node_mac {
	char mac[17];
	struct _node_mac * next;
} node_mac_t;

node_mac_t * create_mac_table ();

void push_mac(node_mac_t * head, char * msg);

int pop_mac(node_mac_t ** head);

int remove_by_mac(node_mac_t ** head, char * msg);

void print_mac_table(node_mac_t * head);


#endif /* LINKEDLIST_MAC_H_ */
