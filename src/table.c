#include "headers/table.h"
#include <stdlib.h>

Table* new_table() {

    Table* table = malloc(sizeof(Table));

    if (table == NULL) {
        return NULL;
    }

    table->num_rows = 0;
    table->root = NULL;

    return table;
    
}