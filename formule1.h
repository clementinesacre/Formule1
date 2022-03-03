#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/sem.h>

#define NBR_VOITURES_TOTAL 20
#define NBR_TOURS 15

#define KEY2 0x1111 //clé sémaphore pour le parent et les enfants
#define DUREE_COURSE 10 //en secondes
#define MAX_SECTEUR 65-1 //durée maximum d'un secteur en secondes
#define MIN_SECTEUR 35 //durée minimum d'un secteur en secondes
#define CHANCE_STAND (NBR_TOURS/3)+1 //chance pour qu'une voiture aille au stand (ici = 1/10)
#define CHANCE_OUT 100 //chance qu'une voiture soit out (ici 1/100)
#define KEY_CONSERVATION 1782 //clé pour la mémoire partagée pour


union semun {
    int val;               /* used for SETVAL only */
    struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
    ushort *array;         /* used for GETALL and SETALL */
};

typedef struct {
	pid_t numero_processus;
	int numero_voiture;
	float secteurs[3];
	float total;
	int stand;
	int out;
	int temps_course;
	int nbr_tour;
	float total_complet;
} voiture;
#define SHMSZ 20 * sizeof(voiture)

typedef struct {
	int voitures[NBR_VOITURES_TOTAL];
	float temps[4];
	
} conservation;
