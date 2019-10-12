/*
 * linkedlist_pid.c
 *
 *  Created on: Oct 10, 2019
 *      Author: juha
 */

#include "linkedlist_pid.h"
#include <stdio.h>

node_pid_t * create_pid_table () {
	node_pid_t * head = NULL;
	head = malloc(sizeof(node_pid_t));
	return head;
}

void push_pid(node_pid_t * head, pid_t pd) {
	node_pid_t * current = head;
	while(current->next != NULL) {
		current = current->next;
	}
	current->next = malloc(sizeof(node_pid_t));
	current->next->pid = pd;
	current->next->next = NULL;
}

int pop_pid(node_pid_t ** head) {
    int retval = -1;
    node_pid_t * next_node = NULL;

    if ((*head)->next == NULL) {
        return -1;
    }
    next_node = (*head)->next->next;
    retval = 0;
    free((*head)->next);
    (*head)->next = next_node;
    return retval;
}

int remove_by_pid(node_pid_t ** head, pid_t pd) {
	node_pid_t * current = *head;
	node_pid_t * temp_node = NULL;

	if(current == NULL) {
		return -1;
	}
	if(current->pid == pd) {
		pop_pid(head);
		return 0;
	}
	while (current->next != NULL) { //work also for last node?
		if(current->next->pid == pd) {
			temp_node = current->next;
			current->next = temp_node->next;
			free(temp_node);
			return 0;
		}
		current = current->next;
	}
	return -1;
}

void print_pid_table(node_pid_t * head) {
	printf("\n PID\n");
	if (head->next == NULL){
		return;
	}
	node_pid_t * current = head->next;
	while (current != NULL) {
		printf("%d\n", current->pid);
		current = current->next;
	}
	printf("\n");
}
