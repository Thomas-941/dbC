-------------------------------------------------------------------------------------
                                   Changelog - Version 1.3 :
-------------------------------------------------------------------------------------
----------------------------------------
    Publication date : 2024-11-19
----------------------------------------

### Nouvelles fonctionnalités :

- **Gestion des bases de données multiples** : Permet de travailler avec plusieurs bases de données.
- **Commande `USE`** : Sélectionne une base de données active pour les opérations.
- **Création et suppression de bases de données** : Ajout de la possibilité de créer et de supprimer des bases de données.
- **Amélioration de la gestion de la mémoire** : Optimisation de la gestion de la mémoire pour les tables et les colonnes.
- **Sélection avec affichage des résultats** : Ajout de la fonctionnalité pour sélectionner des données et afficher les résultats.
- **Gestion des erreurs améliorée** : Amélioration de la gestion des erreurs pour les commandes de base de données.

### Erreurs connues :

- **Commande `SELECT`** : Les résultats ne sont pas affichés si la condition WHERE n'est pas spécifiée.
- **Commande `DELETE`** : Ne fonctionne pas correctement si des options comme WHERE ou le choix de la table sont ajoutées.
- **Erreur de syntaxe lors de l'insertion** : Se produit si les arguments ne sont pas fournis correctement.
- **Table pleine** : La table atteint le nombre maximum de lignes (actuellement 100 lignes), ce qui empêche l'insertion de nouvelles lignes.

### Améliorations à apporter à l'avenir :

- **Gestion des erreurs d'entrée** : Améliorer la validation des entrées utilisateur.
- **Validation des données** : Ajouter des vérifications pour s'assurer que les données insérées sont valides.

-------------------------------------------------------------------------------------