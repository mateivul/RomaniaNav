/**
 * ═══════════════════════════════════════════════════════════════════════
 *                    MOLDOVA NAVIGATOR v1.0
 * ═══════════════════════════════════════════════════════════════════════
 * 
 * Aplicație C++ consolă pentru navigație rutieră în regiunea Moldova (România)
 * Folosește backtracking iterativ pentru găsirea TUTUROR traseelor între orașe
 * 
 * Tehnologii:
 * - C++11 (fără STL containers - doar arrays clasice)
 * - Backtracking iterativ cu structura clasică
 * - Culori ANSI pentru formatare
 * - Variabile globale (fără clase/struct-uri complexe)
 * 
 * Autor: Moldova Navigator Team
 * Data: Februarie 2026
 * ═══════════════════════════════════════════════════════════════════════
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <limits>
#include <chrono>
#include <cmath>
using namespace std;

// ═══════════════════════════════════════════════════
//              CONSTANTE GLOBALE
// ═══════════════════════════════════════════════════
const int NR_ORASE = 12;
const int MAX_TRASEE = 500;
const int MAX_LUNGIME_TRASEU = 20;

// ═══════════════════════════════════════════════════
//              CULORI ANSI
// ═══════════════════════════════════════════════════
const string RESET  = "\033[0m";
const string BOLD   = "\033[1m";
const string RED    = "\033[31m";
const string GREEN  = "\033[32m";
const string YELLOW = "\033[33m";
const string CYAN   = "\033[36m";
const string GRAY   = "\033[90m";
const string BLUE   = "\033[34m";
const string MAGENTA = "\033[35m";

// ═══════════════════════════════════════════════════
//              DATE ORAȘE
// ═══════════════════════════════════════════════════
string numeOrase[NR_ORASE];
double latitudine[NR_ORASE];
double longitudine[NR_ORASE];

// ═══════════════════════════════════════════════════
//              MATRICI GRAF (ADIACENȚĂ)
// ═══════════════════════════════════════════════════
int distanta[NR_ORASE][NR_ORASE];       // distanta[i][j] = km sau 0 dacă nu există drum
string tipDrum[NR_ORASE][NR_ORASE];     // "DN2", "E85", "DJ209", etc.
int vitezaMax[NR_ORASE][NR_ORASE];      // 70, 80, 90, 100, 130 km/h

// ═══════════════════════════════════════════════════
//              BACKTRACKING
// ═══════════════════════════════════════════════════
int x[MAX_LUNGIME_TRASEU];              // Soluția curentă (vectorul BKT)
int vizitat[NR_ORASE];                  // Marcare noduri vizitate (evită cicluri)
int sursa, destinatia;                  // Orașele între care căutăm

// ═══════════════════════════════════════════════════
//              STOCAREA TUTUROR TRASEELOR
// ═══════════════════════════════════════════════════
int trasee[MAX_TRASEE][MAX_LUNGIME_TRASEU];  // Fiecare traseu e un array de indici
int lungimeTraseu[MAX_TRASEE];               // Câte orașe are fiecare traseu
double distanteTrasee[MAX_TRASEE];           // Distanța totală (km)
double timpTrasee[MAX_TRASEE];               // Timp total (ore)
int nrTrasee;                                // Câte trasee am găsit

// ═══════════════════════════════════════════════════
//              FUNCȚII AUXILIARE
// ═══════════════════════════════════════════════════

/**
 * Afișează un separator grafic
 */
void separator() {
    cout << "══════════════════════════════════════════════════" << endl;
}

/**
 * Afișează lista orașelor disponibile în format frumos
 */
