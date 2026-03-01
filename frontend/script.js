// ===== GLOBAL STATE =====
let map;
let citiesData = [];
let edgesData = [];
let cityMarkers = [];
let edgeLines = [];
let routeLines = [];
let highlightMarkers = [];
let clickSelectMode = 'start'; // 'start' or 'end'
let allPathsData = []; // store option 2 results
let compareA = -1, compareB = -1; // indices for comparison
let currentSort = 'distance'; // sort mode
let sortDirection = 'asc';   // 'asc' | 'desc'
let currentSelectedIdx = 0; // which path is highlighted on map in option 2
let option4Data = null;     // stored result for option 4 (to re-render on mode change)
let transportMode = 'car';  // 'car' | 'bicycle' | 'walking'
let isDarkMode = true;
let darkTileLayer = null;
let lightTileLayer = null;

const ACCENT_COLOR = '#e94560';

const roadColors = {
    'A': '#e74c3c',
    'E': '#2ecc71',
    'DN': '#3498db',
    'DJ': '#f39c12'
};

// ===== INIT =====
document.addEventListener('DOMContentLoaded', async function () {
    document.getElementById('results').innerHTML =
        '<div class="loading"><div class="loading-spinner"></div><br>Se încarcă harta...</div>';
    initMap();
    await loadCities();
    populateSelects();
    setupEvents();
    document.getElementById('results').innerHTML =
        '<div class="placeholder"><div class="placeholder-icon">🧭</div>' +
        '<p>Selectează două orașe și apasă <strong>Execută cerere</strong></p></div>';
});

function initMap() {
    map = L.map('map', {
        center: [46.85, 26.9],
        zoom: 8,
        zoomControl: true
    });

    darkTileLayer = L.tileLayer('https://{s}.basemaps.cartocdn.com/dark_all/{z}/{x}/{y}.png', {
        attribution: '&copy; OpenStreetMap contributors &copy; CARTO',
        maxZoom: 19
    });

    lightTileLayer = L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
        attribution: '&copy; OpenStreetMap contributors',
        maxZoom: 18
    });

    darkTileLayer.addTo(map);
}

async function loadCities() {
    try {
        const resp = await fetch('/api/cities');
        const data = await resp.json();
        citiesData = data.cities;
        edgesData = data.edges;
        drawCitiesAndEdges();
    } catch (e) {
        console.error('Eroare la incarcarea datelor:', e);
    }
}

function drawCitiesAndEdges() {
    // Draw edges
    edgesData.forEach(edge => {
        let c1 = citiesData[edge.from];
        let c2 = citiesData[edge.to];
        let color = roadColors[edge.type] || '#555';

        let line = L.polyline(
            [[c1.lat, c1.lon], [c2.lat, c2.lon]],
            { color: color, weight: 2, opacity: 0.35, dashArray: '5,5' }
        ).addTo(map);

        line.bindTooltip(edge.road + ' (' + edge.dist + ' km)', {
            sticky: true,
            className: 'edge-tooltip'
        });

        edgeLines.push(line);
    });

    // Draw city markers
    citiesData.forEach((city) => {
        let marker = L.circleMarker([city.lat, city.lon], {
            radius: 7,
            fillColor: '#e94560',
            color: '#fff',
            weight: 2,
            fillOpacity: 0.9
        }).addTo(map);

        marker.bindTooltip(city.name, {
            permanent: false,
            direction: 'top',
            offset: [0, -10]
        });

        // Click to select
        marker.on('click', function () {
            selectCityFromMap(city.name);
        });

        cityMarkers.push(marker);
    });
}

function selectCityFromMap(name) {
    let startSel = document.getElementById('startCity');
    let endSel = document.getElementById('endCity');

    if (clickSelectMode === 'start') {
        startSel.value = name;
        clickSelectMode = 'end';
        showClickHint('Acum click pentru DESTINAȚIE sau selectează manual');
    } else {
        endSel.value = name;
        clickSelectMode = 'start';
        showClickHint('Click pe pin: selectează START');
    }
    updateMarkerHighlights();
    validateCities();
    updateClickModeIndicator();
}

function showClickHint(text) {
    document.getElementById('clickHint').textContent = '💡 ' + text;
}

