#include "Graphic.h"
#include <unistd.h>
#include "FM.h"
#include "Graphic.h"
#include <curses.h>
#include <panel.h>

#define ENTER 10
#define ESCAPE 27

typedef struct
{
    off_t taille;
    char *nom;
    char type;
    char *droit;
}Fic;

void updatePrintInfoBox(WINDOW* W_info)
{
	wclear(W_info);
	box(W_info,0,0);


	mvwaddstr(W_info,2,2,"'<==': Dossier parent");
	mvwaddstr(W_info,3,2,"'==>': Entrer");
	mvwaddstr(W_info,4,2,"'Up / Down': Naviguer");
	mvwaddstr(W_info,6,2,"'v': Enable extensions  view");
	mvwaddstr(W_info,7,2,"'l': Enable selection  view");
	mvwaddstr(W_info,8,2,"'e': Editer ");
	mvwaddstr(W_info,9,2,"'x': Executer ");
	mvwaddstr(W_info,10,2,"'DEL': Supprimer element ");

	mvwaddstr(W_info,18,4,"=== Information Fichier ===");
}

int main(void)
{
	WINDOW *W_main,*W_info, *W_menu;
	int maxx,maxy,halfx;
	int tailleMenu = 2;	// hauteur de la bar de menu
	int ch, choix=-1;
	init_curses();

/* calculate window sizes and locations */
	getmaxyx(stdscr,maxy,maxx);
	halfx = maxx >> 1;						/* half the screen width */

// Allocation des Fenetres principales
	if( (W_main = newwin(maxy-tailleMenu,halfx+50,tailleMenu,0)) == NULL)
	{
		endwin();
		puts("Unable to create 'W_main' window");
		return 1;
	}
	if( (W_info = newwin(maxy-tailleMenu,halfx-50,tailleMenu,halfx+50)) == NULL)
	{
		endwin();
		puts("Unable to create 'W_info' window");
		return 1;
	}
	if( (W_menu = newwin(tailleMenu,maxx,0,0)) == NULL)
	{
		endwin();
		puts("Unable to create 'W_menu' window");
		return 1;
	}


	// Dessin des contours des fenetres.
	box(W_info,0,0);
	box(W_main,0,0);
	
	updatePrintInfoBox(W_info);


	mvwaddstr(W_main,0,1,"Window des dossiers (taper ~ pour quitter)\n");


	mvwprintw(W_menu, 0, 1, "Menu");
	

	// Couleurs
	wbkgd(W_main,COLOR_PAIR(4));	// fenetre principale
	wbkgd(W_menu,COLOR_PAIR(4));	// menu
	wbkgd(W_info,COLOR_PAIR(4));	// Panel info

	// Refresh des window
	touchwin(W_main); touchwin(W_info);
	wrefresh(W_main); wrefresh(W_info);

	int FicSelectioned = 1;



	Fic *tab = infoDossier();	// Info des elements du dossier.

	int showExt = 1,highlight=1;	// booleen pour afficher - masquer les extensions des fichiers

	int startx,starty = 20;
	starty=30;
	int cpt;

	

/* ============== BOUCLE principale ======================================================== */
	do
	{
		// ============ Refresh ========================================================	
			mvwprintw(W_main,0,1,"Window des dossiers (taper ~ pour quitter)");
			mvwprintw(W_menu, 0, 1, "%s", G_pwd());
			touchwin(W_menu); touchwin(W_info); touchwin(W_main);
			wrefresh(W_menu); wrefresh(W_info); wrefresh(W_main);
			doupdate();

		// ==============================================================================
		affichFromTabFic(tab, W_main, showExt);	// get les infos sur tous les fichiers:
		// ============================================================================


		
	

		if(tab[0].taille != 0)
		{
			if(tab[FicSelectioned].type == 'F') 
			{
				mvwprintw(W_info,20,4,"Fichier");
				if(getExtention(tab[FicSelectioned].nom) != NULL ) mvwprintw(W_info, 20, 12, "%s", getExtention(tab[FicSelectioned].nom));
			}
			else mvwprintw(W_info,20,4,"Dossier"); touchwin(W_menu); touchwin(W_info); wrefresh(W_menu); wrefresh(W_info);

			// color en jaune l'element selectionne
			attron(COLOR_PAIR(7));	mvprintw(0, maxx/2, tab[FicSelectioned].nom);	attron(COLOR_PAIR(4));

			mvwprintw(W_info,22,4,"Nom  : %s", tab[FicSelectioned].nom);
			mvwprintw(W_info,21,4,"Taille : %ld bytes",tab[FicSelectioned].taille);
		}
		else mvwprintw(W_info,23,4,"Aucun fichier dans le dossier");
		
		


		touchwin(W_info);wrefresh(W_info);


		ch=getch();

		switch(ch)
		{
			// ====================================================================================================
			//=======================================	NAVIGATION	===============================================
			case (KEY_UP):	// ====>> Touche  UP (Z)
			{
				if(tab[0].taille == 0) break;
				if (FicSelectioned < tab[0].taille) FicSelectioned++;		// selection du fichier suivant.
				updatePrintInfoBox(W_info);
				break;
			}


			case (KEY_DOWN):	// ====>> Touche  DOWN (S)
			{
				if(tab[0].taille == 0) break;
				if (FicSelectioned > 1) FicSelectioned --;
				updatePrintInfoBox(W_info);

				
				break;
			}


			case (KEY_RIGHT):
			{
				if(tab[0].taille == 0) break;
				if(tab[FicSelectioned].type == 'D')	
					{
						G_cd(tab[FicSelectioned].nom);
						free(tab); tab= NULL;
						tab=infoDossier();		// mise a jours des info des elements du dossier.
						FicSelectioned=1;

						wclear(W_menu);		
						waddstr(W_menu, G_pwd());	// Affiche le pwd dans le menu
						touchwin(W_main); touchwin(W_menu);
						wrefresh(W_main); wrefresh(W_info);
						break;
					}
				break;
			}

			case('e'):
			{
				ExecuterFic(getCheminCompletFic(FicSelectioned,tab), 'e');
				break;
			}

			case('r'):
			{
				wclear(W_menu);	touchwin(W_menu); wrefresh(W_menu);
				break;
			}


			case(KEY_LEFT):
			{
				free(tab);
				tab= NULL;

				G_cd("..");
				tab=infoDossier();		// mise a jours des info des elements du dossier.
				FicSelectioned=1;

				wclear(W_menu);		
				waddstr(W_menu, G_pwd());	// Affiche le pwd dans le menu
				touchwin(W_main); touchwin(W_menu);
				wrefresh(W_main); wrefresh(W_info);
				break;
			}

			case(KEY_DC):
			{
				if(remove(tab[FicSelectioned].nom) == 0) 
					{
						werase(W_menu);
						mvwprintw(W_menu,0,30,"Fichier Supprime! ");
						touchwin(W_menu); wrefresh(W_menu);

						free(tab); tab=NULL;
						tab=infoDossier();
					}

					break;
			}


			case ('v'):		// AFFICHE - MASQUE LES EXTENSIONS
			{
				werase(W_menu);
				if(showExt==1)
				{
					showExt=0;
					mvwprintw(W_menu,0,30,"Extension disabled! ");
				}
				else{showExt=1; mvwprintw(W_menu,0,30,"Extension enabled!  "); }
				touchwin(W_menu);
				wrefresh(W_menu);
				refresh();
				break;
			}


			case ('l'):
			{
				choix=H_Menu(W_main,highlight,tab[0].taille,tab); //On affiche le contenu du tableau avec le  highlight
				if(choix != -1)	
					{
						FicSelectioned=choix-1;
					}
					
				wclear(W_menu);	

				touchwin(W_main); touchwin(W_menu); touchwin(W_info);
				wrefresh(W_main); wrefresh(W_menu);	wrefresh(W_info);

				// fleche du haut et fleche du bas pour se déplacer Appuyer de nouveau sur l pour sortir
				break;
			}
		
			case ('x'):
			{
				if(tab[0].taille == 0)	break;	// Si dossier vide
				if(tab[FicSelectioned].type == 'D')	break;	// Si element selectionne est un dossier

				// Liste des exectutions predefinies. p=python, sh=bash, etc..
				if (getExtention(tab[FicSelectioned].nom) == ".py")  ExecuterFic(getCheminCompletFic(FicSelectioned,tab), 'p'); break;
				
				// Sinon, on execute simplement.
				ExecuterFic(getCheminCompletFic(FicSelectioned,tab), 'x');
				break;
			}

		}
		
		//wrefresh(W_main);
		
	} while(ch != '~');
	
	endwin();


	printf("Thanks for using Green File Manager :) \n");


	return 0;
}
