#include <stdio.h>
#include <stdlib.h>
#include  <stdbool.h>
#include  <string.h>
#define Total_block 100
#define Facteur_block 4

    typedef struct {
        char name[20];
        float price;
        int id;
    }produit;

    typedef struct {
        produit enregisrement[4];
        int nb_enregistrement;
        int ADR;
        int next;
    }Bloc;

    typedef struct {
    bool occupied;
    int nm_bloc;
    }allocation;

    typedef struct{
        allocation T[Total_block];
    }Tallocation;

     typedef struct {
       char Nom_du_fichier[30];
        int Taille_du_fichier;
        int nb_enregistrement;
        int adresse_firstblock;
        int org_globale; // Mode d'organisation globale (0: contigu, 1: cha�n�
        int org_interne;// Mode d'organisation interne (0: non, 1: oui)
        int etat;//(1: existe , 0: existe pas)
    }mt;

    typedef struct {
       char T[sizeof(Bloc)*Total_block + sizeof(allocation)];
       char meta[sizeof(mt) *Total_block];
       int file_count;
       FILE *Ms;
    }Ms;

void updateTabAlloc(Ms *ms , int pos, bool occupied)
{
    if (pos < 0 || pos >= Total_block) {
    fprintf(stderr, "Error: Block position %d is out of range.\n", pos);
    return;
}
    Tallocation T;
    memcpy(&T , ms->T ,  sizeof(Tallocation));
    T.T[pos].occupied = occupied;
    memcpy(ms->T , &T , sizeof(Tallocation));
}

