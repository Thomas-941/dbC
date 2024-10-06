#ifndef REPL_C
#define REPL_C

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>

#include "headers/repl.h"
#include "headers/btree.h"
#include "headers/constants.h"

#define TABLE_MAX_ROWS 100

typedef enum { META_COMMAND_SUCCESS, META_COMMAND_UNRECOGNIZED_COMMAND } MetaCommandResult;

typedef enum { PREPARE_SUCCESS, PREPARE_UNRECOGNIZED_STATEMENT, PREPARE_SYNTAX_ERROR } PrepareResult;

typedef enum { STATEMENT_INSERT, STATEMENT_SELECT, STATEMENT_DELETE } StatementType;

typedef struct {

  StatementType type;
  Row row_to_insert;
  uint32_t id_to_delete;
  char* where_condition;

} Statement;

typedef struct {

  char* buffer;
  size_t buffer_length;
  ssize_t input_length;

} InputBuffer;

typedef enum { EXECUTE_SUCCESS, EXECUTE_TABLE_FULL, EXECUTE_DUPLICATE_KEY, EXECUTE_NOT_FOUND, EXECUTE_UNRECOGNIZED_STATEMENT } ExecuteResult;

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

void print_prompt() { printf("db > "); }

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

void close_input_buffer(InputBuffer* input_buffer) {

  free(input_buffer->buffer);
  free(input_buffer);
  
}

void show_help() {
    printf("Commandes disponibles :\n");
    printf(".exit - Quitte le programme.\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf(".help - Affiche l'aide avec la liste des commandes disponibles.\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf(".tables - Affiche la liste des tables disponibles.\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("insert - Insère une nouvelle entrée dans la table.\n");
    printf("Format : insert <id> <nom_utilisateur> <email>\n");
    printf("Exemple : insert 1 user1 user1@test.fr\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("select - Sélectionne et affiche les entrées de la table, avec une condition optionnelle.\n");
    printf("Format : select * from <nom_table> where <condition>;\n");
    printf("Exemple : select * from users where id = 1;\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("delete - Supprime une entrée de la table par son identifiant.\n");
    printf("Format : delete from <nom_table> where <condition>;\n");
    printf("Exemple : delete from users where id = 1;\n");
    printf("-----------------------------------------------------------------------------------------\n");
}



MetaCommandResult do_meta_command(InputBuffer* input_buffer) {

  if (strcmp(input_buffer->buffer, ".exit") == 0) {

    close_input_buffer(input_buffer);

    exit(EXIT_SUCCESS);

  } else if (strcmp(input_buffer->buffer, ".help") == 0) {

    show_help();

    return META_COMMAND_SUCCESS;

  } else if (strcmp(input_buffer->buffer, ".tables") == 0) {

    printf("Liste des tables :\n");
    printf("- users\n");

    return META_COMMAND_SUCCESS;

  } else {

    printf("Commande non reconnue : '%s'\n\n Pour plus d'information sur les commandes prises en charge et leur utilisation, utilisez .help\n", input_buffer->buffer);

    return META_COMMAND_UNRECOGNIZED_COMMAND;

  }

}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {

  if (strncmp(input_buffer->buffer, "insert", 6) == 0) {

    statement->type = STATEMENT_INSERT;

    char username[MAX_USERNAME_LENGTH + 1];
    char email[MAX_EMAIL_LENGTH + 1];

    int args_assigned = sscanf(input_buffer->buffer, "insert %d %32s %255s", &(statement->row_to_insert.id), username, email);

    if (args_assigned < 3) {
      return PREPARE_SYNTAX_ERROR;
    }


    if (strlen(email) < MAX_EMAIL_LENGTH) {
        strncpy(statement->row_to_insert.email, email, MAX_EMAIL_LENGTH);
        statement->row_to_insert.email[MAX_EMAIL_LENGTH - 1] = '\0';
    } else {
        fprintf(stderr, "Erreur : L'email dépasse la longueur maximale.\n");
        return PREPARE_SYNTAX_ERROR;
    }

    statement->row_to_insert.email[MAX_EMAIL_LENGTH - 1] = '\0';

    strncpy(statement->row_to_insert.username, username, MAX_USERNAME_LENGTH);
    statement->row_to_insert.username[MAX_USERNAME_LENGTH - 1] = '\0';

    return PREPARE_SUCCESS;

  }

  if (strncmp(input_buffer->buffer, "select", 6) == 0) {

    statement->type = STATEMENT_SELECT;
    statement->where_condition = NULL;

    char* where_pos = strstr(input_buffer->buffer, "where");

    if (where_pos != NULL) {
      statement->where_condition = strdup(where_pos + 6);
    }

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

  return PREPARE_UNRECOGNIZED_STATEMENT;

}

ExecuteResult execute_insert(Statement* statement, Table* table) {

  if (table->num_rows >= TABLE_MAX_ROWS) {
    return EXECUTE_TABLE_FULL;
  }

  Row* row_to_insert = &(statement->row_to_insert);
  Node* existing_node = search(table->root, row_to_insert->id);

  if (existing_node != NULL) {
    return EXECUTE_DUPLICATE_KEY;
  }

  table->root = insert(table->root, *row_to_insert);
  table->num_rows += 1;

  return EXECUTE_SUCCESS;

}

void print_row_if_condition(Row* row, const char* condition) {

    if (condition == NULL) {

        printf("(%d, %s, %s)\n", row->id, row->username, row->email);

        return;

    }

    char id_str[20];

    snprintf(id_str, sizeof(id_str), "%d", row->id);

    if (strstr(id_str, condition) != NULL || strstr(row->username, condition) != NULL || strstr(row->email, condition) != NULL) { 
      printf("(%d, %s, %s)\n", row->id, row->username, row->email);
    }

}

ExecuteResult execute_select(Statement* statement, Table* table) {

    void print_row_with_condition(Row* row) {
        print_row_if_condition(row, statement->where_condition);
    }
    
    inorder_traversal(table->root, print_row_with_condition);

    return EXECUTE_SUCCESS;

}

ExecuteResult execute_delete(Statement* statement, Table* table) {

  Node* node_to_delete = search(table->root, statement->id_to_delete);

  if (node_to_delete == NULL) {
    return EXECUTE_NOT_FOUND;
  }

  table->root = delete(table->root, statement->id_to_delete);
  table->num_rows -= 1;

  return EXECUTE_SUCCESS;

}

ExecuteResult execute_statement(Statement* statement, Table* table) {

  switch (statement->type) {

    case (STATEMENT_INSERT):
      return execute_insert(statement, table);
    case (STATEMENT_SELECT):
      return execute_select(statement, table);
    case (STATEMENT_DELETE):
      return execute_delete(statement, table);

  }

  return EXECUTE_UNRECOGNIZED_STATEMENT;

}

void repl(Table* table) {
  InputBuffer* input_buffer = new_input_buffer();

  while (true) {

    print_prompt();
    read_input(input_buffer);

    if (input_buffer->buffer[0] == '.') {

      switch (do_meta_command(input_buffer)) {

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
        printf("Instruction reconnue\n");
        break;
      case (PREPARE_SYNTAX_ERROR):
        printf("Erreur de syntaxe. Impossible d'analyser l'instruction.\n");
        continue;
      case (PREPARE_UNRECOGNIZED_STATEMENT):
        printf("Mot-clé non reconnu au début de '%s'.\n",
               input_buffer->buffer);
        continue;

    }

    ExecuteResult result = execute_statement(&statement, table);

    switch (result) {

      case (EXECUTE_SUCCESS):
        printf("Exécuté avec succès.\n");
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

#endif