function updateMarkerHighlights() {
    let startName = document.getElementById('startCity').value;
    let endName = document.getElementById('endCity').value;

    cityMarkers.forEach((marker, idx) => {
        let city = citiesData[idx];
        if (city.name === startName) {
            marker.setStyle({ fillColor: '#2ecc71', radius: 10, weight: 3 });
        } else if (city.name === endName) {
            marker.setStyle({ fillColor: '#e74c3c', radius: 10, weight: 3 });
        } else {
            marker.setStyle({ fillColor: '#e94560', radius: 7, weight: 2 });
        }
    });
}

function populateSelects() {
    let startSel = document.getElementById('startCity');
    let endSel = document.getElementById('endCity');

    let sorted = [...citiesData].sort((a, b) => a.name.localeCompare(b.name, 'ro'));

    sorted.forEach(city => {
        let o1 = document.createElement('option');
        o1.value = city.name;
        o1.textContent = city.name;
        startSel.appendChild(o1);

        let o2 = document.createElement('option');
        o2.value = city.name;
        o2.textContent = city.name;
        endSel.appendChild(o2);
    });

    if (citiesData.length > 1) {
        startSel.value = citiesData[0].name;  // Iasi
        endSel.value = citiesData[4].name;    // Bacau
    }
    updateMarkerHighlights();
}

function setupEvents() {
    document.getElementById('searchBtn').addEventListener('click', doSearch);
    document.getElementById('resetBtn').addEventListener('click', doReset);
    document.getElementById('themeToggle').addEventListener('click', toggleTheme);
    document.getElementById('startCity').addEventListener('change', function () {
        updateMarkerHighlights();
        validateCities();
        updateClickModeIndicator();
    });
    document.getElementById('endCity').addEventListener('change', function () {
        updateMarkerHighlights();
        validateCities();
        updateClickModeIndicator();
    });
    document.addEventListener('keydown', function (e) {
        if (e.key === 'Enter') doSearch();
    });
    setupResizeHandle();
    updateClickModeIndicator();
}

function validateCities() {
    let startName = document.getElementById('startCity').value;
    let endName   = document.getElementById('endCity').value;
    let btn = document.getElementById('searchBtn');
    if (startName && endName && startName === endName) {
        btn.disabled = true;
        btn.title    = 'Selectați orașe diferite';
    } else {
        btn.disabled = false;
        btn.title    = '';
    }
}

function updateClickModeIndicator() {
    document.getElementById('startCity').closest('.input-group')
        .classList.toggle('click-active', clickSelectMode === 'start');
    document.getElementById('endCity').closest('.input-group')
        .classList.toggle('click-active', clickSelectMode === 'end');
}

function setupResizeHandle() {
    const handle  = document.getElementById('resizeHandle');
    const sidebar = document.querySelector('.sidebar');
    let isResizing = false;

    handle.addEventListener('mousedown', function (e) {
        isResizing = true;
        handle.classList.add('dragging');
        document.body.style.cursor     = 'col-resize';
        document.body.style.userSelect = 'none';
        e.preventDefault();
    });

    let resizeTimer = null;
    document.addEventListener('mousemove', function (e) {
        if (!isResizing) return;
        let newWidth = Math.max(280, Math.min(650, e.clientX));
        sidebar.style.width    = newWidth + 'px';
        sidebar.style.minWidth = newWidth + 'px';
        clearTimeout(resizeTimer);
        resizeTimer = setTimeout(() => map.invalidateSize(), 50);
    });

    document.addEventListener('mouseup', function () {
        if (!isResizing) return;
        isResizing = false;
        handle.classList.remove('dragging');
        document.body.style.cursor     = '';
        document.body.style.userSelect = '';
        map.invalidateSize();
    });
}

// ===== THEME TOGGLE =====
function toggleTheme() {
    isDarkMode = !isDarkMode;
    const btn = document.getElementById('themeToggle');
    if (isDarkMode) {
        document.body.classList.remove('light-mode');
        map.removeLayer(lightTileLayer);
        darkTileLayer.addTo(map);
        btn.textContent = '☀️';
    } else {
        document.body.classList.add('light-mode');
        map.removeLayer(darkTileLayer);
        lightTileLayer.addTo(map);
        btn.textContent = '🌙';
    }
}

