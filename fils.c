#include "formule1.h"
#include "utile.h"

int lancer_voiture(int case_tableau, pid_t pid_voiture, int num_voiture, voiture *cars, int shmid, int semid, struct sembuf *sb1, struct sembuf *sb2, int duree, int course) {
	/*Fonction représentant une voiture, elle met à jour régèlièrement des valeurs telles que les temps dans les différents 
	* secteurs, le nombre de tour, le meilleur tour, ...
	* La fonction s'arrête dès que la voiture est out, ou qu'elle a atteint le nombre de tour maximum, ou encore lorsque 
	* le temps imparti est écoulé.
	*/

	float nouveau;
	float ancien;
	float ancien_total;
	int presence_stand;
	int presence_out;
	float moyenne = (MAX_SECTEUR+1 - ((MAX_SECTEUR+1-MIN_SECTEUR)/2)) *3 ;
	int total;
	int actuel;

	//Attacher le segment à l'espace mémoire
	if ((cars = shmat(shmid, NULL, 0)) == (void*)(-1)) {
	    perror("shmat voiture");
	    exit(1);
	}
	
	if (semop(semid, sb1, 1) == -1) { 
	    perror("semop1 voiture réservation");
	    exit(1); 
	}

	cars[case_tableau].temps_course = 0;
	cars[case_tableau].numero_voiture = num_voiture;
	cars[case_tableau].numero_processus = pid_voiture;
	cars[case_tableau].secteurs[0] = aleatoire(MIN_SECTEUR, MAX_SECTEUR);
	cars[case_tableau].secteurs[1] = aleatoire(MIN_SECTEUR, MAX_SECTEUR);
	cars[case_tableau].secteurs[2] = aleatoire(MIN_SECTEUR, MAX_SECTEUR);
	cars[case_tableau].total = cars[case_tableau].secteurs[0] + cars[case_tableau].secteurs[1] + cars[case_tableau].secteurs[2];
	cars[case_tableau].total_complet = cars[case_tableau].total ;
	cars[case_tableau].stand = 0 ; 
	cars[case_tableau].out = 0 ;
	cars[case_tableau].nbr_tour = (int)cars[case_tableau].total_complet/moyenne;

	//infos pour la boucle
	if (course == 1) {
		total = NBR_TOURS;
		actuel = cars[case_tableau].nbr_tour;
	}
	else {
		total = duree;
		actuel = temps();
	}

	if (semop(semid, sb2, 1) == -1) {
	    perror("semop1 voiture libération");
	    exit(1); 
	}

	while (actuel < total) {
		if (semop(semid, sb1, 1) == -1) { 
		    perror("semop2 voiture réservation");
		    exit(1); 
		}
		sleep(0.05);

		presence_out = aleatoire(1, CHANCE_OUT);
		if (presence_out == 1) {
			cars[case_tableau].out = 1;
			if (semop(semid, sb2, 1) == -1) {
				perror("semop2 voiture libération");
				exit(1); 
			}
			//Se détacher de l'espace mémoire
			if (shmdt(cars) == -1) {
				perror("shmdt voiture");
				exit(1);
			}
			return 0;
		}

		presence_stand = aleatoire(1, CHANCE_STAND);
		if (presence_stand == 1) {
			cars[case_tableau].stand = 1;
		}
		else {
			ancien_total = cars[case_tableau].total;
			cars[case_tableau].total = 0;
			for (int compteur = 0 ; compteur < 3 ; compteur++) {
				nouveau = aleatoire(MIN_SECTEUR, MAX_SECTEUR);
				ancien = cars[case_tableau].secteurs[compteur];
				cars[case_tableau].secteurs[compteur] = comparer(ancien, nouveau);
				cars[case_tableau].total += nouveau;
			}
			cars[case_tableau].total_complet += cars[case_tableau].total;
			cars[case_tableau].total = comparer(ancien_total, cars[case_tableau].total);
			cars[case_tableau].stand = 0;

			cars[case_tableau].nbr_tour = (int)cars[case_tableau].total_complet/moyenne ;
		}
	
		//infos pour la boucle
		if (course == 1) {
			actuel = cars[case_tableau].nbr_tour;
		}
		else if (course == 0) {
			actuel = temps();
		}

		if (semop(semid, sb2, 1) == -1) {
		    perror("semop2 voiture libération");
		    exit(1);
		}

	}
	//Se détacher de l'espace mémoire
	if (shmdt(cars) == -1) {
		perror("shmdt voiture");
		exit(1);
	}
	return 0 ;
}
