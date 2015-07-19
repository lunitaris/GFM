#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <curses.h>
#include <string.h>
#include <ctype.h>




int compterFichier(DIR* dir);  //Retourne le nombre de dossier/ficher dans le dossier courant

int isDirectory(struct dirent* ent);	// retourne 1 si le fichier spécifie via la structure dirent est un dossier. 0 sinon

int G_cd(char* dossier); 	// change le repertoire courant.

off_t getSizeFic(char* nom);	// retourne la taille en byte du fichier

char* G_pwd(); // retourne le dossier courant.

struct Fic* infoDossier();		// LA FONCTION la plus importante. Elle stock toutes les info des fichiers dans un tableau de structures Fic.

char* getExtention(char* nomFichier);    // retourne l'extension du fichier (extenSSSSSion ;D)

char* getCheminCompletFic(int index, struct Fic* tab);	// retoure le chemin complet du fichier tab[index].nom

void ExecuterFic(char *cheminFic, char flag);	// execute en fonction du flag le fichier cheminFic.