// ===== RESET =====
function doReset() {
    clearRoutes();
    allPathsData = [];
    compareA = -1;
    compareB = -1;
    currentSelectedIdx = 0;
    option4Data = null;
    transportMode = 'car';
    clickSelectMode = 'start';
    showClickHint('Click pe pin pentru selecție rapidă');
    document.getElementById('results').innerHTML =
        '<div class="placeholder"><div class="placeholder-icon">🧭</div>' +
        '<p>Selectează două orașe și apasă <strong>Execută cerere</strong></p></div>';
    updateMarkerHighlights();
    validateCities();
    updateClickModeIndicator();
}

function clearRoutes() {
    routeLines.forEach(l => map.removeLayer(l));
    routeLines = [];
    highlightMarkers.forEach(m => map.removeLayer(m));
    highlightMarkers = [];
}

// ===== SEARCH =====
async function doSearch() {
    let startName = document.getElementById('startCity').value;
    let endName = document.getElementById('endCity').value;
    let option = document.getElementById('optionSelect').value;

    if (!startName || !endName) return;

    let resultsDiv = document.getElementById('results');
    let btn = document.getElementById('searchBtn');

    btn.disabled = true;
    btn.textContent = 'Se calculează...';
    resultsDiv.innerHTML = '<div class="loading"><div class="loading-spinner"></div><br>Se calculează...</div>';

    try {
        let url = '/api/route?start=' + encodeURIComponent(startName) +
            '&end=' + encodeURIComponent(endName) +
            '&option=' + option;

        let resp = await fetch(url);
        let data = await resp.json();

        if (data.error) {
            resultsDiv.innerHTML = '<div class="error-msg">⚠️ ' + data.error + '</div>';
            return;
        }

        clearRoutes();

        switch (parseInt(option)) {
            case 1: showOption1(data); break;
            case 2: showOption2(data); break;
            case 3: showOption3(data); break;
            case 4: showOption4(data); break;
        }
    } catch (e) {
        resultsDiv.innerHTML = '<div class="error-msg">⚠️ Eroare de conexiune</div>';
    } finally {
        btn.disabled = false;
        btn.textContent = 'Execută cerere';
        validateCities();
    }
}

// ===== GET CITY COORDS =====
function getCityCoords(name) {
    let c = citiesData.find(x => x.name === name);
    return c ? [c.lat, c.lon] : null;
}

// ===== DRAW ROUTE ON MAP =====
// colorByType=true → color each segment by road type; false → single solid color
function drawRoute(result, color, colorByType, addMarkers) {
    if (!result.segmente) return;

    result.segmente.forEach(seg => {
        let from = getCityCoords(seg.from);
        let to   = getCityCoords(seg.to);
        if (from && to) {
            let segColor = colorByType ? (roadColors[seg.roadType] || color) : color;
            let line = L.polyline([from, to], {
                color:   segColor,
                weight:  5,
                opacity: colorByType ? 0.85 : 0.8
            }).addTo(map);
            line.bindTooltip(seg.roadFull + ' (' + seg.distance + ' km)', { sticky: true });
            routeLines.push(line);
        }
    });

    if (addMarkers && result.nodes) {
        result.nodes.forEach((name, idx) => {
            let coords = getCityCoords(name);
            if (coords) {
                let isEndpoint = (idx === 0 || idx === result.nodes.length - 1);
                let marker = L.circleMarker(coords, {
                    radius:      isEndpoint ? 12 : 8,
                    fillColor:   isEndpoint ? (idx === 0 ? '#2ecc71' : '#e74c3c') : '#fff',
                    color:       '#111',
                    weight:      2,
                    fillOpacity: 0.95
                }).addTo(map);
                marker.bindTooltip(name, { permanent: isEndpoint, direction: 'top', offset: [0, -10] });
                highlightMarkers.push(marker);
            }
        });
        let allCoords = result.nodes.map(n => getCityCoords(n)).filter(c => c);
        if (allCoords.length > 0)
            map.fitBounds(allCoords, { padding: [50, 50] });
    }
}

// ===== BUILD PATH HTML =====
function buildPathHTML(nodes) {
    let html = '<div class="path-display">';
    nodes.forEach((n, i) => {
        if (i > 0) html += '<span class="path-arrow">➜</span>';
        html += '<span class="path-city">' + n + '</span>';
    });
    html += '</div>';
    return html;
}

