# Système de Base de Données en C

L'objectif de ce TP est de créer un système de base de données en C et de le rendre persistant (BDD et gestion des commandes SQL).

## Table des Matières

- [Auteur et Version](#auteur-et-version)
- [Compilation](#compilation)
- [Stockage et Persistance](#stockage-et-persistance)
- [Fonctionnalités](#fonctionnalités)
- [Changelog](#changelog)
- [Historique des Changelogs](#historique-des-changelogs)
- [Structure du Projet](#structure-du-projet)

---

## Auteur et Version

**Auteur :** Thomas GRUNDSZTEJN - Defl3ct  
**Version :** 1.3 (2024-10-06)

---

## Compilation

Pour compiler le programme, utilisez la commande :

```
make
```

Lancez le programme en exécutant :

```
./dbC.exe
```

Pour compiler le programme et le programme de test, utilisez la commande :

```
make all
```

Lancez le programme en exécutant :

```
./dbC.exe
```

Lancez le programme de test en exécutant :

```
./test.exe
```

---

## Stockage et Persistance

Les fichiers liés à la BDD seront stockés dans le dossier `store`. (Non implémenté pour le moment)

---

## Fonctionnalités

Ce système de base de données prend en charge les fonctionnalités suivantes :

- **Création de bases de données** : Vous pouvez créer une nouvelle base de données qui sera stockée dans le dossier `store`.
- **Gestion des tables** : Ajoutez, modifiez et supprimez des tables dans la base de données.
- **Gestion des colonnes** : Ajoutez, modifiez et supprimez des colonnes dans les tables.
- **Insertion, sélection et suppression de données** : Insérez de nouvelles lignes, sélectionnez des données existantes et supprimez des lignes par leur identifiant.
- **Commandes REPL** : Utilisez une interface en ligne de commande pour interagir avec la base de données.

### Commandes disponibles

- `.exit` : Quitte le programme.
- `.help` : Affiche l'aide avec la liste des commandes disponibles.
- `.tables` : Affiche la liste des tables disponibles.
- `.columns <nom_table>` : Affiche les colonnes de la table spécifiée.
- `insert <id> <nom_utilisateur> <email>` : Insère une nouvelle entrée dans la table.
- `select` : Sélectionne et affiche les entrées de la table, avec une condition optionnelle.
- `delete <id>` : Supprime une entrée de la table par son identifiant.
- `use <nom_base>` : Sélectionne une base de données.

---

## Changelog

Consultez le fichier `current_changelog.md` pour les mises à jour et les modifications apportées au système.

---

## Historique des Changelogs

Pour l'historique des changelogs, consultez le dossier `changelog`.

---

## Structure du Projet

Le projet est structuré comme suit :

- `src/headers/` : Contient les fichiers d'en-tête pour les structures de données et les déclarations de fonctions.
- `src/` : Contient les fichiers source pour l'implémentation des fonctionnalités.
- `src/tests.c` : Contient les tests unitaires pour vérifier le bon fonctionnement des différentes fonctionnalités.
- `README.md` : Ce fichier, qui fournit des informations sur le projet.

---
