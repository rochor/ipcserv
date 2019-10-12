/*
 * linkedlist_ip.c
 *
 *  Created on: Sep 8, 2019
 *      Author: juha
 */


/*
 * linkedlist.c
 *
 *  Created on: Aug 26, 2019
 *      Author: juha
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist_ip.h"

void dlist_init(DList *dlist) { //initialize list
	for(int i = 0; i < MAX_DNODE; i++) {
		dlist->pool[i].next = DNULL;
	}
	dlist->first = DNULL;
	dlist->last = DNULL;
}

int dlist_find_free_idx(DList *dlist){ //find free index from array
	for(int i = 0; i< MAX_DNODE; i++) {
		if(dlist->pool[i].next == DNULL && i != dlist->last) {
			return i;
		}
	}
	return DNULL;
}

 void dnode_push(DList *dlist, const char *str) { // add entry
	if(dlist->first == DNULL) {
		dlist->first = 0;
		dlist->last = 0;
	}
	else {
		dlist->pool[dlist->last].next = dlist_find_free_idx(dlist);
		dlist->last = dlist->pool[dlist->last].next;
	}
	strncpy(dlist->pool[dlist->last].domain, str, sizeof(dlist->pool[0].domain));
}

void dnode_pop(DList *dlist) { //remove entry
	if(dlist->last == DNULL) { //no elements in array
		return;
	}
	else if (dlist->first == dlist->last) { //one element in array
		dlist->pool[dlist->first].next = DNULL;
		dlist->first = DNULL;
		dlist->last = DNULL;
		return;
	}
	else { //two elements in array
		int i = dlist->first;
		 do {
			if (dlist->pool[i].next == dlist->last) { //found second-last element
				dlist->last = i; //mark second-last as last
				dlist->pool[i].next = DNULL; //mark last element as null
				return;
			}
			i = dlist->pool[i].next;
		} while (dlist->pool[i].next != DNULL);
	}
}

void dnode_findstr_pop(DList *dlist, char *str) //pop entry based on content
{
	if(dlist->last == DNULL) { //no elements in array
		return;
	}
	else if (dlist->first == dlist->last) { //one element in array
		if(strcmp(dlist->pool[dlist->first].domain, str) == 0) {
			dlist->pool[dlist->first].next = DNULL;
			dlist->first = DNULL;
			dlist->last = DNULL;
		}
		return;
	}
	else { //two or more elements in array
		int i = dlist->first;
		if (strcmp(dlist->pool[i].domain, str) == 0) { //first element must be removed
			dlist->first = dlist->pool[i].next; //second element is now the first
			dlist->pool[i].next = DNULL; //first element leads to nowhere
			return;
		}
		 do {
			if (strcmp(dlist->pool[dlist->pool[i].next].domain, str) == 0) { //found a match for removal
				int rm = dlist->pool[i].next; //element to remove
				if(rm == dlist->last) { //the element we will remove is the last one
					//rm->next should already be null
					dlist->pool[i].next = DNULL; //i->next shall be null
					dlist->last = i; //last shall point to i
					return;
				}
				else { //the element we will remove has a next element of its own
					dlist->pool[i].next = dlist->pool[rm].next; //i->next will 'jump over' rm
					dlist->pool[rm].next = DNULL; //rm will now refer to nothing
					return;
				}
			}
			i = dlist->pool[i].next;
		} while (dlist->pool[i].next != DNULL);
	}
}

void dnode_findstr_update(DList *dlist, char *str, char *strnew) { // update entry based on content
	if(dlist->last == DNULL) { //no elements in array
		return;
	}
	else {
		size_t i = dlist->first;
		if (strcmp(dlist->pool[i].domain, str) == 0) { //found a match
			strncpy(dlist->pool[i].domain, strnew, sizeof(dlist->pool[0].domain));
			return;
		}
		do {
			if (strcmp(dlist->pool[dlist->pool[i].next].domain, str) == 0) { //found a match
				strncpy(dlist->pool[dlist->pool[i].next].domain, strnew, sizeof(dlist->pool[0].domain));
				return;
			}
			i = dlist->pool[i].next;
		} while (dlist->pool[i].next != DNULL);
	}
}

void dlist_print(DList *dlist) { //print the list
	size_t i = dlist->first;
	if(dlist->last == DNULL) {
		return;
	}
	do {
		puts(dlist->pool[i].domain);
		i = dlist->pool[i].next;
	} while (i != DNULL);
}
