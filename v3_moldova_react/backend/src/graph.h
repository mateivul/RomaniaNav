#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>

const int NR_ORASE = 12;
const int MAX_TRASEE = 500;
const int MAX_LUNGIME_TRASEU = 20;

struct Oras {
    std::string nume;
    double latitudine;
    double longitudine;
};

struct Traseu {
    std::vector<int> orase;
    double distanta;
    double timp;
};

class Graf {
private:
    Oras orase[NR_ORASE];
    int distanta[NR_ORASE][NR_ORASE];
    std::string tipDrum[NR_ORASE][NR_ORASE];
    int vitezaMax[NR_ORASE][NR_ORASE];
    int x[MAX_LUNGIME_TRASEU];
    int vizitat[NR_ORASE];
    int sursa, destinatia;
    std::vector<Traseu> trasee;
    
    void INIT(int k);
    int EXISTA(int k);
    int VALID(int k);
    int SOLUTIE(int k);
    void TIPAR(int k);
    void adaugaDrum(int o1, int o2, int dist, const std::string& tip, int viteza);

public:
    Graf();
    void initializeazaDate();
    std::vector<Traseu> calculeazaTrasee(int sursa, int destinatia);
    const Oras& getOras(int index) const;
    int getNrOrase() const { return NR_ORASE; }
    std::string getTipDrum(int o1, int o2) const;
    int getVitezaMax(int o1, int o2) const;
    int getDistanta(int o1, int o2) const;
};

#endif
