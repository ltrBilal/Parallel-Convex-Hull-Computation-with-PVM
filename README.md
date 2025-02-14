# Parallel-Convex-Hull-Computation-with-PVM

## Description

Ce projet a pour objectif d'implémenter un algorithme parallélisé pour calculer l'enveloppe convexe haute d'un ensemble de points en utilisant une approche Diviser pour Régner. Le projet a été réalisé dans le cadre de Programmation Parallèle Haute Performance à l'Université de Bretagne Occidentale.

## Algorithme

- L'algorithme suit les étapes suivantes :

1. Génération aléatoire d'un ensemble de points.

2. Diviser l'ensemble en deux sous-ensembles.

3. Calculer l'enveloppe convexe haute pour chaque sous-ensemble en parallèle.

4. Fusionner les deux sous-enveloppes pour obtenir l'enveloppe finale.

- Deux versions parallèles sont implémentées :

    - Version simple : Création récursive de processus fils pour diviser et fusionner les solutions.

    - Version Maître-Esclave : Un processus maître gère la distribution et la fusion des sous-enveloppes entre plusieurs processus esclaves.

## Technologies Utilisées

- C pour l'implémentation de l'algorithme

- PVM (Parallel Virtual Machine) pour la parallélisation

- Gnuplot pour la visualisation des résultats