void afiseazaListaOrase() {
    cout << endl;
    cout << CYAN << BOLD << "ORAȘE DISPONIBILE:" << RESET << endl;
    
    for (int i = 0; i < NR_ORASE; i++) {
        cout << "  " << YELLOW << setw(2) << i << RESET << ". " << numeOrase[i];
        
        // Formatare pe 3 coloane
        if ((i + 1) % 3 == 0 || i == NR_ORASE - 1) {
            cout << endl;
        } else {
            // Padding pentru aliniere
            int padding = 20 - numeOrase[i].length();
            for (int j = 0; j < padding; j++) cout << " ";
        }
    }
    cout << endl;
}

/**
 * Convertește ore în format "2h 30min" sau "42min"
 * 
 * @param ore - Timpul în ore (ex: 1.5, 0.7)
 * @return String formatat (ex: "1h 30min", "42min")
 */
string formateazaTimp(double ore) {
    int oreIntregi = (int)ore;
    int minute = (int)((ore - oreIntregi) * 60);
    
    if (oreIntregi > 0) {
        return to_string(oreIntregi) + "h " + to_string(minute) + "min";
    } else {
        return to_string(minute) + "min";
    }
}

/**
 * Adaugă un drum bidirectional între două orașe
 * 
 * @param o1 - Indexul primului oraș (0-11)
 * @param o2 - Indexul celui de-al doilea oraș (0-11)
 * @param dist - Distanța în km
 * @param tip - Tipul drumului ("DN2", "E85", "DJ209", etc.)
 * @param viteza - Viteza maximă în km/h
 */
void adaugaDrum(int o1, int o2, int dist, string tip, int viteza) {
    // Drum o1 → o2
    distanta[o1][o2] = dist;
    tipDrum[o1][o2] = tip;
    vitezaMax[o1][o2] = viteza;
    
    // Drum o2 → o1 (bidirectional)
    distanta[o2][o1] = dist;
    tipDrum[o2][o1] = tip;
    vitezaMax[o2][o1] = viteza;
}

/**
 * Inițializează toate datele aplicației (orașe + drumuri)
 */
