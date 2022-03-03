#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


float comparer(float avant, float apres) {
	/*La fonction permet de renvoyer le minimum entre 2 floats.*/

	if (avant < apres) {
		return avant;
	}
	return apres;
}

int temps() {
	/*La fonction permet de renvoyer l'heure actuelle du calendrier 
	* en secondes.
	*/

	time_t ta;
	time_t tb;

	time (&ta);
	tb = time (NULL);

	return tb;
}

float aleatoire(int minimum, int maximum) {
	/*La fonction renvoie un float aléatoire, entre 'minimum' et 'maximum'.*/
	srand(rand()+getpid());
	return (rand()%(maximum + 1 - minimum) + minimum ) + (rand()%1000/1000.0);
}
