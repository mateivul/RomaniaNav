Last login: Sat Feb 14 17:41:32 on ttys004
matei ~
❯ cd Personal/Projects/C++/MapNav/moldova_navigator_v1
matei ~/Personal/Projects/C++/MapNav/moldova_navigator_v1
❯ code
matei ~/Personal/Projects/C++/MapNav/moldova_navigator_v1
❯ code main.cpp
matei ~/Personal/Projects/C++/MapNav/moldova_navigator_v1
❯ cd..
zsh: command not found: cd..
matei ~/Personal/Projects/C++/MapNav/moldova_navigator_v1
❯ cd ..
matei ~/.Trash
❯ cd
matei ~
❯ cd Personal/Projects/C++/MapNav/RomaniaNav
matei ~/Personal/Projects/C++/MapNav/RomaniaNav
❯ mkdir -p v1_moldova_console/src
mkdir -p v2_moldova_html/src
mkdir -p v3_romania_console/src
mkdir -p v4_romania_html_complete/src
mkdir -p docs/screenshots
matei ~/Personal/Projects/C++/MapNav/RomaniaNav
  UW PICO 5.09                           New Buffer





















                                  [ New file ]
^G Get Help  ^O WriteOut  ^R Read File ^Y Prev Pg   ^K Cut Text  ^C Cur Pos
^X Exit      ^J Justify   ^W Where is  ^V Next Pg   ^U UnCut Text^T To Spell
❯ ls -la

total 0
  UW PICO 5.09                           New Buffer





















                                  [ New file ]
^G Get Help  ^O WriteOut  ^R Read File ^Y Prev Pg   ^K Cut Text  ^C Cur Pos
^X Exit      ^J Justify   ^W Where is  ^V Next Pg   ^U UnCut Text^T To Spell
drwxr-xr-x  7 matei  staff  224 Feb 14 18:19 .
drwxr-xr-x  4 matei  staff  128 Feb 14 18:18 ..
drwxr-xr-x@ 3 matei  staff   96 Feb 14 18:19 docs
  UW PICO 5.09                           New Buffer





















                               [ Read 332 lines ]
^G Get Help  ^O WriteOut  ^R Read File ^Y Prev Pg   ^K Cut Text  ^C Cur Pos
^X Exit      ^J Justify   ^W Where is  ^V Next Pg   ^U UnCut Text^T To Spell
  UW PICO 5.09                        File: README.md

# 🗺️ Moldova Navigator v1.0

Aplicație C++ consolă pentru navigație rutieră în regiunea **Moldova (România)*$
Folosește **backtracking iterativ** pentru găsirea **TUTUROR** traseelor între $

---

## 📋 Descriere

**Moldova Navigator** este un sistem de navigație rutieră care găsește toate tr$

- Distanțe între localități
- Tipuri de drumuri (Europene, Naționale, Județene)
- Viteze maxime permise
- Timpi estimați de parcurgere
- Comparații între moduri de transport

---

## ⚙️ Compilare

^G Get Help  ^O WriteOut  ^R Read File ^Y Prev Pg   ^K Cut Text  ^C Cur Pos
^X Exit      ^J Justify   ^W Where is  ^V Next Pg   ^U UnCut Text^T To Spell# 🗺️ Moldova Navigator v1.0

Aplicație C++ consolă pentru navigație rutieră în regiunea **Moldova (România)**.
Folosește **backtracking iterativ** pentru găsirea **TUTUROR** traseelor între orașe.

---

## 📋 Descriere

**Moldova Navigator** este un sistem de navigație rutieră care găsește toate traseele posibile între 12 orașe din regiunea Moldova, România. Aplicația utilizează un algoritm de backtracking iterativ pentru a explora toate drumurile disponibile și oferă informații detaliate despre:

- Distanțe între localități
- Tipuri de drumuri (Europene, Naționale, Județene)
- Viteze maxime permise
- Timpi estimați de parcurgere
- Comparații între moduri de transport

---

## ⚙️ Compilare

### Cerințe de sistem:
- Compilator C++ cu suport pentru C++11 sau mai recent
- Sistem de operare: Linux, macOS, sau Windows

### Comenzi de compilare:

#### Linux/macOS:
```bash
g++ -std=c++11 -o moldova_navigator main.cpp
```