void returnetat(Tallocation t ,int nm_bloc){
    if(t.T[nm_bloc].occupied == true){
        printf("le bloc %d est occuper\n",nm_bloc);
    }else{
        printf("le bloc %d est libre\n",nm_bloc);
    }
}
/*


void initialiserMs(Ms *ms, const char *nomFichier) {
    // V�rifie si le pointeur Ms est valide
    if (ms == NULL) {
        fprintf(stderr, "Erreur : Le pointeur Ms est NULL.\n");
        return;
    }

    // Initialisation du fichier
    ms->Ms = fopen(nomFichier, "wb+"); // Ouvre le fichier en lecture/�criture binaire
    if (ms->Ms == NULL) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier %s.\n", nomFichier);
        exit(EXIT_FAILURE);
    }

    // Initialisation des blocs en m�moire
    memset(ms->T, 0, sizeof(ms->T)); // Initialise tous les blocs � 0

    // Initialisation des autres champs
    ms->nm_bloc = 0; // Aucun bloc utilis� au d�part
    ms->occupied = false; // Indique que la m�moire secondaire est vide

    printf("Initialisation de la structure Ms termin�e.\n");
}


void modifierTableAllocation(Ms *ms, int blocIndex, bool occupe) {
    // V�rifie si le pointeur Ms est valide
    if (ms == NULL) {
        fprintf(stderr, "Erreur : Le pointeur Ms est NULL.\n");
        return;
    }

    // V�rifie si l'indice du bloc est valide
    if (blocIndex < 0 || blocIndex >= Total_block) {
        fprintf(stderr, "Erreur : Indice de bloc invalide (%d).\n", blocIndex);
        return;
    }

    // La table d'allocation est stock�e dans les premiers Total_block octets de T
    char *tableAllocation = ms->T;

    // Met � jour l'�tat du bloc dans la table d'allocation
    tableAllocation[blocIndex] = occupe ? 1 : 0;

    // Marque le fichier comme modifi�
    ms->occupied = true;

    printf("Bloc %d %s dans la table d'allocation.\n", blocIndex, occupe ? "occup�" : "lib�r�");
}

void viderMs(Ms *ms) {
    // V�rifie si le pointeur Ms est valide
    if (ms == NULL) {
        fprintf(stderr, "Erreur : Le pointeur Ms est NULL.\n");
        return;
    }

    // Remplit la m�moire secondaire avec des z�ros
    memset(ms->T, 0, sizeof(ms->T));

    // R�initialise les champs
    ms->nm_bloc = 0;
    ms->occupied = false;

    printf("M�moire secondaire vid�e avec succ�s.\n");
}
void chargerDepuisFichier(Ms *ms, const char *nom_fichier) {
    if (ms == NULL || nom_fichier == NULL) {
        fprintf(stderr, "Erreur : Pointeur invalide.\n");
        return;
    }

    FILE *f = fopen(nom_fichier, "a+");
    if (f == NULL) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier %s.\n", nom_fichier);
        return;
    }

    Bloc buffer; // Buffer pour stocker un bloc d'enregistrements
    buffer.nb_enregistrement = 0;

    produit temp;
    while (fscanf(f, "%19s %2s %9s", temp.name, temp.price, temp.id) == 3) {
        // Ajouter l'enregistrement au buffer
        buffer.enregisrement[buffer.nb_enregistrement++] = temp;

        // Si le buffer est plein, on le sauvegarde dans la m�moire secondaire
        if (buffer.nb_enregistrement == 5) {
            if (ms->nm_bloc >= Total_block) {
                fprintf(stderr, "Erreur : Pas assez d'espace dans la m�moire secondaire.\n");
                fclose(f);
                return;
            }

            memcpy(ms->T + ms->nm_bloc * sizeof(Bloc), &buffer, sizeof(Bloc));
            modifierTableAllocation(ms, ms->nm_bloc, true);
            ms->nm_bloc++;

            buffer.nb_enregistrement = 0; // R�initialiser le buffer
        }
    }

    // Sauvegarder le reste du buffer (si non vide)
    if (buffer.nb_enregistrement> 0) {
        if (ms->nm_bloc >= Total_block) {
            fprintf(stderr, "Erreur : Pas assez d'espace dans la m�moire secondaire.\n");
            fclose(f);
            return;
        }

        memcpy(ms->T + ms->nm_bloc * sizeof(Bloc), &buffer, sizeof(Bloc));
        modifierTableAllocation(ms, ms->nm_bloc, true);
        ms->nm_bloc++;
    }

    fclose(f);
    printf("Fichier %s charg� dans la m�moire secondaire.\n", nom_fichier);
}
void chargerDansBuffer(Ms *ms) {
    Bloc buffer;
    produit temp;
    buffer.nb_enregistrement = 0;  // R�initialiser le nombre d'enregistrements

    while (buffer.nb_enregistrement < 3) {
        printf("Entrez le nom du produit : ");
        scanf("%20s", &temp.name); // Saisie du nom du produit


        printf("Entrez le prix du produit : ");
        scanf("%f", &temp.price); // Saisie du prix

        printf("Entrez l'ID du produit : ");
        scanf("%d", &temp.id); // Saisie de l'ID

        // Ajouter le produit au buffer
        buffer.enregisrement[buffer.nb_enregistrement++] = temp;
    }
        // Si le buffer est plein, le sauvegarder dans la m�moire secondaire
        if (buffer.nb_enregistrement == 3) {
            if (ms->nm_bloc >= Total_block) {
                fprintf(stderr, "Erreur : Pas assez d'espace dans la m�moire secondaire.\n");
                return;
            }

            // Sauvegarder le buffer dans la m�moire secondaire
            memcpy(ms->T + ms->nm_bloc * sizeof(Bloc), &buffer, sizeof(Bloc));
            modifierTableAllocation(ms, ms->nm_bloc, true); // Marquer le bloc comme occup�
            ms->nm_bloc++; // Incr�menter le nombre de blocs

            // R�initialiser le buffer
            buffer.nb_enregistrement = 0;
        }


    // Sauvegarder le reste du buffer (s'il reste des produits)
    if (buffer.nb_enregistrement > 0) {
        if (ms->nm_bloc >= Total_block) {
            fprintf(stderr, "Erreur : Pas assez d'espace dans la m�moire secondaire.\n");
            return;
        }
        memcpy(ms->T + ms->nm_bloc * sizeof(Bloc), &buffer, sizeof(Bloc));
        modifierTableAllocation(ms, ms->nm_bloc, true); // Marquer le bloc comme occup�
        ms->nm_bloc++; // Incr�menter le nombre de blocs
    }

    printf("Les donn�es ont �t� charg�es dans la m�moire secondaire.\n");
}
void chargerMs(Ms *ms, const char *nomFichier) {
    if (ms == NULL || nomFichier == NULL) {
        fprintf(stderr, "Erreur : Pointeur invalide.\n");
        return;
    }

    // Ouvre le fichier binaire en lecture
    ms->Ms = fopen(nomFichier, "a+");  // Ouvre le fichier en mode lecture binaire
    if (ms->Ms == NULL) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier %s.\n", nomFichier);
        return;
    }

    // R�initialiser l'�tat de la m�moire secondaire
    memset(ms->T, 0, sizeof(ms->T));  // Remplir les blocs avec des z�ros
    ms->nm_bloc = 0;  // Aucun bloc utilis� au d�but
    ms->occupied = false;  // Initialiser l'occupation � faux

    Bloc buffer;  // Buffer pour lire un bloc
    int blocIndex = 0;  // Indice pour le bloc actuel
    while (fread(&buffer, sizeof(Bloc), 1, ms->Ms) == 1) {
        // Si le bloc est valide, le charger dans la m�moire secondaire
        if (buffer.nb_enregistrement > 0) {
            memcpy(ms->T + ms->nm_bloc * sizeof(Bloc), &buffer, sizeof(Bloc));

            // Mettre � jour la table d'allocation
            modifierTableAllocation(ms, ms->nm_bloc, true);

            ms->nm_bloc++;  // Incr�menter le compteur de blocs utilis�s
        }
    }

    fclose(ms->Ms);  // Fermer le fichier
    printf("M�moire secondaire charg�e depuis le fichier %s.\n", nomFichier);
}
void afficherElementsMs(Ms *ms) {
    printf("Affichage des �l�ments dans la m�moire secondaire :\n");

    // Parcourir tous les blocs dans la m�moire secondaire
    for (int i = 0; i < ms->nm_bloc; i++) {
        // Lire le bloc courant depuis la m�moire secondaire
        Bloc bloc;
        memcpy(&bloc, ms->T + i * sizeof(Bloc), sizeof(Bloc));

        printf("Bloc %d:\n", i + 1);

        // Parcourir les enregistrements dans le bloc
        for (int j = 0; j < bloc.nb_enregistrement; j++) {
            produit temp = bloc.enregisrement[j]; // Acc�der � l'enregistrement j

            // Afficher les d�tails du produit
            printf("  Produit %d:\n", j + 1);
            printf("    Nom   : %s\n", temp.name);
            printf("    Prix  : %f\n", temp.price); // Utilisez temp.price[0] pour afficher le prix
            printf("    ID    : %d\n", temp.id);    // Utilisez temp.id[0] pour afficher l'ID
        }
    }
}

    #include <stdbool.h>
#include <string.h>

// Structure pour repr�senter le r�sultat de la recherche
typedef struct {
    int bloc;     // Num�ro du bloc
    int position; // Position dans le bloc
    bool trouve;  // Indique si l'enregistrement est trouv�
} ResultatRecherche;

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



;

// Fonction de recherche utilisant un buffer
ResultatRecherche rechercherParIDAvecBuffer(Ms *ms, int idRecherche, bool globaleChainee, bool interneTriee) {
    ResultatRecherche resultat = {-1, -1, false}; // Initialisation du r�sultat

    // Cr�ation d'un buffer pour contenir plusieurs blocs
    Bloc buffer[3];

    int blocCourant = 0;

    while (blocCourant < ms->nm_bloc) {
        // Charger les blocs dans le buffer
        int blocsCharges = 0;
        for (int i = 0; i < 3 && blocCourant < ms->nm_bloc; i++, blocCourant++) {
            memcpy(&buffer[i], ms->T + blocCourant * sizeof(Bloc), sizeof(Bloc));
            blocsCharges++;
        }

        // Recherche dans les blocs du buffer
        for (int i = 0; i < blocsCharges; i++) {
            Bloc *bloc = &buffer[i];

            if (interneTriee) {
                // Recherche binaire dans un bloc tri�
                int gauche = 0, droite = bloc->nb_enregistrement - 1;
                while (gauche <= droite) {
                    int milieu = (gauche + droite) / 2;
                    if (bloc->enregisrement[milieu].id == idRecherche) {
                        resultat.bloc = blocCourant - blocsCharges + i; // Bloc correspondant
                        resultat.position = milieu;                     // Position dans le bloc
                        resultat.trouve = true;
                        return resultat;
                    } else if (bloc->enregisrement[milieu].id < idRecherche) {
                        gauche = milieu + 1;
                    } else {
                        droite = milieu - 1;
                    }
                }
            } else {
                // Recherche lin�aire dans un bloc non tri�
                for (int j = 0; j < bloc->nb_enregistrement; j++) {
                    if (bloc->enregisrement[j].id == idRecherche) {
                        resultat.bloc = blocCourant - blocsCharges + i; // Bloc correspondant
                        resultat.position = j;                          // Position dans le bloc
                        resultat.trouve = true;
                        return resultat;
                    }
                }
            }
        }
    }

    // Si aucun r�sultat n'a �t� trouv�
    printf("Enregistrement avec ID %d introuvable.\n", idRecherche);
    return resultat;
}
*/

