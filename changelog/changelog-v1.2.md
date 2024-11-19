-------------------------------------------------------------------------------------
                                   Changelog - Version 1.2 :
-------------------------------------------------------------------------------------
----------------------------------------
    Publication date : 2024-10-06
----------------------------------------

Status :

    - programmes compilable (test et programme)
    - Options implémentés : INSERT, SELECT, DELETE, WHERE (pour le moment seule une table est disponible et hard codée (users))
    - Programme compilable via Makefile
    - Gestion de la mémoire pour les noeuds de l'arbre binaire OK

Erreurs : 
    - Lors de l'utilisation du where la commande est reconnu mais aucun retour n'est affiché.
    - le DELETE ne fonctionne pas si on met des options comme where ou le choix de la table (utilisation actuelle : delete <id>)
    - Erreur de syntaxe lors de l'insertion si les arguments ne sont pas fournis correctement
    - La table est pleine si le nombre maximum de lignes est atteint (actuellement 100 lignes)

-------------------------------------------------------------------------------------