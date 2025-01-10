#include <stdio.h>
#include <stdlib.h>
#include  <stdbool.h>
#include  <string.h>
#define Total_block 100
#define Facteur_block 4
#include <windows.h>
#include <limits.h>



typedef struct {
    int bloc;     // Numéro du bloc
    int position; // Position dans le bloc
    bool trouve;  // Indique si l'enregistrement est trouvé
} ResultatRecherche;


void gotoxy(int x, int y) {
    COORD c;
    c.X = x;
    c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
void color(int couleurDuTexte, int couleurDeFond) {
    HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(H, couleurDeFond * 16 + couleurDuTexte);
}

    typedef struct {
        char name[20];
        float price;
        int id;
    }produit;

    typedef struct {
        produit enregisrement[Facteur_block];
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
void initialisebuffer(Bloc *buffer , int index){
    buffer->nb_enregistrement=0;
    buffer->ADR = index;
    for(int i = 0 ; i<4 ; i++){
    buffer->enregisrement[i].id = 0;
    buffer->enregisrement[i].price = 0.0;
    strcpy(buffer->enregisrement[i].name,"a");
    }
    buffer->next = -1;
}
void initialiseTallocation(Tallocation T){
    for(int i = 0 ; i<Total_block ;i++){
        T.T[i].nm_bloc=i;
        T.T[i].occupied=false;
    }
}
void initialisemeta(mt *meta){
    meta->adresse_firstblock=-1;
    meta->etat = 0;
    meta->nb_enregistrement=-1;
    strcpy(meta->Nom_du_fichier , "r");
    meta->org_globale=-1;
    meta->org_interne =-1;
    meta->Taille_du_fichier =-1;
}
void initialisems(Ms *ms){
    Tallocation T;
    initialiseTallocation(T);
    Bloc buffer;
    mt meta;
    initialisemeta(&meta);
    ms->file_count = 0;
    memcpy(ms->T , &T ,sizeof(Tallocation));
    for(int i = 0 ; i<Total_block ; i++){
        initialisebuffer(&buffer , i);
        memcpy(ms->T + sizeof(Tallocation) + sizeof(Bloc)*i, &buffer  ,sizeof(Bloc));
        memcpy(ms->meta + i*sizeof(meta), &meta , sizeof(mt));
    }
}
produit lireProduit() {

    produit p;

    // Lire le nom du produit
    printf("Entrez le nom du produit :  \n");
    scanf("%s", p.name);

    // Lire le prix du produit
    printf("Entrez le prix du produit :  \n");
    scanf("%f", &p.price);

    // Lire l'ID du produit
    printf("Entrez l'ID du produit :  \n");
    scanf("%d", &p.id);

    return p;
}
int getFreeBlocPos(Ms *ms){
    Tallocation T;
    memcpy(&T , ms->T , sizeof(Tallocation));
    int j = 0;
    while(j < Total_block )
    {
        if(T.T[j].occupied == false)
        {
            return j;
        }
        j++;
    }
    return -1;
}

void creationfichier(Ms *ms){
    //initialization de meta
    mt meta;
    memcpy(&meta ,ms->meta,sizeof(meta));
    int i = 0;
    while(meta.etat == 1 && i<Total_block){
        i++;
        memcpy(&meta ,ms->meta +i*sizeof(mt),sizeof(mt));
    }
    int posmeta = i ;
    if(i==Total_block){
        printf("pas d'espace disponible pour mettre la meta");
    }else{
    int NE,mode_OG ,mode_OI;
    char nom_fichier[30];
    produit p;
    Tallocation T;
    // Demander le nombre d'enregistrements
    printf("Entrez le nombre d'enregistrements :  \n");
    scanf("%d", &NE);
    meta.nb_enregistrement = NE;
    produit enregistrement[NE];
    // Demander le mode d'organisation interne
    printf("Entrez le mode d'organisation global (1 pour mode chaine, 0 pour mode contigu) : \n");
    scanf("%d", &mode_OG);
    meta.org_globale = mode_OG;
    // Demander le mode d'organisation global
    printf("Entrez le mode d'organisation interne (1 pour mode ordonne , 0 pour mode non-ordonne ) : \n");
    scanf("%d", &mode_OI);
    meta.org_interne = mode_OI;
    // Demander le nom du fichier
    printf("Entrez le nom du fichier :  \n");
    scanf("%s", nom_fichier);
    strcpy(meta.Nom_du_fichier,nom_fichier);

     div_t temp = div(NE , 4);
     if(temp.rem == 0){
        meta.Taille_du_fichier = temp.quot;
     }else{
        meta.Taille_du_fichier = temp.quot+1;
     }

    //stock les donnee en tableau
    printf("entrez les enregistrement necessaire \n");
    for(i=0 ; i<NE ; i++){
        p = lireProduit();
        enregistrement[i] = p;
    }


    if(mode_OI == 1){
    //organise le tableau d'apres la cle ID :
        for (i = 0; i < NE; i++) {
        int minIdIndex = i;
            for (int j = i + 1; j < NE; j++) {
                if (enregistrement[j].id < enregistrement[minIdIndex].id) {
                    minIdIndex = j;
                }
            }
                if (minIdIndex != i) {
                produit temp = enregistrement[i];
                enregistrement[i] = enregistrement[minIdIndex];
                enregistrement[minIdIndex] = temp;
                }
        }
    }
    Bloc buffer2;
    //recherche de l'espace en ms
    if(mode_OG == 0){
    memcpy(&T , ms->T  , sizeof(Tallocation));
    i=0;
    int j=0;
    while(i<meta.Taille_du_fichier && j<=Total_block+1){
        if(T.T[j].occupied == false){
            i++;
        }else{
            i=0;
        }
        j++;
    }

    if(i == meta.Taille_du_fichier){
    meta.adresse_firstblock = j-i;
    meta.etat=1;
    ms->file_count++;
    memcpy( ms->meta + sizeof(mt)*posmeta , &meta , sizeof(mt));
    }else{
    printf("pas d'espace");
    }

    int w=0;
    for(i=0 ; i<meta.Taille_du_fichier ; i++){
        memcpy(&buffer2 , ms->T + sizeof(Tallocation) + meta.adresse_firstblock*sizeof(Bloc) + i*sizeof(Bloc) ,sizeof(Bloc));
        j=0;
        while(j<4 && w<meta.nb_enregistrement){
            buffer2.enregisrement[j] = enregistrement[w];
            buffer2.nb_enregistrement++;
            w++;
            j++;
        }
        memcpy(ms->T + sizeof(Tallocation) + i*sizeof(Bloc) + meta.adresse_firstblock*sizeof(Bloc) ,&buffer2 ,sizeof(Bloc));
        updateTabAlloc(ms,buffer2.ADR,true);
    }
    }else{
        memcpy(&T , ms->T  , sizeof(Tallocation));
        i=0;
        int j=0;
        while(i<meta.Taille_du_fichier && j<Total_block){
            if(T.T[j].occupied == false){
                i++;
            }
            if(i == 1){
                meta.adresse_firstblock = j;
            }
            j++;
        }
        if(i==meta.Taille_du_fichier){
        meta.etat=1;
        memcpy( ms->meta + sizeof(mt)* posmeta, &meta , sizeof(mt));
        ms->file_count++;
        }else{
        printf("pas d'espace");
        }
        j=1;
        i=0;
        int w=0;
        while(j <= meta.Taille_du_fichier)
        {
            int freePos = getFreeBlocPos(ms);
            int NextFreePos;
            memcpy(&buffer2 , ms->T +sizeof(Tallocation)+ freePos*sizeof(Bloc) + meta.adresse_firstblock * sizeof(Bloc)  , sizeof(Bloc));
            for(i = 0; i < 4; i++)
            {
                if(w < meta.nb_enregistrement)
                {
                    buffer2.enregisrement[i] = enregistrement[w];
                    buffer2.nb_enregistrement++;
                    w++;
                }
            }
            updateTabAlloc(ms , freePos, true);
            NextFreePos = getFreeBlocPos(ms);
            buffer2.next = NextFreePos;

            if(j == meta.Taille_du_fichier) // if last bloc does not have next
            {
                buffer2.next = -1;
            }
            memcpy(ms->T + sizeof(Tallocation) + freePos*sizeof(Bloc),&buffer2 ,sizeof(Bloc));
            j++;
        }
    }
    }
}
const char *getFileNameForBlock(int blockIndex, mt *fileTable, int fileCount, Ms *ms) {
    for (int i = 0; i < fileCount; i++) {
        if (fileTable[i].etat == 1) { // Only check existing files
            if (fileTable[i].org_globale == 0) { // Contiguous
                int startBlock = fileTable[i].adresse_firstblock;
                int endBlock = startBlock + fileTable[i].Taille_du_fichier - 1;
                if (blockIndex >= startBlock && blockIndex <= endBlock) {
                    return fileTable[i].Nom_du_fichier;
                }
            } else { // Chained
                int currentBlockIndex = fileTable[i].adresse_firstblock;
                while (currentBlockIndex != -1) {
                    Bloc *currentBlock = (Bloc *)(ms->T + sizeof(Tallocation) + currentBlockIndex * sizeof(Bloc));
                    if (currentBlock->ADR == blockIndex) { // Compare ADR
                        return fileTable[i].Nom_du_fichier;
                    }
                    currentBlockIndex = currentBlock->next;
                }
            }
        }
    }
    return NULL;
}
void affichageEnBloc(Ms *ms) {
    printf("Affichage de l'etat des blocs de la memoire:\n");
    printf("+---------------------+---------------------+---------------------+---------------------+\n");
    printf("| Block Index         | File Name           | Nb Enregistrements   | Etat (Free / Occupied)|\n");
    printf("+---------------------+---------------------+---------------------+---------------------+\n");

    char *dataAreaStart = ms->T + sizeof(Tallocation);

    for (int i = 0; i < Total_block; i++) {
        Bloc *blocPtr = (Bloc *)(dataAreaStart + i * sizeof(Bloc));
        const char *fileName = getFileNameForBlock(i, (mt*)ms->meta, ms->file_count, ms); // Pass ms

        if (fileName == NULL || blocPtr->nb_enregistrement == 0) {
            fileName = "Free";
            color(10, 0);
        } else {
            color(12, 0);
        }

        printf("| %-19d | %-19s | %-19d | %-19s |\n", i, fileName, blocPtr->nb_enregistrement, (blocPtr->nb_enregistrement == 0 ? "Free" : "Occupied"));
        printf("+---------------------+---------------------+---------------------+---------------------+\n");
    }
    color(15, 0);
}
ResultatRecherche rechercherParIDAvecBuffer(Ms *ms, int idRecherche, const char *nomFichier) {
    ResultatRecherche resultat = {-1, -1, false};
    mt *fileTable = (mt*)ms->meta;

    // Find the file metadata
    int fileIndex = -1;
    for (int i = 0; i < ms->file_count; i++) {
        if (strcmp(fileTable[i].Nom_du_fichier, nomFichier) == 0 && fileTable[i].etat == 1) {
            fileIndex = i;
            break;
        }
    }

    if (fileIndex == -1) {
        printf("Fichier '%s' introuvable.\n", nomFichier);
        return resultat; // File not found
    }

    int firstBlock = fileTable[fileIndex].adresse_firstblock;
    int orgGlobale = fileTable[fileIndex].org_globale;
    int orgInterne = fileTable[fileIndex].org_interne;
    int tailleFichier = fileTable[fileIndex].Taille_du_fichier;

    // Define a buffer to store the block data
    Bloc bufferBloc;

    if (orgGlobale == 0) { // Contiguous
        // Load blocks in the range
        for (int blocCourant = firstBlock; blocCourant < firstBlock + tailleFichier; blocCourant++) {
            // Copy block data into buffer
            memcpy(&bufferBloc, ms->T + sizeof(Tallocation) + blocCourant * sizeof(Bloc), sizeof(Bloc));

            for (int j = 0; j < bufferBloc.nb_enregistrement; j++) {
                if (bufferBloc.enregisrement[j].id == idRecherche) {
                    resultat.bloc = blocCourant;
                    resultat.position = j;
                    resultat.trouve = true;
                    return resultat;
                }
            }
        }
    } else { // Chained
        int blocCourant = firstBlock;
        while (blocCourant != -1) {
            // Load the current block into the buffer
            memcpy(&bufferBloc, ms->T + sizeof(Tallocation) + blocCourant * sizeof(Bloc), sizeof(Bloc));

            if (orgInterne) { // Binary search if sorted
                int gauche = 0, droite = bufferBloc.nb_enregistrement - 1;
                while (gauche <= droite) {
                    int milieu = (gauche + droite) / 2;
                    if (bufferBloc.enregisrement[milieu].id == idRecherche) {
                        resultat.bloc = blocCourant;
                        resultat.position = milieu;
                        resultat.trouve = true;
                        return resultat;
                    } else if (bufferBloc.enregisrement[milieu].id < idRecherche) {
                        gauche = milieu + 1;
                    } else {
                        droite = milieu - 1;
                    }
                }
            } else { // Linear search
                for (int j = 0; j < bufferBloc.nb_enregistrement; j++) {
                    if (bufferBloc.enregisrement[j].id == idRecherche) {
                        resultat.bloc = blocCourant;
                        resultat.position = j;
                        resultat.trouve = true;
                        return resultat;
                    }
                }
            }

            // Move to the next block in the chain
            blocCourant = bufferBloc.next;
        }
    }

    printf("Enregistrement avec ID %d introuvable dans le fichier '%s'.\n", idRecherche, nomFichier);
    return resultat;
}
void insertion(Ms *ms , produit p){

    Tallocation T;
    Bloc buffer;
    int index;
    mt meta;
    int i=0;
    char filename[20];
    memcpy(&meta ,ms->meta + i*sizeof(mt),sizeof(mt));
    printf("entrez le nom du fichier : \n");

    scanf("%s" , filename);
    while(strcmp(meta.Nom_du_fichier, filename)!=0){
            i++;
        memcpy(&meta ,ms->meta + i*sizeof(mt),sizeof(mt));
    }
    index = i;
    memcpy(&T , ms->T , sizeof(Tallocation));
     i = 0;
    int j = 1;

    if(meta.org_globale == 0){
        if(meta.org_interne == 0)
        {//TNO
                memcpy(&buffer,ms->T + meta.adresse_firstblock * (sizeof(Bloc)) + sizeof(Tallocation) + (meta.Taille_du_fichier-1) * sizeof(Bloc), sizeof(Bloc));//lire dernier bloc en buffer
                if(buffer.nb_enregistrement<4){//l'espace existe dans le dernier bloc du fichier

                    buffer.enregisrement[buffer.nb_enregistrement] = p;
                    buffer.nb_enregistrement++;
                    memcpy(ms->T + buffer.ADR * sizeof(Bloc) + sizeof(Tallocation), &buffer, sizeof(Bloc));
                    meta.nb_enregistrement++;
                }else{//besoin de mettre le produit dans un nouveau bloc
                    if(T.T[meta.adresse_firstblock+meta.Taille_du_fichier].occupied == false){
                        memcpy(&buffer,ms->T + meta.adresse_firstblock * (sizeof(Bloc)) + sizeof(Tallocation) + meta.Taille_du_fichier * sizeof(Bloc), sizeof(Bloc));
                        buffer.enregisrement[0] = p;
                        buffer.nb_enregistrement++;
                        memcpy(ms->T + buffer.ADR * sizeof(Bloc)  + sizeof(Tallocation), &buffer, sizeof(Bloc));
                        meta.Taille_du_fichier++;
                        meta.nb_enregistrement++;
                        updateTabAlloc(ms , meta.adresse_firstblock + meta.Taille_du_fichier-1 , true);
                    }else{//impossible
                        printf("pas d'espace dans les blocs du fichier.");
                    }
                }
        }else//TO
            {
                if(meta.nb_enregistrement % 4 != 0) {//cas ou il ya de l'espace dans le bloc
                    produit tab[meta.nb_enregistrement+1];
                    memcpy(&buffer , ms->T + meta.adresse_firstblock * sizeof(Bloc) + sizeof(Tallocation) , sizeof(Bloc));//premier bloc
                    i=1;
                    j=0;
                    while(j<meta.nb_enregistrement){
                        for(int k = 0 ; k<4 ; k++){
                            if(j<meta.nb_enregistrement)
                            {
                                tab[j] = buffer.enregisrement[k];
                                j++;
                            }

                        }
                        memcpy(&buffer , ms->T + meta.adresse_firstblock * sizeof(Bloc)+ sizeof(Tallocation) + sizeof(Bloc) * i , sizeof(Bloc));//prochain bloc
                        i++;
                    }
                    tab[meta.nb_enregistrement] = p;
                    for (int i = 0; i <= meta.nb_enregistrement; i++) {//trier tab
                    int minIdIndex = i;
                        for (int j = i + 1; j <= meta.nb_enregistrement; j++) {
                            if (tab[j].id < tab[minIdIndex].id) {
                                minIdIndex = j;
                            }
                        }
                            if (minIdIndex != i) {
                            produit temp = tab[i];
                            tab[i] = tab[minIdIndex];
                            tab[minIdIndex] = temp;
                            }
                    }
                    int w=0;
                    for(int i = 0 ; i<meta.Taille_du_fichier ; i++){
                    memcpy(&buffer , ms->T + sizeof(Tallocation) + meta.adresse_firstblock*sizeof(Bloc) + i*sizeof(Bloc) ,sizeof(Bloc));
                    j=0;
                    while(j<4 && w<=meta.nb_enregistrement){
                        buffer.enregisrement[j] = tab[w];
                        w++;
                        j++;
                    }
                    memcpy(ms->T + sizeof(Tallocation) + sizeof(Bloc) * buffer.ADR , &buffer , sizeof(Bloc));
                    }
                    buffer.nb_enregistrement++;
                    memcpy(ms->T + sizeof(Tallocation) + sizeof(Bloc) * buffer.ADR , &buffer , sizeof(Bloc));
                    meta.nb_enregistrement++;
                    } else{//on a besoin d'un nouveau bloc
                        if(T.T[meta.adresse_firstblock + meta.Taille_du_fichier].occupied == false){
                            produit tab[meta.nb_enregistrement+1];
                            updateTabAlloc(ms,meta.adresse_firstblock + meta.Taille_du_fichier,true);
                            memcpy(&buffer , ms->T + meta.adresse_firstblock * sizeof(Bloc) + sizeof(Tallocation) , sizeof(Bloc));//premier bloc
                            i=1;
                            j=0;
                            while(j < meta.nb_enregistrement){
                                for(int k = 0 ; k<4 ; k++){
                                    tab[j] = buffer.enregisrement[k];
                                    j++;
                                }
                                memcpy(&buffer , ms->T + meta.adresse_firstblock * sizeof(Bloc)+ sizeof(Tallocation) + sizeof(Bloc) * i , sizeof(Bloc));//prochain bloc
                                i++;
                            }
                            tab[meta.nb_enregistrement] = p;

                            for (int i = 0; i <= meta.nb_enregistrement; i++) {//trier tab
                            int minIdIndex = i;
                                for (int j = i + 1; j <= meta.nb_enregistrement; j++) {
                                    if (tab[j].id < tab[minIdIndex].id) {
                                        minIdIndex = j;
                                    }
                                }
                                    if (minIdIndex != i) {
                                    produit temp = tab[i];
                                    tab[i] = tab[minIdIndex];
                                    tab[minIdIndex] = temp;
                                    }
                            }
                            int w=0;
                            for(int i = 0 ; i<meta.Taille_du_fichier+1 ; i++){
                            memcpy(&buffer , ms->T + sizeof(Tallocation) + meta.adresse_firstblock*sizeof(Bloc) + i*sizeof(Bloc) ,sizeof(Bloc));
                            j=0;
                            while(j<4 && w<=meta.nb_enregistrement){
                                buffer.enregisrement[j] = tab[w];
                                w++;
                                j++;
                            }
                            memcpy(ms->T + sizeof(Tallocation) + sizeof(Bloc) * buffer.ADR , &buffer , sizeof(Bloc));
                            }
                            buffer.nb_enregistrement++;
                            memcpy(ms->T + sizeof(Tallocation) + sizeof(Bloc) * buffer.ADR , &buffer , sizeof(Bloc));
                            meta.nb_enregistrement++;
                            meta.Taille_du_fichier++;
                    }else{
                        printf("pas d'espace");
                    }
                }
                if(meta.nb_enregistrement == 0){
                    memcpy(&buffer , ms->T + sizeof(Tallocation) + meta.adresse_firstblock * sizeof(Bloc) , sizeof(Bloc));
                    buffer.enregisrement[0] = p;
                    memcpy( ms->T + sizeof(Tallocation) + buffer.ADR * sizeof(Bloc) ,&buffer , sizeof(Bloc));
                    meta.nb_enregistrement++;
                }

    }
    }else{//L
        if(meta.org_interne == 0){//LNO

            memcpy(&buffer , ms->T + meta.adresse_firstblock * sizeof(Bloc) + sizeof(Tallocation), sizeof(Bloc));
            int pos;
               while(buffer.next != -1){//trouver dernier bloc
                    memcpy(&buffer , ms->T + (buffer.next) * (sizeof(Bloc)) + sizeof(Tallocation), sizeof(Bloc));
               }
           if(buffer.nb_enregistrement<4){//dernier bloc a de l'espace
                buffer.enregisrement[buffer.nb_enregistrement] = p;
                buffer.nb_enregistrement++;
                memcpy(ms->T + buffer.ADR * sizeof(Bloc) + sizeof(Tallocation), &buffer, sizeof(Bloc));
                meta.nb_enregistrement++;
           }else{//dernier bloc a pas d'espace
                i = 0;
                while(T.T[i].occupied == true && i<=Total_block){
                    i++;
                }
                if(i==Total_block){
                    printf("pas d'espace disponible");
                }else{
                    pos = i;
                    memcpy(&buffer , ms->T + i*sizeof(Bloc) + sizeof(Tallocation) , sizeof(Bloc));
                    buffer.enregisrement[0] = p;
                    buffer.nb_enregistrement++;
                    memcpy( ms->T + i*sizeof(Bloc) + sizeof(Tallocation) , &buffer , sizeof(Bloc));
                    updateTabAlloc(ms , pos , true);
                    meta.nb_enregistrement++;
                    meta.Taille_du_fichier++;
                }

           }
        }else{//LO
            if(meta.nb_enregistrement % 4 != 0) {//cas ou il ya de l'espace dans le bloc
                   produit tab[meta.nb_enregistrement+1];
                   memcpy(&buffer,ms->T + sizeof(Tallocation) + sizeof(Bloc)*meta.adresse_firstblock  ,sizeof(Bloc));
                   int w=0;
                   int blocCount = 0;
                   while(buffer.next != -1){
                        for(int i = 0 ; i < 4 ; i++){
                            tab[w] = buffer.enregisrement[i];
                            w++;
                        }
                    memcpy(&buffer,ms->T + sizeof(Tallocation) + buffer.next*sizeof(Bloc)  ,sizeof(Bloc) );
                    blocCount++;
                   }
                   int rest = meta.nb_enregistrement - blocCount*4;
                      if(rest != 0)
                      {
                          for(int i = 0 ; i < rest ; i++){
                                tab[w] = buffer.enregisrement[i];
                                w++;
                            }
                          tab[meta.nb_enregistrement] = p;
                      }
                  for (int i = 0; i <= meta.nb_enregistrement; i++) {//trier tab
                    int minIdIndex = i;
                        for (int j = i + 1; j <= meta.nb_enregistrement; j++) {
                            if (tab[j].id < tab[minIdIndex].id) {
                                minIdIndex = j;
                            }
                        }
                        if (minIdIndex != i) {
                        produit temp = tab[i];
                        tab[i] = tab[minIdIndex];
                        tab[minIdIndex] = temp;
                        }
                    }

                    memcpy(&buffer , ms->T + sizeof(Tallocation) + sizeof(Bloc)*meta.adresse_firstblock , sizeof(Bloc));
                    w=0;
                    while(buffer.next != -1){
                        for(int i = 0 ; i < 4 ; i++){
                             buffer.enregisrement[i] = tab[w];
                             w++;
                        }
                        memcpy(ms->T + sizeof(Tallocation) + sizeof(Bloc)*buffer.ADR , &buffer ,  sizeof(Bloc));
                        memcpy(&buffer , ms->T + sizeof(Tallocation) + sizeof(Bloc)*buffer.next , sizeof(Bloc));
                    }
                        for(int i = 0 ; i < rest + 1 ; i++){
                             buffer.enregisrement[i] = tab[w];
                             w++;
                        }
                        buffer.nb_enregistrement++;
                        memcpy(ms->T + sizeof(Tallocation) + sizeof(Bloc)*buffer.ADR , &buffer ,  sizeof(Bloc));
                        meta.nb_enregistrement++;
                        //setmeta
            }else{
                if(meta.nb_enregistrement == 0){
                    memcpy(&buffer , ms->T + sizeof(Tallocation) + meta.adresse_firstblock * sizeof(Bloc) , sizeof(Bloc));
                    buffer.enregisrement[0] = p;
                    buffer.nb_enregistrement++;
                    memcpy( ms->T + sizeof(Tallocation) + buffer.ADR * sizeof(Bloc) ,&buffer , sizeof(Bloc));
                    updateTabAlloc( ms , buffer.ADR , true);
                }else
                {
                int freeB = getFreeBlocPos(ms);
                if(freeB != -1){
                         memcpy(&buffer,ms->T + sizeof(Tallocation) + sizeof(Bloc)*meta.adresse_firstblock  ,sizeof(Bloc));
                        while(buffer.next != -1){
                          memcpy(&buffer,ms->T + sizeof(Tallocation) + buffer.next*sizeof(Bloc)  ,sizeof(Bloc) );
                        }
                    buffer.next = getFreeBlocPos(ms);
                    updateTabAlloc(ms , buffer.next , true);
                    memcpy(ms->T + sizeof(Tallocation) + sizeof(Bloc)*buffer.ADR , &buffer ,  sizeof(Bloc));
                  produit tab[meta.nb_enregistrement+1];
                  memcpy(&buffer,ms->T + sizeof(Tallocation) + sizeof(Bloc)*meta.adresse_firstblock  ,sizeof(Bloc));
                  int w=0;
                  while(buffer.next != -1){
                        for(int i = 0 ; i < 4 ; i++){
                            tab[w] = buffer.enregisrement[i];
                            w++;
                        }
                    memcpy(&buffer,ms->T + sizeof(Tallocation) + buffer.next*sizeof(Bloc)  ,sizeof(Bloc) );
                  }
                  tab[meta.nb_enregistrement] = p;
                  for (int i = 0; i <= meta.nb_enregistrement; i++) {//trier tab
                    int minIdIndex = i;
                        for (int j = i + 1; j <= meta.nb_enregistrement; j++) {
                            if (tab[j].id < tab[minIdIndex].id) {
                                minIdIndex = j;
                            }
                        }
                        if (minIdIndex != i) {
                        produit temp = tab[i];
                        tab[i] = tab[minIdIndex];
                        tab[minIdIndex] = temp;
                        }
                    }

                    memcpy(&buffer , ms->T + sizeof(Tallocation) + sizeof(Bloc)*meta.adresse_firstblock , sizeof(Bloc));
                    w=0;
                    while(buffer.next != -1){
                        for(int i = 0 ; i < 4 ; i++){
                             buffer.enregisrement[i] = tab[w];
                             w++;
                        }
                        memcpy(ms->T + sizeof(Tallocation) + sizeof(Bloc)*buffer.ADR , &buffer ,  sizeof(Bloc));
                        memcpy(&buffer , ms->T + sizeof(Tallocation) + sizeof(Bloc)*buffer.next , sizeof(Bloc));
                    }

                        buffer.enregisrement[0] = tab[meta.nb_enregistrement];
                        buffer.nb_enregistrement++;
                        memcpy(ms->T + sizeof(Tallocation) + sizeof(Bloc)*buffer.ADR , &buffer ,  sizeof(Bloc));
                        meta.nb_enregistrement++;
                        meta.Taille_du_fichier++;
                        //setmeta
                }else{
                printf("pas d'espace");
                }
            }
            }
        }
    }
    memcpy( ms->meta + index*sizeof(mt) , &meta , sizeof(mt));
}
int getPreviousBlockIndex(Ms *ms, int currentBlocIndex) {
    // Loop through the blocks to find the previous block (depends on how blocks are chained)
    for (int i = 0; i < Total_block; i++) {  // Loop through available blocks (not just total)
        // Treat ms->T as an array of Bloc objects
        Bloc *blocPtr = (Bloc *)(ms->T+ sizeof(Tallocation) + i * sizeof(Bloc));  // Cast to pointer to Bloc
        if (blocPtr->next == currentBlocIndex) {
            return i;  // Return the index of the previous block
        }
    }
    return -1;  // Return -1 if no previous block is found (e.g., if it's the first block)
}
void supprimerLogiquement(Ms *ms, int idRecherche, const char *nomFichier) {
    // Step 1: Search for the record (using the corrected function)
    ResultatRecherche resultat = rechercherParIDAvecBuffer(ms, idRecherche, nomFichier);

    if (!resultat.trouve) {
        printf("Enregistrement avec ID %d introuvable dans le fichier '%s', suppression annulée.\n", idRecherche, nomFichier);
        return;
    }

    // Step 2: Mark the record as logically deleted
    Bloc bloc;
    memcpy(&bloc, ms->T + sizeof(Tallocation) + resultat.bloc * sizeof(Bloc), sizeof(Bloc));

    // Check if the record is already deleted (name starts with '*')
    if (bloc.enregisrement[resultat.position].name[0] == '*') {
        printf("Enregistrement avec ID %d est déjà supprimé logiquement.\n", idRecherche);
        return;
    }

    // Mark as deleted by prepending '*' to the name
    char deletedName[21]; // One extra char for '*' and null terminator
    snprintf(deletedName, sizeof(deletedName), "*%s", bloc.enregisrement[resultat.position].name);
    strncpy(bloc.enregisrement[resultat.position].name, deletedName, 20); // Copy back to the block

    // Write the modified block back to memory
    memcpy(ms->T + sizeof(Tallocation) + resultat.bloc * sizeof(Bloc), &bloc, sizeof(Bloc));

     printf("Enregistrement avec ID %d dans le fichier '%s' supprimé logiquement.\n", idRecherche, nomFichier);
}
void supprimerPhysiquement(Ms *ms, int idRecherche, const char *nomFichier) {
    ResultatRecherche resultat = rechercherParIDAvecBuffer(ms, idRecherche, nomFichier);
    if (!resultat.trouve) {
        printf("Enregistrement avec ID %d introuvable, suppression annulée.\n", idRecherche);
        return;
    }

    // Get file metadata
    mt *fileTable = (mt*)ms->meta;
    int fileIndex = -1;
    for (int i = 0; i < ms->file_count; i++) {
        if (strcmp(fileTable[i].Nom_du_fichier, nomFichier) == 0 && fileTable[i].etat == 1) {
            fileIndex = i;
            break;
        }
    }

    if (fileIndex == -1) {
        printf("Fichier '%s' introuvable.\n", nomFichier);
        return;
    }

    bool interneTriee = fileTable[fileIndex].org_interne;
    bool globaleChainee = fileTable[fileIndex].org_globale;

    // Use a buffer for optimization
    Bloc buffer;
    memcpy(&buffer, ms->T + sizeof(Tallocation) + resultat.bloc * sizeof(Bloc), sizeof(Bloc));

    if (interneTriee) {
        for (int i = resultat.position; i < buffer.nb_enregistrement - 1; i++) {
            buffer.enregisrement[i] = buffer.enregisrement[i + 1];

    }
    } else {
        buffer.enregisrement[resultat.position] = buffer.enregisrement[buffer.nb_enregistrement - 1];

    }
    buffer.enregisrement[buffer.nb_enregistrement - 1].id = 0;
    buffer.enregisrement[buffer.nb_enregistrement - 1].price = 0.0;
    strcpy(buffer.enregisrement[buffer.nb_enregistrement - 1].name, "");
    buffer.nb_enregistrement--;


    // Write the modified buffer back to memory
    memcpy(ms->T + sizeof(Tallocation) + resultat.bloc * sizeof(Bloc), &buffer, sizeof(Bloc));

    if (buffer.nb_enregistrement == 0) {
        modifierTableAllocation(ms, resultat.bloc, false);
        
        if (globaleChainee) {
            int previousBlocIndex = getPreviousBlockIndex(ms, resultat.bloc);

            if (previousBlocIndex != -1) {
                Bloc previousBloc;
                memcpy(&previousBloc, ms->T + sizeof(Tallocation) + previousBlocIndex * sizeof(Bloc), sizeof(Bloc));
                previousBloc.next = buffer.next;//Use buffer.next instead of bloc.next
                memcpy(ms->T + sizeof(Tallocation) + previousBlocIndex * sizeof(Bloc), &previousBloc, sizeof(Bloc));
            } else {
                fileTable[fileIndex].adresse_firstblock = -1;
                fileTable[fileIndex].etat = 0;
                ms->file_count--;
                memcpy(ms->meta, fileTable, sizeof(mt)*Total_block);
            }
        }
    }

    printf("Enregistrement avec ID %d dans le fichier '%s' supprimé physiquement.\n", idRecherche, nomFichier);
}
void defragmenterFichier(Ms *ms, const char *nom_fichier) {
    if (ms == NULL || nom_fichier == NULL) {
        fprintf(stderr, "Erreur : Le pointeur Ms ou le nom du fichier est NULL.\n");
        return;
    }

    Tallocation tallocation;
    memcpy(&tallocation, ms->T, sizeof(Tallocation));

    // Locate metadata for the file
    mt meta;
    int meta_index = -1;
    for (int i = 0; i < Total_block; i++) {
        memcpy(&meta, ms->meta + i * sizeof(mt), sizeof(mt));
        if (strcmp(meta.Nom_du_fichier, nom_fichier) == 0 && meta.etat == 1) {
            meta_index = i;
            break;
        }
    }

    if (meta_index == -1) {
        fprintf(stderr, "Erreur : Fichier '%s' introuvable.\n", nom_fichier);
        return;
    }

    Bloc defrag_blocks[Total_block];
    int defrag_count = 0;

    // Traverse all blocks in the linked list of the file
    int current_block = meta.adresse_firstblock;
    while (current_block != -1) {
        Bloc bloc;
        memcpy(&bloc, ms->T + sizeof(Tallocation) + current_block * sizeof(Bloc), sizeof(Bloc));
        defrag_blocks[defrag_count++] = bloc;
        updateTabAlloc(ms, current_block, false); // Mark the block as unallocated
        current_block = bloc.next;
    }

    // Allocate new blocks contiguously
    int new_first_block = getFreeBlocPos(ms);
    if (new_first_block == -1 || new_first_block + defrag_count > Total_block) {
        fprintf(stderr, "Erreur : Pas assez d'espace pour défragmenter le fichier '%s'.\n", nom_fichier);
        return;
    }

    for (int i = 0; i < defrag_count; i++) {
        Bloc bloc = defrag_blocks[i];
        int block_pos = new_first_block + i;

        bloc.ADR = block_pos;
        bloc.next = (i == defrag_count - 1) ? -1 : (block_pos + 1);

        memcpy(ms->T + sizeof(Tallocation) + block_pos * sizeof(Bloc), &bloc, sizeof(Bloc));
        updateTabAlloc(ms, block_pos, true);
    }

    // Update metadata
    meta.adresse_firstblock = new_first_block;
    meta.Taille_du_fichier = defrag_count;
    memcpy(ms->meta + meta_index * sizeof(mt), &meta, sizeof(mt));

    printf("Défragmentation terminée pour le fichier '%s'.\n", nom_fichier);
    displayTallocation(tallocation); // Display updated allocation table
}
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
void renameFile(Ms *ms, const char currentName) {
    // Get the file metadata
    mt *fileTable = (mt*)ms->meta;

    // Search for the file in the metadata
    int fileIndex = -1;
    for (int i = 0; i < ms->file_count; i++) {
        if (strcmp(fileTable[i].Nom_du_fichier, currentName) == 0 && fileTable[i].etat == 1) {
            fileIndex = i;
            break;
        }
    }

    // If the file is not found, print an error message and return
    if (fileIndex == -1) {
        printf("Fichier '%s' introuvable ou déjà supprimé.\n", currentName);
        return;
    }

    // Ask the user for the new name
    char newName[30];  // Ensure the name size fits within the field limit
    printf("Entrez le nouveau nom pour le fichier '%s': ", currentName);
    fgets(newName, sizeof(newName), stdin);

    // Remove the newline character if present
    newName[strcspn(newName, "\n")] = '\0';

    // Update the file name
    strncpy(fileTable[fileIndex].Nom_du_fichier, newName, sizeof(fileTable[fileIndex].Nom_du_fichier) - 1);
    fileTable[fileIndex].Nom_du_fichier[sizeof(fileTable[fileIndex].Nom_du_fichier) - 1] = '\0';  // Ensure null-termination

    // Write the updated metadata back to memory
    memcpy(ms->meta, fileTable, sizeof(mt) * Total_block);

    printf("Le fichier '%s' a été renommé en '%s'.\n", currentName, newName);
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
void suppfichier(Ms *ms , Tallocation *talloc , mt *metainfo ){

    bool choix = metainfo->org_globale ;

    if(choix ==true){
        suppfichiercntg(ms, talloc, metainfo);
    }
    else{
        suppfichierchaine(ms, talloc, metainfo);
    }

}

void afficherElementsbuffer(Bloc buffer){
        for (int j = 0; j < 4; j++) {
            // Afficher les détails du produit
            printf("  Produit %d:\n", j + 1);
            printf("    Nom   : %s\n", buffer.enregisrement[j].name);
            printf("    Prix  : %f\n", buffer.enregisrement[j].price); // Utilisez temp.price[0] pour afficher le prix
            printf("    ID    : %d\n", buffer.enregisrement[j].id);    // Utilisez temp.id[0] pour afficher l'ID
        }
}
void afficherElementsMs(Ms *ms) {
    printf("Affichage des elements dans la mémoire secondaire :\n");

    // Parcourir tous les blocs dans la mémoire secondaire
    for (int i = 0; i < 5; i++) {
        // Lire le bloc courant depuis la mémoire secondaire
        Bloc bloc;
        memcpy(&bloc, ms->T + i * sizeof(Bloc) + sizeof(Tallocation), sizeof(Bloc));

        printf("Bloc %d:\n", i + 1);

        // Parcourir les enregistrements dans le bloc
        afficherElementsbuffer(bloc);
    }
}
void afficherMETAMs(Ms *ms) {
    printf("Affichage des meta dans la memoire secondaire (les 4 premiers):\n");
    mt meta;
    for(int i=0 ; i<4 ; i++){
        memcpy(&meta , ms->meta + i*sizeof(mt),sizeof(mt));
        printf("Nom du fichier:             %s\n", meta.Nom_du_fichier);
        printf("Taille du fichier:          %d\n", meta.Taille_du_fichier);
        printf("Nombre d'enregistrements :  %d\n", meta.nb_enregistrement);
        printf("Adresse du premier bloc :   %d\n", meta.adresse_firstblock);
        printf("Organisation globale :      %d\n", meta.org_globale);
        printf("Organisation interne :      %d\n", meta.org_interne);
        printf("etat du fichier      :      %d\n", meta.etat );
        printf("___________________________________\n");
    }
    printf("ms->T file count = %d \n" , ms->file_count);
}

// Include all the function declarations and type definitions from the provided code...
void displayMenu() {
    printf("\n=== MENU PRINCIPAL ===\n");
    printf("1. Initialiser la mémoire secondaire\n");
    printf("2. Créer un fichier\n");
    printf("3. Afficher l'état de la mémoire secondaire\n");
    printf("4. Afficher les métadonnées des fichiers\n");
    printf("5. Rechercher un enregistrement par ID\n");
    printf("6. Insérer un nouvel enregistrement\n");
    printf("7. Supprimer un enregistrement (logique)\n");
    printf("8. Supprimer un enregistrement (physique)\n");
    printf("9. Défragmenter un fichier\n");
    printf("10. Supprimer un fichier\n");
    printf("11. Renommer un fichier\n");
    printf("12. Compactage de la mémoire secondaire\n");
    printf("13. Vider la mémoire secondaire\n");
    printf("14. Quitter\n");
    printf("Choisissez une option : ");
}

int main() {
    Ms ms;
    int choix;
    char nomFichier[30];
    int idRecherche;
    produit p;


    do {
        displayMenu();
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                initialisems(&ms);
                printf("Mémoire secondaire initialisée.\n");
                break;
            case 2:
                creationfichier(&ms);
                break;
            case 3:
                affichageEnBloc(&ms);
                break;
            case 4:
                afficherMETAMs(&ms);
                break;
            case 5:
                printf("Entrez le nom du fichier : ");
                scanf("%s", nomFichier);
                printf("Entrez l'ID à rechercher : ");
                scanf("%d", &idRecherche);
                rechercherParIDAvecBuffer(&ms, idRecherche, nomFichier);
                break;
            case 6:
                p = lireProduit();
                insertion(&ms, p);
                break;
            case 7:
                printf("Entrez le nom du fichier : ");
                scanf("%s", nomFichier);
                printf("Entrez l'ID à supprimer : ");
                scanf("%d", &idRecherche);
                supprimerLogiquement(&ms, idRecherche, nomFichier);
                break;
            case 8:
                printf("Entrez le nom du fichier : ");
                scanf("%s", nomFichier);
                printf("Entrez l'ID à supprimer : ");
                scanf("%d", &idRecherche);
                supprimerPhysiquement(&ms, idRecherche, nomFichier);
                break;
            case 9:
                printf("Entrez le nom du fichier à défragmenter : ");
                scanf("%s", nomFichier);
                defragmenterFichier(&ms, nomFichier);
                break;
            case 10:
                printf("Entrez le nom du fichier à supprimer : ");
                scanf("%s", nomFichier);
                suppfichier(&ms, NULL, NULL); // Update the parameters as needed
                break;
            case 11:
                printf("Entrez le nom actuel du fichier : ");
                scanf("%s", nomFichier);
                renameFile(&ms, nomFichier);
                break;
            case 12:
                compacterMs(&ms, NULL); // Update the second parameter as needed
                break;
            case 13:
                initialisems(&ms);
                printf("Mémoire secondaire vidée.\n");
                break;
            case 14:
                printf("Quitter le programme.\n");
                break;
            default:
                printf("Choix invalide. Réessayez.\n");
                break;
        }
    } while (choix != 14);

    return 0;
}