// ===== FORMAT TIME =====
function formatTime(hours) {
    let h = Math.floor(hours);
    let m = Math.round((hours - h) * 60);
    if (h > 0) return h + '<span class="time-unit">h</span> ' + m + '<span class="time-unit">min</span>';
    return m + '<span class="time-unit">min</span>';
}

// ===== OPTION 1: MINIMUM DISTANCE =====
function showOption1(data) {
    let r = data.result;
    drawRoute(r, ACCENT_COLOR, true, true);

    let html = '';

    // Big stat
    html += '<div class="result-card">';
    html += '<div class="result-title">🏆 Distanța minimă</div>';
    html += '<div class="big-stat"><span class="value">' + r.distance + '</span><span class="unit">km</span>';
    html += '<div class="label">' + r.nodes.length + ' orașe parcurse</div></div>';
    html += buildPathHTML(r.nodes);
    html += '</div>';

    // Time estimate
    html += '<div class="result-card">';
    html += '<div class="result-title">⏱️ Timp estimat</div>';
    html += '<div class="big-stat"><span class="value">' + formatTime(r.totalTime) + '</span></div>';
    html += '</div>';

    // Segments
    html += buildSegmentsTable(r.segmente);

    document.getElementById('results').innerHTML = html;
}

// ===== OPTION 2: ALL PATHS =====
const PATHS_DISPLAY_LIMIT = 50;

function getRoadTypeSummary(path) {
    let dist = {};
    if (path.segmente) {
        path.segmente.forEach(s => {
            if (!dist[s.roadType]) dist[s.roadType] = 0;
            dist[s.roadType] += s.distance;
        });
    }
    return dist;
}

function showOption2(data) {
    allPathsData = data.results;
    compareA = -1;
    compareB = -1;
    currentSort = 'distance';
    sortDirection = 'asc';
    currentSelectedIdx = 0;

    if (allPathsData.length > 0)
        drawRoute(allPathsData[0], ACCENT_COLOR, true, true);

    renderAllPaths();
}

function getSortedIndices() {
    let idxs = allPathsData.map((_, i) => i);
    const dir = sortDirection === 'asc' ? 1 : -1;
    if (currentSort === 'distance')
        idxs.sort((a, b) => dir * (allPathsData[a].distance - allPathsData[b].distance));
    else if (currentSort === 'time')
        idxs.sort((a, b) => dir * (allPathsData[a].totalTime - allPathsData[b].totalTime));
    else if (currentSort === 'stops')
        idxs.sort((a, b) => dir * (allPathsData[a].nodes.length - allPathsData[b].nodes.length));
    return idxs;
}

