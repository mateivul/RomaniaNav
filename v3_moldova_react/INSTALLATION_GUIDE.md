# Moldova Navigator v3.0 - Ghid de Instalare

## Structura creată

✓ backend/
  ├─ CMakeLists.txt
  ├─ src/graph.h
  └─ libs/ (pentru cpp-httplib)

✓ frontend/
  └─ package.json

## Pași următori

### 1. Completează fișierele backend

Copiază codul complet pentru:
- `backend/src/graph.cpp` 
- `backend/src/main.cpp`

Din documentația IMPLEMENTATION_GUIDE_V3.md

### 2. Instalează dependințe frontend

```bash
cd frontend
npm install
```

### 3. Creează componentele React

Creează fișierele în `frontend/src/`:
- App.jsx
- index.js
- index.css
- components/Map.jsx
- components/Controls.jsx
- components/RouteList.jsx
- components/RouteDetails.jsx
- api/backend.js

Vezi cod complet în IMPLEMENTATION_GUIDE_V3.md

### 4. Compilează backend

```bash
cd backend/build
cmake ..
make
```

### 5. Rulează aplicația

**IMPORTANT: Pornește ÎNTÂI backend-ul.** Dacă frontend-ul pornește fără backend, vei vedea "Offline" și "Nu s-au putut încărca orașele" (proxy ECONNREFUSED).

**Terminal 1 – Backend (pornește primul):**
```bash
cd v3_moldova_react/backend/build
cmake ..   # doar prima dată
make       # doar prima dată
./moldova_server
```
Ar trebui să vezi: `Server running on http://localhost:8081`

**Terminal 2 – Frontend:**
```bash
cd v3_moldova_react/frontend
npm start
```

Alternativ, din `frontend/`: `npm run backend` în Terminal 1, apoi `npm start` în Terminal 2.

## URL-uri

- Backend: http://localhost:8081
- Frontend: http://localhost:3001