void initializeazaDate() {
    // ───────────────────────────────────────────────────
    //              INIȚIALIZARE ORAȘE
    // ───────────────────────────────────────────────────
    numeOrase[0] = "Suceava";        latitudine[0] = 47.6439;  longitudine[0] = 26.2578;
    numeOrase[1] = "Botoșani";       latitudine[1] = 47.7408;  longitudine[1] = 26.6566;
    numeOrase[2] = "Iași";           latitudine[2] = 47.1585;  longitudine[2] = 27.6014;
    numeOrase[3] = "Bacău";          latitudine[3] = 46.5670;  longitudine[3] = 26.9146;
    numeOrase[4] = "Piatra Neamț";   latitudine[4] = 46.9267;  longitudine[4] = 26.3816;
    numeOrase[5] = "Roman";          latitudine[5] = 46.9231;  longitudine[5] = 26.9244;
    numeOrase[6] = "Vaslui";         latitudine[6] = 46.6402;  longitudine[6] = 27.7297;
    numeOrase[7] = "Galați";         latitudine[7] = 45.4353;  longitudine[7] = 28.0080;
    numeOrase[8] = "Vatra Dornei";   latitudine[8] = 47.3456;  longitudine[8] = 25.3621;
    numeOrase[9] = "Fălticeni";      latitudine[9] = 47.4597;  longitudine[9] = 26.2985;
    numeOrase[10] = "Rădăuți";       latitudine[10] = 47.8415; longitudine[10] = 25.9207;
    numeOrase[11] = "Pașcani";       latitudine[11] = 47.2472; longitudine[11] = 26.7130;
    
    // ───────────────────────────────────────────────────
    //              INIȚIALIZARE MATRICE DISTANȚE (cu 0)
    // ───────────────────────────────────────────────────
    for (int i = 0; i < NR_ORASE; i++) {
        for (int j = 0; j < NR_ORASE; j++) {
            distanta[i][j] = 0;
            tipDrum[i][j] = "";
            vitezaMax[i][j] = 0;
        }
    }
    
    // ───────────────────────────────────────────────────
    //              ADĂUGARE DRUMURI BIDIRECTIONALE
    // ───────────────────────────────────────────────────
    adaugaDrum(0, 9,  30,  "DN2",   90);   // Suceava - Fălticeni
    adaugaDrum(0, 10, 42,  "DN29",  90);   // Suceava - Rădăuți
    // adaugaDrum(0, 5,  70,  "E85",   100);  // Suceava - Roman
    adaugaDrum(0, 8,  110, "DN17",  80);   // Suceava - Vatra Dornei
    // adaugaDrum(0, 4,  85,  "DN15",  80);   // Suceava - Piatra Neamț
    adaugaDrum(1, 10, 55,  "DN29A", 80);   // Botoșani - Rădăuți
    adaugaDrum(1, 9,  65,  "DN29B", 80);   // Botoșani - Fălticeni
    adaugaDrum(1, 2,  110, "E583",  100);  // Botoșani - Iași
    adaugaDrum(2, 11, 35,  "DN28",  90);   // Iași - Pașcani
    adaugaDrum(2, 6,  70,  "E581",  100);  // Iași - Vaslui
    adaugaDrum(2, 5,  50,  "DN28",  90);   // Iași - Roman
    adaugaDrum(2, 3,  120, "E85",   100);  // Iași - Bacău
    adaugaDrum(3, 5,  35,  "E85",   100);  // Bacău - Roman
    adaugaDrum(3, 4,  60,  "DN15",  80);   // Bacău - Piatra Neamț
    // adaugaDrum(3, 6,  90,  "DN2",   90);   // Bacău - Vaslui
    adaugaDrum(3, 7,  120, "E85",   100);  // Bacău - Galați
    adaugaDrum(4, 5,  18,  "DN15",  80);   // Piatra Neamț - Roman
    adaugaDrum(4, 8,  80,  "DN15",  80);   // Piatra Neamț - Vatra Dornei
    adaugaDrum(5, 11, 20,  "DN2",   90);   // Roman - Pașcani
    adaugaDrum(6, 7,  72,  "DN24",  90);   // Vaslui - Galați
    adaugaDrum(6, 11, 45,  "DN24",  90);   // Vaslui - Pașcani
    adaugaDrum(8, 10, 95,  "DN17",  80);   // Vatra Dornei - Rădăuți
    adaugaDrum(9, 10, 35,  "DJ209", 70);   // Fălticeni - Rădăuți (Județean)
    adaugaDrum(9, 5,  55,  "DN2",   90);   // Fălticeni - Roman
}

// ═══════════════════════════════════════════════════
//              FUNCȚII BACKTRACKING
// ═══════════════════════════════════════════════════

/**
 * Inițializează nivelul k (x[k] = -1)
 * 
 * @param k - Nivelul curent în backtracking
 */
void INIT(int k) {
    x[k] = -1;
}

/**
 * Verifică dacă mai există orașe de încercat pe nivelul k
 * 
 * @param k - Nivelul curent
 * @return 1 dacă mai există orașe, 0 dacă nu
 */
int EXISTA(int k) {
    return (x[k] < NR_ORASE - 1);
}

/**
 * Verifică dacă x[k] este un oraș valid pentru traseul curent
 * 
 * @param k - Nivelul curent în backtracking
 * @return 1 dacă valid, 0 dacă invalid
 * 
 * Validări:
 * 1. Există drum între orașul precedent și x[k]
 * 2. x[k] nu este deja vizitat (evită cicluri)
 * 3. x[k] nu este sursa
 */
