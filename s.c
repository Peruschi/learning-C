#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>

#include <stdio.h>

typedef char ALIGN[24];
typedef union header{
	struct {
		union header *next;
	       	size_t size;
		bool is_free;
	} info;
	ALIGN placeholder;
} Header;

static Header *head, *tail;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void* apply_memory(size_t size);
static Header* get_free_block(size_t size);
static Header* split_block(Header *curr, size_t size);

void* malloc(size_t size);
void free(void* block);

int main(void){
	printf("%ld\n",sizeof(Header));
	
	int *ptr1 = malloc(sizeof(int));
	*ptr1 = 10;
	printf("%d %p\n", *ptr1, ptr1);

	free(ptr1);
	
	int *ptr2 = malloc(sizeof(int));
	*ptr2 = 20;
	printf("%d %p\n", *ptr2, ptr2);

	double *ptr3 = malloc(sizeof(double));
	*ptr3 = 30;
	printf("%.0f %p\n", *ptr3, ptr3);
	
	return 0;
}

static void* apply_memory(size_t size){
	void *block = sbrk(size);
	if (block == (void*)-1){
		return NULL;
	}
	else {
		return block;
	}
}

static Header* get_free_block(size_t size){
	Header *curr = head;
	while (curr){
		if (size <= curr->info.size && curr->info.is_free){
			return curr; 
		}
		if (size <= curr->info.size && curr->info.is_free){
			return split_block(curr, size);
		}
		curr = curr->info.next;
	}
	
	return NULL;
}

static Header* split_block(Header* curr, size_t size){
	size_t total_size = sizeof(Header) + size;

	Header *new_header = (void*)curr + total_size;
	new_header->info.next = curr->info.next;
	new_header->info.size = size;
	new_header->info.is_free = true;

	curr->info.next = new_header;
	curr->info.size -= total_size;

	return new_header;
}

void* malloc(size_t size){
	if (!size){
		return NULL;
	}

	pthread_mutex_lock(&mutex);

	Header *header = get_free_block(size);
	if (header){
		header->info.is_free = false;
		pthread_mutex_unlock(&mutex);
		return (void*)(header + 1);
	}

	size_t needed_total_size = sizeof(Header) + size;
	header = apply_memory(needed_total_size);
	if (!header){
		pthread_mutex_unlock(&mutex);
		return NULL;
	}
	header->info.is_free = false;
	header->info.size = size;
	header->info.next = NULL;
	
	if (!head){
		head = header;
	}
	if (tail){
		tail->info.next = header;
	}
	tail = header;

	pthread_mutex_unlock(&mutex);
	return (void*)(header + 1);
}

void free(void* block){
	if (!block){
		return ;
	}

	pthread_mutex_lock(&mutex);
	
	void *program_break = sbrk(0);
	Header *header = (Header*)block - 1;
	if (program_break == block + header->info.size){
		if (head == tail){
			head = tail = NULL;
		}
		else {
			Header * curr = head;
			while (curr){
				if (curr->info.next == tail){
					curr->info.next = NULL;
					tail = curr;
				}
				curr = curr->info.next;
			}
		}
		sbrk(-header->info.size - sizeof(Header));
		pthread_mutex_unlock(&mutex);
		return ;
	}

	header->info.is_free = true;

	pthread_mutex_unlock(&mutex);

}
