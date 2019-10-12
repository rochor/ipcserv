/*
 * server.c
 *
 *  Created on: Aug 26, 2019
 *      Author: juha
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include "linkedlist.h"
#include "linkedlist_mac.h"
#include "linkedlist_ip.h"
#include "linkedlist_pid.h"
#include "test.h"
#include "shmem.h"

#define SOCKET_NAME "/tmp/DemoSocket" //needs to be a path. if located in /tmp the output will be destroyed
#define BUFFER_SIZE 128
#define MAX_CLIENT_SUPPORTED 32

/*An array of File Descriptors which the server process
 * is maintaining in order to talk with the connected clients.
 * Master socket FD is also a member of this array.*/
int monitored_fd_set[MAX_CLIENT_SUPPORTED];

/*Each connected client's intermediate result is maintained in
 * this client array*/
//int client_result [MAX_CLIENT_SUPPORTED] = {0};

/*Remove all the FDs, if any, from the array*/
static void initialize_monitor_fd_set(){
	int i = 0;
	for(; i < MAX_CLIENT_SUPPORTED; i++) {
		monitored_fd_set[i] = -1;
	}
}

/*Add a new FD to the monitored fd_set array*/
static void add_to_monitored_fd_set(int skt_fd){
	int i = 0;
	for(; i < MAX_CLIENT_SUPPORTED; i++) {
		if(monitored_fd_set[i] != -1)
			continue;
		monitored_fd_set[i] = skt_fd;
		break;
	}
}

static void print_fd_set(){
	int i = 0;
	for(; i < MAX_CLIENT_SUPPORTED; i++) {
		if(monitored_fd_set[i] != -1)
			printf("monitored_fd_set[%d]: %d\n", i, monitored_fd_set[i]);
	}
}

/*Remove the FD from monitored_fd_set array*/
static void remove_from_monitored_fd_set(int skt_fd){
	int i = 0;
	for(; i < MAX_CLIENT_SUPPORTED; i++){

		if(monitored_fd_set[i] != skt_fd)
			continue;
		monitored_fd_set[i] = -1;
		break;
	}
}

/* Clone all the FDs in monitored fd_set array into fd_set Data structure*/
static void refresh_fd_set(fd_set *fd_set_ptr){
	FD_ZERO(fd_set_ptr); //init fd set
	int i = 0;
	for(; i < MAX_CLIENT_SUPPORTED; i++){
		if(monitored_fd_set[i] != -1){
			FD_SET(monitored_fd_set[i], fd_set_ptr); //toggle FD set true
		}
	}
}
/* Get the numerical max value among all FDs which server is monitoring*/

static int get_max_fd(){
	int i = 0;
	int max = -1;

	for(; i < MAX_CLIENT_SUPPORTED; i++){
		if(monitored_fd_set[i] > max)
			max = monitored_fd_set[i];
	}
	return max;
}

static void send_print(int conn_socket, sync_msg_t * smess, OPCODE op_code) {
	int i, ret;
	for(i = 0; i<MAX_CLIENT_SUPPORTED; i++) {
		if(monitored_fd_set[i] > 0 && monitored_fd_set[i] != conn_socket) {
			//printf("sending to monitored_fd_set[%d]: %d\n", i, monitored_fd_set[i]);
			smess->op_code = op_code;
			ret = write(monitored_fd_set[i], smess, sizeof(sync_msg_t));
			if(ret == -1) {
				perror("write");
				break;
			}
			smess->op_code = PRINT;
			//memset(buffer, 0, BUFFER_SIZE);
			ret = write(monitored_fd_set[i], smess, sizeof(sync_msg_t));
			if(ret == -1) {
				perror("write");
				break;
			}
		}
	}
}

static void send_print_mac(int conn_socket, sync_msg_mac_t * smess, OPCODE op_code) {
	int i, ret;
	for(i = 0; i<MAX_CLIENT_SUPPORTED; i++) {
		if(monitored_fd_set[i] > 0 && monitored_fd_set[i] != conn_socket) {
			//printf("sending to monitored_fd_set[%d]: %d\n", i, monitored_fd_set[i]);
			smess->op_code = op_code;
			ret = -1;
			/*even if write data is a struct sync_msg_mac_t,
			 * client is always reading a struct of size sync_msg_t
			 * which is larger than sync_msg_mac_t. therefore writer must send always
			 * a struct of size sync_msg_t populated with some garbage data.
			 * op_code is located in same place in both structs so the client can
			 * cast the sync_msg_t struct to sync_msg_mac_t struct later */
			ret = write(monitored_fd_set[i], smess, sizeof(sync_msg_t));
			if(ret == -1) {
				perror("write");
				break;
			}
			smess->op_code = PRINT_MAC;
			ret = -1;
			//memset(buffer, 0, BUFFER_SIZE);
			ret = write(monitored_fd_set[i], smess, sizeof(sync_msg_t));
			if(ret == -1) {
				perror("write");
				break;
			}
		}
	}
}