// compactage de la m�moire secondaire

void compacterMs(Ms *ms, Tallocation *talloc) {

    // Check if the memory structure pointer is NULL
    if (ms == NULL) {
        fprintf(stderr, "Erreur : Le pointeur Ms est NULL.\n");
        return;
    }

    Bloc buffer; // Buffer to temporarily hold a single block
    int writeIndex = 0; // Index for writing to the memory space
    int readIndex = 0; // Index for reading from the memory space

    int DF = 0;  // Index of the first free block
    int FF = 0;  // Index of the last free block
    int i = 0;   // General index for iteration

    // Iterate through existing blocks until all blocks are processed
    while (DF < Total_block || FF < Total_block) {
        // Find the first free block
        while (i < Total_block && talloc->T[i].occupied == true) {
            i++; // Increment index until a free block is found
        }
        DF = i; // Set DF to the index of the first free block

        // If all blocks are occupied, print a message and exit
        if (i >= Total_block) {
            printf("Tous les blocs sont occup�s\n");
            return;
        } else {
            // Count the number of consecutive free blocks after DF
            while (i < Total_block && talloc->T[i].occupied == false) {
                i++; // Move to the next block
            }
            // If there are no more occupied blocks, print a message and exit
            if (i >= Total_block) {
                printf("Pas d'autre bloc occup�\n");
                return;
            } else {
                FF=i-1; // last free block count
                // Set readIndex to the next block after the last free block
                readIndex = FF + 1; // Start reading from the first block after FF
                writeIndex = DF; // Start writing from the first free block

                // Move all occupied blocks to the writeIndex position
                while (readIndex < Total_block && talloc->T[readIndex].occupied == true) {
                    // Read the current block into the buffer
                    memcpy(&buffer, &ms->T[readIndex], sizeof(Bloc));  ////////////////////////////////////////////

                    // Move the occupied block to the writeIndex position
                    memcpy(&ms->T[writeIndex], &buffer, sizeof(Bloc));/////////////////////////////////////////////

                    updateTabAlloc(ms , writeIndex, true); // Mark the new position as occupied
                    updateTabAlloc(ms , readIndex, false); // Mark the old position as free

                    writeIndex++; // Increment write index for the next write
                    readIndex++;  // Increment read index to process the next block
                }
            }
        }
        // Reset indices for the next iteration
        DF = 0; // Reset DF for the next iteration
        FF = 0; // Reset FF for the next iteration
        i = 0;  // Reset i for the next iteration
    }
}

