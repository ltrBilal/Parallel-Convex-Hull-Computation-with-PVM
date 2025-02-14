#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "pvm3.h" 
#include "point.h"

// Fonction de comparaison pour trier les points par leur coordonnée x
static int compareX(point **i, point **j)
{ return (*i)->x - (*j)->x; }

// Crée une structure de points en triant les points par leur coordonnée x
point *creer_S(int nbPoints, int table_x[], int table_y[])
{
    // Alloue de la mémoire pour un tableau de pointeurs de points
    point **pts;
    pts = (point **)malloc(nbPoints * sizeof(point*));
    // Initialise chaque point dans le tableau
    for(int i = 0; i < nbPoints; i++){
        pts[i] = point_alloc(); // Alloue de la mémoire pour un point
        pts[i]->x = table_x[i]; // Assigne la coordonnée x du point
        pts[i]->y = table_y[i]; // Assigne la coordonnée y du point
    }
    // Trie le tableau de points par leur coordonnée x
    qsort(pts, nbPoints, sizeof(point *), (__compar_fn_t) compareX);
    // Lie les points entre eux avec l'attribut next
    for(int i= 0; i < nbPoints - 1; i++){
        pts[i]->next = pts[i+1]; // Lie le point courant au suivant
    }
    // Retourne le premier point de l'ensemble
    return pts[0];
}

// Alloue de la mémoire pour une structure de problème
pb_t *pb_alloc()
{
    pb_t *pb;
    pb = (pb_t *)malloc(sizeof(pb_t)); // Alloue de la mémoire pour une structure de problème
    pb->nbPoints1 = 0; // Initialise la taille du premier ensemble de points
    pb->nbPoints2 = 0; // Initialise la taille du deuxième ensemble de points
    pb->type = 0; // Initialise le type de problème
    pb->pts1 = point_alloc();
    pb->pts2 = point_alloc();
    return pb;
}

// Libère la mémoire allouée pour une structure de problème
void pb_free(pb_t *pb)
{
    if (pb->pts1) point_free(pb->pts1); // Libère la mémoire allouée pour le premier ensemble de points
    if (pb->pts2) point_free(pb->pts2); // Libère la mémoire allouée pour le deuxième ensemble de points
    free(pb); // Libère la mémoire allouée pour la structure de problème
}

// Copie les coordonnées des points dans deux tableaux
void copier_cordonnee(point *pts, int table_x[], int table_y[])
{
    point *p;
    int i=0;
    // Parcourt tous les points et copie leurs coordonnées dans les tableaux
    for (p=pts; p!=NULL; p=p->next) {
        table_x[i] = p->x; // Copie la coordonnée x du point
        table_y[i] = p->y; // Copie la coordonnée y du point
        i++; // Incrémente l'indice pour le prochain point
    }
}

// Envoie une structure de problème à un processus spécifique
void send_pb(int tid, pb_t *pb)
{
    pvm_initsend(PvmDataDefault); // Initialise l'envoi de données
    pvm_pkint(&(pb->nbPoints1), 1, 1); // Empaquette la taille du premier ensemble de points
    pvm_pkint(&(pb->nbPoints2), 1, 1); // Empaquette la taille du deuxième ensemble de points
    pvm_pkint(&(pb->type), 1, 1); // Empaquette le type de problème
    // Préparation des données à envoyer
    int table_x[pb->nbPoints1], table_y[pb->nbPoints1];
    copier_cordonnee(pb->pts1, table_x, table_y); // Copie les coordonnées du premier ensemble de points
    pvm_pkint(table_x, pb->nbPoints1, 1); // Empaquette les coordonnées x du premier ensemble de points
    pvm_pkint(table_y, pb->nbPoints1, 1); // Empaquette les coordonnées y du premier ensemble de points
    if (0 != pb->nbPoints2){
        int table_x2[pb->nbPoints2], table_y2[pb->nbPoints2];
        copier_cordonnee(pb->pts2, table_x2, table_y2); // Copie les coordonnées du deuxième ensemble de points
        pvm_pkint(table_x2, pb->nbPoints2, 1); // Empaquette les coordonnées x du deuxième ensemble de points
        pvm_pkint(table_y2, pb->nbPoints2, 1); // Empaquette les coordonnées y du deuxième ensemble de points
    }
    // Envoi du message
    pvm_send(tid, MSG_PB);
    pb_free(pb);
}  

// Reçoit une structure de problème d'un processus spécifique
pb_t *receive_pb(int tid, int *sender)
{
    int tag, taille, bufid;
    bufid = pvm_recv(tid, -1); // Reçoit un message d'un processus spécifique
    pvm_bufinfo(bufid, &taille, &tag, sender); // Obtient des informations sur le message reçu
    if (tag != MSG_PB) return NULL; // Vérifie si le message est du type attendu
    pb_t *pb = pb_alloc(); // Alloue de la mémoire pour une structure de problème
    pvm_upkint(&(pb->nbPoints1), 1, 1); // Démpaquette la taille du premier ensemble de points
    pvm_upkint(&(pb->nbPoints2), 1, 1); // Démpaquette la taille du deuxième ensemble de points
    pvm_upkint(&(pb->type), 1, 1); // Démpaquette le type de problème
    pb->pts1 = point_alloc(); // Alloue de la mémoire pour le premier ensemble de points
    int table_x[pb->nbPoints1], table_y[pb->nbPoints1];
    pvm_upkint(table_x, pb->nbPoints1, 1); // Démpaquette les coordonnées x du premier ensemble de points
    pvm_upkint(table_y, pb->nbPoints1, 1); // Démpaquette les coordonnées y du premier ensemble de points
    pb->pts1 = creer_S(pb->nbPoints1, table_x, table_y); // Crée le premier ensemble de points
    if (0 != pb->nbPoints2) {
        pb->pts2 = point_alloc(); // Alloue de la mémoire pour le deuxième ensemble de points
        int table_x2[pb->nbPoints2], table_y2[pb->nbPoints2];
        pvm_upkint(table_x2, pb->nbPoints2, 1); // Démpaquette les coordonnées x du deuxième ensemble de points
        pvm_upkint(table_y2, pb->nbPoints2, 1); // Démpaquette les coordonnées y du deuxième ensemble de points
        pb->pts2 = creer_S(pb->nbPoints2, table_x2, table_y2); // Crée le deuxième ensemble de points
    }
    return pb;
}