int VALID(int k) {
    int orasPrecedent;
    
    // Găsește orașul precedent
    if (k == 1) {
        orasPrecedent = sursa;      // Primul nivel: precedentul e sursa
    } else {
        orasPrecedent = x[k - 1];   // Altfel: e orașul de pe nivelul k-1
    }
    
    // Validare 1: Există drum între orasPrecedent și x[k]?
    if (distanta[orasPrecedent][x[k]] == 0) {
        return 0;                   // NU există drum
    }
    
    // Validare 2: Orașul x[k] e deja vizitat?
    if (vizitat[x[k]] == 1) {
        return 0;                   // DA, e vizitat (ar crea ciclu)
    }
    
    // Validare 3: Orașul x[k] e sursa?
    if (x[k] == sursa) {
        return 0;                   // DA, nu vrem să ne întoarcem la start
    }
    
    return 1;                       // OK, e valid!
}

/**
 * Verifică dacă am ajuns la destinație
 * 
 * @param k - Nivelul curent
 * @return 1 dacă x[k] == destinația, 0 altfel
 */
int SOLUTIE(int k) {
    return (x[k] == destinatia);
}

/**
 * Salvează traseul curent în array-ul global de trasee
 * 
 * @param k - Nivelul curent (lungimea traseului fără sursă)
 */
void TIPAR(int k) {
    if (nrTrasee >= MAX_TRASEE) {
        return;                     // Prea multe trasee, oprim
    }
    
    // Construim traseul: SURSA → x[1] → x[2] → ... → x[k]
    trasee[nrTrasee][0] = sursa;
    for (int i = 1; i <= k; i++) {
        trasee[nrTrasee][i] = x[i];
    }
    
    lungimeTraseu[nrTrasee] = k + 1;    // +1 pentru sursă
    
    // Calculăm distanța totală
    double dist = 0;
    for (int i = 0; i < k; i++) {
        int oras1 = trasee[nrTrasee][i];
        int oras2 = trasee[nrTrasee][i + 1];
        dist += distanta[oras1][oras2];
    }
    distanteTrasee[nrTrasee] = dist;
    
    // Calculăm timpul total
    double timp = 0;
    for (int i = 0; i < k; i++) {
        int oras1 = trasee[nrTrasee][i];
        int oras2 = trasee[nrTrasee][i + 1];
        timp += (double)distanta[oras1][oras2] / vitezaMax[oras1][oras2];
    }
    timpTrasee[nrTrasee] = timp;
    
    nrTrasee++;
}

/**
 * Loop principal backtracking iterativ
 * Găsește TOATE traseele între sursa și destinația globale
 */
void BKT() {
    int k = 1;                          // Nivelul curent (1 = primul oraș după sursă)
    INIT(k);                            // x[1] = -1
    nrTrasee = 0;                       // Counter trasee găsite
    
    // Resetare vector vizitat
    for (int i = 0; i < NR_ORASE; i++) {
        vizitat[i] = 0;
    }
    
    // Marcăm sursa ca vizitată
    vizitat[sursa] = 1;
    
    while (k > 0) {                     // Cât timp nu am terminat
        if (EXISTA(k)) {                // Mai sunt orașe de încercat?
            x[k] = x[k] + 1;            // Trecem la următorul oraș
            
            if (VALID(k)) {             // E ok acest oraș?
                vizitat[x[k]] = 1;      // Îl marcăm ca vizitat
                
                if (SOLUTIE(k)) {       // Am ajuns la destinație?
                    TIPAR(k);           // Salvăm traseul
                    vizitat[x[k]] = 0;  // Demarcăm (pentru alte trasee)
                } else {
                    k = k + 1;          // Mergem mai adânc
                    INIT(k);            // Inițializăm noul nivel
                }
            }
        } else {
            k = k - 1;                  // Revenim (backtrack)
            if (k > 0) {
                vizitat[x[k]] = 0;      // Demarcăm orașul
            }
        }
    }
}

// ═══════════════════════════════════════════════════
//              FUNCȚII UTILITĂȚI TRASEE
// ═══════════════════════════════════════════════════

/**
 * Sortează traseele după distanță (bubble sort)
 * Cel mai scurt traseu va fi la index 0
 */
