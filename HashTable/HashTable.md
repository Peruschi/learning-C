# HashTable
#### Strcut
- 以`char*`(string)為key(index)
- 以`char*`(string)為value
#### Technology
- Open Addressing: Double hashing
- ELF Hash

```c
#include<stdio.h>
#include<string.h>

// ht -> hash table
// HashTable of items
typedef struct {
    char *key;
    char *value;
} ht_item;

// record deleted location
static ht_item DELETED_ITEM = {.key = NULL, .value = NULL};

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

// resize HashTable
void hash_table_resize(hash_table *table, const int base_slot_size);
void hash_table_resize_up(hash_table *table);
void hash_table_resize_down(hash_table *table);

// calculate prime
unsigned int prime(int num);

int main(void){
    int base_size = 3;
    // The size is preferably a prime number
    hash_table *table = create_ht(base_size);
    
    // store into HashTable
    insert_ht(table, "烏拉拉", "Ohhhh...");
    printf("1: key = %s, value = %s\n", "烏拉拉", search_ht(table, "烏拉拉"));
    
    insert_ht(table, "ji3", "Oasdh...");
    printf("2: key = %s, value = %s\n", "ji3", search_ht(table, "ji3"));
    
    insert_ht(table, "烏sad拉拉", "Oadgarghh...");
    printf("3: key = %s, value = %s\n", "烏sad拉拉", search_ht(table, "烏sad拉拉"));
    
    insert_ht(table, "烏asdad拉", "OadgdsdsaAarghh...");
    printf("4: key = %s, value = %s\n", "烏asdad拉", search_ht(table, "烏asdad拉"));
    
    insert_ht(table, "烏sad拉grhhk 拉", "Oadgarghh...");
    printf("5: key = %s, value = %s\n", "烏sad拉grhhk 拉", search_ht(table, "烏sad拉grhhk 拉"));
    
    insert_ht(table, " 烏sa d拉 拉", "Oadgarghh...");
    printf("6: key = %s, value = %s\n", " 烏sa d拉 拉", search_ht(table, " 烏sa d拉 拉"));
    
    // delete from HashTable
    delete_ht(table, "烏拉拉");
    if (search_ht(table, "烏拉拉") == NULL){
        printf("\nSuccessfully deleted!\n");
    }
    else {
        printf("1: key = %s, value = %s\n", "烏拉拉", search_ht(table, "烏拉拉"));
    }
    
    // store into HashTable again
    insert_ht(table, "烏拉拉", "Oh我好餓h...");
    printf("change-1: key = %s, value = %s\n\n", "烏拉拉", search_ht(table, "烏拉拉"));
    
    // change in size
    printf("base_size = %d, new_size = %d\n", base_size, table->slot_size);
    
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
    int load = 100 * table->count / table->slot_size;
    if (70 < load){
        hash_table_resize_up(table);
    }
    
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
    int load = 100 * table->count / table->slot_size;
    if (load < 10){
        hash_table_resize_down(table);
    }
    
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

void hash_table_resize_up(hash_table *table){
    const int new_slot_size = prime(table->slot_size << 1);
    hash_table_resize(table, new_slot_size);
}

void hash_table_resize_down(hash_table *table){
    const int new_slot_size = prime(table->slot_size >> 1);
    hash_table_resize(table, new_slot_size);
}

void hash_table_resize(hash_table *table, const int new_slot_size){
    const int base_slot_size = table->slot_size;
    const int base_count = table->count;
    hash_table *temp_table = create_ht(new_slot_size);
    
    for (int i = 0; i < base_slot_size; i++){
        ht_item *item = table->items[i];
        if (!item || item == &DELETED_ITEM) continue;
        insert_ht(temp_table, item->key, item->value);
    }
    
    table->slot_size = new_slot_size;
    table->count = base_count;
    
    ht_item **temp_items = table->items;
    table->items = temp_table->items;
    temp_table->items = temp_items;
    
    temp_table->slot_size = base_slot_size;
    
    free_ht(temp_table);
}

unsigned int prime(int num){
    unsigned int next_prime = num;
    for (int i = 1; i < 40; i++){
        next_prime = i * i + i + 41;
        if (num < next_prime){
            return next_prime;
        }
    }
    return next_prime;
}
```
#### 輸出結果
```
Finished in 4 ms
1: key = 烏拉拉, value = Ohhhh...
2: key = ji3, value = Oasdh...
3: key = 烏sad拉拉, value = Oadgarghh...
4: key = 烏asdad拉, value = OadgdsdsaAarghh...
5: key = 烏sad拉grhhk 拉, value = Oadgarghh...
6: key =  烏sa d拉 拉, value = Oadgarghh...

Successfully deleted!
change-1: key = 烏拉拉, value = Oh我好餓h...

base_size = 3, new_size = 43
```