void suppfichiercntg(Ms *ms , Tallocation *talloc , mt *metainfo) { //addr firstblock and nb erg from met

    int nb_reg = metainfo->nb_enregistrement;
    int addr_block = metainfo->adresse_firstblock;

    int block_to_delete = ceil((double)nb_reg / Facteur_block);
    int i = 0;
    while(i<block_to_delete){
        updateTabAlloc(ms , addr_block, false);
        addr_block = addr_block + 1;
        i++;
    }
    compacterMs(ms, talloc);
}

void suppfichierchaine(Ms *ms , Tallocation *talloc , mt *metainfo) {

    int nb_reg = metainfo->nb_enregistrement;
    int addr_block = metainfo->adresse_firstblock;
    Bloc buffer ;
    int block_to_delete = ceil((double)nb_reg / Facteur_block);
    int i = 0;
   while(i<block_to_delete){
        updateTabAlloc(ms , addr_block, false);
        memcpy(&buffer, &ms->T[addr_block], sizeof(Bloc)); //////////////////////////////////////////////
        addr_block = buffer.next;
        i++;
    }
    compacterMs(ms, talloc);
}

suppfichier(Ms *ms , Tallocation *talloc , mt *metainfo ){

    bool choix = metainfo->org_globale ;

    if(choix ==true){
        suppfichiercntg(ms, talloc, metainfo);
    }
    else{
        suppfichierchaine(ms, talloc, metainfo);
    }

}


