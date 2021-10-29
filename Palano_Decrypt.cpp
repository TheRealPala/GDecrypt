#include <iostream>
#include <time.h>
#include <string.h>
#include <stdlib.h>
using namespace std;

void help(){
    cout << "\n\nExample:\tPalano_DeCrypt.exe -f filename.txt -c 1234\n\nElenco parametri accettati:\n-f:\tNome del file da decriptare\n-c:\tChiave di cifratura (LA CHIAVE DEVE ESSERE NUMERICA)\n-h:\tStampa elenco dei parametri accettati dal programma (--help viene accettato)\n";
}

void interParam(int argc, char** argv, char** param){
    char EMPTY = 0;
    char* file = &EMPTY;
    char* chiave = &EMPTY; 

    if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")){
        help();
        exit(EXIT_FAILURE );
    }

    if (argc < 5){
        cout << "Errore nell'inserimento dei parametri!";
        help();
        exit(EXIT_FAILURE );
    }

    else {
        for (int i = 1; i < argc; ++i){
            if (!strcmp(argv[i], "-f")){
                file = argv[i + 1]; 
            }
            if (!strcmp(argv[i], "-c")){
                chiave = argv[i + 1]; 
            }
        }
        
    }
    if (*file == EMPTY){
        cout << "Errore nell'inserimento dei parametri!";
        help();
        exit(EXIT_FAILURE );
    }

    if (*chiave == EMPTY){
        cout << "Errore nell'inserimento dei parametri!";
        help();
        exit(EXIT_FAILURE );  
    }

    //Controllo validità chiave
    for (int i = 0; i < strlen(chiave); ++i){
         int c = chiave[i] - '0';
         if (c < 0 || c > 9){
            cout << "Chiave inserita non valida!\nLA CHIAVE DEVE ESSERE NUMERICA!\nIl programma terminera' sedutastante!\n";
            exit(EXIT_FAILURE ); 
         }   
    }
    param[0] = file;
    param[1] = chiave;
}

void initAlfa(char* a){
    for (int i = 0; i < 26; ++i){
        a[i] = 'A' + i;
    }
}

void shift (char aShift [][26], char* alfa, char* chiave){
    int lenChiave = strlen(chiave);
    int c;
    for (int i = 0; i < lenChiave; ++i){
        c = chiave[i] - '0'; //Trasformo le cifre della chiave da char ad int
        for (int j = 0; j < strlen(alfa); ++j){
            int pos = (j + c) % 26; //calcolo posizione di shift dell'alfabeto
            aShift[i][j] = alfa[pos];
        }
    }
}

int getIndex(char chr, char* str){
   int index = -1;
   for (int i = 0; i < strlen(str); ++i){
       if (chr == str[i]){
           index = i;
           break;
       }
   }
   return index; 
}

char* getStringDecrypt(char* str, char aShift[][26], char* alfa, int lenkey){
    char* decryptStr = new char[strlen(str) + 1]; //Salvo la variabile nell'heap per evitare danni con il push/pop;
    bzero(decryptStr, strlen(str));
    decryptStr[strlen(decryptStr)] = '\0';
    int inK = -1;
    for (int i = 0; i < strlen(str); ++i){ 
        //Gestione caratteri non contenuti nell'alfabeto
        if (str[i] == '\n'){
            decryptStr[i] = '\n';
        }

        else if (str[i] >= ' ' && str[i] <= '@'){
            decryptStr[i] = str[i];
        }

        else if (str[i] >= '[' && str[i] <= 96 /*Il 96 sarebbe l'accento grave */){
            decryptStr[i] = str[i];
        }

        else if (str[i] >= '{' && str[i] <= '~'){
            decryptStr[i] = str[i];
        }
        //Gestione caratteri contenuti nell'alfabeto
        else {
            inK = (inK + 1) % lenkey; //Calcolo di quale alfabeto shiftato devo utilizzare (dopo l'utilizzo dell'ultmo devo ricominciare con il primo)
            bool isMin = str[i] >= 'a' && str[i] <= 'z' ? true : false;
            if (!isMin){ //costrutto per gestire lettere minuscole
                int index = getIndex(str[i], aShift[inK]); //ottengo il numero dell'alfabeto inKesimo alfabeto criptato della iEsima lettera della stringa da cifrare
                decryptStr[i] = alfa[index];
            }
            else { //i - ' ' ed i + ' ' sono per gestire le lettere minuscole perchè l'alfabeto generato è tutto in maiuscolo (' ' = 32(base 10))
                int index = getIndex(str[i] - ' ', aShift[inK]); //ottengo il numero dell'alfabeto NORMALE della iEsima lettera della stringa da cifrare
                decryptStr[i] = alfa[index] + ' ';
            }
        }
    }
    decryptStr[strlen(str)] = '\0';
    return decryptStr;
}

void decryptFile(char* filename, char aShift[][26], char* alfa, int lenkey){
    FILE *f;
    f = fopen(filename, "r");
    if (!f){
        cout << "File specificato non trovato!\nIl programma terminera' sedutastante!\n";
            exit(EXIT_FAILURE ); 
    }
    FILE *fDecrypt;
    char* fileDecryptName = strcat(filename, ".decrypt");
    cout << "Nome del file decryptato: " << fileDecryptName;
    fDecrypt = fopen(fileDecryptName, "w");
    if (!fDecrypt){
        cout << "Errore durante la creazione del file: " << fileDecryptName << endl;
        exit(EXIT_FAILURE);
    }
    char tmp[256]; 
    while (!feof(f)){
        fgets(tmp, 256, f);
        char* pt = strchr(tmp, 13); //ELIMINO IL "CR" ed il "LF"
        if (pt){
            *pt = '\0';
        }
        if (strlen(tmp) > 0){
            char* strDecrypt = getStringDecrypt(tmp, aShift, alfa, lenkey);
            fprintf(fDecrypt, "%s", strDecrypt);
        }
        bzero(tmp, 254);
        tmp[255] = '\0';
    }
    
    fclose(f);
}

int main(int argc, char** argv){
    if (argc < 2){ //Controllo che si siano passati i parametri
        cout << "Parametri non inseriti!\nIl programma terminera' sedutastante!";
        help();
        exit(EXIT_FAILURE );
    }
    char** param = new char* [2]; // param[0] = nome file, param[1] = chiave
    interParam(argc, argv, param); //Gestione parametri passati dall'utente
    char alfa [26];
    initAlfa(alfa);//Generazione alfabeto
    char alfaShift [strlen(param[1])][26]; //Tabella di tante righe quante sono le cifre della chiave e tante colonne quante le lettere dell'alfabeto
    shift(alfaShift, alfa, param[1]);//Generazione alfabeti shiftati
    decryptFile(param[0], alfaShift, alfa, strlen(param[1])); //Decrypt del file
    delete(param);
    return 0;
}