function renderAllPaths() {
    const container = document.getElementById('results');
    const scrollTop = container ? container.scrollTop : 0;

    let sortedIdxs = getSortedIndices();
    let total     = allPathsData.length;
    let showCount = Math.min(total, PATHS_DISPLAY_LIMIT);
    let html = '';

    // Header card
    let titleText = total <= PATHS_DISPLAY_LIMIT
        ? '📋 ' + total + ' trasee găsite'
        : '📋 ' + total + ' trasee găsite · afișate primele ' + PATHS_DISPLAY_LIMIT;
    html += '<div class="result-card">';
    html += '<div class="result-title">' + titleText + '</div>';
    const arrow = sortDirection === 'asc' ? ' ↑' : ' ↓';
    html += '<div class="paths-controls">';
    html += '<button class="sort-btn ' + (currentSort === 'distance' ? 'active' : '') + '" onclick="sortPaths(\'distance\')">📏 Distanță' + (currentSort === 'distance' ? arrow : '') + '</button>';
    html += '<button class="sort-btn ' + (currentSort === 'time' ? 'active' : '') + '" onclick="sortPaths(\'time\')">⏱️ Timp' + (currentSort === 'time' ? arrow : '') + '</button>';
    html += '<button class="sort-btn ' + (currentSort === 'stops' ? 'active' : '') + '" onclick="sortPaths(\'stops\')">📍 Opriri' + (currentSort === 'stops' ? arrow : '') + '</button>';
    html += '</div>';
    html += '<div class="compare-hint">Click = afișează pe hartă &nbsp;·&nbsp; ⚖️ = selectează pentru comparație</div>';
    html += '</div>';

    // Compare panel — shown when both A and B are selected
    if (compareA >= 0 && compareB >= 0)
        html += buildComparePanel(allPathsData[compareA], allPathsData[compareB]);

    // Path cards — first PATHS_DISPLAY_LIMIT only
    sortedIdxs.slice(0, showCount).forEach((origIdx, rank) => {
        let p = allPathsData[origIdx];

        let cardClass = 'path-card';
        if (origIdx === currentSelectedIdx) cardClass += ' selected';
        if (origIdx === compareA)           cardClass += ' compare-a';
        else if (origIdx === compareB)      cardClass += ' compare-b';

        let timeMin      = Math.round(p.totalTime * 60);
        let intermediate = p.nodes.length - 2;
        let roadSummary  = getRoadTypeSummary(p);

        let cmpLabel  = (origIdx === compareA) ? '🔵 A' : (origIdx === compareB) ? '🟠 B' : '⚖️';
        let cmpActive = (origIdx === compareA || origIdx === compareB) ? ' cmp-active' : '';

        html += '<div class="' + cardClass + '" onclick="showPathFromCard(' + origIdx + ')">';

        // Header row: rank + stats + compare button
        html += '<div class="path-header">';
        html += '<span class="path-rank">#' + (rank + 1) + '</span>';
        html += '<div class="path-stats">';
        html += '<span>📏 <span class="path-stat-val">' + p.distance + '</span> km</span>';
        html += '<span>⏱️ <span class="path-stat-val">' + timeMin + '</span> min</span>';
        if (intermediate > 0)
            html += '<span>📍 <span class="path-stat-val">' + intermediate + '</span> opr.</span>';
        html += '</div>';
        html += '<button class="cmp-btn' + cmpActive + '" onclick="event.stopPropagation(); toggleCompare(' + origIdx + ')" title="Selectează pentru comparație">' + cmpLabel + '</button>';
        html += '</div>';

        // Road type badges
        html += '<div class="path-road-types">';
        ['A', 'E', 'DN', 'DJ'].forEach(t => {
            if (roadSummary[t])
                html += '<span class="road-tag road-' + t + '">' + t + ' ' + roadSummary[t] + 'km</span>';
        });
        html += '</div>';

        html += buildPathHTML(p.nodes);
        html += '</div>';
    });

    if (total > PATHS_DISPLAY_LIMIT)
        html += '<div class="paths-more-hint">... și încă ' + (total - PATHS_DISPLAY_LIMIT) + ' trasee neafișate</div>';

    if (container) {
        container.innerHTML = html;
        container.scrollTop = scrollTop;
    }
}

function sortPaths(mode) {
    if (mode === currentSort) {
        sortDirection = sortDirection === 'asc' ? 'desc' : 'asc';
    } else {
        currentSort = mode;
        sortDirection = 'asc';
    }
    renderAllPaths();
}

function showPathFromCard(idx) {
    currentSelectedIdx = idx;
    clearRoutes();
    drawRoute(allPathsData[idx], ACCENT_COLOR, true, true);
    renderAllPaths();
}

function toggleCompare(idx) {
    if (compareA === idx) {
        compareA = -1;
    } else if (compareB === idx) {
        compareB = -1;
    } else if (compareA === -1) {
        compareA = idx;
    } else if (compareB === -1) {
        compareB = idx;
    } else {
        compareB = idx; // replace B
    }

    clearRoutes();
    if (compareA >= 0 && compareB >= 0) {
        drawRoute(allPathsData[compareA], '#3498db', false, false);
        drawRoute(allPathsData[compareB], '#e67e22', false, false);
        let allCoords = [];
        [...allPathsData[compareA].nodes, ...allPathsData[compareB].nodes].forEach(n => {
            let c = getCityCoords(n);
            if (c) allCoords.push(c);
        });
        if (allCoords.length > 0) map.fitBounds(allCoords, { padding: [50, 50] });
    } else {
        // One or none selected — show current path
        let showIdx = compareA >= 0 ? compareA : compareB >= 0 ? compareB : currentSelectedIdx;
        if (showIdx >= 0 && allPathsData[showIdx])
            drawRoute(allPathsData[showIdx], ACCENT_COLOR, true, true);
    }

    renderAllPaths();
}

