/*
 * linkedlist_pid.h
 *
 *  Created on: Oct 10, 2019
 *      Author: juha
 */


/*
 * linkedlist.h
 *
 *  Created on: Aug 27, 2019
 *      Author: juha
 */

#ifndef LINKEDLIST_PID_H_
#define LINKEDLIST_PID_H_

#include <stdlib.h>
#include <sys/types.h>

typedef struct _node_pid {
	pid_t pid;
	struct _node_pid * next;
} node_pid_t;

node_pid_t * create_pid_table ();

void push_pid(node_pid_t * head, pid_t pd);

int pop_pid(node_pid_t ** head);

int remove_by_pid(node_pid_t ** head, pid_t pd);

void print_pid_table(node_pid_t * head);

#endif /* LINKEDLIST_PID_H_ */
