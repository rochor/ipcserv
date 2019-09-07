/*
 * linkedlist.c
 *
 *  Created on: Aug 26, 2019
 *      Author: juha
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

node_t * create_table () {
	node_t * head = NULL;
	head = malloc(sizeof(node_t));
	return head;
}

void push(node_t * head, msg_body_t msg) {
	node_t * current = head;
	while(current->next != NULL) {
		current = current->next;
	}
	current->next = malloc(sizeof(node_t));
	current->next->msg_body = msg;
	/*strcpy(current->next->msg_body.destination, msg.destination);
	current->next->msg_body.mask = msg.mask;
	strcpy(current->next->msg_body.gateway_ip, msg.gateway_ip);
	strcpy(current->next->msg_body.oif, msg.oif);*/
	current->next->next = NULL;
}

int pop(node_t ** head) {
    int retval = -1;
    node_t * next_node = NULL;

    if ((*head)->next == NULL) {
        return -1;
    }
    next_node = (*head)->next->next;
    retval = 0;
    free((*head)->next);
    (*head)->next = next_node;
    return retval;
}

int remove_by_destmsk(node_t ** head, char* dest, char msk) {
	node_t * current = *head;
	node_t * temp_node = NULL;

	if(current == NULL) {
		return -1;
	}
	if(	strcmp(current->msg_body.destination, dest) == 0 &&
		current->msg_body.mask == msk) {
		pop(head);
		return 0;
	}
	//strcpy(current->next->msg_body.destination, dest);

	while (current->next != NULL) { //work also for last node?
		if(	strcmp(current->next->msg_body.destination, dest) == 0 &&
			current->next->msg_body.mask == msk) {
			temp_node = current->next;
			current->next = temp_node->next;
			free(temp_node);
			return 0;
		}
		current = current->next;
	}
	return -1;
}

int update_by_destmsk(node_t ** head, msg_body_t msg) {
	node_t * current = *head;
	//node_t * temp_node = NULL;

	if(current == NULL) {
		return -1;
	}
	if(	strcmp(current->msg_body.destination, msg.destination) == 0 &&
		current->msg_body.mask == msg.mask) {
		printf("one - %s %s\n", current->msg_body.destination, msg.destination);
		current->msg_body = msg;
		return 0;
	}
	//strcpy(current->next->msg_body.destination, dest);

	while (current->next != NULL) { //work also for last node?
		if(	strcmp(current->next->msg_body.destination, msg.destination) == 0 &&
			current->next->msg_body.mask == msg.mask) {
			printf("%s %s\n", current->msg_body.destination, msg.destination);
			current->next->msg_body = msg;
			return 0;
		}
		current = current->next;
	}
	return -1;
}

void print_table(node_t * head) {
	printf("\nDestination/Subnet   Gateway(IP)    OIF\n");
	if (head->next == NULL){
		return;
	}
	node_t * current = head->next;
	while (current != NULL) {
		printf("%s/%d   %s    %s\n",
				current->msg_body.destination,
				(int)current->msg_body.mask,
				current->msg_body.gateway_ip,
				current->msg_body.oif);
		current = current->next;
	}
	printf("\n");
}
