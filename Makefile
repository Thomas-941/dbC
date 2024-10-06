CC = gcc
CFLAGS = -Wall -Wextra -g -I./src/headers

SRCS = src/btree.c src/repl.c src/table.c src/main.c src/tests.c
OBJS = $(SRCS:.c=.o)
TARGET_TEST = test.exe
TARGET_MAIN = dbC.exe

# Règle pour compiler les fichiers .c en .o
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Règle pour compiler le programme principal
$(TARGET_MAIN): src/main.o src/table.o src/btree.o src/repl.o
	@echo "Création de l'exécutable $(TARGET_MAIN)"
	$(CC) $(CFLAGS) -o $@ $^

# Règle pour compiler le test
$(TARGET_TEST): src/tests.o src/btree.o src/repl.o src/table.o
	@echo "Création de l'exécutable $(TARGET_TEST)"
	$(CC) $(CFLAGS) -o $@ $^

# Ajout de la cible 'all'
all: $(TARGET_MAIN) $(TARGET_TEST)
	@echo "Compilation terminée."

# Règle pour nettoyer les fichiers générés
clean:
	rm -f src/*.o $(TARGET_TEST) $(TARGET_MAIN)

# Règle pour compiler main.o
src/main.o: src/main.c
	$(CC) $(CFLAGS) -c $< -o $@ 

# Règle pour compiler les tests
src/tests.o: src/tests.c
	$(CC) $(CFLAGS) -c $< -o $@ 

# Règle pour compiler btree.o
src/btree.o: src/btree.c
	$(CC) $(CFLAGS) -c $< -o $@ 

# Règle pour compiler repl.o
src/repl.o: src/repl.c
	$(CC) $(CFLAGS) -c $< -o $@ 

# Règle pour compiler table.o
src/table.o: src/table.c
	$(CC) $(CFLAGS) -c $< -o $@ 
