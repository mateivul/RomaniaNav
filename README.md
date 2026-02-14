# 🗺️ Romania Navigator

Aplicație de navigație rutieră dezvoltată în C++ folosind **backtracking iterativ** 
pentru găsirea tuturor traseelor între orașe din România.

Proiect realizat pentru cursul de **Informatică** - [Numele Școlii Tale].

---

## 📚 Structura Proiectului

### ✅ [Faza 1: Moldova - Consolă](v1_moldova_console/)
- **Status**: ✅ Completă
- **Orașe**: 12 (regiunea Moldova)
- **Interfață**: Consolă C++ cu culori ANSI
- **Features**:
  - Distanța minimă între 2 orașe
  - Toate traseele posibile
  - Tipuri drumuri (DN, DJ, E, A)
  - Timp de parcurgere

### ⏳ [Faza 2: Moldova - HTML](v2_moldova_html/)
- **Status**: 🚧 În dezvoltare
- **Interfață**: Dual (Consolă + HTML cu hartă)

### ⏳ [Faza 3: România - Consolă](v3_romania_console/)
- **Status**: 📋 Planificat
- **Orașe**: 25 (toată România)

### ⏳ [Faza 4: România - Complete](v4_romania_html_complete/)
- **Status**: 📋 Planificat
- **Features premium**: Export PDF, statistici, comparații

---

## 🚀 Quick Start - Faza 1
```bash
cd v1_moldova_console/src
g++ -std=c++11 -o moldova_navigator main.cpp
./moldova_navigator
```

---

## 🧠 Tehnologii

- **Limbaj**: C++11
- **Algoritm**: Backtracking iterativ
- **Structuri**: Arrays statice, matrici de adiacență

---

## 📊 Date

### Moldova (12 orașe):
Suceava, Botoșani, Iași, Bacău, Piatra Neamț, Roman, 
Vaslui, Galați, Vatra Dornei, Fălticeni, Rădăuți, Pașcani

---

## 🎓 Autor

**[Numele Tău]**  
[Școala/Colegiul]  
An școlar 2024-2025

---

## 📜 Licență

MIT License
