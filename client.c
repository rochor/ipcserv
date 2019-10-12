#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <unistd.h>
#include "linkedlist.h"
#include "linkedlist_mac.h"
#include "linkedlist_ip.h"
#include "shmem.h"

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128

int data_socket;
node_t * head;
node_mac_t * mac_head;

static void signal_handler(int sig){
    printf("Flushing routing and ARP table at server request\n", sig);
	while(pop(&head) > -1) {
	}
	while(pop_mac(&mac_head) > -1) {
	}
}

static void ctrlC_signal_handler(int sig){
    pid_t pd = getpid();
    int ret;
    ret = write(data_socket, &pd, sizeof(pd));
	if(ret == -1) {
		perror("write");
	}
	printf("Ctrl-C pressed\n");
	printf("Bye Bye\n");
    exit(0);
}

static void abort_signal_handler(int sig){
    printf("process is aborted\n");
    printf("Bye Bye\n");
    exit(0);
}

int main(int argc, char *argv[])
{
	struct sockaddr_un addr;
	//int i;
	int ret;

	//char buffer[BUFFER_SIZE];
	DList *ip_list_mem;
	ip_list_mem = (DList *)malloc(sizeof(DList));
	sync_msg_t smes;
	sync_msg_mac_t * smes_mac;

	/* Create data socket */
	//printf("conn000\n");
	data_socket = socket(AF_UNIX, SOCK_STREAM, 0);

	if(data_socket == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	signal(SIGUSR1, signal_handler);
	signal(SIGINT, ctrlC_signal_handler);
	signal(SIGABRT, abort_signal_handler);
	/*
	 * For portability clear the whole structure, since
	 * some implementations have additional (nonstandard) fields
	 * in the structure.
	 */
	memset(&addr, 0, sizeof(struct sockaddr_un));

	/* Connect socket to socket address*/
	//printf("conn1\n");
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path)-1);
	ret = connect(data_socket, (const struct sockaddr *) &addr,
			sizeof(struct sockaddr_un));

	if (ret == -1) {
		fprintf(stderr, "The server is down.\n");
		exit(EXIT_FAILURE);
	}
	printf("Connected to server\n");
	/* Receive result */
	head = create_table();
	mac_head = create_mac_table();
	//node_ip_t * ip_head = create_ip_table();
	smes_mac = (sync_msg_mac_t *)&smes;
	for(;;){
		memset(&smes, 0, sizeof(sync_msg_t));
		ret = -1;
		//printf("mset\n");
		ret = read(data_socket, &smes, sizeof(sync_msg_t));
		//printf("read\n");
		if(ret == -1) {
			perror("read");
			exit(EXIT_FAILURE);
		}
		/*Ensure buffer is 0-terminated*/
		if(smes.op_code == CREATE) {
			push(head, smes.msg_body);
			//printf("create\n");
		}
		else if(smes.op_code == UPDATE) {
			update_by_destmsk(&head, smes.msg_body);
			//printf("update\n");
		}
		else if(smes.op_code == DELETE) {
			printf("%s %d\n", smes.msg_body.destination, smes.msg_body.mask);
			remove_by_destmsk(&head, smes.msg_body.destination, smes.msg_body.mask);
			//printf("delete\n");
		}
		else if(smes.op_code == PRINT) {
			print_table(head);
		}
		else if(smes_mac->op_code == CREATE_MAC) {
			push_mac(mac_head, smes_mac->mac);
			int rc = read_shared_memory(SHMEM_KEY, ip_list_mem, sizeof(DList), sizeof(DList));
			if(rc < 0){
				printf("Error reading from shared memory\n");
				return 0;
			}
		}
		else if(smes_mac->op_code == DELETE_MAC) {
			remove_by_mac(&mac_head, smes_mac->mac); //not working perfectly, can by mistake remove only mac or ip
			int rc = read_shared_memory(SHMEM_KEY, ip_list_mem, sizeof(DList), sizeof(DList));
			if(rc < 0){
				printf("Error reading from shared memory\n");
				return 0;
			}
		}
		else if(smes_mac->op_code == PRINT_MAC) {
			print_mac_table(mac_head, ip_list_mem);
		}
		else if(smes_mac->op_code == GETPID) {
			pid_t pd = getpid();
			ret = write(data_socket, &pd, sizeof(pd));
			if(ret == -1) {
				perror("write");
				break;
			}
		}
		else {
			printf("Server error\n");
			printf("Server opcode: %d \n", smes.op_code);
			printf("Bye bye... \n");
			break;
		}
	}
	/*Close socket*/

	close(data_socket);

	exit(EXIT_SUCCESS);
}
