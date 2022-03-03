#include "formule1.h"
#include "utile.h"

void afficheur(voiture *cars, int shmid, int semid, struct sembuf *sb1, struct sembuf *sb2, union semun arg, int duree, conservation *resultat_intermediaire, int nbr_voitures, int course) {
	/* Fonction représentant l'afficheur, qui met à jour les scores des différentes voitures régulièrement.
	* Selon qu'on soit dans le mode course finale ou non, la fonction affiche les scores selon un certain ordre.
	* La fonction s'arrête lorsque le temps imparti est écoulé, ou que toutes les voitures ont fini la course 
	* (soit elles ont fini le nombre de tours demandé, soit elles sont out).
	*/

	float meilleur_s1;
	float meilleur_s2;
	float meilleur_s3;
	float meilleur_total;
	char *indice;
	int total;
	int actuel;
	int place_finale = 0;
	int nbr_voitures_out = 0;
	float secondes;
	int tableau_trie[nbr_voitures];

	if (course == 1) {
		total = nbr_voitures - nbr_voitures_out;
		actuel = place_finale;
	}
	else {
		total = duree;
		actuel = temps();
	}

	//permet à toutes les voitures d'initialiser leurs valeurs avant l'affichage
	//pas besoin de sémaphore, les données sont relues 
	int attente_voitures = 1;
	while(attente_voitures == 1) {
		int nbr_voitures_pretes = 0;
		for (int i=0 ; i<20; i++) {
			if (cars[i].secteurs[0] != 0.0) {
				nbr_voitures_pretes ++;
			}
		}
		if (nbr_voitures_pretes == nbr_voitures) {
			attente_voitures = 0;
		}
	}

	while (actuel < total) {
		//Réservation du sémaphore
		if (semop(semid, sb1, 1) == -1) { 
		    perror("semop pere réservation");
		    exit(1); 
		}
		sleep(2);

		system("clear");
		printf("PLACE   VOITURE   Section1   Section2   Section3     TOTAL    Diff_temps     STAND_P	OUT	TOUR\n") ;
		printf("-----------------------------------------------------------------------------------------------------\n");
		if (course == 1) { //tri en mode course
			//tri des voitures 
			float copie_total_temps[nbr_voitures];
			for (int i=0 ; i<nbr_voitures ; i++) {
				copie_total_temps[i] = cars[i].total_complet;
			}

			//récupère le total de seconde le plus élevè pour un usage ultérieur
			int indice_total_complet_max = 0;
			for (int i=0 ; i<nbr_voitures ; i++) {
				if (cars[i].total_complet > cars[indice_total_complet_max].total_complet) {
					indice_total_complet_max = i;
				}
			}
			float total_complet_max = cars[indice_total_complet_max].total_complet + 1.0 ;

			//récupère le plus grand nombre de tour fait
			int max_tour = 0 ;
			for (int i=0 ; i<nbr_voitures ; i++) {
				if (cars[i].nbr_tour > max_tour) {
					max_tour = cars[i].nbr_tour;
				}
			}

			//cree un tableau contenant les nombres de tour de max_tour à 0
			int tours[max_tour+1];
			for (int i=0 ; i<max_tour+1 ; i++) {
				tours[i] = max_tour-i;
			}
		
			//tri d'un tableau sur base du nbr de tour puis du nbr de secondes totales
			int index = 0;
			int indice_tour_min;
			float min;
			int compteur;
			for (int i=0 ; i<max_tour+1 ; i++) {
				for (int k=0 ; k<nbr_voitures ; k++) {
					min = total_complet_max;
					indice_tour_min = 0;
					compteur = 0;
					for (int j=0 ; j<nbr_voitures ; j++) {
						if ( (cars[j].nbr_tour == tours[i]) && (copie_total_temps[j] < min) ) {
							min = copie_total_temps[j];
							indice_tour_min = j;
						}
						else {
							compteur++;
						}
					}
					if (compteur < nbr_voitures) {
						tableau_trie[index] = cars[indice_tour_min].numero_voiture;
						copie_total_temps[indice_tour_min] = total_complet_max;
						index++;
					}
				}
			}
		}
		else { //tri en mode p1, p2, p3, q1, q2, q3
			//Triage de la liste du meilleur temps de voiture
			float tab_d[nbr_voitures];
			int minimum;
			for(int k=0; k<nbr_voitures; k++) {
				tab_d[k] = cars[k].total;
			}

			for(int j=0; j<nbr_voitures; j++) {
				minimum = 0;
				for (int i=0; i<nbr_voitures; i++) {
					if (tab_d[i] < tab_d[minimum]) {
						minimum = i;
					}
				}
				tableau_trie[j] = cars[minimum].numero_voiture;
				tab_d[minimum] = 200;
			}
		}

		//Affichage des scores et des meilleurs	
		meilleur_s1 = 66.0;
		meilleur_s2 = 66.0;
		meilleur_s3 = 66.0;
		float total_precedent;
		place_finale = 0 ;
		nbr_voitures_out = 0 ;
		meilleur_total = 200.0 ;
		for (int l=0; l<nbr_voitures; l++) {
			for (int m=0; m<nbr_voitures; m++) {
				if (cars[m].numero_voiture == tableau_trie[l]) {
					if (cars[m].nbr_tour >= NBR_TOURS) { //calcule les voitures qui ont fini les tours
						place_finale+=1;
					}
					indice = "";
					if(l+1 < 10) { //pour afficher un 0 devant si c'est un nombre
						indice = "0";
					}
					
					secondes = cars[m].total;
					if (course == 1) { //selon le type de course dans lequel on est
						secondes = cars[m].total_complet;
					}

					char *longueur_diff_temps = "   ";
					if ( ((secondes-total_precedent)<-100) || ((secondes-total_precedent)>999) ) {
						longueur_diff_temps = "";
					}

					if(l == 0) {
						printf("  %s%d	  %d	   %.3f     %.3f     %.3f     %.3f        /		%d	 %d	 %d\n", indice, l+1, cars[m].numero_voiture, cars[m].secteurs[0], cars[m].secteurs[1], cars[m].secteurs[2], secondes, cars[m].stand, cars[m].out, cars[m].nbr_tour);
						total_precedent = secondes;
						meilleur_total = cars[m].total;
					}
					else {
						printf("  %s%d	  %d	   %.3f     %.3f     %.3f     %.3f	%.3f%s	%d	 %d	 %d\n", indice, l+1, cars[m].numero_voiture, cars[m].secteurs[0], cars[m].secteurs[1], cars[m].secteurs[2], secondes, (secondes-total_precedent), longueur_diff_temps, cars[m].stand, cars[m].out, cars[m].nbr_tour);
						total_precedent = secondes;
					}

					if (cars[m].secteurs[0] < meilleur_s1) {
						meilleur_s1 = cars[m].secteurs[0];
					}
					if (cars[m].secteurs[1] < meilleur_s2) {
						meilleur_s2 = cars[m].secteurs[1];
					}
					if (cars[m].secteurs[2] < meilleur_s3) {
						meilleur_s3 = cars[m].secteurs[2];
					}

					if (cars[m].out == 1) {
						nbr_voitures_out+=1 ;
					}
				}
			}
		}
		printf("\nMeilleur S1 : %.3f\n", meilleur_s1);
		printf("Meilleur S2 : %.3f\n", meilleur_s2);
		printf("Meilleur S3 : %.3f\n\n", meilleur_s3);
		printf("Meilleur total : %.3f\n\n", meilleur_total);		

		//Libération du sémaphore
		if (semop(semid, sb2, 1) == -1) {
		    perror("semop afficheur libération");
		    exit(1); 
		}

		//infos pour la boucle
		if (course == 1) {
			total = nbr_voitures - nbr_voitures_out;
			actuel = place_finale;
		}
		else {
			actuel = temps();
		}
	}

	for (int z=0; z<nbr_voitures; z++) {
		resultat_intermediaire->voitures[z] = tableau_trie[z] ;
	}
	resultat_intermediaire->temps[0] = meilleur_s1;
	resultat_intermediaire->temps[1] = meilleur_s2;
	resultat_intermediaire->temps[2] = meilleur_s3;
	resultat_intermediaire->temps[3] = meilleur_total;

	
	//Se détacher de l'espace mémoire
	if (shmdt(cars) == -1) {
		perror("shmdt afficheur");
		exit(1);
	}
	//Effacer la mémoire partagée
	struct shmid_ds buf;
	if ((shmctl(shmid, IPC_RMID, &buf)) < 0) {
		perror("shmctl afficheur");
		exit(1);
	}
}