/*

int main() {
    Ms ms;

    // Initialiser la m�moire secondaire
    initialiserMs(&ms, "ms.txt");

    // Charger des produits dans le buffer
    chargerDansBuffer(&ms);

    // Afficher les �l�ments de la m�moire secondaire
    afficherElementsMs(&ms);

    // Rechercher un enregistrement par ID
    int idRecherche;
    printf("Entrez l'ID � rechercher : ");
    scanf("%d", &idRecherche);

    // Effectuer la recherche avec buffer
    ResultatRecherche resultat = rechercherParIDAvecBuffer(&ms, idRecherche, false, false);

    // Afficher le r�sultat
    if (resultat.trouve) {
        printf("Enregistrement trouv� dans le bloc %d, position %d.\n", resultat.bloc, resultat.position);
    } else {
        printf("Enregistrement non trouv�.\n");
    }

    return 0;
}





/*
void initialiserLaMs( Ms *Ms ){

    printf("Initialiser la Ms ...\n");

    for(int i=0; i<sizeof(bloc)*Total_block ;i++){
        Ms->T[i]='\0';
    }
    for(int i=0; i<Total_block ;i++){
        Ms->nm_bloc=i;
    }

}


 void MajTabledallocation(Ms *Ms ) {
    printf("MajTabledallocation ...\n");
    allocation T[Total_block-1];
    bloc buffer;

    for(int j=1; j<Total_block ;j++) {
        fseek(Ms, sizeof(buffer), SEEK_SET+1);
        for(int i = j*sizeof(bloc); i< (j+1)*sizeof(bloc) ;i++) {
            if(Ms->T[i]!='\0') {
                T[j].occupied=true;
                break;
            }

        }
        T[j].nm_bloc = j;
        fwrite(&buffer, sizeof(buffer), 1, Ms->T[0]);
    }



/*
int main(){
    Ms Ms;

    initialiserLaMs(&Ms);

    produit p;
    strcpy(p.age, "25");
    strcpy(p.id, "100");
    strcpy(p.name,"Ahmed");

    memcpy(&Ms.T[0], &p, sizeof(produit));


    for(int i=0; i<sizeof(bloc) ;i++){
        memcpy(&Ms.T[i], &p, sizeof(produit));
    }

    for(int i=0; i<5 ;i++){
        printf("Ms->T[%d] = %c\n",i,Ms.T[i]);
    }




    return 0;
}int main() {
    Ms Ms;

    initialiserLaMs(&Ms);
    produit p;

    for (int i = 0; i < sizeof(bloc); i++) {
        printf("Ms->T[%d] = %c\n", i, Ms.T[i]);
    }


    for(int i=0; i<5 ;i++) {

        printf("Entrez le nom : ");
        scanf(" %20s", p.name);


        printf("Entrez l'�ge : ");
        scanf(" %3s", p.age);


        printf("Entrez l'ID : ");
        scanf(" %10s", p.id);


        memcpy(&Ms.T[i * sizeof(produit)], &p, sizeof(produit));
    }


    for (int i = 0; i < sizeof(bloc); i++) {
        printf("Ms->T[%d] = %c\n", i, Ms.T[i]);
    }


    return 0;
}


*/
