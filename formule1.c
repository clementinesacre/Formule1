#include "formule1.h"
#include "utile.h"
#include "pere.h"
#include "fils.h"


void podium(char *seance, int place1, int place2, int place3) {
	/*Fonction qui exécuté le code en fonction du type de course lancée, ainsi qu'un podium avec les numéros de voitures gagnantes.*/
	/*printf("__________________________________________________________________________________________________");
	printf("\n\nCOURSE '%s' TERMINEE. \nLe classement ci-dessus est le classement final de la séance. Podium : \n\n", seance);
	printf("              %d\n", place1);
	printf("          %d\n", place2);
	printf("                 %d\n\n", place3);*/

	char *espace2 = "";
	if (place2 < 10) {
		espace2 = " ";
	}

	printf("__________________________________________________________________________________________________");
	printf("\n\nCOURSE '%s' TERMINEE. \nLe classement ci-dessus est le classement final de la séance. Podium : \n\n", seance);

	printf("               %d  \n", place1);
	printf("           %s%d \033[3;103;30m 1 \033[0m %d \n", espace2, place2, place3);
	printf("           \033[3;100;30m 2 \033[0m\033[3;103;30m   \033[0m\033[3;43;30m 3 \033[0m \n\n");
}


int main(int argc, char **argv) {
	/*Fonction principale qui, selon les paramètres entrés par l'utilisateur, lance le programme.
	* Si l'utilisateur rentre de mauvais paramètres, une documentation s'affiche.
	* Si l'utilisateur rentre des paramètres acceptés, la fonction lance les voitures ainsi que l'afficheur.
	* A la fin de chaque partie, elle écrit les scores dans un fichier texte.
	*/

	int numero_voitures[NBR_VOITURES_TOTAL] = {44, 77, 16, 5, 33, 23, 55, 4, 3, 31, 10, 26, 11, 18, 7, 99, 20, 8, 63, 6};
	int duree = temps() + DUREE_COURSE;
	int pid_pere = getpid();
	int nbr_voitures_partie; 
	pid_t pid;
	
	//Création adresse mémoire partagée + attachement du segment à l'espace mémoire
	voiture *cars;
	int shmid;
	key_t key = 5678;
	if ((shmid = shmget(key, SHMSZ, 0666 | IPC_CREAT)) < 0) {
		perror("shmid main cars");
		exit(1);
	}
	if ((cars = shmat(shmid, NULL, 0)) == (void*)(-1)) {
	    perror("shmat main cars");
	    exit(1);
	}


	//Sructure qui enregistre les résultats finaux pour les écrire dans un fichier
	conservation *resultat_intermediaire;

	//Création sémaphore
	int semid;
    union semun arg;
	if ((semid = semget(KEY2, 1, 0666 | IPC_CREAT)) == -1) {
    	perror("semget main creation");
    	exit(1); 
	}
	//Initialisation du sémaphore à 1
	arg.val = 1; 
	if (semctl(semid, 0, SETVAL, arg) == -1) {
		perror("semctl main init");
		exit(1); 
	}
	struct sembuf sb1 = {0, -1, 0}; //Permet de réserver le sémaphore
	struct sembuf sb2 = {0, +1, 0};	//Permet de relâcher le sémaphore


	FILE *fichier_ecriture;
	FILE *fichier_lecture;
	char *fichier_a_ecrire;
	char *fichier_a_lire;


	//Récupération des paramètres éventuels
	char *my_argv[argc-1] ; 
	int nbr_argument;
	for (nbr_argument = 1 ; nbr_argument < argc ; nbr_argument++) {
		my_argv[nbr_argument-1] = argv[nbr_argument];		
	}
	my_argv[argc - 1] = NULL;
	int verification = 0;
	struct stat contenu_fichier;


	char *mot = "";
	if (nbr_argument-2 >= 0) { //vérifier qu'il y a au moins un paramètre entré
		//Vérification des paramètres, et selon, initialisation de certaines variables à des valeurs précises
		if (strcmp(my_argv[0],(mot="fri")) == 0) {
			if (nbr_argument-2 >= 1) { //permet de vérifier qu'il y a au moins 2 paramètres
				if (strcmp(my_argv[1],(mot="p1")) == 0) {
					fichier_a_ecrire = "p1.txt";
					verification = 1;
					nbr_voitures_partie = NBR_VOITURES_TOTAL;
				}
				else if (strcmp(my_argv[1],(mot="p2")) == 0) {
					fichier_a_ecrire = "p2.txt";
					verification = 1;
					nbr_voitures_partie = NBR_VOITURES_TOTAL;
				}
			}
		}
		else if (strcmp(my_argv[0],(mot="sat")) == 0) {
			if (nbr_argument-2 >= 1) {
				if (strcmp(my_argv[1],(mot="p3")) == 0) {
					fichier_a_ecrire = "p3.txt";
					verification = 1;
					nbr_voitures_partie = NBR_VOITURES_TOTAL;
				}
				else if (strcmp(my_argv[1],(mot="q1")) == 0) {
					fichier_a_ecrire = "q1.txt";
					verification = 1;
					nbr_voitures_partie = NBR_VOITURES_TOTAL;
				}
				else if (strcmp(my_argv[1],(mot="q2")) == 0) {
					stat("q1.txt", &contenu_fichier) ;
					if (contenu_fichier.st_size == 0) { //vérifie si le fichier est vide, pour être sur que l'ordre des courses est correct
						printf("Il faut d'abord lancer les 1e qualifications.\n\n");
					}
					else {
						fichier_a_lire = "q1.txt";
						fichier_a_ecrire = "q2.txt";
						verification = 2;
						nbr_voitures_partie = 15;
					}
				}
				else if (strcmp(my_argv[1],(mot="q3")) == 0) {
					stat("q2.txt", &contenu_fichier);
					if (contenu_fichier.st_size == 0) { 
						printf("Il faut d'abord lancer les 2e qualifications.\n\n");
					}
					else {
						fichier_a_lire = "q2.txt";
						fichier_a_ecrire = "q3.txt";
						verification = 2;
						nbr_voitures_partie = 10;
					}
				}
			}
		}
		else if (strcmp(my_argv[0],(mot="sun")) == 0) {
			if (nbr_argument-2 >= 1) {
				if (strcmp(my_argv[1],(mot="race")) == 0) {
					stat("q3.txt", &contenu_fichier);
					if (contenu_fichier.st_size == 0) {
						printf("Il faut d'abord lancer les 3e qualifications.\n\n");
					}
					else {
						fichier_a_lire = "q3.txt";
						fichier_a_ecrire = "race.txt";
						verification = 3;
						nbr_voitures_partie = NBR_VOITURES_TOTAL;
					}
				}
			}
		}
	}
	if (verification == 0) { //Quand les paramètres sont incorrects
		printf("\nutilisation : fichier -jour -etape\n\n\n");
		printf("jour : fri, sat, sun\n\n");
		printf("etape : p1, p2           (avec fri) \n        p3, q1, q2, q3   (avec sat) \n        race             (avec sun)\n\n");
	}
	else if (verification == 1) { //Pour p1, p2, p3, q1
		//Le père réserve le sémaphore pour être sure que tous les fils démarrent en même temps
		if (semop(semid, &sb1, 1) == -1){
			perror("semop pere réservation verificateur==1");
			exit(1); 
		}		
		for(int i=0; i<nbr_voitures_partie; i++) {
			//Création des fils et lancement des voitures
			if((pid = fork())==0){ 
				lancer_voiture(i, getpid(), numero_voitures[i], cars, shmid, semid, &sb1, &sb2, duree, 0);		
				break;
			}
		}
		if (getpid() == pid_pere) {
			//Le père relâche le sémaphore et devient l'afficheur
			if (semop(semid, &sb2, 1) == -1){
				perror("semop pere libération verificateur==1");
				exit(1); 
			}
			sleep(0.5);
			afficheur(cars, shmid, semid, &sb1, &sb2, arg, duree, resultat_intermediaire, nbr_voitures_partie, 0); //Lancement du père en tant qu'afficheur

			
			//Après la partie, le père note les résultats dans le fichier adéquat
			fichier_ecriture = fopen(fichier_a_ecrire, "w");	
			if(fichier_ecriture == NULL){
				printf("Error creation file p1/p2/p3/q1.txt\n");
				exit(1);
			}
		
			for(int i=0; i<nbr_voitures_partie;i++) {
				fwrite(&resultat_intermediaire->voitures[i], sizeof(int), 1, fichier_ecriture);
			}
			for(int i=0; i<4;i++) {
				fwrite(&resultat_intermediaire->temps[i], sizeof(float), 1, fichier_ecriture);
			}
			fclose(fichier_ecriture);

			podium(mot, resultat_intermediaire->voitures[0], resultat_intermediaire->voitures[1], resultat_intermediaire->voitures[2]);
		}
	}
	else if ((verification == 2) || (verification == 3)) {
		//Lecture des données précédentes
		fichier_lecture = fopen(fichier_a_lire, "r");
		if(fichier_lecture == NULL){
				printf("Error creation file q1/q2/q3.txt\n");
				exit(1);
		}
		fseek(fichier_lecture, 0, SEEK_SET);
		int voitures_precedentes[NBR_VOITURES_TOTAL];
		float resultats_precedents[4];
		for(int i=0; i<NBR_VOITURES_TOTAL;i++) {
			fread(&voitures_precedentes[i], sizeof(int), 1, fichier_lecture);
		}
		for(int i=0; i<4;i++) {
			fread(&resultats_precedents[i], sizeof(float), 1, fichier_lecture);
		}
		fclose(fichier_lecture);
		

		if (verification == 2) {
			if (semop(semid, &sb1, 1) == -1) { //sémaphore réservation
				perror("semop pere réservation verification==2");
				exit(1); 
			}
			for(int i=0; i<nbr_voitures_partie; i++) {
				if((pid = fork())==0){ //Création des fils et lancement des voitures
					lancer_voiture(i, getpid(), voitures_precedentes[i], cars, shmid, semid, &sb1, &sb2, duree, 0);		
					break;
				}
			}
			if (getpid() == pid_pere) {
				if (semop(semid, &sb2, 1) == -1) { //sémaphore libération
					perror("semop pere libération verificateur==2");
					exit(1); 
				}
				sleep(0.5);
				afficheur(cars, shmid, semid, &sb1, &sb2, arg, duree, resultat_intermediaire, nbr_voitures_partie, 0); //Lancement du père en tant qu'afficheur
			

				//écriture des nouvelles données
				for(int i=nbr_voitures_partie; i<NBR_VOITURES_TOTAL;i++) {
					resultat_intermediaire->voitures[i] = voitures_precedentes[i] ;
				}
				fichier_ecriture = fopen(fichier_a_ecrire, "w");
				if(fichier_ecriture == NULL){
					printf("Error creation file q2/q3.txt\n");
					exit(1);
				}
				for(int i=0; i<NBR_VOITURES_TOTAL;i++) {
					fwrite(&resultat_intermediaire->voitures[i], sizeof(int), 1, fichier_ecriture);
				}
				for(int i=0; i<4;i++) {
					fwrite(&resultat_intermediaire->temps[i], sizeof(float), 1, fichier_ecriture);
				}
				fclose(fichier_ecriture);

				podium(mot, resultat_intermediaire->voitures[0], resultat_intermediaire->voitures[1], resultat_intermediaire->voitures[2]);
			}
		}

		else if (verification == 3) {
			if (semop(semid, &sb1, 1) == -1) {
				perror("semop pere réservation verificateur==3");
				exit(1); 
			}
			for(int i=0; i<nbr_voitures_partie; i++) {
				if((pid = fork())==0){ //Création des fils et lancement des voitures
					lancer_voiture(i, getpid(), voitures_precedentes[i], cars, shmid, semid, &sb1, &sb2, duree, 1);		
					break;
				}
			}
			if (getpid() == pid_pere) {
				if (semop(semid, &sb2, 1) == -1) {
					perror("semop pere libération verificateur==3");
					exit(1); 
				}
				sleep(0.5);
				afficheur(cars, shmid, semid, &sb1, &sb2, arg, duree, resultat_intermediaire, nbr_voitures_partie, 1); //Lancement du père en tant qu'afficheur

				//écriture des nouvelles données
				for(int i=nbr_voitures_partie; i<NBR_VOITURES_TOTAL;i++) {
					resultat_intermediaire->voitures[i] = voitures_precedentes[i] ;
				}
				fichier_ecriture = fopen(fichier_a_ecrire, "w");
				if(fichier_ecriture == NULL){
					printf("Error creation file race.txt\n");
					exit(1);
				}
				for(int i=0; i<NBR_VOITURES_TOTAL;i++) {
					fwrite(&resultat_intermediaire->voitures[i], sizeof(int), 1, fichier_ecriture);
				}
				for(int i=0; i<4;i++) {
					fwrite(&resultat_intermediaire->temps[i], sizeof(float), 1, fichier_ecriture);
				}
				fclose(fichier_ecriture);

				podium(mot, resultat_intermediaire->voitures[0], resultat_intermediaire->voitures[1], resultat_intermediaire->voitures[2]);
			}
			//suppression des fichiers, remise des compteurs à 0
			fichier_ecriture = fopen("p1.txt", "w");
			if(fichier_ecriture == NULL){
				printf("Error creation file p1.txt\n");
				exit(1);
			}
			fclose(fichier_ecriture);

			fichier_ecriture = fopen("p2.txt", "w");
			if(fichier_ecriture == NULL){
				printf("Error creation file p2.txt\n");
				exit(1);
			}
			fclose(fichier_ecriture);

			fichier_ecriture = fopen("p3.txt", "w");
			if(fichier_ecriture == NULL){
				printf("Error creation file p3.txt\n");
				exit(1);
			}
			fclose(fichier_ecriture);

			fichier_ecriture = fopen("q1.txt", "w");
			if(fichier_ecriture == NULL){
				printf("Error creation file q1.txt\n");
				exit(1);
			}
			fclose(fichier_ecriture);
			fichier_ecriture = fopen("q2.txt", "w");
			if(fichier_ecriture == NULL){
				printf("Error creation file q2.txt\n");
				exit(1);
			}
			fclose(fichier_ecriture);

			fichier_ecriture = fopen("q3.txt", "w");
			if(fichier_ecriture == NULL){
				printf("Error creation file q3.txt\n");
				exit(1);
			}
			fclose(fichier_ecriture);
		}
	}

	return 0;
}

