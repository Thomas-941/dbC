#ifndef REPL_H
#define REPL_H

#include "table.h"
#include "btree.h"
#include <stdbool.h>
#include <string.h>

// Enumération des résultats de commande
typedef enum { META_COMMAND_SUCCESS, META_COMMAND_UNRECOGNIZED_COMMAND } MetaCommandResult;

// Structure pour l'input buffer
typedef struct {

    char* buffer;
    size_t buffer_length;
    ssize_t input_length;

} InputBuffer;

// Déclarations des fonctions
void show_tables(Table* tables);
void show_columns(Table* table);
void repl(Database* db);
void add_table(Table** tables);
void add_table_with_columns(Table** tables);
MetaCommandResult do_meta_command(InputBuffer* input_buffer, Database* db);
void create_database(const char* db_name);
void delete_database(const char* db_name);
void list_databases();

#endif