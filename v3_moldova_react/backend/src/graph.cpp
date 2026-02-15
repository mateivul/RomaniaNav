#include "graph.h"
#include <algorithm>

void Graf::adaugaDrum(int o1, int o2, int dist, const std::string& tip, int viteza) {
    distanta[o1][o2] = distanta[o2][o1] = dist;
    tipDrum[o1][o2] = tipDrum[o2][o1] = tip;
    vitezaMax[o1][o2] = vitezaMax[o2][o1] = viteza;
}

Graf::Graf() {
    for (int i = 0; i < NR_ORASE; i++) {
        for (int j = 0; j < NR_ORASE; j++) {
            distanta[i][j] = 0;
            tipDrum[i][j] = "";
            vitezaMax[i][j] = 0;
        }
    }
}

void Graf::initializeazaDate() {
    orase[0] = {"Suceava", 47.6439, 26.2578};
    orase[1] = {"Botoșani", 47.7408, 26.6566};
    orase[2] = {"Iași", 47.1585, 27.6014};
    orase[3] = {"Bacău", 46.5670, 26.9146};
    orase[4] = {"Piatra Neamț", 46.9267, 26.3816};
    orase[5] = {"Roman", 46.9231, 26.9244};
    orase[6] = {"Vaslui", 46.6402, 27.7297};
    orase[7] = {"Galați", 45.4353, 28.0080};
    orase[8] = {"Vatra Dornei", 47.3456, 25.3621};
    orase[9] = {"Fălticeni", 47.4597, 26.2985};
    orase[10] = {"Rădăuți", 47.8415, 25.9207};
    orase[11] = {"Pașcani", 47.2472, 26.7130};

    adaugaDrum(0, 9, 30, "DN2", 90);
    adaugaDrum(0, 10, 42, "DN29", 90);
    adaugaDrum(0, 5, 70, "E85", 100);
    adaugaDrum(0, 8, 110, "DN17", 80);
    adaugaDrum(0, 4, 85, "DN15", 80);
    adaugaDrum(1, 10, 55, "DN29A", 80);
    adaugaDrum(1, 9, 65, "DN29B", 80);
    adaugaDrum(1, 2, 110, "E583", 100);
    adaugaDrum(2, 11, 35, "DN28", 90);
    adaugaDrum(2, 6, 70, "E581", 100);
    adaugaDrum(2, 5, 50, "DN28", 90);
    adaugaDrum(2, 3, 120, "E85", 100);
    adaugaDrum(3, 5, 35, "E85", 100);
    adaugaDrum(3, 4, 60, "DN15", 80);
    adaugaDrum(3, 6, 90, "DN2", 90);
    adaugaDrum(3, 7, 120, "E85", 100);
    adaugaDrum(4, 5, 18, "DN15", 80);
    adaugaDrum(4, 8, 80, "DN15", 80);
    adaugaDrum(5, 11, 20, "DN2", 90);
    adaugaDrum(6, 7, 72, "DN24", 90);
    adaugaDrum(6, 11, 45, "DN24", 90);
    adaugaDrum(8, 10, 95, "DN17", 80);
    adaugaDrum(9, 10, 35, "DJ209", 70);
    adaugaDrum(9, 5, 55, "DN2", 90);
}
void Graf::INIT(int k) { x[k] = -1; }

int Graf::EXISTA(int k) { return (x[k] < NR_ORASE - 1); }

int Graf::VALID(int k) {
    int orasPrecedent = (k == 1) ? sursa : x[k - 1];
    if (distanta[orasPrecedent][x[k]] == 0) return 0;
    if (vizitat[x[k]] == 1) return 0;
    if (x[k] == sursa) return 0;
    return 1;
}

int Graf::SOLUTIE(int k) { return (x[k] == destinatia); }

void Graf::TIPAR(int k) {
    if ((int)trasee.size() >= MAX_TRASEE) return;
    Traseu t;
    t.orase.push_back(sursa);
    for (int i = 1; i <= k; i++) t.orase.push_back(x[i]);
    t.distanta = 0;
    t.timp = 0;
    for (size_t i = 0; i < t.orase.size() - 1; i++) {
        int o1 = t.orase[i], o2 = t.orase[i + 1];
        t.distanta += distanta[o1][o2];
        t.timp += (double)distanta[o1][o2] / vitezaMax[o1][o2];
    }
    trasee.push_back(t);
}

std::vector<Traseu> Graf::calculeazaTrasee(int s, int d) {
    sursa = s;
    destinatia = d;
    trasee.clear();
    for (int i = 0; i < NR_ORASE; i++) vizitat[i] = 0;
    vizitat[sursa] = 1;

    int k = 1;
    INIT(k);
    while (k > 0) {
        if (EXISTA(k)) {
            x[k] = x[k] + 1;
            if (VALID(k)) {
                vizitat[x[k]] = 1;
                if (SOLUTIE(k)) {
                    TIPAR(k);
                    vizitat[x[k]] = 0;
                } else {
                    k = k + 1;
                    INIT(k);
                }
            }
        } else {
            k = k - 1;
            if (k > 0) vizitat[x[k]] = 0;
        }
    }

    std::sort(trasee.begin(), trasee.end(), [](const Traseu& a, const Traseu& b) {
        return a.distanta < b.distanta;
    });
    return trasee;
}

const Oras& Graf::getOras(int index) const { return orase[index]; }
std::string Graf::getTipDrum(int o1, int o2) const { return tipDrum[o1][o2]; }
int Graf::getVitezaMax(int o1, int o2) const { return vitezaMax[o1][o2]; }
int Graf::getDistanta(int o1, int o2) const { return distanta[o1][o2]; }
