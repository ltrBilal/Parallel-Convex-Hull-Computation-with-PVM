#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "pvm3.h"
#include "point.h"

// Fonction pour calculer le point d'enveloppe (Upper Hull)
void calcul_pointUH(pb_t *pb)
{
    pb->pts1 = point_UH(pb->pts1);
    pb->type = PB_FUSION;
}

// Fonction pour fusionner les données
void merge_data(pb_t *pb)
{
    pb->nbPoints1 = pb->nbPoints1 + pb->nbPoints2; 	
    pb->pts1 = point_merge_UH(pb->pts1, pb->pts2);	
    point_free(pb->pts2);
    pb->pts2 = NULL;
    pb->nbPoints2 = 0;
}

// Programme esclave
int main()
{
    extern pb_t *receive_pb();
    int parent, sender[1]; // Identifiant du processus parent et envoyeur
    pb_t *pb; // Problème courant à traiter

    parent = pvm_parent(); // Récupération de l'identifiant du processus parent

    // Boucle pour traiter les problèmes reçus jusqu'à ce qu'il n'y en ait plus
    while ((pb = receive_pb(parent, sender)) != NULL) {
        if (pb->type == PB_UH) { // Si le problème est de type calcul d'enveloppe
            calcul_pointUH(pb); // Calcul du point d'enveloppe
        } else { // Sinon, si le problème est de type fusion
            merge_data(pb); // Fusion des données
        }
        // Envoi de la solution au processus parent
        send_pb(parent, pb);
    }

    pvm_exit(); // Terminaison du processus esclave
    exit(0);
}