int main(int argc, char *argv[]) {
	struct sockaddr_un name; //struct containing socket type and name

	int ret;
	int connection_socket; //master socket
	int data_socket; //socket for exchanging data with slaves

	char buffer[BUFFER_SIZE];
	node_t * head = create_table();
	node_mac_t * mac_head = create_mac_table();
	node_pid_t * pid_head = create_pid_table();
	DList *ip_list;
	DList *ip_list_mem;
	ip_list = (DList *)malloc(sizeof(DList));
	ip_list_mem = (DList *)malloc(sizeof(DList));
	dlist_init(ip_list);
	//printf("sizof DList - %d\n", sizeof(DList));
	//pop_linkedlist(head);

	msg_body_t b;
	sync_msg_t smes;
	sync_msg_mac_t smes_mac;

	/*new*/
	fd_set readfds; //file descriptor set. bits until 1024 indicating FD id
	int comm_socket_fd, i; //comm socket FD #
	initialize_monitor_fd_set(); //clean the help array
	add_to_monitored_fd_set(0); //allow console input

	/*In case the program exited inadvertently on the last run,
	 * remove the socket.*/
	unlink(SOCKET_NAME);

	/*Create Master socket (stream) */
	/*use SOCK_DGRAM for Datagram based communication*/
	connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);

	if(connection_socket == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	printf("Master socket created\n");

	/*initialize*/
	memset(&name, 0, sizeof(struct sockaddr_un));
	/*Specify the socket credentials*/
	name.sun_family = AF_UNIX;
	strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path)-1);

	/*Bind socket to socket name*/
	/*Purpose of bind() system call is that application() dictate the underlying
	 * operating system the criteria of receiving the data. Here, bind() system call
	 * is telling theOS that if sender process sends the data destined to socket "/tmp/DemoSocket",
	 * then such data needs to be delivered to this server process (the server process)
	 */
	ret = bind(connection_socket, (const struct sockaddr*) &name,
			sizeof(struct sockaddr_un));

	if (ret == -1) {
		perror("bind");
		exit(EXIT_FAILURE);
	}
	printf("bind() call succeed\n");
	/*
	 * Prepare for accepting connections. The backlog size is set to
	 *  20. So while one request is being processed other requests
	 *  can be waiting.
	 */
	ret = listen(connection_socket, 20);
	if(ret == -1) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	/*Add master socket to monitored set of FDs*/
	add_to_monitored_fd_set(connection_socket);

	/* This is the main loop for handling connections*/
	/*All server process usually runs 24x7. Good Servers should always
	 * be up and running and never go down.
	 */
	for(;;) {
		/*Copy the entire monitored FDs to readfds*/
		printf("-------------------------\n");

		print_table(head);

		printf( "Enter command:\n"
				"1 - Create routing table entry\n"
				"2 - Update routing table entry\n"
				"3 - Delete routing table entry\n"
				"4 - Create ARP table entry\n"
				"5 - Delete ARP table entry\n"
				"6 - Flush routing and ARP table");

		refresh_fd_set(&readfds);
		print_fd_set();

		/*Wait for incoming connection or user choice*/
		/*Call the select system call, server process blocks here.
		 * Linux OS keeps this process blocked until the connection initiation request or
		 * data request arrives on any of the file descriptors in the 'readfds' set*/
		select(get_max_fd() + 1, &readfds, NULL, NULL, NULL);

		if(FD_ISSET(connection_socket, &readfds)) {
			/*Data arrives on Master socket only when new client
			 * connects with the server (that is, 'connect' call
			 * is invoked on client side)*/
			printf("New client connection received\n");

			data_socket = accept(connection_socket, NULL, NULL);

			if(data_socket == -1) {
				perror("accept");
				exit(EXIT_FAILURE);
			}

			add_to_monitored_fd_set(data_socket);
			if (head->next != NULL) {
				node_t * current = head->next;
				smes.op_code = CREATE;
				//--------todo - send also the MAC table------------
				while (current != NULL) { //send the routing table
					smes.msg_body = current->msg_body;
					ret = write(data_socket, &smes, sizeof(sync_msg_t));
					if(ret == -1) {
						perror("write");
						break;
					}
					current = current->next;
				}
			}
			smes.op_code = PRINT;
			ret = write(data_socket, &smes, sizeof(sync_msg_t));
			if(ret == -1) {
				perror("write");
				break;
			}
			smes.op_code = GETPID;
			ret = write(data_socket, &smes, sizeof(sync_msg_t));
			if(ret == -1) {
				perror("write");
				break;
			}
		}
		else if(FD_ISSET(0, &readfds)) { //change to menu input create,update,delete
			//memset(buffer, 0, BUFFER_SIZE);
			ret = read(0, buffer, BUFFER_SIZE);
			//1printf("Input read from console : %s\n", buffer);
			int i = (int)buffer[0];
			if(i == '1') {
				//memset(buffer, 0, BUFFER_SIZE);
				printf( "\nProvide details (Destination_IP  Mask  Gateway_IP  OIF):\n");
				scanf("%s %d %s %s", smes.msg_body.destination, &i, smes.msg_body.gateway_ip, smes.msg_body.oif);
				smes.msg_body.mask = (char)i;
				push(head, smes.msg_body);
				send_print(connection_socket, &smes, CREATE);
			}
			else if(i == '2') {
				printf( "\nProvide details (Destination_IP  Mask  Gateway_IP  OIF):\n");
				scanf("%s %d %s %s", smes.msg_body.destination, &i, smes.msg_body.gateway_ip, smes.msg_body.oif);
				smes.msg_body.mask = (char)i;
				update_by_destmsk(&head, smes.msg_body);
				send_print(connection_socket, &smes, UPDATE);
			}
			else if(i == '3') {
				printf( "\nProvide details (Destination_IP  Mask):\n");
				scanf("%s %d", b.destination, &i);
				b.mask = (char)i;
				remove_by_destmsk(&head, b.destination, b.mask);
				smes.msg_body = b;
				send_print(connection_socket, &smes, DELETE);
			}
			else if(i == '4') {
				char macip[16];
				printf( "\nProvide details (MAC_address IP_address):\n");
				scanf("%s %s", smes_mac.mac, macip);
				push_mac(mac_head, smes_mac.mac);
				dnode_push(ip_list, macip); //no memory yet allocated to it
				create_and_write_shared_memory (SHMEM_KEY, ip_list, sizeof(DList));
				int rc = read_shared_memory(SHMEM_KEY, ip_list_mem, sizeof(DList), sizeof(DList));
				if(rc < 0){
					printf("Error reading from shared memory\n");
					return 0;
				}
				print_mac_table(mac_head, ip_list_mem);
				send_print_mac(connection_socket, &smes_mac, CREATE_MAC);
			}
			else if(i == '5') {
				char macip[16];
				printf( "\nProvide details (MAC_address IP_address):\n");
				scanf("%s %s", smes_mac.mac, macip);
				remove_by_mac(&mac_head, smes_mac.mac); //not working perfectly, can by mistake remove only mac or ip
				dnode_findstr_pop(ip_list, macip); //to improve, create single function that checks mac & ip found in same place before removal
				create_and_write_shared_memory (SHMEM_KEY, ip_list, sizeof(DList));
				int rc = read_shared_memory(SHMEM_KEY, ip_list_mem, sizeof(DList), sizeof(DList));
				if(rc < 0){
					printf("Error reading from shared memory\n");
					return 0;
				}
				print_mac_table(mac_head, ip_list_mem);
				send_print_mac(connection_socket, &smes_mac, DELETE_MAC);
			}
			else if(i == '6') {
				while(pop_mac(&mac_head) > -1) { //flush mac table from server
				}
				while(pop(&head) > -1) { //flush
				}
				while(ip_list->last != DNULL) {
					dnode_pop(ip_list);
				}
				create_and_write_shared_memory (SHMEM_KEY, ip_list, sizeof(DList));
				node_pid_t *pdd = pid_head;
				while(pdd->next != NULL) {
					kill(pdd->next->pid, SIGUSR1); //send signal to clients to flush their tables
					pdd = pdd->next;
				}

			}
			else {
				printf("invalid choice\n");
			}
		}
		else { /* Data arrives from client to either add the PID or remove it, if it exists
			Find the client who has sent us the data request*/
			i = 0, comm_socket_fd = -1;
			for(; i<MAX_CLIENT_SUPPORTED; i++) {
				if(FD_ISSET(monitored_fd_set[i], &readfds)) {
					comm_socket_fd = monitored_fd_set[i];

					/*Prepare the buffer to recv the data*/
					memset(buffer,0,BUFFER_SIZE);
					/*Wait for next data packet*/
					/*Server is blocked here. Waiting for the data to arrive from client
					 * 'read' is a blocking system call*/
					//printf("Waiting for data from the client\n");
					ret = read(comm_socket_fd, buffer, BUFFER_SIZE);

					if(ret == -1) {
						perror("read");
						exit(EXIT_FAILURE);
					}
					/*Add received summand*/
					pid_t pd;
					memcpy(&pd, buffer, sizeof(pid_t));
					if(remove_by_pid(&pid_head, pd) == -1) {
						push_pid(pid_head, pd);
						printf("pushed pid %d\n", (int)pd);
					}
					else {
						close(monitored_fd_set[i]);
						remove_from_monitored_fd_set(monitored_fd_set[i]);
						printf("removed pid %d\n", (int)pd);
					}
				}
			}
		}
	}
	/*go to select and block*/
	/*close the master socket*/
	close(connection_socket);
	remove_from_monitored_fd_set(connection_socket);
	printf("connection closed..\n");
	free(ip_list);

	/* Serverould release resources before getting terminated
	 * Unlink the socket
	 */
	unlink(SOCKET_NAME);
	exit(EXIT_SUCCESS);
}