void sortTrasee() {
    for (int i = 0; i < nrTrasee - 1; i++) {
        for (int j = 0; j < nrTrasee - i - 1; j++) {
            if (distanteTrasee[j] > distanteTrasee[j + 1]) {
                // Swap distanțe
                double tempDist = distanteTrasee[j];
                distanteTrasee[j] = distanteTrasee[j + 1];
                distanteTrasee[j + 1] = tempDist;
                
                // Swap timpuri
                double tempTimp = timpTrasee[j];
                timpTrasee[j] = timpTrasee[j + 1];
                timpTrasee[j + 1] = tempTimp;
                
                // Swap lungimi
                int tempLung = lungimeTraseu[j];
                lungimeTraseu[j] = lungimeTraseu[j + 1];
                lungimeTraseu[j + 1] = tempLung;
                
                // Swap trasee (array-uri complete)
                for (int p = 0; p < MAX_LUNGIME_TRASEU; p++) {
                    int tempTr = trasee[j][p];
                    trasee[j][p] = trasee[j + 1][p];
                    trasee[j + 1][p] = tempTr;
                }
            }
        }
    }
}

/**
 * Afișează un traseu: oraș1 → oraș2 → ... → orașN (X km)
 * 
 * @param index - Indexul traseului în array-ul trasee
 */
void afiseazaTraseu(int index) {
    for (int i = 0; i < lungimeTraseu[index]; i++) {
        cout << numeOrase[trasee[index][i]];
        
        if (i < lungimeTraseu[index] - 1) {
            cout << GRAY << " → " << RESET;
        }
    }
    
    cout << " " << YELLOW << "(" << fixed << setprecision(0) 
         << distanteTrasee[index] << " km)" << RESET;
}

/**
 * Afișează detalii segment cu segment pentru un traseu
 * 
 * @param index - Indexul traseului în array-ul trasee
 */
void afiseazaDetaliiTraseu(int index) {
    cout << endl;
    cout << CYAN << BOLD << "DETALII TRASEU:" << RESET << endl;
    separator();
    
    double distantaTotala = 0;
    double timpTotal = 0;
    
    for (int i = 0; i < lungimeTraseu[index] - 1; i++) {
        int oras1 = trasee[index][i];
        int oras2 = trasee[index][i + 1];
        
        int dist = distanta[oras1][oras2];
        string tip = tipDrum[oras1][oras2];
        int viteza = vitezaMax[oras1][oras2];
        double timp = (double)dist / viteza;
        
        distantaTotala += dist;
        timpTotal += timp;
        
        cout << endl;
        cout << YELLOW << "Segment " << (i + 1) << ": " << RESET 
             << numeOrase[oras1] << GRAY << " → " << RESET << numeOrase[oras2] << endl;
        cout << "  Distanță: " << GREEN << dist << " km" << RESET << endl;
        
        // Determină tipul drumului
        string tipComplet;
        if (tip[0] == 'E') {
            tipComplet = tip + " (European)";
        } else if (tip[0] == 'D' && tip[1] == 'N') {
            tipComplet = tip + " (Național)";
        } else if (tip[0] == 'D' && tip[1] == 'J') {
            tipComplet = tip + " (Județean)";
        } else if (tip[0] == 'A') {
            tipComplet = tip + " (Autostradă)";
        } else {
            tipComplet = tip;
        }
        
        cout << "  Tip drum: " << CYAN << tipComplet << RESET << endl;
        cout << "  Viteză max: " << MAGENTA << viteza << " km/h" << RESET << endl;
        cout << "  Timp: " << BLUE << formateazaTimp(timp) << RESET << endl;
    }
    
    cout << endl;
    separator();
    cout << BOLD << "TOTAL: " << RESET << GREEN << fixed << setprecision(0) 
         << distantaTotala << " km" << RESET << ", " 
         << BLUE << formateazaTimp(timpTotal) << RESET << endl;
    separator();
    cout << endl;
}

