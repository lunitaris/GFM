#include "FM.h"


void init_curses();

// fonctions servant a afficher les icones des fichiers.
WINDOW *createBox(int height, int width, int starty, int startx);

void delBox(WINDOW *local_win);

void affichSimpleNomemory(DIR* dir, WINDOW* fenetre);

WINDOW *createBoxD(int height, int width, int starty, int startx);
WINDOW *createBoxF(int height, int width, int starty, int startx, char* ext);

int getSeparationY();

int getEspacementIcons(struct Fic*);

void affichFromTabFic(struct Fic*, WINDOW*, int);	// affiche les icones des elements dans le dossier.


//fonction servant a gérer le highlight
void H_Menu2(WINDOW*, int ,int ,struct Fic*);
int H_Menu(WINDOW* , int ,int ,struct Fic*);