function clearCompare() {
    compareA = -1;
    compareB = -1;
    clearRoutes();
    if (currentSelectedIdx >= 0 && allPathsData[currentSelectedIdx])
        drawRoute(allPathsData[currentSelectedIdx], ACCENT_COLOR, true, true);
    renderAllPaths();
}

function buildComparePanel(a, b) {
    let html = '<div class="compare-panel">';

    html += '<div class="compare-title-row">';
    html += '<span>⚖️ Comparație trasee</span>';
    html += '<button class="cmp-close-btn" onclick="clearCompare()" title="Închide">✕</button>';
    html += '</div>';

    html += '<div class="compare-grid">';

    // Column headers
    html += '<div class="compare-val compare-val-a compare-col-header" style="color:#3498db">🔵 Traseu A</div>';
    html += '<div class="compare-label"></div>';
    html += '<div class="compare-val compare-val-b compare-col-header" style="color:#e67e22">🟠 Traseu B</div>';

    // Distance
    let dA = a.distance <= b.distance ? 'better' : 'worse';
    let dB = b.distance <= a.distance ? 'better' : 'worse';
    html += '<div class="compare-val compare-val-a ' + dA + '">' + a.distance + ' km</div>';
    html += '<div class="compare-label">📏 Distanță</div>';
    html += '<div class="compare-val compare-val-b ' + dB + '">' + b.distance + ' km</div>';

    // Time
    let tA = Math.round(a.totalTime * 60), tB = Math.round(b.totalTime * 60);
    let tcA = tA <= tB ? 'better' : 'worse', tcB = tB <= tA ? 'better' : 'worse';
    html += '<div class="compare-val compare-val-a ' + tcA + '">' + tA + ' min</div>';
    html += '<div class="compare-label">⏱️ Timp</div>';
    html += '<div class="compare-val compare-val-b ' + tcB + '">' + tB + ' min</div>';

    // Intermediate stops
    let sA = a.nodes.length - 2, sB = b.nodes.length - 2;
    let scA = sA <= sB ? 'better' : 'worse', scB = sB <= sA ? 'better' : 'worse';
    html += '<div class="compare-val compare-val-a ' + scA + '">' + sA + ' opr.</div>';
    html += '<div class="compare-label">📍 Opriri</div>';
    html += '<div class="compare-val compare-val-b ' + scB + '">' + sB + ' opr.</div>';

    // Road types used
    let rtA = getRoadTypeSummary(a), rtB = getRoadTypeSummary(b);
    html += '<div class="compare-val compare-val-a compare-road-cell">';
    ['A','E','DN','DJ'].forEach(t => { if (rtA[t]) html += '<span class="road-tag road-' + t + '" style="font-size:0.68em;padding:1px 4px">' + t + '</span>'; });
    html += '</div>';
    html += '<div class="compare-label">🛣️ Drumuri</div>';
    html += '<div class="compare-val compare-val-b compare-road-cell">';
    ['A','E','DN','DJ'].forEach(t => { if (rtB[t]) html += '<span class="road-tag road-' + t + '" style="font-size:0.68em;padding:1px 4px">' + t + '</span>'; });
    html += '</div>';

    html += '</div>'; // compare-grid

    // Full paths
    html += '<div class="compare-paths">';
    html += '<div class="compare-path-a">🔵 ' + a.nodes.join(' → ') + '</div>';
    html += '<div class="compare-path-b">🟠 ' + b.nodes.join(' → ') + '</div>';
    html += '</div>';

    html += '</div>';
    return html;
}