// ═══════════════════════════════════════════════════
//              FUNCȚII INPUT UTILIZATOR
// ═══════════════════════════════════════════════════

/**
 * Citește un oraș de la tastatură cu validare
 * 
 * @param mesaj - Mesajul afișat utilizatorului
 * @return Index-ul orașului (0-11)
 */
int citesteOras(string mesaj) {
    int oras;
    while (true) {
        cout << "  " << mesaj;
        cin >> oras;
        
        if (cin.fail() || oras < 0 || oras >= NR_ORASE) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << RED << "  [!] Număr invalid! Alegeți între 0 și " 
                 << (NR_ORASE - 1) << "." << RESET << endl;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return oras;
        }
    }
}

/**
 * Citește opțiunea din meniu cu validare
 * 
 * @return Opțiunea aleasă (1-5) sau -1 dacă invalid
 */
int citesteOptiune() {
    int opt;
    cout << "  " << CYAN << "Alegeți opțiunea (1-5): " << RESET;
    cin >> opt;
    
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return -1;  // Invalid
    }
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return opt;
}

// ═══════════════════════════════════════════════════
//              FUNCȚII MENIU
// ═══════════════════════════════════════════════════

/**
 * Afișează meniul principal al aplicației
 */
void afiseazaMeniu() {
    cout << endl;
    cout << CYAN << BOLD << "╔════════════════════════════════════════════════╗" << RESET << endl;
    cout << CYAN << BOLD << "║       MOLDOVA NAVIGATOR v1.0                   ║" << RESET << endl;
    cout << CYAN << BOLD << "║       Sistem de navigație rutieră              ║" << RESET << endl;
    cout << CYAN << BOLD << "╚════════════════════════════════════════════════╝" << RESET << endl;
    
    afiseazaListaOrase();
    
    separator();
    cout << BOLD << "              MENIU PRINCIPAL" << RESET << endl;
    separator();
    cout << "  1. Distanța minimă între 2 localități" << endl;
    cout << "  2. Toate traseele posibile" << endl;
    cout << "  3. Tipuri de drum pe un traseu" << endl;
    cout << "  4. Timpul de parcurgere" << endl;
    cout << "  5. Ieșire" << endl;
    separator();
    cout << endl;
}

// ═══════════════════════════════════════════════════
//              OPȚIUNI MENIU
// ═══════════════════════════════════════════════════

/**
 * Opțiunea 1: Distanța minimă între 2 localități
 */
void optiuneDistantaMinima() {
    cout << endl;
    cout << CYAN << BOLD << "═══ DISTANȚA MINIMĂ ═══" << RESET << endl << endl;
    
    sursa = citesteOras("Oraș de plecare (număr): ");
    destinatia = citesteOras("Oraș de destinație (număr): ");
    
    if (sursa == destinatia) {
        cout << RED << "  [!] Sursa și destinația sunt identice!" << RESET << endl;
        return;
    }
    
    cout << endl << YELLOW << "Se calculează traseele..." << RESET << endl;
    
    auto start = chrono::high_resolution_clock::now();
    BKT();
    auto end = chrono::high_resolution_clock::now();
    long long ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    
    if (nrTrasee == 0) {
        cout << RED << "  [!] Nu există niciun traseu între aceste localități!" 
             << RESET << endl;
        return;
    }
    
    sortTrasee();
    
    cout << GREEN << "S-au găsit " << nrTrasee << " trasee în " << ms << " ms!" 
         << RESET << endl << endl;
    
    separator();
    cout << BOLD << "TRASEUL MINIM: " << RESET;
    afiseazaTraseu(0);
    cout << endl;
    cout << "Distanță: " << GREEN << fixed << setprecision(0) 
         << distanteTrasee[0] << " km" << RESET << endl;
    cout << "Timp estimat: " << BLUE << formateazaTimp(timpTrasee[0]) 
         << RESET << endl;
    separator();
}

/**
 * Opțiunea 2: Toate traseele posibile
 */
