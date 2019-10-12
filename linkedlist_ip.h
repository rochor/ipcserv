/*
 * linkedlist_ip.h
 *
 *  Created on: Sep 8, 2019
 *      Author: juha
 */

#ifndef LINKEDLIST_IP_H_
#define LINKEDLIST_IP_H_

#include <stdlib.h>
#include "linkedlist.h"

#define MAX_DNODE 16            // Max. domain string length
#define MAX_DLEN 64             // Max. number of list nodes
#define DNULL (MAX_DLEN + 1)    // NULL value

typedef struct DNode {
    char domain[MAX_DNODE];
    size_t next;
} DNode;

typedef struct DList { //linked list in shared memory
    DNode pool[MAX_DLEN];      // fixed-size space for nodes
    size_t first;                // global list head
    size_t last;				// last node address
} DList;

void dlist_init(DList *dlist); //initialize list

int dlist_find_free_idx(DList *dlist); //find free index from array

void dnode_push(DList *dlist, const char *str); // add entry

void dnode_pop(DList *dlist); //remove entry

void dnode_findstr_pop(DList *dlist, char *str); //pop entry based on content

void dnode_findstr_update(DList *dlist, char *str, char *strnew); // update entry based on content

void dlist_print(DList *dlist); //print the list

#endif /* LINKEDLIST_IP_H_ */
