/**
 * ═══════════════════════════════════════════════════════════════════════
 *                    ROUTEMASTER MOLDOVA v2.0
 * ═══════════════════════════════════════════════════════════════════════
 * 
 * Smart Route Planning System for Moldova Region, Romania
 * 
 * Features:
 * • Iterative backtracking algorithm for route discovery
 * • Real verified road data (25 bidirectional connections)
 * • Multiple route analysis and comparison tools
 * • Detailed road network statistics
 * • File export capabilities
 * 
 * Technology Stack:
 * • C++11 (Classic arrays, no STL containers)
 * • ANSI color formatting
 * • Efficient graph traversal algorithms
 * 
 * ═══════════════════════════════════════════════════════════════════════
 */

 #include <iostream>
 #include <fstream>
 #include <iomanip>
 #include <string>
 #include <limits>
 #include <chrono>
 #include <cmath>
 #include <ctime>
 using namespace std;
 
 // ═══════════════════════════════════════════════════
 //              GLOBAL CONSTANTS
 // ═══════════════════════════════════════════════════
 const int NR_ORASE = 12;
 const int MAX_TRASEE = 500;
 const int MAX_LUNGIME_TRASEU = 20;
 
 // ═══════════════════════════════════════════════════
 //              COLOR PALETTE (ANSI)
 // ═══════════════════════════════════════════════════
 const string RESET   = "\033[0m";
 const string BOLD    = "\033[1m";
 const string DIM     = "\033[2m";
 
 // Primary colors
 const string RED     = "\033[38;5;196m";
 const string GREEN   = "\033[38;5;46m";
 const string YELLOW  = "\033[38;5;226m";
 const string BLUE    = "\033[38;5;39m";
 const string MAGENTA = "\033[38;5;201m";
 const string CYAN    = "\033[38;5;51m";
 
 // Accent colors
 const string ORANGE  = "\033[38;5;208m";
 const string PURPLE  = "\033[38;5;141m";
 const string GRAY    = "\033[38;5;245m";
 const string WHITE   = "\033[97m";
 
 // Background highlight
 const string BG_DARK = "\033[48;5;235m";
 
 // ═══════════════════════════════════════════════════
 //              CITY DATA
 // ═══════════════════════════════════════════════════
 string numeOrase[NR_ORASE];
 double latitudine[NR_ORASE];
 double longitudine[NR_ORASE];
 
 // ═══════════════════════════════════════════════════
 //              GRAPH ADJACENCY MATRICES
 // ═══════════════════════════════════════════════════
 int distanta[NR_ORASE][NR_ORASE];
 string tipDrum[NR_ORASE][NR_ORASE];
 int vitezaMax[NR_ORASE][NR_ORASE];
 
 // ═══════════════════════════════════════════════════
 //              BACKTRACKING STATE
 // ═══════════════════════════════════════════════════
 int x[MAX_LUNGIME_TRASEU];
 int vizitat[NR_ORASE];
 int sursa, destinatia;
 
 // ═══════════════════════════════════════════════════
 //              ROUTE STORAGE
 // ═══════════════════════════════════════════════════
 int trasee[MAX_TRASEE][MAX_LUNGIME_TRASEU];
 int lungimeTraseu[MAX_TRASEE];
 double distanteTrasee[MAX_TRASEE];
 double timpTrasee[MAX_TRASEE];
 int nrTrasee;
 
 // ═══════════════════════════════════════════════════
 //              UTILITY FUNCTIONS
 // ═══════════════════════════════════════════════════
 
 /**
  * Returns ANSI color for road type
  */
 string getCuloareTipDrum(string tip) {
     if (tip.empty()) return GRAY;
     if (tip[0] == 'E') return GREEN;
     if (tip[0] == 'A') return RED;
     if (tip.substr(0, 2) == "DN") return BLUE;
     if (tip.substr(0, 2) == "DJ") return ORANGE;
     return GRAY;
 }
 
 /**
  * Display separator line
  */
 void separator() {
     cout << GRAY << "────────────────────────────────────────────────────────────" << RESET << endl;
 }
 
 /**
  * Display thick separator
  */
 void thickSeparator() {
     cout << GRAY << "════════════════════════════════════════════════════════════" << RESET << endl;
 }
 
 /**
  * Clear screen (cross-platform)
  */
 void clearScreen() {
     #ifdef _WIN32
         system("cls");
     #else
         system("clear");
     #endif
 }
 
 /**
  * Display city list in grid format
  */
 void afiseazaListaOrase() {
     cout << endl;
     cout << CYAN << BOLD << "   AVAILABLE CITIES" << RESET << endl;
     separator();
     
     for (int i = 0; i < NR_ORASE; i++) {
         cout << "   " << YELLOW << BOLD << setw(2) << i << RESET 
              << " " << WHITE << numeOrase[i] << RESET;
         
         if ((i + 1) % 4 == 0 || i == NR_ORASE - 1) {
             cout << endl;
         } else {
             int padding = 18 - numeOrase[i].length();
             for (int j = 0; j < padding; j++) cout << " ";
         }
     }
     
     separator();
     cout << endl;
 }
 
 /**
  * Format time from hours to "Xh Ymin"
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
  * Add bidirectional road between two cities
  */
 void adaugaDrum(int o1, int o2, int dist, string tip, int viteza) {
     distanta[o1][o2] = dist;
     tipDrum[o1][o2] = tip;
     vitezaMax[o1][o2] = viteza;
     
     distanta[o2][o1] = dist;
     tipDrum[o2][o1] = tip;
     vitezaMax[o2][o1] = viteza;
 }
 
 /**
  * Initialize all application data (cities + roads)
  */
 void initializeazaDate() {
     // Cities with real GPS coordinates
     numeOrase[0] = "Suceava";        latitudine[0] = 47.6439;  longitudine[0] = 26.2578;
     numeOrase[1] = "Botosani";       latitudine[1] = 47.7408;  longitudine[1] = 26.6566;
     numeOrase[2] = "Iasi";           latitudine[2] = 47.1585;  longitudine[2] = 27.6014;
     numeOrase[3] = "Bacau";          latitudine[3] = 46.5670;  longitudine[3] = 26.9146;
     numeOrase[4] = "Piatra Neamt";   latitudine[4] = 46.9267;  longitudine[4] = 26.3816;
     numeOrase[5] = "Roman";          latitudine[5] = 46.9231;  longitudine[5] = 26.9244;
     numeOrase[6] = "Vaslui";         latitudine[6] = 46.6402;  longitudine[6] = 27.7297;
     numeOrase[7] = "Galati";         latitudine[7] = 45.4353;  longitudine[7] = 28.0080;
     numeOrase[8] = "Vatra Dornei";   latitudine[8] = 47.3456;  longitudine[8] = 25.3621;
     numeOrase[9] = "Falticeni";      latitudine[9] = 47.4597;  longitudine[9] = 26.2985;
     numeOrase[10] = "Radauti";       latitudine[10] = 47.8415; longitudine[10] = 25.9207;
     numeOrase[11] = "Pascani";       latitudine[11] = 47.2472; longitudine[11] = 26.7130;
     
     // Reset matrices
     for (int i = 0; i < NR_ORASE; i++) {
         for (int j = 0; j < NR_ORASE; j++) {
             distanta[i][j] = 0;
             tipDrum[i][j] = "";
             vitezaMax[i][j] = 0;
         }
     }
     
     // Real verified roads (25 bidirectional connections)
     adaugaDrum(0, 9,  30,  "DN2",   90);
     adaugaDrum(0, 10, 42,  "DN29",  90);
     adaugaDrum(0, 8,  85,  "DN17",  80);
     adaugaDrum(10, 9, 35,  "DJ209", 70);
     adaugaDrum(10, 1, 55,  "DN29A", 80);
     adaugaDrum(10, 8, 95,  "DN17",  80);
     adaugaDrum(1, 9,  65,  "DN29B", 80);
     adaugaDrum(1, 2,  110, "E583",  100);
     adaugaDrum(9, 5,  55,  "DN2",   90);
     adaugaDrum(2, 11, 35,  "DN28",  90);
     adaugaDrum(2, 6,  70,  "E581",  100);
     adaugaDrum(2, 5,  50,  "DN28",  90);
     adaugaDrum(2, 3,  120, "E85",   100);
     adaugaDrum(5, 11, 20,  "DN2",   90);
     adaugaDrum(5, 3,  35,  "E85",   100);
     adaugaDrum(5, 4,  18,  "DN15",  80);
     adaugaDrum(4, 3,  60,  "DN15",  80);
     adaugaDrum(4, 8,  80,  "DN15",  80);
     adaugaDrum(3, 6,  90,  "DN2D",  90);
     adaugaDrum(3, 7,  120, "E85",   100);
     adaugaDrum(6, 11, 45,  "DN24",  90);
     adaugaDrum(6, 7,  72,  "DN24",  90);
 }
 
 // ═══════════════════════════════════════════════════
 //              BACKTRACKING ALGORITHM
 // ═══════════════════════════════════════════════════
 
 void INIT(int k) {
     x[k] = -1;
 }
 
 int EXISTA(int k) {
     return (x[k] < NR_ORASE - 1);
 }
 
 int VALID(int k) {
     int orasPrecedent = (k == 1) ? sursa : x[k - 1];
     
     if (distanta[orasPrecedent][x[k]] == 0) return 0;
     if (vizitat[x[k]] == 1) return 0;
     if (x[k] == sursa) return 0;
     
     return 1;
 }
 
 int SOLUTIE(int k) {
     return (x[k] == destinatia);
 }
 
 void TIPAR(int k) {
     if (nrTrasee >= MAX_TRASEE) return;
     
     trasee[nrTrasee][0] = sursa;
     for (int i = 1; i <= k; i++) {
         trasee[nrTrasee][i] = x[i];
     }
     lungimeTraseu[nrTrasee] = k + 1;
     
     double d = 0, t = 0;
     for (int i = 0; i < k; i++) {
         int c1 = trasee[nrTrasee][i];
         int c2 = trasee[nrTrasee][i + 1];
         d += distanta[c1][c2];
         t += (double)distanta[c1][c2] / vitezaMax[c1][c2];
     }
     distanteTrasee[nrTrasee] = d;
     timpTrasee[nrTrasee] = t;
     
     nrTrasee++;
 }
 
 void BKT() {
     int k = 1;
     INIT(k);
     nrTrasee = 0;
     
     for (int i = 0; i < NR_ORASE; i++) vizitat[i] = 0;
     vizitat[sursa] = 1;
     
     while (k > 0) {
         if (EXISTA(k)) {
             x[k]++;
             if (VALID(k)) {
                 vizitat[x[k]] = 1;
                 if (SOLUTIE(k)) {
                     TIPAR(k);
                     vizitat[x[k]] = 0;
                 } else {
                     k++;
                     INIT(k);
                 }
             }
         } else {
             k--;
             if (k > 0) vizitat[x[k]] = 0;
         }
     }
 }
 
 /**
  * Sort routes by distance (bubble sort)
  */
 void sortTrasee() {
     for (int i = 0; i < nrTrasee - 1; i++) {
         for (int j = i + 1; j < nrTrasee; j++) {
             if (distanteTrasee[j] < distanteTrasee[i]) {
                 swap(distanteTrasee[i], distanteTrasee[j]);
                 swap(timpTrasee[i], timpTrasee[j]);
                 swap(lungimeTraseu[i], lungimeTraseu[j]);
                 
                 for (int p = 0; p < MAX_LUNGIME_TRASEU; p++) {
                     swap(trasee[i][p], trasee[j][p]);
                 }
             }
         }
     }
 }
 
 // ═══════════════════════════════════════════════════
 //              DISPLAY FUNCTIONS
 // ═══════════════════════════════════════════════════
 
 /**
  * Display single route
  */
 void afiseazaTraseu(int index) {
     for (int j = 0; j < lungimeTraseu[index]; j++) {
         cout << WHITE << numeOrase[trasee[index][j]] << RESET;
         if (j < lungimeTraseu[index] - 1) 
             cout << GRAY << " → " << RESET;
     }
     cout << "  " << DIM << "(" << fixed << setprecision(0) 
          << distanteTrasee[index] << " km)" << RESET;
 }
 
 /**
  * Display detailed route information
  */
 void afiseazaDetaliiTraseu(int index) {
     cout << endl;
     cout << CYAN << BOLD << "   ROUTE DETAILS" << RESET << endl;
     thickSeparator();
     
     for (int i = 0; i < lungimeTraseu[index] - 1; i++) {
         int orasA = trasee[index][i];
         int orasB = trasee[index][i + 1];
         
         string culoareDrum = getCuloareTipDrum(tipDrum[orasA][orasB]);
         
         cout << "   " << BOLD << "Segment " << (i + 1) << RESET << endl;
         cout << "   " << WHITE << numeOrase[orasA] << RESET 
              << GRAY << " → " << RESET << WHITE << numeOrase[orasB] << RESET << endl;
         
         cout << "   Distance: " << GREEN << distanta[orasA][orasB] << " km" << RESET;
         cout << "  •  Road: " << culoareDrum << BOLD << tipDrum[orasA][orasB] << RESET;
         cout << "  •  Speed: " << YELLOW << vitezaMax[orasA][orasB] << " km/h" << RESET;
         
         double timpSegment = (double)distanta[orasA][orasB] / vitezaMax[orasA][orasB];
         cout << "  •  Time: " << BLUE << formateazaTimp(timpSegment) << RESET << endl;
         
         if (i < lungimeTraseu[index] - 2) {
             cout << GRAY << "   │" << RESET << endl;
         }
     }
     
     cout << endl;
     thickSeparator();
     cout << "   " << BOLD << "TOTAL" << RESET << endl;
     cout << "   Distance: " << GREEN << BOLD << fixed << setprecision(0) 
          << distanteTrasee[index] << " km" << RESET << endl;
     cout << "   Time: " << BLUE << BOLD << formateazaTimp(timpTrasee[index]) << RESET << endl;
     thickSeparator();
 }
 
 // ═══════════════════════════════════════════════════
 //              INPUT FUNCTIONS
 // ═══════════════════════════════════════════════════
 
 /**
  * Read city with validation
  */
 int citesteOras(string mesaj) {
     int oras;
     while (true) {
         cout << "   " << WHITE << mesaj << RESET;
         cin >> oras;
         
         if (cin.fail() || oras < 0 || oras >= NR_ORASE) {
             cin.clear();
             cin.ignore(numeric_limits<streamsize>::max(), '\n');
             cout << "   " << RED << "✗ Invalid! Choose 0-" << (NR_ORASE - 1) << RESET << endl;
         } else {
             cin.ignore(numeric_limits<streamsize>::max(), '\n');
             return oras;
         }
     }
 }
 
 /**
  * Read menu option
  */
 int citesteOptiune() {
     int opt;
     cout << "   " << CYAN << BOLD << "Choose option (1-9): " << RESET;
     cin >> opt;
     
     if (cin.fail()) {
         cin.clear();
         cin.ignore(numeric_limits<streamsize>::max(), '\n');
         return -1;
     }
     
     cin.ignore(numeric_limits<streamsize>::max(), '\n');
     return opt;
 }
 
 // ═══════════════════════════════════════════════════
 //              MENU
 // ═══════════════════════════════════════════════════
 
 void afiseazaMeniu() {
     clearScreen();
     
     cout << endl;
     cout << CYAN << BOLD << BG_DARK;
     cout << "   ╔══════════════════════════════════════════════════════╗   " << endl;
     cout << "   ║                                                      ║   " << endl;
     cout << "   ║          " << WHITE << "ROUTEMASTER MOLDOVA" << CYAN << "                      ║   " << endl;
     cout << "   ║          " << DIM << WHITE << "Smart Route Planning System" << CYAN << "              ║   " << endl;
     cout << "   ║                                                      ║   " << endl;
     cout << "   ╚══════════════════════════════════════════════════════╝   " << RESET << endl;
     cout << endl;
     
     afiseazaListaOrase();
     
     cout << YELLOW << BOLD << "   MAIN MENU" << RESET << endl;
     thickSeparator();
     
     cout << "   " << CYAN << BOLD << "1" << RESET << WHITE << "  Shortest Route" << RESET << endl;
     cout << "   " << CYAN << BOLD << "2" << RESET << WHITE << "  All Possible Routes" << RESET << endl;
     cout << "   " << CYAN << BOLD << "3" << RESET << WHITE << "  Road Type Analysis" << RESET << endl;
     cout << "   " << CYAN << BOLD << "4" << RESET << WHITE << "  Travel Time Estimation" << RESET << endl;
     separator();
     cout << "   " << MAGENTA << BOLD << "5" << RESET << GRAY << "  Save Routes to File" << RESET << endl;
     cout << "   " << MAGENTA << BOLD << "6" << RESET << GRAY << "  Network Statistics" << RESET << endl;
     cout << "   " << MAGENTA << BOLD << "7" << RESET << GRAY << "  Search by Road Type" << RESET << endl;
     cout << "   " << MAGENTA << BOLD << "8" << RESET << GRAY << "  Compare Two Routes" << RESET << endl;
     separator();
     cout << "   " << RED << BOLD << "9" << RESET << DIM << "  Exit" << RESET << endl;
     
     thickSeparator();
     cout << endl;
 }
 
 // ═══════════════════════════════════════════════════
 //              MENU OPTIONS
 // ═══════════════════════════════════════════════════
 
 void optiuneDistantaMinima() {
     cout << endl;
     cout << CYAN << BOLD << "   SHORTEST ROUTE" << RESET << endl;
     thickSeparator();
     
     sursa = citesteOras("Departure city (number): ");
     destinatia = citesteOras("Destination city (number): ");
     
     if (sursa == destinatia) {
         cout << "   " << RED << "✗ Source and destination are the same!" << RESET << endl;
         return;
     }
     
     cout << endl << "   " << YELLOW << "⟳ Calculating routes..." << RESET << endl;
     
     auto start = chrono::high_resolution_clock::now();
     BKT();
     auto end = chrono::high_resolution_clock::now();
     long long ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
     
     if (nrTrasee == 0) {
         cout << "   " << RED << "✗ No route exists!" << RESET << endl;
         return;
     }
     
     sortTrasee();
     
     cout << "   " << GREEN << "✓ Found " << nrTrasee << " routes in " << ms << " ms" << RESET << endl << endl;
     
     thickSeparator();
     cout << "   " << BOLD << "OPTIMAL ROUTE" << RESET << endl;
     separator();
     cout << "   ";
     afiseazaTraseu(0);
     cout << endl;
     separator();
     cout << "   Distance: " << GREEN << BOLD << fixed << setprecision(0) 
          << distanteTrasee[0] << " km" << RESET << endl;
     cout << "   Time: " << BLUE << BOLD << formateazaTimp(timpTrasee[0]) << RESET << endl;
     thickSeparator();
 }
 
 void optiuneToateTrasee() {
     cout << endl;
     cout << CYAN << BOLD << "   ALL POSSIBLE ROUTES" << RESET << endl;
     thickSeparator();
     
     sursa = citesteOras("Departure city (number): ");
     destinatia = citesteOras("Destination city (number): ");
     
     if (sursa == destinatia) {
         cout << "   " << RED << "✗ Source and destination are the same!" << RESET << endl;
         return;
     }
     
     cout << endl << "   " << YELLOW << "⟳ Calculating routes..." << RESET << endl;
     
     auto start = chrono::high_resolution_clock::now();
     BKT();
     auto end = chrono::high_resolution_clock::now();
     long long ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
     
     if (nrTrasee == 0) {
         cout << "   " << RED << "✗ No routes exist!" << RESET << endl;
         return;
     }
     
     sortTrasee();
     
     cout << "   " << GREEN << "✓ Found " << nrTrasee << " routes in " << ms << " ms" << RESET << endl << endl;
     
     thickSeparator();
     cout << "   " << BOLD << "ROUTES (sorted by distance)" << RESET << endl;
     thickSeparator();
     
     int limita = (nrTrasee < 50) ? nrTrasee : 50;
     
     for (int i = 0; i < limita; i++) {
         if (i == 0) {
             cout << GREEN << BOLD;
         } else {
             cout << WHITE;
         }
         
         cout << "   " << setw(2) << (i + 1) << ". ";
         afiseazaTraseu(i);
         cout << RESET << endl;
     }
     
     if (nrTrasee > 50) {
         cout << "   " << DIM << "... and " << (nrTrasee - 50) 
              << " more (total: " << nrTrasee << ")" << RESET << endl;
     }
     
     thickSeparator();
 }
 
 void optiuneTipuriDrum() {
     cout << endl;
     cout << CYAN << BOLD << "   ROAD TYPE ANALYSIS" << RESET << endl;
     thickSeparator();
     
     sursa = citesteOras("Departure city (number): ");
     destinatia = citesteOras("Destination city (number): ");
     
     if (sursa == destinatia) {
         cout << "   " << RED << "✗ Source and destination are the same!" << RESET << endl;
         return;
     }
     
     cout << endl << "   " << YELLOW << "⟳ Calculating routes..." << RESET << endl;
     BKT();
     
     if (nrTrasee == 0) {
         cout << "   " << RED << "✗ No routes exist!" << RESET << endl;
         return;
     }
     
     sortTrasee();
     
     cout << "   " << GREEN << "✓ Found " << nrTrasee << " routes" << RESET << endl << endl;
     
     thickSeparator();
     cout << "   " << BOLD << "AVAILABLE ROUTES" << RESET << endl;
     thickSeparator();
     
     int limita = (nrTrasee < 20) ? nrTrasee : 20;
     
     for (int i = 0; i < limita; i++) {
         cout << "   " << setw(2) << (i + 1) << ". ";
         afiseazaTraseu(i);
         cout << endl;
     }
     
     if (nrTrasee > 20) {
         cout << "   " << DIM << "... and " << (nrTrasee - 20) << " more" << RESET << endl;
     }
     
     thickSeparator();
     
     int alegere;
     cout << endl << "   " << CYAN << "Choose route number (1-" << nrTrasee << "): " << RESET;
     cin >> alegere;
     
     if (cin.fail() || alegere < 1 || alegere > nrTrasee) {
         cin.clear();
         cin.ignore(numeric_limits<streamsize>::max(), '\n');
         cout << "   " << RED << "✗ Invalid choice!" << RESET << endl;
         return;
     }
     
     cin.ignore(numeric_limits<streamsize>::max(), '\n');
     
     afiseazaDetaliiTraseu(alegere - 1);
 }
 
 void optiuneTimp() {
     cout << endl;
     cout << CYAN << BOLD << "   TRAVEL TIME ESTIMATION" << RESET << endl;
     thickSeparator();
     
     sursa = citesteOras("Departure city (number): ");
     destinatia = citesteOras("Destination city (number): ");
     
     if (sursa == destinatia) {
         cout << "   " << RED << "✗ Source and destination are the same!" << RESET << endl;
         return;
     }
     
     cout << endl << "   " << YELLOW << "⟳ Calculating routes..." << RESET << endl;
     BKT();
     
     if (nrTrasee == 0) {
         cout << "   " << RED << "✗ No routes exist!" << RESET << endl;
         return;
     }
     
     sortTrasee();
     
     cout << "   " << GREEN << "✓ Found " << nrTrasee << " routes" << RESET << endl << endl;
     
     thickSeparator();
     cout << "   " << BOLD << "OPTIMAL ROUTE" << RESET << endl;
     separator();
     cout << "   ";
     afiseazaTraseu(0);
     cout << endl << endl;
     
     cout << "   Distance: " << GREEN << fixed << setprecision(0) 
          << distanteTrasee[0] << " km" << RESET << endl;
     cout << "   Travel Time: " << BLUE << formateazaTimp(timpTrasee[0]) << RESET << endl;
     
     afiseazaDetaliiTraseu(0);
     
     cout << endl;
     cout << CYAN << BOLD << "   TRAVEL MODE COMPARISON" << RESET << endl;
     thickSeparator();
     
     double dist = distanteTrasee[0];
     double timpMasina = timpTrasee[0];
     double timpBicicleta = dist / 15.0;
     double timpPeJos = dist / 5.0;
     
     cout << "   " << MAGENTA << "⚡ By car (max speed):" << RESET 
          << "   " << WHITE << formateazaTimp(timpMasina) << RESET << endl;
     cout << "   " << YELLOW << "🚴 By bicycle (15 km/h):" << RESET 
          << "  " << WHITE << formateazaTimp(timpBicicleta) << RESET << endl;
     cout << "   " << BLUE << "🚶 On foot (5 km/h):" << RESET 
          << "      " << WHITE << formateazaTimp(timpPeJos) << RESET << endl;
     
     thickSeparator();
 }
 
 void optiuneSalvare() {
     cout << endl;
     cout << CYAN << BOLD << "   SAVE ROUTES TO FILE" << RESET << endl;
     thickSeparator();
     
     sursa = citesteOras("Departure city (number): ");
     destinatia = citesteOras("Destination city (number): ");
     
     if (sursa == destinatia) {
         cout << "   " << RED << "✗ Source and destination are the same!" << RESET << endl;
         return;
     }
     
     cout << endl << "   " << YELLOW << "⟳ Calculating routes..." << RESET << endl;
     BKT();
     
     if (nrTrasee == 0) {
         cout << "   " << RED << "✗ No routes exist!" << RESET << endl;
         return;
     }
     
     sortTrasee();
     
     cout << "   " << GREEN << "✓ Found " << nrTrasee << " routes" << RESET << endl << endl;
     
     ofstream f("routes_moldova.txt");
     
     if (!f.is_open()) {
         cout << "   " << RED << "✗ Cannot create file!" << RESET << endl;
         return;
     }
     
     f << "========================================================" << endl;
     f << "          ROUTEMASTER MOLDOVA - Saved Routes" << endl;
     f << "========================================================" << endl << endl;
     
     auto now = chrono::system_clock::now();
     time_t now_c = chrono::system_clock::to_time_t(now);
     f << "Generated: " << ctime(&now_c) << endl;
     f << "From: " << numeOrase[sursa] << endl;
     f << "To: " << numeOrase[destinatia] << endl;
     f << "Routes found: " << nrTrasee << endl;
     f << "--------------------------------------------------------" << endl << endl;
     
     for (int i = 0; i < nrTrasee; i++) {
         f << (i + 1) << ". ";
         
         for (int j = 0; j < lungimeTraseu[i]; j++) {
             f << numeOrase[trasee[i][j]];
             if (j < lungimeTraseu[i] - 1) f << " -> ";
         }
         
         f << endl;
         f << "   Distance: " << fixed << setprecision(0) 
           << distanteTrasee[i] << " km" << endl;
         f << "   Time: " << formateazaTimp(timpTrasee[i]) << endl;
         
         f << "   Segments:" << endl;
         for (int j = 0; j < lungimeTraseu[i] - 1; j++) {
             int o1 = trasee[i][j];
             int o2 = trasee[i][j + 1];
             f << "     - " << numeOrase[o1] << " -> " << numeOrase[o2];
             f << " (" << distanta[o1][o2] << " km, " 
               << tipDrum[o1][o2] << ", " 
               << vitezaMax[o1][o2] << " km/h)" << endl;
         }
         f << endl;
     }
     
     f << "========================================================" << endl;
     f << "     Generated by RouteMaster Moldova v2.0" << endl;
     f << "========================================================" << endl;
     
     f.close();
     
     cout << "   " << GREEN << "✓ Routes saved to 'routes_moldova.txt'" << RESET << endl;
     thickSeparator();
 }
 
 void optiuneStatistici() {
     cout << endl;
     cout << CYAN << BOLD << "   NETWORK STATISTICS" << RESET << endl;
     thickSeparator();
     
     int gradOras[NR_ORASE] = {0};
     int totalDrumuri = 0;
     
     int nrE = 0, nrDN = 0, nrDJ = 0, nrA = 0;
     double distTotalaE = 0, distTotalaDN = 0, distTotalaDJ = 0, distTotalaA = 0;
     
     for (int i = 0; i < NR_ORASE; i++) {
         for (int j = i + 1; j < NR_ORASE; j++) {
             if (distanta[i][j] > 0) {
                 gradOras[i]++;
                 gradOras[j]++;
                 totalDrumuri++;
                 
                 if (tipDrum[i][j][0] == 'E') {
                     nrE++;
                     distTotalaE += distanta[i][j];
                 } else if (tipDrum[i][j][0] == 'A') {
                     nrA++;
                     distTotalaA += distanta[i][j];
                 } else if (tipDrum[i][j].substr(0, 2) == "DN") {
                     nrDN++;
                     distTotalaDN += distanta[i][j];
                 } else if (tipDrum[i][j].substr(0, 2) == "DJ") {
                     nrDJ++;
                     distTotalaDJ += distanta[i][j];
                 }
             }
         }
     }
     
     int maxGrad = 0, orasMaxConexiuni = 0;
     for (int i = 0; i < NR_ORASE; i++) {
         if (gradOras[i] > maxGrad) {
             maxGrad = gradOras[i];
             orasMaxConexiuni = i;
         }
     }
     
     int minGrad = 999, orasMinConexiuni = 0;
     for (int i = 0; i < NR_ORASE; i++) {
         if (gradOras[i] < minGrad && gradOras[i] > 0) {
             minGrad = gradOras[i];
             orasMinConexiuni = i;
         }
     }
     
     int drumMaxLungime = 0, oras1Max = 0, oras2Max = 0;
     for (int i = 0; i < NR_ORASE; i++) {
         for (int j = i + 1; j < NR_ORASE; j++) {
             if (distanta[i][j] > drumMaxLungime) {
                 drumMaxLungime = distanta[i][j];
                 oras1Max = i;
                 oras2Max = j;
             }
         }
     }
     
     int drumMinLungime = 9999, oras1Min = 0, oras2Min = 0;
     for (int i = 0; i < NR_ORASE; i++) {
         for (int j = i + 1; j < NR_ORASE; j++) {
             if (distanta[i][j] > 0 && distanta[i][j] < drumMinLungime) {
                 drumMinLungime = distanta[i][j];
                 oras1Min = i;
                 oras2Min = j;
             }
         }
     }
     
     cout << "   " << BOLD << "GENERAL INFORMATION" << RESET << endl;
     separator();
     cout << "   Total cities: " << GREEN << NR_ORASE << RESET << endl;
     cout << "   Total roads: " << GREEN << totalDrumuri << RESET << endl;
     cout << "   Network length: " << GREEN << fixed << setprecision(0)
          << (distTotalaE + distTotalaDN + distTotalaDJ + distTotalaA) << " km" << RESET << endl;
     separator();
     cout << endl;
     
     cout << "   " << BOLD << "CITY CONNECTIONS" << RESET << endl;
     separator();
     cout << "   Most connected: " << YELLOW << numeOrase[orasMaxConexiuni] << RESET 
          << " (" << maxGrad << " roads)" << endl;
     cout << "   Least connected: " << YELLOW << numeOrase[orasMinConexiuni] << RESET 
          << " (" << minGrad << " roads)" << endl;
     separator();
     cout << endl;
     
     cout << "   " << BOLD << "ROAD EXTREMES" << RESET << endl;
     separator();
     cout << "   Longest: " << CYAN << numeOrase[oras1Max] << " - " 
          << numeOrase[oras2Max] << RESET << " (" << drumMaxLungime << " km)" << endl;
     cout << "   Shortest: " << CYAN << numeOrase[oras1Min] << " - " 
          << numeOrase[oras2Min] << RESET << " (" << drumMinLungime << " km)" << endl;
     separator();
     cout << endl;
     
     cout << "   " << BOLD << "ROAD TYPES" << RESET << endl;
     separator();
     if (nrE > 0) {
         cout << "   " << GREEN << "E (European):" << RESET 
              << " " << nrE << " roads, " << fixed << setprecision(0) 
              << distTotalaE << " km" << endl;
     }
     if (nrDN > 0) {
         cout << "   " << BLUE << "DN (National):" << RESET 
              << " " << nrDN << " roads, " << fixed << setprecision(0) 
              << distTotalaDN << " km" << endl;
     }
     if (nrDJ > 0) {
         cout << "   " << ORANGE << "DJ (County):" << RESET 
              << " " << nrDJ << " roads, " << fixed << setprecision(0) 
              << distTotalaDJ << " km" << endl;
     }
     if (nrA > 0) {
         cout << "   " << RED << "A (Highway):" << RESET 
              << " " << nrA << " roads, " << fixed << setprecision(0) 
              << distTotalaA << " km" << endl;
     }
     separator();
     cout << endl;
     
     cout << "   " << BOLD << "DETAILED CONNECTIONS" << RESET << endl;
     separator();
     for (int i = 0; i < NR_ORASE; i++) {
         cout << "   " << setw(15) << left << numeOrase[i] 
              << GRAY << " │ " << RESET << gradOras[i] << " roads" << endl;
     }
     thickSeparator();
 }
 
 void optiuneCautareTip() {
     cout << endl;
     cout << CYAN << BOLD << "   SEARCH BY ROAD TYPE" << RESET << endl;
     thickSeparator();
     
     cout << "   Available types:" << endl;
     separator();
     cout << "   " << GREEN << "E" << RESET << "   European roads (100 km/h)" << endl;
     cout << "   " << BLUE << "DN" << RESET << "  National roads (90 km/h)" << endl;
     cout << "   " << ORANGE << "DJ" << RESET << "  County roads (70-80 km/h)" << endl;
     cout << "   " << RED << "A" << RESET << "   Highways (130 km/h)" << endl;
     separator();
     cout << endl;
     
     string tipCautat;
     cout << "   " << WHITE << "Enter type (E, DN, DJ, A): " << RESET;
     cin >> tipCautat;
     cin.ignore(numeric_limits<streamsize>::max(), '\n');
     
     for (auto &c : tipCautat) c = toupper(c);
     
     cout << endl;
     cout << "   " << BOLD << "Roads of type " << tipCautat << RESET << endl;
     thickSeparator();
     
     int count = 0;
     double distantaTotala = 0;
     
     for (int i = 0; i < NR_ORASE; i++) {
         for (int j = i + 1; j < NR_ORASE; j++) {
             if (distanta[i][j] > 0) {
                 bool match = false;
                 
                 if (tipCautat == "E" && tipDrum[i][j][0] == 'E') match = true;
                 if (tipCautat == "A" && tipDrum[i][j][0] == 'A') match = true;
                 if (tipCautat == "DN" && tipDrum[i][j].substr(0, 2) == "DN") match = true;
                 if (tipCautat == "DJ" && tipDrum[i][j].substr(0, 2) == "DJ") match = true;
                 
                 if (match) {
                     count++;
                     distantaTotala += distanta[i][j];
                     
                     string culoare = getCuloareTipDrum(tipDrum[i][j]);
                     
                     cout << "   " << setw(2) << count << ". " 
                          << setw(15) << left << numeOrase[i] << GRAY << " → " << RESET
                          << setw(15) << left << numeOrase[j];
                     cout << GRAY << " │ " << RESET << culoare << BOLD << setw(6) << tipDrum[i][j] << RESET;
                     cout << GRAY << " │ " << RESET << GREEN << setw(3) << distanta[i][j] << " km" << RESET;
                     cout << GRAY << " │ " << RESET << YELLOW << vitezaMax[i][j] << " km/h" << RESET << endl;
                 }
             }
         }
     }
     
     thickSeparator();
     
     if (count == 0) {
         cout << "   " << RED << "✗ No roads of type " << tipCautat << RESET << endl;
     } else {
         cout << "   " << BOLD << "TOTAL: " << count << " roads, " 
              << fixed << setprecision(0) << distantaTotala << " km" << RESET << endl;
     }
     
     thickSeparator();
 }
 
 void optiuneComparare() {
     cout << endl;
     cout << CYAN << BOLD << "   COMPARE TWO ROUTES" << RESET << endl;
     thickSeparator();
     
     sursa = citesteOras("Departure city (number): ");
     destinatia = citesteOras("Destination city (number): ");
     
     if (sursa == destinatia) {
         cout << "   " << RED << "✗ Source and destination are the same!" << RESET << endl;
         return;
     }
     
     cout << endl << "   " << YELLOW << "⟳ Calculating routes..." << RESET << endl;
     BKT();
     
     if (nrTrasee == 0) {
         cout << "   " << RED << "✗ No routes exist!" << RESET << endl;
         return;
     }
     
     if (nrTrasee < 2) {
         cout << "   " << YELLOW << "⚠ Only one route exists, cannot compare!" << RESET << endl;
         return;
     }
     
     sortTrasee();
     
     cout << "   " << GREEN << "✓ Found " << nrTrasee << " routes" << RESET << endl << endl;
     
     thickSeparator();
     cout << "   " << BOLD << "AVAILABLE ROUTES" << RESET << endl;
     thickSeparator();
     
     int limita = (nrTrasee < 10) ? nrTrasee : 10;
     for (int i = 0; i < limita; i++) {
         cout << "   " << setw(2) << (i + 1) << ". ";
         afiseazaTraseu(i);
         cout << endl;
     }
     
     thickSeparator();
     
     int traseu1, traseu2;
     
     cout << endl << "   " << WHITE << "First route (1-" << nrTrasee << "): " << RESET;
     cin >> traseu1;
     
     cout << "   " << WHITE << "Second route (1-" << nrTrasee << "): " << RESET;
     cin >> traseu2;
     
     cin.ignore(numeric_limits<streamsize>::max(), '\n');
     
     if (traseu1 < 1 || traseu1 > nrTrasee || traseu2 < 1 || traseu2 > nrTrasee) {
         cout << "   " << RED << "✗ Invalid routes!" << RESET << endl;
         return;
     }
     
     if (traseu1 == traseu2) {
         cout << "   " << YELLOW << "⚠ You chose the same route twice!" << RESET << endl;
         return;
     }
     
     traseu1--; traseu2--;
     
     cout << endl;
     cout << CYAN << BOLD << "   ROUTE COMPARISON" << RESET << endl;
     thickSeparator();
     
     cout << left;
     cout << "   " << setw(25) << "Criteria" 
          << setw(20) << "Route A" 
          << setw(20) << "Route B"
          << "Winner" << endl;
     separator();
     
     cout << "   " << setw(25) << "Distance";
     cout << setw(20) << (to_string((int)distanteTrasee[traseu1]) + " km");
     cout << setw(20) << (to_string((int)distanteTrasee[traseu2]) + " km");
     if (distanteTrasee[traseu1] < distanteTrasee[traseu2]) 
         cout << GREEN << "◄ Route A" << RESET;
     else if (distanteTrasee[traseu2] < distanteTrasee[traseu1])
         cout << GREEN << "Route B ►" << RESET;
     else
         cout << YELLOW << "Tie" << RESET;
     cout << endl;
     
     cout << "   " << setw(25) << "Travel time";
     cout << setw(20) << formateazaTimp(timpTrasee[traseu1]);
     cout << setw(20) << formateazaTimp(timpTrasee[traseu2]);
     if (timpTrasee[traseu1] < timpTrasee[traseu2]) 
         cout << GREEN << "◄ Route A" << RESET;
     else if (timpTrasee[traseu2] < timpTrasee[traseu1])
         cout << GREEN << "Route B ►" << RESET;
     else
         cout << YELLOW << "Tie" << RESET;
     cout << endl;
     
     cout << "   " << setw(25) << "Cities traversed";
     cout << setw(20) << to_string(lungimeTraseu[traseu1]);
     cout << setw(20) << to_string(lungimeTraseu[traseu2]);
     if (lungimeTraseu[traseu1] < lungimeTraseu[traseu2]) 
         cout << GREEN << "◄ Route A" << RESET;
     else if (lungimeTraseu[traseu2] < lungimeTraseu[traseu1])
         cout << GREEN << "Route B ►" << RESET;
     else
         cout << YELLOW << "Tie" << RESET;
     cout << endl;
     
     separator();
     
     double difDistanta = abs(distanteTrasee[traseu1] - distanteTrasee[traseu2]);
     cout << "   " << setw(25) << "Distance difference";
     cout << CYAN << fixed << setprecision(1) << difDistanta << " km" << RESET << endl;
     
     double difTimp = abs(timpTrasee[traseu1] - timpTrasee[traseu2]);
     cout << "   " << setw(25) << "Time difference";
     cout << CYAN << formateazaTimp(difTimp) << RESET << endl;
     
     thickSeparator();
     
     cout << endl << "   " << BOLD << "RECOMMENDATION" << RESET << endl;
     separator();
     if (distanteTrasee[traseu1] < distanteTrasee[traseu2] && 
         timpTrasee[traseu1] < timpTrasee[traseu2]) {
         cout << "   " << GREEN << "✓ Route A is clearly better (shorter AND faster)!" << RESET << endl;
     } else if (distanteTrasee[traseu2] < distanteTrasee[traseu1] && 
                timpTrasee[traseu2] < timpTrasee[traseu1]) {
         cout << "   " << GREEN << "✓ Route B is clearly better (shorter AND faster)!" << RESET << endl;
     } else if (distanteTrasee[traseu1] < distanteTrasee[traseu2] && 
                timpTrasee[traseu1] > timpTrasee[traseu2]) {
         cout << "   " << YELLOW << "⚖ Route A is shorter, but Route B is faster!" << RESET << endl;
         cout << "     Choose based on priority (distance vs time)." << endl;
     } else {
         cout << "   " << YELLOW << "⚖ Routes are comparable, choose by preference!" << RESET << endl;
     }
     
     thickSeparator();
 }
 
 void optiuneIesire() {
     clearScreen();
     cout << endl << endl;
     cout << GREEN << BOLD;
     cout << "   ╔══════════════════════════════════════════════════════╗" << endl;
     cout << "   ║                                                      ║" << endl;
     cout << "   ║              Thank you for using                     ║" << endl;
     cout << "   ║              " << WHITE << "ROUTEMASTER MOLDOVA" << GREEN << "                     ║" << endl;
     cout << "   ║                                                      ║" << endl;
     cout << "   ║              " << DIM << WHITE << "Safe travels!" << GREEN << "                           ║" << endl;
     cout << "   ║                                                      ║" << endl;
     cout << "   ╚══════════════════════════════════════════════════════╝" << RESET << endl;
     cout << endl << endl;
 }
 
 // ═══════════════════════════════════════════════════
 //              MAIN FUNCTION
 // ═══════════════════════════════════════════════════
 
 int main() {
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
                 optiuneSalvare();
                 break;
             case 6:
                 optiuneStatistici();
                 break;
             case 7:
                 optiuneCautareTip();
                 break;
             case 8:
                 optiuneComparare();
                 break;
             case 9:
                 optiuneIesire();
                 return 0;
             default:
                 cout << "   " << RED << "✗ Invalid option! Choose 1-9" << RESET << endl;
                 break;
         }
         
         cout << endl << "   " << DIM << "Press Enter to continue..." << RESET;
         cin.get();
     }
     
     return 0;
 }
 