// ===== OPTION 3: ROAD TYPES =====
function showOption3(data) {
    let r = data.result;
    drawRoute(r, ACCENT_COLOR, true, true);

    let html = '';

    // Path
    html += '<div class="result-card">';
    html += '<div class="result-title">🛣️ Tipuri de drum pe traseu</div>';
    html += '<div class="big-stat"><span class="value">' + r.distance + '</span><span class="unit">km</span></div>';
    html += buildPathHTML(r.nodes);
    html += '</div>';

    // Road type summary
    let roadStats = {};
    r.segmente.forEach(seg => {
        if (!roadStats[seg.roadType]) roadStats[seg.roadType] = 0;
        roadStats[seg.roadType] += seg.distance;
    });

    html += '<div class="result-card">';
    html += '<div class="result-title">📊 Distribuție pe tipuri de drum</div>';
    html += '<div class="road-summary">';

    let maxDist = Math.max(...Object.values(roadStats));
    let types = ['A', 'E', 'DN', 'DJ'];
    types.forEach(t => {
        let dist = roadStats[t] || 0;
        if (dist > 0) {
            let pct = Math.round((dist / r.distance) * 100);
            let barW = Math.round((dist / maxDist) * 100);
            html += '<div class="road-bar-row">';
            html += '<div class="road-bar-label"><span class="road-tag road-' + t + '">' + t + '</span></div>';
            html += '<div class="road-bar-track"><div class="road-bar-fill road-' + t + '" style="width:' + barW + '%"></div></div>';
            html += '<div class="road-bar-value">' + dist + ' km (' + pct + '%)</div>';
            html += '</div>';
        }
    });

    html += '</div>';
    html += '</div>';

    // Detailed segments table
    html += buildSegmentsTable(r.segmente);

    // Speed legend
    html += '<div class="result-card">';
    html += '<div class="result-title">🚗 Viteze (medie estimată / max. legală)</div>';
    html += '<div class="speed-info">';
    html += '<span class="speed-item"><span class="road-tag road-A">A</span> 110 km/h <span class="speed-max">(max 130)</span></span>';
    html += '<span class="speed-item"><span class="road-tag road-E">E</span> 85 km/h <span class="speed-max">(max 100)</span></span>';
    html += '<span class="speed-item"><span class="road-tag road-DN">DN</span> 70 km/h <span class="speed-max">(max 90)</span></span>';
    html += '<span class="speed-item"><span class="road-tag road-DJ">DJ</span> 55 km/h <span class="speed-max">(max 70)</span></span>';
    html += '</div>';
    html += '</div>';

    document.getElementById('results').innerHTML = html;
}

// ===== OPTION 4: TRAVEL TIME =====
const transportSpeeds = {
    car:     { A: 110, E: 85,  DN: 70, DJ: 55 },
    bicycle: { A: 18,  E: 18,  DN: 18, DJ: 18 },
    walking: { A: 5,   E: 5,   DN: 5,  DJ: 5  }
};

const transportLabels = {
    car:     { icon: '🚗', label: 'Mașină'    },
    bicycle: { icon: '🚲', label: 'Bicicletă' },
    walking: { icon: '🚶', label: 'Pe jos'    }
};

function getSpeedForMode(roadType, mode) {
    return transportSpeeds[mode][roadType] || transportSpeeds[mode]['DN'];
}

function showOption4(data) {
    option4Data  = data.result;
    transportMode = 'car';
    drawRoute(option4Data, ACCENT_COLOR, true, true);
    renderOption4();
}

function switchTransportMode(mode) {
    transportMode = mode;
    renderOption4();
}