#### Windows (MinGW):
```bash
g++ -std=c++11 -o moldova_navigator.exe main.cpp
```

#### Cu optimizări:
```bash
g++ -std=c++11 -O2 -o moldova_navigator main.cpp
```

---

## 🚀 Rulare

### Linux/macOS:
```bash
./moldova_navigator
```

### Windows:
```bash
moldova_navigator.exe
```

---

## 🎯 Funcționalități

### 1. **Distanța minimă între 2 localități**
Găsește și afișează traseul cel mai scurt între două orașe alese.

**Exemplu:**
```
Oraș de plecare: 0 (Suceava)
Oraș de destinație: 2 (Iași)

Rezultat:
  Traseul minim: Suceava → Roman → Iași (120 km)
  Timp estimat: 1h 15min
```

---

### 2. **Toate traseele posibile**
Afișează **TOATE** traseele disponibile între două orașe, sortate după distanță (de la cel mai scurt la cel mai lung).

**Exemplu:**
```
S-au găsit 8 trasee în 12 ms!

1. Suceava → Roman → Iași (120 km)
2. Suceava → Fălticeni → Roman → Iași (135 km)
3. Suceava → Piatra Neamț → Roman → Iași (153 km)
...
```

**Limitare:** Pentru claritate, se afișează primele 50 de trasee în consolă.

---

### 3. **Tipuri de drum pe un traseu**
Afișează detalii segment cu segment pentru un traseu ales:
- Distanță per segment
- Tip drum (European, Național, Județean, Autostradă)
- Viteză maximă permisă
- Timp estimat per segment

**Exemplu:**
```
Segment 1: Suceava → Roman
  Distanță: 70 km
  Tip drum: E85 (European)
  Viteză max: 100 km/h
  Timp: 42 min

Segment 2: Roman → Iași
  Distanță: 50 km
  Tip drum: DN28 (Național)
  Viteză max: 90 km/h
  Timp: 33 min

TOTAL: 120 km, 1h 15min
```

---

### 4. **Timpul de parcurgere**
Calculează timpul estimat pentru traseul optim cu viteza maximă permisă pe fiecare segment.

**BONUS:** Comparație moduri de transport
```
Comparație moduri de transport:
  Cu mașina (viteză max):  1h 15min
  Cu bicicleta (15 km/h):  8h 0min
  Pe jos (5 km/h):         24h 0min
```

---

### 5. **Ieșire**
Închide aplicația.

---

## 🗺️ Orașe disponibile (12)

| Index | Oraș            | Latitudine | Longitudine |
|-------|-----------------|------------|-------------|
| 0     | Suceava         | 47.6439    | 26.2578     |
| 1     | Botoșani        | 47.7408    | 26.6566     |
| 2     | Iași            | 47.1585    | 27.6014     |
| 3     | Bacău           | 46.5670    | 26.9146     |
| 4     | Piatra Neamț    | 46.9267    | 26.3816     |
| 5     | Roman           | 46.9231    | 26.9244     |
| 6     | Vaslui          | 46.6402    | 27.7297     |
| 7     | Galați          | 45.4353    | 28.0080     |
| 8     | Vatra Dornei    | 47.3456    | 25.3621     |
| 9     | Fălticeni       | 47.4597    | 26.2985     |
| 10    | Rădăuți         | 47.8415    | 25.9207     |
| 11    | Pașcani         | 47.2472    | 26.7130     |

---

## 🛣️ Drumuri disponibile (24 drumuri bidirectionale)

Aplicația conține 24 de drumuri reale din regiunea Moldova:

### Drumuri europene (E):
- E85: Suceava-Roman, Bacău-Roman, Iași-Bacău, Bacău-Galați
- E581: Iași-Vaslui
- E583: Botoșani-Iași

### Drumuri naționale (DN):
- DN2, DN15, DN17, DN24, DN28, DN29, DN29A, DN29B

### Drumuri județene (DJ):
- DJ209: Fălticeni-Rădăuți

---

## 🎨 Caracteristici tehnice

### Tehnologii folosite:
- **Limbaj:** C++11
- **Paradigmă:** Programare procedurală
- **Algoritm:** Backtracking iterativ (structura clasică: INIT, EXISTA, VALID, SOLUTIE, TIPAR, BKT)
- **Structuri de date:** Arrays statice (fără STL containers)
- **Formatare:** Culori ANSI pentru interfață

