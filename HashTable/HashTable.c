#include<stdio.h>
#include<string.h>

// ht -> hash table
// HashTable of items
typedef struct {
    char *key;
    char *value;
} ht_item;

// record deleted location
const static ht_item DELETED_ITEM = {.key = NULL, .value = NULL};

// HashTable
typedef struct {
    int slot_size;
    int count;
    ht_item **items;
} hash_table;

// check function of malloc
inline void* check(const void* ptr);

// create (HashTable, HashTable of items)
inline ht_item* create_ht_item(const char *key, const char *value);
hash_table* create_ht(const int slot_size);

// free (HashTable, HashTable of items)
inline void free_ht_item(ht_item *item);
void free_ht(hash_table *table);

// calculate Hash Value
unsigned int hash(const char *key);
inline unsigned int get_hash_value(const int slot_size, const char *key, const int weights);

// basic operation: (insert, search, delete)
void insert_ht(hash_table *table, const char *key, const char *value);
char* search_ht(const hash_table *table, const char *key);
void delete_ht(hash_table *table, const char *key);

int main(void){
    // The size is preferably a prime number
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





inline void* check(const void* ptr){
    if (ptr == NULL){
        printf("Memory allocation failed!\n");
        abort();
    }
    return (void*)ptr;
}

inline ht_item* create_ht_item(const char *key, const char *value){
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

inline void free_ht_item(ht_item *item){
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
    return hash & 0x7FFFFFFF;
}

inline unsigned int get_hash_value(const int slot_size, const char *key, const int weights){
    unsigned int hash_value_1 = hash(key);
    unsigned int hash_value_2 = hash(key);
    return (hash_value_1 + (weights * hash_value_2 + 1)) % slot_size;
}

void insert_ht(hash_table *table, const char *key, const char *value){
    int slot_size = table->slot_size, i = 0;
    int index = get_hash_value(slot_size, key, i++);
    ht_item *item = table->items[index];
    
    while (item && item != &DELETED_ITEM){
        if (!strcmp(item->key, key)){
            char *temp = item->value;
            free(temp);
            item->value = check(strdup(value));
            return ;
        }
        index = get_hash_value(slot_size, key, i++);
        item = table->items[index];
    }
    
    table->items[index] = create_ht_item(key, value);
    table->count++;
}

char* search_ht(const hash_table *table, const char *key){
    int slot_size = table->slot_size, i = 0;
    int index = get_hash_value(slot_size, key, i++);
    ht_item *item = table->items[index];
    
    while (item){
        if (item != &DELETED_ITEM && !strcmp(item->key, key)){
            return item->value;
        }
        index = get_hash_value(slot_size, key, i++);
        item = table->items[index];
    }
    
    return NULL;
}

void delete_ht(hash_table *table, const char *key){
    int slot_size = table->slot_size, i = 0;
    int index = get_hash_value(slot_size, key, i++);
    ht_item *item = table->items[index];
    
    while (item){
        if (item != &DELETED_ITEM && !strcmp(item->key, key)){
            free_ht_item(item);
            table->items[index] = &DELETED_ITEM;
            table->count--;
            return ;
        }
        index = get_hash_value(slot_size, key, i++);
        item = table->items[index];
    }
}
