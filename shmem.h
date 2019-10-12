/*
 * shmem.h
 *
 *  Created on: Sep 8, 2019
 *      Author: juha
 */

#ifndef SHMEM_H_
#define SHMEM_H_

#define SHMEM_KEY "/macip"

int create_and_write_shared_memory (char *mmap_key, DList *ip_list, unsigned int size);

int read_shared_memory(char *mmap_key, DList *ip_list, unsigned int list_size, unsigned int bytes_to_read);

#endif /* MAKEDBG_SHMEM_H_ */
