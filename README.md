# MapNav Moldova

Navigator interactiv pentru rețeaua rutieră din Moldova, cu hartă live și calcul de trasee prin backtracking.

## Stack

- **Backend** — server HTTP scris în C++ pur (fără framework), servește API JSON și fișiere statice
- **Frontend** — HTML + CSS + JavaScript vanilla, hartă cu [Leaflet.js](https://leafletjs.com/)
- **Date** — graf reprezentat prin liste de adiacență (`data.txt`)

## Build & Run

```bash
g++ -O2 -o server server.cpp
./server
```

Aplicația rulează la `http://localhost:8080`.

## Funcționalități

| Opțiune | Descriere |
|---|---|
| **1. Distanța minimă** | Cel mai scurt traseu (Dijkstra prin BKT sortat) |
| **2. Toate traseele** | Toate traseele simple găsite, sortabile și comparabile |
| **3. Tipuri de drum** | Distribuția pe tipuri (A / E / DN / DJ) cu bare vizuale |
| **4. Timp de parcurgere** | Timp estimat pentru 🚗 Mașină, 🚲 Bicicletă, 🚶 Pe jos |

**Altele:** dark/light mode, sidebar reglabil (drag), click pe pin pentru selecție rapidă.

## Format date (`data.txt`)

```
<nr_orase>
<Nume> <lat> <lon>
...
<nr_drumuri>
<idx_a> <idx_b> <distanta_km> <cod_drum>
...
```

Tipuri de drum recunoscute: `A` (autostradă), `E` (european), `DN` (național), `DJ` (județean).
