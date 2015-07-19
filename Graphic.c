#include "FM.h"

#define ENTER 10
#define ESCAPE 27

// Nom des menus
const char *menu1 = "Fichier";
const char *menu2 = "Edition";


const int CONST_SIZE_ICON = 4;
const int CONST_STARTX = 10;
const int CONST_STARTY = 10;
const int CONST_ESPACEMENT = 6;

void init_curses()
{
        initscr();
        start_color();
        init_pair(1,COLOR_WHITE,COLOR_BLUE);
        init_pair(2,COLOR_BLUE,COLOR_WHITE);
        init_pair(3,COLOR_RED,COLOR_BLACK);
        init_pair(4,COLOR_GREEN,COLOR_BLACK);
        init_pair(5,COLOR_MAGENTA,COLOR_BLACK);
        init_pair(6,COLOR_CYAN,COLOR_BLACK);
        init_pair(7,COLOR_YELLOW,COLOR_BLACK);
        noecho();  // desactive echo unitules
        keypad(stdscr,TRUE);   // active les touches F1 F2 etc.
}


WINDOW *createBox(int height, int width, int starty, int startx)
{       WINDOW *local_win;

        local_win = newwin(height, width, starty, startx);
        box(local_win, 0 , 0);          /* 0, 0 gives default characters 
                                         * for the vertical and horizontal
                                         * lines                        */
        // Colore la box
        wbkgd(local_win,COLOR_PAIR(5));
        wrefresh(local_win);            /* Show that box                */

        return local_win;
}

WINDOW *createBoxD(int height, int width, int starty, int startx)
{       WINDOW *local_win;

        local_win = newwin(height, width, starty, startx);
        box(local_win, 0 , 0); 
        // Colore la box
        wbkgd(local_win,COLOR_PAIR(6));

        wrefresh(local_win);            /* Show that box                */

        return local_win;
}

WINDOW *createBoxF(int height, int width, int starty, int startx, char* ext)
{       WINDOW *local_win;

        local_win = newwin(height, width, starty, startx);
        box(local_win, 0 , 0); 
        // Colore la box
        wbkgd(local_win,COLOR_PAIR(5));
        waddstr(local_win, ext);
        wrefresh(local_win);            /* Show that box                */

        return local_win;
}

void delBox(WINDOW *local_win)
{       
        /* box(local_win, ' ', ' '); : This won't produce the desired
         * result of erasing the window. It will leave it's four corners 
         * and so an ugly remnant of window. 
         */
        wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
        /* The parameters taken are 
         * 1. win: the window on which to operate
         * 2. ls: character to be used for the left side of the window 
         * 3. rs: character to be used for the right side of the window 
         * 4. ts: character to be used for the top side of the window 
         * 5. bs: character to be used for the bottom side of the window 
         * 6. tl: character to be used for the top left corner of the window 
         * 7. tr: character to be used for the top right corner of the window 
         * 8. bl: character to be used for the bottom left corner of the window 
         * 9. br: character to be used for the bottom right corner of the window
         */
        wrefresh(local_win);
        delwin(local_win);
}




void affichSimpleNomemory(DIR* dir, WINDOW* fenetre)
{
    // affiche la liste des fichiers dans le dossier specifie en parametres.
    // En une couleur les fichiers, une autre pour les dossiers.
    // Ne memorise pas la liste des fichiers / dossiers. (affichage bete et simple)
    int startx = 10;
    struct dirent* ent = NULL;

    while ((ent = readdir(dir)) != NULL)
    {
        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
        {
            if(isDirectory(ent))    // SI le fichier est un dossier
            {
                fenetre= createBoxD(3, 3, 10, startx);
                startx+=5;
            }
            else
            {
                fenetre= createBoxF(3, 3, 10, startx, "");
                startx+=5;
            }

        }
            
    }
}

typedef struct
{
    off_t taille;
    char *nom;
    char type;
    char *droit;
}Fic;

int getSeparationY()
{
    int maxx,maxy,halfx;
    getmaxyx(stdscr,(maxy),maxx);
    halfx = maxx >> 1;
    return halfx+40;    // retourne la position limite a laquelle les icons reviendront a la ligne
}


