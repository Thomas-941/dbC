#ifndef REPL_C
#define REPL_C

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include "headers/repl.h"
#include "headers/btree.h"
#include "headers/constants.h"

// Déclaration des variables globales
Database* current_db = NULL; // Initialiser la base de données actuelle à NULL

// Fonction pour créer un nouvel InputBuffer
InputBuffer* new_input_buffer() {

    InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));

    if (input_buffer == NULL) {
        fprintf(stderr, "Erreur : Échec de l'allocation mémoire pour le buffer d'entrée.\n");
        exit(EXIT_FAILURE);
    }

    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;

}

// Fonction pour afficher le terminal
void print_prompt() {

    if (current_db != NULL) {
        printf("db (%s) > ", current_db->name); // Affiche le nom de la base de donnée sélectionnée
    } else {
        printf("db > "); // Par défaut
    }

}

// Fonction pour lire l'entrée de l'utilisateur
void read_input(InputBuffer* input_buffer) {

    size_t buffer_size = 256;

    input_buffer->buffer = malloc(buffer_size);

    if (input_buffer->buffer == NULL) {

        fprintf(stderr, "Erreur lors de l'allocation de mémoire pour le buffer d'entrée.\n");

        exit(EXIT_FAILURE);

    }

    if (fgets(input_buffer->buffer, buffer_size, stdin) == NULL) {

        printf("Erreur lors de la lecture de l'entrée\n");

        exit(EXIT_FAILURE);

    }

    input_buffer->input_length = strlen(input_buffer->buffer);

    if (input_buffer->buffer[input_buffer->input_length - 1] == '\n') {

        input_buffer->buffer[input_buffer->input_length - 1] = '\0';
        input_buffer->input_length--;

    }

}

// Fonction pour fermer l'InputBuffer
void close_input_buffer(InputBuffer* input_buffer) {

    free(input_buffer->buffer);
    free(input_buffer);

}

// Fonction pour afficher l'aide
void show_help() {

    printf("Commandes disponibles :\n");
    printf(".exit - Quitte le programme.\n");
    printf(".help - Affiche l'aide avec la liste des commandes disponibles.\n");
    printf(".tables - Affiche la liste des tables disponibles.\n");
    printf(".columns <nom_table> - Affiche les colonnes de la table spécifiée.\n");
    printf("insert - Insère une nouvelle entrée dans la table.\n");
    printf("Format : insert <id> <nom_utilisateur> <email>\n");
    printf("select - Sélectionne et affiche les entrées de la table, avec une condition optionnelle.\n");
    printf("delete - Supprime une entrée de la table par son identifiant.\n");
    printf("use <nom_base> - Sélectionne une base de données.\n");

}

// Fonction pour créer une base de données
void create_database(const char* db_name) {

    // Vérifier si la base de données existe déjà
    if (database_exists(db_name)) {
        printf("La base de données '%s' existe déjà.\n", db_name);
        return;
    }

    // Créer la base de données
    Database* db = create_database(db_name);
    printf("Base de données '%s' créée avec succès.\n", db_name);

}

// Fonction pour lister les bases de données
void list_databases() {

    DIR* dir = opendir("store");
    struct dirent* entry;

    if (dir == NULL) {
        printf("Erreur lors de l'ouverture du dossier 'store'.\n");
        return;
    }

    printf("Bases de données disponibles :\n");

    while ((entry = readdir(dir)) != NULL) {

        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            printf("- %s\n", entry->d_name);
        }

    }

    closedir(dir);

}

// Fonction pour supprimer une base de données
void delete_database(const char* db_name) {

    char path[256];

    snprintf(path, sizeof(path), "store/%s", db_name);

    if (rmdir(path) == 0) {
        printf("Base de données '%s' supprimée avec succès.\n", db_name);
    } else {
        printf("Erreur lors de la suppression de la base de données '%s'.\n", db_name);
    }

}

// Fonction pour traiter les méta-commandes
MetaCommandResult do_meta_command(InputBuffer* input_buffer, Database* db) {

    if (strncmp(input_buffer->buffer, "use ", 4) == 0) {

        char db_name[MAX_DB_NAME_LENGTH];

        sscanf(input_buffer->buffer + 4, "%s", db_name);

        current_db = create_database(db_name); // Sélectionner la base de données

        return META_COMMAND_SUCCESS;

    }

    if (strcmp(input_buffer->buffer, ".exit") == 0) {

        close_input_buffer(input_buffer);

        exit(EXIT_SUCCESS);

    } else if (strcmp(input_buffer->buffer, ".help") == 0) {

        show_help();

        return META_COMMAND_SUCCESS;

    } else if (strcmp(input_buffer->buffer, ".tables") == 0) {

        show_tables(db->tables);

        return META_COMMAND_SUCCESS;

    } else if (strncmp(input_buffer->buffer, ".create ", 8) == 0) {

        char db_name[MAX_DB_NAME_LENGTH];

        sscanf(input_buffer->buffer + 8, "%s", db_name);

        create_database(db_name);

        return META_COMMAND_SUCCESS;

    } else if (strncmp(input_buffer->buffer, ".delete ", 8) == 0) {

        char db_name[MAX_DB_NAME_LENGTH];

        sscanf(input_buffer->buffer + 8, "%s", db_name);

        delete_database(db_name);

        return META_COMMAND_SUCCESS;

    } else {
        printf("Commande non reconnue : '%s'\n", input_buffer->buffer);
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }

}

