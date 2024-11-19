#include "headers/table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include <sys/stat.h>
#include <dirent.h>

// Fonction pour créer une nouvelle table
Table* new_table() {

    Table* table = malloc(sizeof(Table));

    if (table == NULL) {
        return NULL;
    }

    table->num_rows = 0;
    table->root = NULL;
    table->columns = NULL;

    return table;

}

// Fonction pour ajouter une colonne à une table
void add_column(Table* table, const char* column_name, const char* column_type) {

    Column* new_column = malloc(sizeof(Column));

    if (new_column == NULL) {
        fprintf(stderr, "Erreur : Échec de l'allocation mémoire pour la nouvelle colonne.\n");
        return;
    }

    strncpy(new_column->name, column_name, MAX_USERNAME_LENGTH);
    strncpy(new_column->type, column_type, MAX_USERNAME_LENGTH);

    new_column->next = table->columns;
    table->columns = new_column;

}

// Fonction pour charger une table depuis un fichier
void load_table(Table* table, const char* path) {

    json_error_t error;
    json_t* root = json_load_file(path, 0, &error);
    
    if (!root) {
        fprintf(stderr, "Erreur lors du chargement du fichier %s : %s\n", path, error.text);
        return;
    }

    json_t* name_json = json_object_get(root, "name");

    if (json_is_string(name_json)) {
        strncpy(table->name, json_string_value(name_json), MAX_USERNAME_LENGTH);
    }

    json_t* num_rows_json = json_object_get(root, "num_rows");

    if (json_is_integer(num_rows_json)) {
        table->num_rows = (uint32_t)json_integer_value(num_rows_json);
    }

    // Logique pour lire les lignes de la table et les insérer dans l'arbre binaire
    // ...

    json_decref(root); // Libérer la mémoire

}

// Fonction pour charger une base de données
void load_database(Database* db) {

    char path[256];

    snprintf(path, sizeof(path), "store/%s", db->name);

    if (!directory_exists(path)) {
        fprintf(stderr, "Le dossier de la base de données %s n'existe pas.\n", db->name);
        return;
    }

    struct dirent* entry;

    DIR* dp = opendir(path);

    if (dp == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du dossier %s.\n", path);
        return;
    }

    while ((entry = readdir(dp))) {

        if (entry->d_type == DT_REG && strstr(entry->d_name, ".json")) {

            Table* new_table = new_table();

            snprintf(path, sizeof(path), "store/%s/%s", db->name, entry->d_name);

            load_table(new_table, path);

            new_table->next = db->tables; // Ajouter la table à la liste des tables
            db->tables = new_table;

        }

    }

    closedir(dp);
}

// Fonction pour créer une base de données
Database* create_database(const char* db_name) {

    Database* db = malloc(sizeof(Database));

    if (db == NULL) {
        fprintf(stderr, "Erreur : Échec de l'allocation mémoire pour la base de données.\n");
        return NULL;
    }

    strncpy(db->name, db_name, MAX_DB_NAME_LENGTH);
    db->tables = NULL;

    create_database_directory(db_name); // Créer le dossier de la base de données
    load_database(db); // Charger les tables depuis les fichiers JSON

    return db;

}

// Fonction pour sauvegarder une table
void save_table(Table* table) {

    char path[256];

    snprintf(path, sizeof(path), "store/%s/%s.json", table->name); // Chemin du fichier JSON

    FILE* file = fopen(path, "w");

    if (file == NULL) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier %s pour l'écriture.\n", path);
        return;
    }

    fprintf(file, "{\n");
    fprintf(file, "  \"name\": \"%s\",\n", table->name);
    fprintf(file, "  \"num_rows\": %d,\n", table->num_rows);
    // Écrire les lignes et autres informations ici
    fprintf(file, "}\n");

    fclose(file);

}

// Fonction pour sauvegarder une base de données
void save_database(Database* db) {

    Table* current_table = db->tables;

    while (current_table != NULL) {
        save_table(current_table);
        current_table = current_table->next;
    }

}

// Fonction pour supprimer une table
void delete_table(Database* db, const char* table_name) {

    Table* current = db->tables;
    Table* previous = NULL;

    while (current != NULL) {

        if (strcmp(current->name, table_name) == 0) {

            if (previous == NULL) {
                db->tables = current->next; // Supprimer la première table
            } else {
                previous->next = current->next; // Supprimer une table intermédiaire
            }

            free_table(current); // Libérer la mémoire de la table
            return;
            
        }

        previous = current;
        current = current->next;

    }

    printf("Table '%s' non trouvée.\n", table_name);

}