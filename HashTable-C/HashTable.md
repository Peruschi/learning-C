# HashTable
簡單的C語言實作
```c
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
    int size;
    int count;
    ht_item **items;
} hash_table;

// check function of malloc
void* check_malloc(const void* ptr);

// create (HashTable, HashTable of items)
ht_item* create_ht_item(const char *key, const char *value);
hash_table* create_ht(const int size);

// free (HashTable, HashTable of items)
void free_ht_item(ht_item *item);
void free_ht(hash_table *table);

// calculate Hash Value
int hash(const char *key, const int size);

// basic operation: (insert, search, delete)
void insert_ht(hash_table *table, const char *key, const char *value);
char* search_ht(const hash_table *table, const char *key);
void delete_ht(hash_table *table, const char *key);

int main(void){
    hash_table *table = create_ht(1000);
    
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
    insert_ht(table, "烏拉拉", "Ohhhh...");
    printf("first: key = %s value = %s\n", "烏拉拉", search_ht(table, "烏拉拉"));
    
    free_ht(table);
    
    return 0;
}





void* check_malloc(const void* ptr){
    if (ptr == NULL){
        printf("Memory allocation failed!\n");
        abort();
    }
    return ptr;
}

ht_item* create_ht_item(const char *key, const char *value){
    ht_item *item = check_malloc(malloc(sizeof(ht_item)));
    item->key = check_malloc(strdup(key));
    item->value = check_malloc(strdup(value));
    return item;
}

hash_table* create_ht(const int size){
    hash_table *table = check_malloc(malloc(sizeof(hash_table)));
    table->size = size;
    table->count = 0;
    table->items = check_malloc(calloc(size, sizeof(ht_item*)));
    return table;
}

void free_ht_item(ht_item *item){
    free(item->key);
    free(item->value);
    free(item);
}

void free_ht(hash_table *table){
    for (size_t i = 0; i < table->size; i++){
        ht_item *item = table->items[i];
        if (item){
            free_ht_item(item);
        }
    }
    free(table->items);
    free(table);
}

int hash(const char *key, const int size){
    unsigned long long hash = 0;
    int len_key = strlen(key);
    for (int i = 0; i < len_key; i++){
        hash += (unsigned long long)pow(199, len_key - (i + 1)) * key[i];
        hash %= size;
    }
    return hash;
}

void insert_ht(hash_table *table, const char *key, const char *value){
    if (table->size == table->count){
        printf("HashTable is full!\n");
        return ;
    }
    
    int index = hash(key, table->size);
    ht_item *item = table->items[index];

    if (item){
        // collision happen (Pending)
        if (strcmp(item->key, key)){
            free_ht_item(item);
        }
        // the key is existed (Pending)
        else {
            free_ht_item(item);
        }
        table->items[index] = NULL;
    }
    
    table->items[index] = create_ht_item(key, value);
    table->count++;
}

char* search_ht(const hash_table *table, const char *key){
    int index = hash(key, table->size);
    ht_item *item = table->items[index];
    
    if (item && !strcmp(item->key, key)){
        return item->value;
    }
    
    return NULL;
}

void delete_ht(hash_table *table, const char *key){
    int index = hash(key, table->size);
    ht_item *item = table->items[index];
    
    if (item && !strcmp(item->key, key)){
        free_ht_item(item);
    }
    
    table->items[index] = NULL;
    table->count--;
}
```
