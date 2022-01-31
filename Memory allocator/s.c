#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#include <stdio.h>

#define MAX_FREE_THRESHOLD 28672
#define MIN_APPLICATION_THRESHOLD 4096

typedef char ALIGN[24];
typedef union header{
        struct {
                union header *next;
                size_t size;
                bool is_free;
        } info;
        ALIGN placeholder;
} Header;

static Header *head = NULL, *tail = NULL, *last_node = NULL;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void* apply_memory(size_t size);
static Header* get_free_block(size_t size);
static Header* split_free_block(Header *curr, size_t size);
static Header* merge_free_block(Header *curr);

void* malloc(size_t size);
void* calloc(size_t num, size_t type_size);
void* realloc(void *block, size_t size);
void free(void* block);

void debug(void* block);

int main(void){
	int *p1 = malloc(sizeof(int));
	int *p2 = malloc(sizeof(int));
	long *p3 = malloc(sizeof(long));
	Header *p4 = malloc(sizeof(Header));

	int num = 50;
	Header **p5 = malloc(num * sizeof(Header*));
	for (int i = 0; i < num; i++){
		p5[i] = malloc(num * sizeof(Header));
	}
	
	puts("First:");
	debug(head);
	
	free(p1);
	free(p2);
	free(p3);
	free(p4);

	puts("Second:");
	debug(head);

	for (int i = 0; i < num; i++){
		free(p5[i]);
	}
	free(p5);

	puts("Third:");
	debug(head);
}

static void* apply_memory(size_t size){
	if (size < MIN_APPLICATION_THRESHOLD){
		size = MIN_APPLICATION_THRESHOLD;
	}

	pthread_mutex_lock(&mutex);
	void *block = sbrk(size);
	pthread_mutex_unlock(&mutex);
	
	if (block == (void*)0xffffffff){
		return NULL;
	}
	else {
		return block;
	}
}

static Header* get_free_block(size_t size){
	Header *curr = last_node;
	while (curr){
		if (curr->info.is_free && size == curr->info.size){
			last_node = curr;
			return curr;
		}
		if (curr->info.is_free && curr->info.next && (curr->info.next)->info.is_free){
			curr = merge_free_block(curr);
		}
		if (curr->info.is_free && size + sizeof(Header) < curr->info.size){
			last_node = curr;
			return split_free_block(curr, size);
		}
		curr = curr->info.next;
	}

	last_node = curr;
	return NULL;
}

static Header* split_free_block(Header *curr, size_t size){
	size_t total_size = sizeof(Header) + size;
	if (curr->info.size < total_size){
		return curr;
	}

	Header *new_header = (void*)curr + total_size;	
	new_header->info.next = curr->info.next;
	new_header->info.size = curr->info.size - total_size;
	new_header->info.is_free = true;

	curr->info.next = new_header;
	curr->info.size = size;

	return curr;
}

static Header* merge_free_block(Header *curr){
	Header *next = curr->info.next;
	curr->info.size += sizeof(Header) + next->info.size;
	curr->info.next = next->info.next;

	return curr;
}

void* malloc(size_t size){
	if (!size){
		return NULL;
	}

	Header *header = get_free_block(size);
	if (header){
		header->info.is_free = false;
		return (void*)(header + 1);
	}

	size_t total_size = sizeof(Header) + size;
	header = apply_memory(total_size);
	if (!header){
		return NULL;
	}
	header->info.size = MIN_APPLICATION_THRESHOLD;
	header = split_free_block(header, size);
	header->info.is_free = false;

	if (!head){
		head = header;
	}
	if (!last_node){
		last_node = header;
	}
	if (tail){
		tail->info.next = header;
		Header *curr = header;
		while (curr->info.next){
			curr = curr->info.next;	
		}
		tail = curr;
	}

	return (void*)(header + 1);
}

void free(void *block){
	if (!block){
		return ; 
	}

	Header *header = (Header*)block - 1;
	header->info.is_free = true;

	Header dummy;
	dummy.info.next = header;
	Header *prev = &dummy;
	Header *curr = header;
	while (curr){
		if (curr->info.is_free && curr->info.next && (curr->info.next)->info.is_free){
			curr = merge_free_block(curr);
		}
		else {
			break;
		}
		prev = prev->info.next;
		curr = curr->info.next;
	}

	pthread_mutex_lock(&mutex);
	void *program_break = sbrk(0);
	pthread_mutex_unlock(&mutex);
	
	if (program_break == block + header->info.size && MIN_APPLICATION_THRESHOLD <= header->info.size){
		if (head == tail){
			head = tail = NULL;
		}
		else {
			tail = prev;
			tail->info.next = NULL;
		}
		sbrk(0 - header->info.size - sizeof(Header));
	}
}

void debug(void* block){
        Header* curr = block;
        printf("%p %p\n", head, tail);
        while (curr){
                printf("%p %zu %u %p\n", curr, curr->info.size, curr->info.is_free, curr->info.next);
                curr = curr->info.next;
        }
}
