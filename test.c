#include <string.h>
#include "test.h"
#include "linkedlist.h"

void pop_linkedlist(node_t * head) {
	msg_body_t b;

	strcpy(b.destination, "192.200.201.100");
	b.mask = 20;
	strcpy(b.gateway_ip, "192.168.1.1");
	strcpy(b.oif, "Ethernet 1");
	push(head, b);

	strcpy(b.destination, "192.200.201.101");
	b.mask = 21;
	strcpy(b.gateway_ip, "192.168.1.2");
	strcpy(b.oif, "Ethernet 2");
	push(head, b);

	strcpy(b.destination, "192.200.201.102");
	b.mask = 22;
	strcpy(b.gateway_ip, "192.168.1.3");
	strcpy(b.oif, "Ethernet 3");
	push(head, b);

	strcpy(b.destination, "192.200.201.103");
	b.mask = 23;
	strcpy(b.gateway_ip, "192.168.1.4");
	strcpy(b.oif, "Ethernet 4");
	push(head, b);

}