// Fonction pour préparer une instruction
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {

    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {

        statement->type = STATEMENT_INSERT;

        char username[MAX_USERNAME_LENGTH + 1];
        char email[MAX_EMAIL_LENGTH + 1];

        int args_assigned = sscanf(input_buffer->buffer, "insert %d %32s %255s", &(statement->row_to_insert.id), username, email);

        if (args_assigned < 3) {
            return PREPARE_SYNTAX_ERROR;
        }

        strncpy(statement->row_to_insert.username, username, MAX_USERNAME_LENGTH);
        strncpy(statement->row_to_insert.email, email, MAX_EMAIL_LENGTH);

        return PREPARE_SUCCESS;

    }

    if (strncmp(input_buffer->buffer, "select", 6) == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    if (strncmp(input_buffer->buffer, "delete", 6) == 0) {

        statement->type = STATEMENT_DELETE;

        int args_assigned = sscanf(input_buffer->buffer, "delete %d", &(statement->id_to_delete));

        if (args_assigned < 1) {
            return PREPARE_SYNTAX_ERROR;
        }

        return PREPARE_SUCCESS;

    }

    if (strncmp(input_buffer->buffer, "use", 3) == 0) {
        statement->type = STATEMENT_USE;
        sscanf(input_buffer->buffer + 4, "%s", statement->db_name);
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;

}

// Fonction pour exécuter une instruction
ExecuteResult execute_statement(Statement* statement, Database* db) {

    switch (statement->type) {

        case (STATEMENT_INSERT):
            return execute_insert(statement, db);
        case (STATEMENT_SELECT):
            return execute_select(statement, db);
        case (STATEMENT_DELETE):
            return execute_delete(statement, db);
        case (STATEMENT_USE):
            return execute_use(statement, db);

    }

    return EXECUTE_UNRECOGNIZED_STATEMENT;

}

// Fonction principale du REPL
void repl(Database* db) {

    InputBuffer* input_buffer = new_input_buffer();

    while (true) {

        print_prompt();
        read_input(input_buffer);

        // Vérification de si une base de données est sélectionnée ou non
        if (current_db == NULL) {

            if (strncmp(input_buffer->buffer, "use ", 4) == 0 || 
                strcmp(input_buffer->buffer, ".create") == 0 || 
                strcmp(input_buffer->buffer, ".delete") == 0 || 
                strcmp(input_buffer->buffer, ".help") == 0 || 
                strcmp(input_buffer->buffer, ".exit") == 0) {

                // Autoriser les commandes spécifiques
                switch (do_meta_command(input_buffer, db)) {

                    case (META_COMMAND_SUCCESS):
                        continue;
                    case (META_COMMAND_UNRECOGNIZED_COMMAND):
                        printf("Commande non reconnue '%s'\n", input_buffer->buffer);
                        continue;

                }

            } else {
                printf("Aucune base de données sélectionnée. Utilisez 'use <nom_base>' pour sélectionner une base de données.\n");
                continue;
            }

        } else {

            // Si une base de données est sélectionnée, traiter les commandes normalement
            if (input_buffer->buffer[0] == '.') {

                switch (do_meta_command(input_buffer, current_db)) {

                    case (META_COMMAND_SUCCESS):
                        continue;
                    case (META_COMMAND_UNRECOGNIZED_COMMAND):
                        printf("Commande non reconnue '%s'\n", input_buffer->buffer);
                        continue;

                }

            }

            Statement statement;

            switch (prepare_statement(input_buffer, &statement)) {

                case (PREPARE_SUCCESS):
                    break;
                case (PREPARE_SYNTAX_ERROR):
                    printf("Erreur de syntaxe. Impossible d'analyser l'instruction.\n");
                    continue;
                case (PREPARE_UNRECOGNIZED_STATEMENT):
                    printf("Mot-clé non reconnu au début de '%s'.\n", input_buffer->buffer);
                    continue;

            }

            ExecuteResult result = execute_statement(&statement, current_db);

            switch (result) {

                case (EXECUTE_SUCCESS):
                    break;
                case (EXECUTE_TABLE_FULL):
                    printf("Erreur : La table est pleine.\n");
                    break;
                case (EXECUTE_DUPLICATE_KEY):
                    printf("Erreur : Clé en double.\n");
                    break;
                case (EXECUTE_NOT_FOUND):
                    printf("Erreur : Élément non trouvé.\n");
                    break;
                case (EXECUTE_UNRECOGNIZED_STATEMENT):
                    printf("Erreur : Instruction non reconnue.\n");
                    break;

            }

            if (statement.where_condition) {
                free(statement.where_condition);
            }

        }

    }

}

// Fonction pour afficher les tables
void show_tables(Table* tables) {

    printf("Liste des tables :\n");

    Table* current_table = tables;

    while (current_table != NULL) {
        printf("%s\n", current_table->name);
        current_table = current_table->next;
    }

}

// Fonction pour afficher les colonnes d'une table
void show_columns(Table* table) {

    if (table == NULL) {
        printf("La table est vide.\n");
        return;
    }

    printf("Colonnes de la table %s :\n", table->name);

    Column* current_column = table->columns;

    while (current_column != NULL) {
        printf("%s\n", current_column->name);
        current_column = current_column->next;
    }

}

// Fonction pour ajouter une table
void add_table(Table** tables) {

    char table_name[MAX_USERNAME_LENGTH];

    printf("Entrez le nom de la nouvelle table : ");
    scanf("%s", table_name);

    Table* new_table = malloc(sizeof(Table));

    if (new_table == NULL) {
        fprintf(stderr, "Erreur : Échec de l'allocation mémoire pour la nouvelle table.\n");
        return;
    }

    strcpy(new_table->name, table_name);

    new_table->num_rows = 0;
    new_table->root = NULL;
    new_table->next = *tables;
    *tables = new_table;

    printf("Table '%s' ajoutée avec succès.\n", table_name);

}

// Fonction pour modifier une colonne
void modify_column(Table* table, const char* column_name, const char* new_type) {

    Column* current_column = table->columns;

    while (current_column != NULL) {

        if (strcmp(current_column->name, column_name) == 0) {

            strncpy(current_column->type, new_type, MAX_USERNAME_LENGTH);

            current_column->type[MAX_USERNAME_LENGTH - 1] = '\0';

            printf("Modification du type de la colonne '%s' réussie.\n", column_name);

            return;

        }

        current_column = current_column->next;

    }

    printf("La colonne '%s' n'a pas été trouvée dans la table '%s'.\n", column_name, table->name);

}

// Fonction pour supprimer une colonne
void remove_column(Table* table, const char* column_name) {

    Column** current_column_ptr = &table->columns;

    while (*current_column_ptr != NULL) {

        if (strcmp((*current_column_ptr)->name, column_name) == 0) {

            Column* temp = *current_column_ptr;
            *current_column_ptr = (*current_column_ptr)->next;

            free(temp);

            printf("La colonne '%s' a été supprimée avec succès.\n", column_name);

            return;

        }

        current_column_ptr = &((*current_column_ptr)->next);

    }

    printf("La colonne '%s' n'a pas été trouvée dans la table '%s'.\n", column_name, table->name);

}

// Fonction pour ajouter une table avec des colonnes
void add_table_with_columns(Table** tables) {

    char table_name[MAX_USERNAME_LENGTH];

    printf("Entrez le nom de la nouvelle table : ");
    scanf("%s", table_name);

    Table* new_table = malloc(sizeof(Table));

    if (new_table == NULL) {
        fprintf(stderr, "Erreur : Échec de l'allocation mémoire pour la nouvelle table.\n");
        return;
    }

    strcpy(new_table->name, table_name);

    new_table->num_rows = 0;
    new_table->root = NULL;
    new_table->columns = NULL;
    new_table->next = *tables;
    *tables = new_table;

    char column_name[MAX_USERNAME_LENGTH];
    char column_type[MAX_USERNAME_LENGTH];

    while (true) {

        printf("Entrez le nom de la colonne (ou 'fin' pour terminer) : ");
        scanf("%s", column_name);

        if (strcmp(column_name, "fin") == 0) break;

        printf("Entrez le type de la colonne : ");
        scanf("%s", column_type);

        add_column(new_table, column_name, column_type);

    }

    printf("Table '%s' ajoutée avec succès.\n", table_name);

}

// Fonction pour exécuter la sélection de la base de données
ExecuteResult execute_use(Statement* statement, Database* db) {
    Database* target_db = create_database(statement->db_name); // Créer ou charger la base de données

    if (target_db == NULL) {
        return EXECUTE_NOT_FOUND;
    }

    current_db = target_db;
    printf("Base de données sélectionnée : %s\n", current_db->name);
    
    return EXECUTE_SUCCESS;
}

#endif