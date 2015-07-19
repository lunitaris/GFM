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

// structure des fichiers, et leur info
typedef struct
{
    off_t taille;
    char *nom;
    char type;
    char *droit;
}Fic;

// structure du navigateur  pour futures update
// typedef struct 
// {
 //    char *next;     // historique dossier suivant;
 //   char *prev;     // historique dossier precedent;
 //   char *cwd;      // dossier courant;
//}Nav;

const char * DEFAULT_TERMINAL = "xterm";
const char * DEFAULT_EDITOR = "nano";

int compterFichier(DIR* dir)
{
    int nbr = 0;
    struct dirent* ent = NULL;

    while ((ent = readdir(dir)) != NULL)
    {
        if (strcmp(ent->d_name, ".") != 0 && /* Si le fichier lu n'est pas . */
            strcmp(ent->d_name, "..") != 0) /*  Et n'est pas .. non plus */
        nbr++; /* Alors on incrémente le compteur */
    }
    return nbr;
}


int isDirectory(struct dirent* ent)
{
    struct stat st;
    lstat(ent->d_name,&st);
    if(S_ISDIR(st.st_mode)) return 1;
    else return 0;

}


int G_cd(char* dossier)
{
    if( chdir( dossier ) == 0 ) return EXIT_SUCCESS;
    else 
	{
        perror( dossier );
        return EXIT_FAILURE;
    }
}


off_t getSizeFic(char* nom)
{
    struct stat st;
    if(stat(nom,&st) == 0) return st.st_size;
    return -1;
}

char* G_pwd()
{
    char* cwd;
    char buff[PATH_MAX + 1];

    cwd = getcwd( buff, PATH_MAX + 1 );
    return cwd;
}



// Fonction qui retourne un tableau de Fic contenant pour chaque cases les info de chaque fichiers.
Fic* infoDossier()
{
    DIR *dossier;
    //struct dirent* ent = NULL;
    int nbFiles = 0;
    int nb=0;
    
    dossier = opendir("./");    // = dossier = opendir ("./");

    nbFiles = compterFichier(dossier);
    Fic * tab = (Fic*)malloc((nbFiles+1) * sizeof(Fic));
    //Fic tab[30];

    // ================== getcwd    ======================================
    //char * cwd;
    //char buff[PATH_MAX+1];
    //cwd = getcwd(buff, PATH_MAX +1);
    // =====================================================================================

    // DIR dossier est inutile, on en recrera un si needed
    closedir(dossier);
    
     // strcpy(tab[0].nom ,cwd);


     tab[0].taille=nbFiles;      // tab[0].taille : nombre de fichiers.

     
    DIR * ici;  
    struct dirent* ent = NULL;
    ici = opendir("./"); // ouverture du flux

    while ((ent = readdir(ici)) != NULL)
    {
        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
        {
            if(isDirectory(ent))    // SI le fichier est un dossier
            {
                nb++;   // fichier numero nb

                tab[nb].nom = (char *)malloc(sizeof(ent->d_name));  // allocation memoire pour le nom
                strcpy(tab[nb].nom , ent->d_name);
                tab[nb].type='D';                       // <==> tab[nb].type =  'D'
                tab[nb].taille = getSizeFic(tab[nb].nom);       // Taille en bytes
            }
            else
            {
                nb++;   // fichier numero nb
                tab[nb].nom = (char *)malloc(sizeof(ent->d_name));  // allocation memoire pour le nom
                strcpy(tab[nb].nom , ent->d_name);
                tab[nb].type =  'F';                  // <==> tab[nb].type =  'F'
                tab[nb].taille = getSizeFic(tab[nb].nom);   // Taille en bytes
            }

        }
            
    }
    (void) closedir (ici);
    return tab;
}


char* getExtention(char* nomFichier)    // retourne l'extension du fichier
{
    char* extention;
    extention = strrchr(nomFichier,'.');
    return extention;
}

char* getCheminCompletFic(int index, Fic* tab)
{

    int tailleChemin, tailleFic;
    tailleChemin = strlen(G_pwd());
    tailleFic = strlen(tab[index].nom);

    char* cheminComplet = (char *)malloc(tailleChemin + tailleFic + 2);  // allocation memoire pour le nom +2:  un case pour le slash et une case pour la fin de chaine.
    strcpy(cheminComplet, G_pwd());
    cheminComplet[tailleChemin]='/';    // ajoute le slash a la fin du path
    strcat(cheminComplet,tab[index].nom);
    return cheminComplet;
}

void ExecuterFic(char *cheminFic, char flag)
{
    char **cmd;
   
    if (flag == 'e')    // Edition du fichier
    {
        cmd = (char **)malloc(6 *sizeof(char*));
        cmd[0]=DEFAULT_TERMINAL;
        cmd[1]="-e";
        if(getenv("EDITOR")!= NULL) cmd[2]=getenv("EDITOR"); else cmd[2]=DEFAULT_EDITOR;
        cmd[3]=cheminFic;
        cmd[4]="2>/dev/null";   // supprime les eventuels messages de Warning
        cmd[5]=NULL;
    }

    else if (flag == 'x')    // Execution du fichier
    {
        cmd = (char **)malloc(4 *sizeof(char*));
        cmd[0]=DEFAULT_TERMINAL;
        cmd[1]="-e";
        cmd[2]=cheminFic;
        cmd[3]=NULL;
    }
    else if (flag == 'p')    // Execution du fichier
    {
        cmd = (char **)malloc(5 *sizeof(char*));
        cmd[0]=DEFAULT_TERMINAL;
        cmd[1]="-e";
        cmd[2]="python";
        cmd[3]=cheminFic;
        cmd[4]=NULL;
    }

    pid_t  pid;
    int    status;

    if ((pid = fork()) < 0) 
        {     /* fork a child process           */
            printf("*** ERROR: forking child process failed\n");
            exit(1);
        }
    else if (pid == 0) 
        {
            // PROCESSUS FILS.          // Si xterm ecrit un warning, il faut executer: # unset SESSION_MANAGER
            if (execvp(*cmd, cmd) < 0) 
            {
                printf("*** ERROR: exec %s \n",cmd);
                exit(1);
            }
         }
         free(cmd);

}