### Performanță:
- Găsește **toate** traseele posibile în sub 50ms pentru majoritatea perechilor de orașe
- Sortare eficientă (Bubble Sort optimizat)
- Validări complete pentru input utilizator

### Limitări:
- Maximum 500 de trasee stocate (constanta MAX_TRASEE)
- Maximum 20 de orașe pe traseu (constanta MAX_LUNGIME_TRASEU)
- Afișare limitată la 50 de trasee în consolă (pentru claritate)

---

## 📊 Exemplu de utilizare completă

```
╔════════════════════════════════════════════════╗
║       MOLDOVA NAVIGATOR v1.0                   ║
║       Sistem de navigație rutieră              ║
╚════════════════════════════════════════════════╝

ORAȘE DISPONIBILE:
  0. Suceava         5. Roman          10. Rădăuți
  1. Botoșani        6. Vaslui         11. Pașcani
  2. Iași            7. Galați
  3. Bacău           8. Vatra Dornei
  4. Piatra Neamț    9. Fălticeni

══════════════════════════════════════════════════
              MENIU PRINCIPAL
══════════════════════════════════════════════════
  1. Distanța minimă între 2 localități
  2. Toate traseele posibile
  3. Tipuri de drum pe un traseu
  4. Timpul de parcurgere
  5. Ieșire
══════════════════════════════════════════════════

Alegeți opțiunea (1-5): 1

═══ DISTANȚA MINIMĂ ═══

  Oraș de plecare (număr): 0
  Oraș de destinație (număr): 7

Se calculează traseele...
S-au găsit 15 trasee în 8 ms!

══════════════════════════════════════════════════
TRASEUL MINIM: Suceava → Roman → Bacău → Galați (225 km)
Distanță: 225 km
Timp estimat: 2h 15min
══════════════════════════════════════════════════
```

---

## 🧪 Testare

### Test 1: Traseu simplu
```bash
Opțiune: 1
Sursă: 0 (Suceava)
Destinație: 9 (Fălticeni)
Rezultat așteptat: 1 traseu, 30 km
```

### Test 2: Traseu complex
```bash
Opțiune: 2
Sursă: 0 (Suceava)
Destinație: 7 (Galați)
Rezultat așteptat: Multiple trasee (>10)
```

### Test 3: Validare input
```bash
Opțiune: 1
Sursă: 0
Destinație: 0
Rezultat așteptat: Eroare "Sursa și destinația sunt identice!"
```

---

## 🐛 Depanare

### Problema: "Comandă nu găsită"
**Soluție:** Asigurați-vă că ați compilat programul și că sunteți în directorul corect.

### Problema: Culori nu se afișează corect pe Windows
**Soluție:** Utilizați Windows Terminal sau activați suport ANSI:
```bash
# PowerShell
Set-ItemProperty HKCU:\Console VirtualTerminalLevel -Type DWORD 1
```

### Problema: Erori de compilare
**Soluție:** Verificați că aveți g++ instalat și suport C++11:
```bash
g++ --version
```

---

## 📖 Structura codului

Fișierul `main.cpp` este organizat în următoarele secțiuni:

1. **Constante și variabile globale** (liniile 1-80)
2. **Funcții auxiliare** (liniile 81-150)
3. **Inițializare date** (liniile 151-250)
4. **Funcții backtracking** (liniile 251-400)
5. **Funcții utilități trasee** (liniile 401-550)
6. **Funcții input utilizator** (liniile 551-620)
7. **Funcții meniu** (liniile 621-850)
8. **Funcția main** (liniile 851-900)

---

## 🔧 Personalizare

### Adăugarea unui oraș nou:
1. Modificați `NR_ORASE` în secțiunea constante
2. Adăugați datele orașului în funcția `initializeazaDate()`
3. Adăugați drumurile către/de la noul oraș

### Modificarea vitezelor maxime:
Editați funcția `adaugaDrum()` cu noile valori de viteză.

---

## 📝 Licență

Acest proiect este creat în scop educațional.

---

## 👨‍💻 Autor

**Moldova Navigator Team**
Februarie 2026

---

## 🙏 Mulțumiri

Mulțumim tuturor utilizatorilor pentru feedback și sugestii de îmbunătățire!

---

**Drum bun! 🚗**