function renderOption4() {
    let r    = option4Data;
    let mode = transportMode;

    // Recalculate total time for current mode
    let totalTime = 0;
    r.segmente.forEach(seg => {
        totalTime += seg.distance / getSpeedForMode(seg.roadType, mode);
    });

    let html = '';

    // Transport mode selector
    html += '<div class="result-card">';
    html += '<div class="result-title">🚦 Mod de transport</div>';
    html += '<div class="paths-controls">';
    ['car', 'bicycle', 'walking'].forEach(m => {
        let active = mode === m ? ' active' : '';
        let lbl = transportLabels[m];
        html += '<button class="sort-btn' + active + '" onclick="switchTransportMode(\'' + m + '\')">' +
                lbl.icon + ' ' + lbl.label + '</button>';
    });
    html += '</div>';
    html += '</div>';

    // Big time display
    html += '<div class="result-card">';
    html += '<div class="result-title">⏱️ Timp total de parcurgere</div>';
    html += '<div class="time-display"><span class="time-big">' + formatTime(totalTime) + '</span></div>';
    html += '<div class="big-stat"><span class="value">' + r.distance + '</span><span class="unit">km</span>';
    html += '<div class="label">' + r.nodes.length + ' orașe parcurse</div></div>';
    html += buildPathHTML(r.nodes);
    html += '</div>';

    // Time per segment (recalculated for current mode)
    html += '<div class="result-card">';
    html += '<div class="result-title">🕐 Timp pe segmente</div>';
    html += '<table class="seg-table">';
    html += '<tr><th>Segment</th><th>Drum</th><th>Dist</th><th>Viteză</th><th>Timp</th></tr>';

    r.segmente.forEach(seg => {
        let speed = getSpeedForMode(seg.roadType, mode);
        let mins  = Math.round((seg.distance / speed) * 60);
        html += '<tr>';
        html += '<td>' + seg.from + ' → ' + seg.to + '</td>';
        html += '<td><span class="road-tag road-' + seg.roadType + '">' + seg.roadFull + '</span></td>';
        html += '<td>' + seg.distance + ' km</td>';
        html += '<td>' + speed + ' km/h</td>';
        html += '<td><strong>' + mins + ' min</strong></td>';
        html += '</tr>';
    });

    html += '</table>';
    html += '</div>';

    // Time breakdown by road type (recalculated)
    let timeByType = {};
    r.segmente.forEach(seg => {
        if (!timeByType[seg.roadType]) timeByType[seg.roadType] = 0;
        timeByType[seg.roadType] += seg.distance / getSpeedForMode(seg.roadType, mode);
    });

    html += '<div class="result-card">';
    html += '<div class="result-title">📊 Timp pe tip de drum</div>';
    html += '<div class="road-summary">';

    let maxTime = Math.max(...Object.values(timeByType));
    ['A', 'E', 'DN', 'DJ'].forEach(t => {
        let time = timeByType[t] || 0;
        if (time > 0) {
            let mins = Math.round(time * 60);
            let pct  = Math.round((time / totalTime) * 100);
            let barW = Math.round((time / maxTime) * 100);
            html += '<div class="road-bar-row">';
            html += '<div class="road-bar-label"><span class="road-tag road-' + t + '">' + t + '</span></div>';
            html += '<div class="road-bar-track"><div class="road-bar-fill road-' + t + '" style="width:' + barW + '%"></div></div>';
            html += '<div class="road-bar-value">' + mins + ' min (' + pct + '%)</div>';
            html += '</div>';
        }
    });

    html += '</div>';
    html += '</div>';

    // Speed info for current mode
    html += '<div class="result-card">';
    if (mode === 'car') {
        html += '<div class="result-title">🚗 Viteze (medie estimată / max. legală)</div>';
        html += '<div class="speed-info">';
        html += '<span class="speed-item"><span class="road-tag road-A">A</span> 110 km/h <span class="speed-max">(max 130)</span></span>';
        html += '<span class="speed-item"><span class="road-tag road-E">E</span> 85 km/h <span class="speed-max">(max 100)</span></span>';
        html += '<span class="speed-item"><span class="road-tag road-DN">DN</span> 70 km/h <span class="speed-max">(max 90)</span></span>';
        html += '<span class="speed-item"><span class="road-tag road-DJ">DJ</span> 55 km/h <span class="speed-max">(max 70)</span></span>';
    } else if (mode === 'bicycle') {
        html += '<div class="result-title">🚲 Viteză medie estimată</div>';
        html += '<div class="speed-info">';
        html += '<span class="speed-item">Viteză medie bicicletă: <strong>18 km/h</strong></span>';
    } else {
        html += '<div class="result-title">🚶 Viteză medie estimată</div>';
        html += '<div class="speed-info">';
        html += '<span class="speed-item">Viteză medie mers pe jos: <strong>5 km/h</strong></span>';
    }
    html += '</div>';
    html += '</div>';

    document.getElementById('results').innerHTML = html;
}

// ===== SEGMENTS TABLE BUILDER =====
function buildSegmentsTable(segments) {
    let html = '<div class="result-card">';
    html += '<div class="result-title">📋 Detalii segmente</div>';
    html += '<table class="seg-table">';
    html += '<tr><th>De la</th><th>Până la</th><th>Drum</th><th>Distanță</th></tr>';

    segments.forEach(seg => {
        html += '<tr>';
        html += '<td>' + seg.from + '</td>';
        html += '<td>' + seg.to + '</td>';
        html += '<td><span class="road-tag road-' + seg.roadType + '">' + seg.roadFull + '</span></td>';
        html += '<td>' + seg.distance + ' km</td>';
        html += '</tr>';
    });

    html += '</table>';
    html += '</div>';
    return html;
}

