#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#include <stdio.h>

typedef struct head {
	size_t size;
	bool is_free;
	struct head *next;
} header;

pthread_mutex_t mutex;

header *head, *tail;

header* get_free_block(size_t size);
void* malloc(size_t size);
void* calloc(size_t num, size_t type_size);
void* realloc(void* block, size_t size);
void free(void* block);


int main(void){
	int *ptr = malloc(sizeof(int));
	int a = 199;
	ptr = realloc(ptr, sizeof(long int));
	ptr = &a + 1;
	printf("%ld\n", sizeof(int));
	printf("%d %p %ld\n", *ptr, ptr, sizeof(ptr));
	
	int *ptr1 = calloc(1, sizeof(int));
	int b = 2000;
	ptr1 = &b;
	printf("%d\n", *ptr1);

	free(ptr);
	printf("喔!!!]\n");
	printf("%d %p\n", *ptr, ptr);
	return 0;
}


header* get_free_block(size_t size){
	header* curr = head;
	while (curr){
		if (size <= curr->size && curr->is_free){
			return curr;
		}
		curr = curr->next;
	}

	return NULL;
}

void* malloc(size_t size){
	if (!size){
		return NULL;
	}

	pthread_mutex_lock(&mutex);

	header *new = get_free_block(size);
	if (new){
		new->is_free = false;
		pthread_mutex_unlock(&mutex);
		return (void*)(new + 1);
	}

	size_t total_size = sizeof(header) + size;
	void *block = sbrk(total_size);
	if (block == (void*)-1){
		pthread_mutex_unlock(&mutex);
		return NULL;
	}

	new = block;
	new->size = size;
	new->is_free = false;
	new->next = NULL;
	if (!head){
		head = new;		
	}
	if (tail){
		tail->next = new;
	}
	tail = new;
	pthread_mutex_unlock(&mutex);
	return (void*)(new + 1);
}

void* calloc(size_t num, size_t type_size){
	if (!num || !type_size){
		return NULL;
	}

	size_t size = num * type_size;
	if (type_size != size / num){
		return NULL;
	}

	void *block = malloc(size);
	if (!block){
		return NULL;
	}

	memset(block, 0, size);
	return block;
}

void* realloc(void* block, size_t size){
	if (!block || !size){
		return NULL;
	}

	header *orig = (header*)block - 1;
	if (size <= orig->size){
		return block;
	}

	void *new = malloc(size);
	if (new){
		memcpy(new, block, size);
		free(block);
	}

	return new;
}

void free(void *block){
	if (!block){
		return ;
	}

	pthread_mutex_lock(&mutex);

	void *program_break = sbrk(0);
	header *orig = (header*)block - 1;
	if ((char*)block + orig->size == program_break){
		if (head == tail){
			head = tail = NULL;
		}
		else {
			header *temp = orig;
			while (temp){
				if (temp->next == tail){
					temp->next = NULL;
					tail = temp;
				}
				temp = temp->next;
			}
		}
		sbrk(0 - sizeof(header) - orig->size);
		pthread_mutex_unlock(&mutex);
		return ;
	}

	orig->is_free = true;
	pthread_mutex_unlock(&mutex);
}