void optiuneToateTrasee() {
    cout << endl;
    cout << CYAN << BOLD << "═══ TOATE TRASEELE POSIBILE ═══" << RESET << endl << endl;
    
    sursa = citesteOras("Oraș de plecare (număr): ");
    destinatia = citesteOras("Oraș de destinație (număr): ");
    
    if (sursa == destinatia) {
        cout << RED << "  [!] Sursa și destinația sunt identice!" << RESET << endl;
        return;
    }
    
    cout << endl << YELLOW << "Se calculează traseele..." << RESET << endl;
    
    auto start = chrono::high_resolution_clock::now();
    BKT();
    auto end = chrono::high_resolution_clock::now();
    long long ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    
    if (nrTrasee == 0) {
        cout << RED << "  [!] Nu există niciun traseu între aceste localități!" 
             << RESET << endl;
        return;
    }
    
    sortTrasee();
    
    cout << GREEN << "S-au găsit " << nrTrasee << " trasee în " << ms << " ms!" 
         << RESET << endl << endl;
    
    separator();
    cout << BOLD << "TOATE TRASEELE (sortate după distanță):" << RESET << endl;
    separator();
    
    int limitaAfisare = (nrTrasee < 50) ? nrTrasee : 50;
    
    for (int i = 0; i < limitaAfisare; i++) {
        cout << "  " << YELLOW << (i + 1) << "." << RESET << " ";
        afiseazaTraseu(i);
        cout << endl;
    }
    
    if (nrTrasee > 50) {
        cout << GRAY << "  ... și încă " << (nrTrasee - 50) 
             << " trasee (total: " << nrTrasee << ")" << RESET << endl;
    }
    
    separator();
}

/**
 * Opțiunea 3: Tipuri de drum pe un traseu
 */
void optiuneTipuriDrum() {
    cout << endl;
    cout << CYAN << BOLD << "═══ TIPURI DE DRUM PE UN TRASEU ═══" << RESET << endl << endl;
    
    sursa = citesteOras("Oraș de plecare (număr): ");
    destinatia = citesteOras("Oraș de destinație (număr): ");
    
    if (sursa == destinatia) {
        cout << RED << "  [!] Sursa și destinația sunt identice!" << RESET << endl;
        return;
    }
    
    cout << endl << YELLOW << "Se calculează traseele..." << RESET << endl;
    
    BKT();
    
    if (nrTrasee == 0) {
        cout << RED << "  [!] Nu există niciun traseu între aceste localități!" 
             << RESET << endl;
        return;
    }
    
    sortTrasee();
    
    cout << GREEN << "S-au găsit " << nrTrasee << " trasee!" << RESET << endl << endl;
    
    separator();
    cout << BOLD << "TRASEE DISPONIBILE:" << RESET << endl;
    separator();
    
    int limitaAfisare = (nrTrasee < 20) ? nrTrasee : 20;
    
    for (int i = 0; i < limitaAfisare; i++) {
        cout << "  " << YELLOW << (i + 1) << "." << RESET << " ";
        afiseazaTraseu(i);
        cout << endl;
    }
    
    if (nrTrasee > 20) {
        cout << GRAY << "  ... și încă " << (nrTrasee - 20) 
             << " trasee (total: " << nrTrasee << ")" << RESET << endl;
    }
    
    separator();
    
    int alegere;
    cout << endl << "  " << CYAN << "Alegeți numărul traseului (1-" 
         << nrTrasee << "): " << RESET;
    cin >> alegere;
    
    if (cin.fail() || alegere < 1 || alegere > nrTrasee) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << RED << "  [!] Alegere invalidă!" << RESET << endl;
        return;
    }
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    afiseazaDetaliiTraseu(alegere - 1);
}

/**
 * Opțiunea 4: Timpul de parcurgere
 */
