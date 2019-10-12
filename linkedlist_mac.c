/*
 * linkedlist.c
 *
 *  Created on: Aug 26, 2019
 *      Author: juha
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist_mac.h"
#include "linkedlist_ip.h"

node_mac_t * create_mac_table () {
	node_mac_t * head = NULL;
	head = malloc(sizeof(node_mac_t));
	return head;
}

void push_mac(node_mac_t * head, char * msg) {
	node_mac_t * current = head;
	while(current->next != NULL) {
		current = current->next;
	}
	current->next = malloc(sizeof(node_mac_t));
	strcpy(current->next->mac, msg);
	current->next->next = NULL;
}

int pop_mac(node_mac_t ** head) {
    int retval = -1;
    node_mac_t * next_node = NULL;

    if ((*head)->next == NULL) {
        return -1;
    }
    next_node = (*head)->next->next;
    retval = 0;
    free((*head)->next);
    (*head)->next = next_node;
    return retval;
}

int remove_by_mac(node_mac_t ** head, char* msg) {
	node_mac_t * current = *head;
	node_mac_t * temp_node = NULL;

	if(current == NULL) {
		return -1;
	}
	if(	strcmp(current->mac, msg) == 0) {
		pop_mac(head);
		return 0;
	}
	//strcpy(current->next->msg_body.destination, dest);

	while (current->next != NULL) { //work also for last node?
		if(	strcmp(current->next->mac, msg) == 0) {
			temp_node = current->next;
			current->next = temp_node->next;
			free(temp_node);
			return 0;
		}
		current = current->next;
	}
	return -1;
}

void print_mac_table(node_mac_t * head_mac, DList *ip_list) {
	printf("\n               MAC       IP address\n");
	if (head_mac->next == NULL){
		return;
	}
	node_mac_t *current_mac = head_mac->next;
	size_t current_ip = ip_list->first;
	while (current_mac != NULL && current_ip != DNULL) {
		printf("%18s %16s\n",
				current_mac->mac, ip_list->pool[current_ip].domain);
		current_mac = current_mac->next;
		current_ip = ip_list->pool[current_ip].next;
	}
	printf("\n");
}
