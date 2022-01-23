#include<stdio.h>
#include<string.h>
#include<math.h>

// ht -> hash table

// HashTable of items
typedef struct {
    char *key;
    char *value;
} ht_item;

// HashTable
typedef struct {
    int slot_size;
    int count;
    ht_item **items;
} hash_table;

// check function of malloc
void* check_malloc(const void* ptr);

// create (HashTable, HashTable of items)
ht_item* create_ht_item(const char *key, const char *value);
hash_table* create_ht(const int slot_size);

// free (HashTable, HashTable of items)
void free_ht_item(ht_item *item);
void free_ht(hash_table *table);

// calculate Hash Value
unsigned int hash(const char *key);

// basic operation: (insert, search, delete)
void insert_ht(hash_table *table, const char *key, const char *value);
char* search_ht(const hash_table *table, const char *key);
void delete_ht(hash_table *table, const char *key);

int main(void){
    hash_table *table = create_ht(22501);
    
    // store into HashTable
    insert_ht(table, "烏拉拉", "Ohhhh...");
    printf("first: key = %s value = %s\n", "烏拉拉", search_ht(table, "烏拉拉"));
    
    // delete from HashTable
    delete_ht(table, "烏拉拉");
    if (search_ht(table, "烏拉拉") == NULL){
        printf("Successfully deleted!\n");
    }
    else {
        printf("second: key = %s value = %s\n", "烏拉拉", search_ht(table, "烏拉拉"));
    }
    
    // store into HashTable again
    insert_ht(table, "烏拉拉", "Oh我好餓h...");
    printf("third: key = %s value = %s\n", "烏拉拉", search_ht(table, "烏拉拉"));
    
    free_ht(table);
    
    return 0;
}





void* check(const void* ptr){
    if (ptr == NULL){
        printf("Memory allocation failed!\n");
        abort();
    }
    return (void*)ptr;
}

ht_item* create_ht_item(const char *key, const char *value){
    ht_item *item = check(malloc(sizeof(ht_item)));
    item->key = check(strdup(key));
    item->value = check(strdup(value));
    return item;
}

hash_table* create_ht(const int slot_size){
    hash_table *table = check(malloc(sizeof(hash_table)));
    table->slot_size = slot_size;
    table->count = 0;
    table->items = check(calloc(slot_size, sizeof(ht_item*)));
    return table;
}

void free_ht_item(ht_item *item){
    free(item->key);
    free(item->value);
    free(item);
}

void free_ht(hash_table *table){
    for (int i = 0; i < table->slot_size; i++){
        ht_item *item = table->items[i];
        if (item){
            free_ht_item(item);
        }
    }
    free(table->items);
    free(table);
}

unsigned int hash(const char *str){
    unsigned int hash = 0, x = 0;
   while (*str){
        hash = (hash << 4) + (*str++);
        if (x = hash & 0xF0000000){
            hash ^= (x >> 24);
            hash &= ~x;
        }
    }
    return (hash & 0x7FFFFFFF);
}

void insert_ht(hash_table *table, const char *key, const char *value){
    if (table->slot_size == table->count){
        printf("HashTable is full!\n");
        return ;
    }
    int slot_size = table->slot_size, index = 0;
    ht_item *item = create_ht_item(key, value);
    
    for (int i = 0; i < slot_size; i++){
        index = (hash(key) + i * hash(key) + 1) % slot_size;
        if (table->items[index]){
            if (strcmp(item->key, key)){
                continue;
            }
            else {
                delete_ht(table, key);
            }
        }
        break;
    }

    table->items[index] = item;
    table->count++;
}

char* search_ht(const hash_table *table, const char *key){
    int slot_size = table->slot_size, index = 0;
    ht_item *item;

    for (int i = 0; i < slot_size; i++){
        index = (hash(key) + i * hash(key) + 1) % slot_size;
        item = table->items[index];
        if (item){
            if (strcmp(item->key, key)){
                continue;
            }
            else {
                return item->value;
            }
        }
    }
    
    return NULL;
}

void delete_ht(hash_table *table, const char *key){
    int slot_size = table->slot_size, index = 0;
    ht_item *item;

    for (int i = 0; i < slot_size; i++){
        index = (hash(key) + i * hash(key) + 1) % slot_size;
        item = table->items[index];
        if (item){
            if (strcmp(item->key, key)){
                continue;
            }
            else {
                free_ht_item(item);
                break;
            }
        }
    }
    
    table->items[index] = NULL;
    table->count--;
}