void optiuneTimp() {
    cout << endl;
    cout << CYAN << BOLD << "═══ TIMPUL DE PARCURGERE ═══" << RESET << endl << endl;
    
    sursa = citesteOras("Oraș de plecare (număr): ");
    destinatia = citesteOras("Oraș de destinație (număr): ");
    
    if (sursa == destinatia) {
        cout << RED << "  [!] Sursa și destinația sunt identice!" << RESET << endl;
        return;
    }
    
    cout << endl << YELLOW << "Se calculează traseele..." << RESET << endl;
    
    BKT();
    
    if (nrTrasee == 0) {
        cout << RED << "  [!] Nu există niciun traseu între aceste localități!" 
             << RESET << endl;
        return;
    }
    
    sortTrasee();
    
    cout << GREEN << "S-au găsit " << nrTrasee << " trasee!" << RESET << endl << endl;
    
    separator();
    cout << BOLD << "TRASEUL OPTIM (cel mai scurt): " << RESET << endl;
    separator();
    cout << endl;
    
    afiseazaTraseu(0);
    cout << endl << endl;
    
    cout << "Distanță totală: " << GREEN << fixed << setprecision(0) 
         << distanteTrasee[0] << " km" << RESET << endl;
    cout << "Timp estimat (viteză maximă): " << BLUE 
         << formateazaTimp(timpTrasee[0]) << RESET << endl;
    
    afiseazaDetaliiTraseu(0);
    
    // BONUS - Comparație moduri de transport
    cout << CYAN << BOLD << "COMPARAȚIE MODURI DE TRANSPORT:" << RESET << endl;
    separator();
    
    double dist = distanteTrasee[0];
    double timpMasina = timpTrasee[0];
    double timpBicicleta = dist / 15.0;  // 15 km/h
    double timpPeJos = dist / 5.0;       // 5 km/h
    
    cout << "  " << MAGENTA << "Cu mașina (viteză max):" << RESET 
         << "  " << formateazaTimp(timpMasina) << endl;
    cout << "  " << YELLOW << "Cu bicicleta (15 km/h):" << RESET 
         << "   " << formateazaTimp(timpBicicleta) << endl;
    cout << "  " << BLUE << "Pe jos (5 km/h):" << RESET 
         << "          " << formateazaTimp(timpPeJos) << endl;
    
    separator();
    cout << endl;
}

/**
 * Opțiunea 5: Ieșire
 */
void optiuneIesire() {
    cout << endl;
    cout << GREEN << BOLD << "╔════════════════════════════════════════════════╗" << RESET << endl;
    cout << GREEN << BOLD << "║                                                ║" << RESET << endl;
    cout << GREEN << BOLD << "║           La revedere!                         ║" << RESET << endl;
    cout << GREEN << BOLD << "║           Drum bun!                            ║" << RESET << endl;
    cout << GREEN << BOLD << "║                                                ║" << RESET << endl;
    cout << GREEN << BOLD << "╚════════════════════════════════════════════════╝" << RESET << endl;
    cout << endl;
}

// ═══════════════════════════════════════════════════
//              FUNCȚIA MAIN
// ═══════════════════════════════════════════════════

int main() {
    // Inițializare date
    initializeazaDate();
    
    int optiune;
    
    while (true) {
        afiseazaMeniu();
        optiune = citesteOptiune();
        
        switch (optiune) {
            case 1:
                optiuneDistantaMinima();
                break;
                
            case 2:
                optiuneToateTrasee();
                break;
                
            case 3:
                optiuneTipuriDrum();
                break;
                
            case 4:
                optiuneTimp();
                break;
                
            case 5:
                optiuneIesire();
                return 0;
                
            default:
                cout << RED << "  [!] Opțiune invalidă! Alegeți între 1 și 5." 
                     << RESET << endl;
                break;
        }
        
        // Pauză înainte de afișarea meniului din nou
        cout << endl << GRAY << "Apăsați Enter pentru a continua..." << RESET;
        cin.get();
    }
    
    return 0;
}
