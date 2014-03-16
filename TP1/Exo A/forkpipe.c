#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define BUFSIZE 20



int main (void){
 	/* Cra du pipe */
    int pfd[2];
    pipe(pfd);

	/* fgets */
    /* [009][input.txt] */

    /* Premier fils: */
    int  pidfils1;
    if ((pidfils1 = fork()) < 0)
    {
        printf("fork 1 failed\n");
        return 2;
    }
 
    if (pidfils1 == 0)
    {	
    	/* Déroulement du fils 1 ICI: */
        int taille;
        char input[107] = ""; 
		char ligne[100] = "";
 		//On ferme la lecture
        close(pfd[0]);
 		
 		FILE *fichier =fopen("input.txt","r");
 		while(fgets(ligne, 100, fichier) != NULL){
                taille = strlen(ligne)-1;
                ligne[taille] = '\0';
                sprintf(input, "[%03d][%s]", taille, ligne);
                write (pfd[1], input, strlen(input));
        }

        close(pfd[1]); /* close the pipe */
    }

    else{
    	//Création du fils 2:
    	int  pidfils2;
	    if ((pidfils2 = fork()) < 0)
	    {
	        printf("fork 1 failed\n");
	        return 2;
	    }
	 
	    if (pidfils2 == 0)
	    {
	    	//Code du fils 2:
	        char buffer[BUFSIZE];
            char *slong_ligne = (char*) malloc(4*sizeof(char));
            slong_ligne[0] = '\0';
            char ligne[100] = ""; //Ligne du fichier input.txt
            //On ferme l'écriture
	        close(pfd[1]);
            int flag=-1;
                    //flag = -1 -> debut de ligne
                    //flag = 0 -> ligne en cours de traitement
                    //flag = -1 -> fin de la ligne
                    int i = 0; //curseur
                    int long_ligne; //longueur de la ligne a traiter
	        while (read(pfd[0], buffer, sizeof(char)*20)>0){
                buffer[20]='\0';
                //strcat (slong_ligne, buffer);
                i=i%20;
                    //printf("%s\n", buffer);
                    while(i<strlen(buffer)){
                        switch(flag){
                            case -1:
                                //Debut de ligne
                                if(strlen(slong_ligne) == 0)
                                    i++; //Skip le crochet ouvrant du header
                                sprintf(slong_ligne, "%s%c", slong_ligne, buffer[i++]);
                                if(strlen(slong_ligne) == 3){
                                    flag = 0;  
                                    long_ligne = atoi(slong_ligne);
                                    i+=2; //Skip le crochet fermant du header et ouvrant du body
                                } 
                                break;
                            case 0:
                                //Ligne en cours de traitement
                                if(strlen(ligne) == long_ligne){
                                    sprintf(ligne, "%s%c", ligne, '\n');
                                    flag = 1;
                                }
                                else{
                                    sprintf(ligne, "%s%c", ligne, buffer[i++]);
                                }
                                break;
                            case 1:
                                //Fin de ligne
                                printf("%s", ligne);
                                i++; //Skip le crochet fermant du body
                                sprintf(ligne, "");
                                *slong_ligne =  '\0';
                                flag = -1;
                                break;

                        }
                    }
            }

	 
	        close(pfd[0]); /* close the pipe */
	    }

 	//Attente du père:
	close(pfd[0]);
    close(pfd[1]);
    waitpid(pidfils1, NULL, WUNTRACED);
    waitpid(pidfils2, NULL, WUNTRACED);
    exit(0);
    }
 
    return 0;
}
