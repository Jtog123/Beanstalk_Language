#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"

#define TABLE_MAX_LOAD 0.75

void initTable(Table* table) {
    table->count = 0;
    table->capacity = 0;
    table->entries = NULL;
}

void freeTable(Table* table) {
    FREE_ARRAY(Entry, table->entries, table->capacity);
    initTable(table);
}

Entry* findEntry(Table* entries, int capacity, ObjString* key) {
    //fold the hash code to grab the index to either find or place the entry
    uint32_t index = key->hash % capacity;
    Entry* tombstone = NULL;
    for(;;) {
        Entry* entry = &entries[index]; // look up the entry in the array(buckets)
        if(entry->key == NULL) {
            if(IS_NIL(entry->value)) {
                //empty entry
                return tombstone != NULL ? tombstone : entry;
            } else {
                //found enty
                if(tombstone == NULL) tombstone = entry;
            }
        } else if(entry->key == key) {
            //found key
            return entry;
        }

        //if we dont find the key or we dont have an empty slot, linear probe forward
        index = (index + 1) % capacity;
    }


}

bool tableGet(Table* table, ObjString* key, Value* value) {
    if(table->count == 0) return false;

    Entry* entry = findEntry(table->entries, table->capacity, key);
    if(entry->key == NULL) return false;

    //dereferenec and assign
    *value = entry->value;
    return true;
}

static void adjustCapacity(Table* table, int capacity) {
    //create new array of entries at the new size, init to empty
    Entry* entries = ALLOCATE(Entry, capacity);
    for(int i = 0 ; i < capacity; i++) {
        entries[i].key = NULL;
        entries[i].value = NIL_VAL;
    }

    table->count = 0;
    //reindexes old entries over to new array
    for(int i = 0; i < table->capacity; i++) {
        Entry* entry = &table->entries[i];
        if(entry->key == NULL) continue;
        //if entry is not NULL, hash stays the same (its cached) but capacity changes, so we get new index
        Entry* dest = findEntry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        table->count++;
    }

    //assign the table the newly resized variables
    table->entries = entries;
    table->capacity = capacity;
}


bool tableSet(Table* table, ObjString* key, Value value) {
    //check the load factor resize table if needed
    if(table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
        int capacity = GROW_CAPACITY(table->capacity);
        adjustCapacity(table, capacity);
    }

    //see where enrty will go in thable
    Entry* entry = findEntry(table->entries, table->capacity, key);
    bool isNewKey = entry->key == NULL; // is it a new entry?
    if(isNewKey && IS_NIL(entry->value)) table->count++;
    //if(isNewKey) table->count++;

    entry->key = key;
    entry->value = value;
    return isNewKey;
}

bool tableDelete(Table* table, ObjString* key) {
    if(table->count == 0) return false;

    //find entry
    Entry* entry = findEntry(table->entries, table->capacity, key);
    if(entry->key == NULL) return false;

    //place tombstone
    entry->key = NULL;
    entry->value = BOOL_VAL(true);
    return true;
}

//copy all entries from one table to another
void tableAddAll(Table* from, Table* to) {
    for(int i = 0 ; i < from->capacity; i++) {
        Entry* entry = &from->entries[i];
        if(entry->key != NULL) {
            tableSet(to, entry->key, entry->value);
        }
    }
}

ObjString* tableFindString(Table* table, const char* chars, int length, uint32_t hash) {
    if(table->count == 0) return NULL;

    uint32_t index = hash % table->capacity;
    for(;;) {
        Entry* entry = &table->entries[index];
        if(entry->key == NULL) {
            // stop if we find an empty nontombstone entry
            if(IS_NIL(entry->value)) return NULL;
        } else if(entry->key->length == length && entry->key->hash == hash && memcmp(entry->key->chars, chars, length) == 0) {
            //found it
            return entry->key;
        }

        index = (index + 1) % table->capacity;
    }
}