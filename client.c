#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "linkedlist.h"
#include "linkedlist_mac.h"

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128

int main(int argc, char *argv[])
{
	struct sockaddr_un addr;
	//int i;
	int ret;
	int data_socket;
	//char buffer[BUFFER_SIZE];
	sync_msg_t smes;
	sync_msg_mac_t * smes_mac;

	/* Create data socket */
	printf("conn000\n");
	data_socket = socket(AF_UNIX, SOCK_STREAM, 0);

	if(data_socket == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	/*
	 * For portability clear the whole structure, since
	 * some implementations have additional (nonstandard) fields
	 * in the structure.
	 */
	memset(&addr, 0, sizeof(struct sockaddr_un));

	/* Connect socket to socket address*/
	printf("conn1\n");
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path)-1);
	ret = connect(data_socket, (const struct sockaddr *) &addr,
			sizeof(struct sockaddr_un));

	if (ret == -1) {
		fprintf(stderr, "The server is down.\n");
		exit(EXIT_FAILURE);
	}
	printf("conn\n");
	/* Receive result */
	node_t * head = create_table();
	node_mac_t * mac_head = create_mac_table();
	smes_mac = (sync_msg_mac_t *)&smes;
	for(;;){
		memset(&smes, 0, sizeof(sync_msg_t));
		ret = -1;
		printf("mset\n");
		ret = read(data_socket, &smes, sizeof(sync_msg_t));
		printf("read\n");
		if(ret == -1) {
			perror("read");
			exit(EXIT_FAILURE);
		}
		/*Ensure buffer is 0-terminated*/
		if(smes.op_code == CREATE) {
			push(head, smes.msg_body);
			printf("create\n");
		}
		else if(smes.op_code == UPDATE) {
			update_by_destmsk(&head, smes.msg_body);
			printf("update\n");
		}
		else if(smes.op_code == DELETE) {
			printf("%s %d\n", smes.msg_body.destination, smes.msg_body.mask);
			remove_by_destmsk(&head, smes.msg_body.destination, smes.msg_body.mask);
			printf("delete\n");
		}
		else if(smes.op_code == PRINT) {
			print_table(head);
		}
		else if(smes_mac->op_code == CREATE_MAC) {
			push_mac(mac_head, smes_mac->mac);
		}
		else if(smes_mac->op_code == DELETE_MAC) {
			remove_by_mac(&mac_head, smes_mac->mac);
		}
		else if(smes_mac->op_code == PRINT_MAC) {
			print_mac_table(mac_head);
		}
		else {
			printf("\nERROR\n");
			printf("%d", smes.op_code);
			printf("%d", smes_mac->op_code);
			break;
		}
	}
	/*Close socket*/

	close(data_socket);

	exit(EXIT_SUCCESS);
}