int getEspacementIcons(Fic *tab)    // retourne l'espacement ideal entre les icons: 
{                                   //plus grande taille de nom de fichier +1

    int indexMax=1;
    int i;
    for(i=1; i<=tab[0].taille; i++)
    {
        // loneeur du nom fichier [i] VS longueur nom fichier [indexMax]
        if( strlen(tab[i].nom) > strlen(tab[indexMax].nom) ) {indexMax=i;}
    }
    return(strlen(tab[indexMax].nom)+1);
}

void affichFromTabFic(Fic *tab, WINDOW* fenetre, int printExtention) //, int printHidden)
{
    int nbFiles = tab[0].taille;       // <==> nbFiles = tab[0].taille
    int LIMITEY; // limite d'affichage pour les icons. est egal à la delimitation du panel info -5.
    int cpt;

    int sizeIcon = CONST_SIZE_ICON;
    int startx = CONST_STARTX;
    int starty = CONST_STARTY;
    int espacement = CONST_ESPACEMENT;


    char* ext;
        // espacement = getEspacementIcons(tab);     // ESPACEMENT  IDEAL ENTRE LES ICONS

        LIMITEY= getSeparationY();  // calcul automatique de la limite.


    for(cpt=1; cpt<=nbFiles; cpt++)
    {
        refresh();

        if(startx >= LIMITEY ) {startx=10; starty+=5;}      // Si on arrive au bout de l'ecran, on saute a la ligne

        // SI le fichier est un dossier
        if(tab[cpt].type == 'D')
        {
            fenetre= createBoxD(sizeIcon, sizeIcon, starty, startx);
            // mvaddnsrt(starty+3, startx, tab[cpt].nom, 4);
            startx+=espacement;

        }
        // SI le fichier est un fichier
        else if(tab[cpt].type == 'F')
        {
            if(printExtention==1)
                {ext=getExtention(tab[cpt].nom);}
            else {ext="";}

             fenetre= createBoxF(sizeIcon, sizeIcon, starty, startx, ext);
             startx+=espacement;
        }
        else    // SI le fichier est autre
        {
             fenetre= createBox(sizeIcon, sizeIcon, starty, startx);
            //mvaddnsrt(starty+3, startx, tab[cpt].nom, 4);
             startx+=espacement;
        }
    }
}


// Fonctions de menu..
// ======================================================



void H_Menu2(WINDOW *W_main, int highlight,int n_choices,Fic *tab)
{


    int maxx,maxy,halfx;
    getmaxyx(stdscr,(maxy),maxx);

    int x, y, i;    

    x = maxx/2;
    y = 3;
    for(i = 1; i <= n_choices; ++i)
    {    if(highlight == i + 1) /* High light the present choice */
        {    
            wattron(W_main, A_REVERSE);             //Défini l'attribue A_REVERSE sur la la fenetre main qui inverse les couleurs
            mvwprintw(W_main,y,x,"%s",tab[i].nom);   //On affiche le tableau
            wattroff(W_main, A_REVERSE);            //Enlève l'attribue
        }
        else
            mvwprintw(W_main, y, x, "%s", tab[i].nom);
        ++y;
    }
    wrefresh(W_main);
}




int H_Menu(WINDOW *W_main, int highlight,int n_choices,Fic *tab)
{
    int i=1,c;
    highlight=1;
    H_Menu2(W_main,highlight,n_choices,tab);
    while(i==1)
    {
        c = getch();
        switch(c)
        {    case (KEY_UP):              //Touche z
                if(highlight == 1) //Envoie le highlight en bas
                    highlight = n_choices;
                else
                    --highlight; //Fait monter le highlight
                break;
            case KEY_DOWN:           //Touche s
                if(highlight == n_choices) //Envoie le highlight en haut
                    highlight = 1;
                else
                    ++highlight; //Fait descendre le highlight
                break;
            default:               
                break;
        }
        H_Menu2(W_main, highlight,n_choices,tab);
        
        if(c==108)              //On sort de la boucle lorsque on appuye sur l
            i=0;
        if(c==10)              //Touche Entrée
        {
            i=0;             //On quitte
            wclear(W_main);
            return highlight; //On retourne le choix que l'on a choisie
        }
    }
    wclear(W_main);
    wrefresh(W_main); 
    return -1;   
}
