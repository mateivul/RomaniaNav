#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <thread>
#include <mutex>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef int socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

using namespace std;

// ===== CONSTANTS =====
const int MAX_CITIES = 50;
const int MAX_ADJ    = 15;
const int MAX_PATHS  = 5000;
const int MAX_X      = MAX_CITIES + 1; // x is 1-indexed

// ===== DATA STRUCTURES =====
struct Road {
    int to;
    int distance;
    string roadType;
    string roadFull;
};

struct City {
    string name;
    double lat;
    double lon;
};

struct Path {
    int nodes[MAX_CITIES];
    int nodeCount;
    int totalDist;
};

int N, M;
City cities[MAX_CITIES];
Road adj[MAX_CITIES][MAX_ADJ];
int adjCount[MAX_CITIES];

// Backtracking state (protected by bktMutex)
std::mutex bktMutex;
Path allPaths[MAX_PATHS];
int  pathCount;
int  x[MAX_X];   // x[k] = orasul de pe pozitia k in traseul curent (1-indexed)
int  bktDest;    // destinatia cautata

// Last-query cache (under bktMutex)
int  cacheStart = -1;
int  cacheEnd   = -1;

// ===== LOAD DATA =====
void loadData(const string& filename) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "EROARE: Nu pot deschide " << filename << endl;
        exit(1);
    }

    fin >> N;

    for (int i = 0; i < N; i++) {
        fin >> cities[i].name >> cities[i].lat >> cities[i].lon;
        for (char& c : cities[i].name)
            if (c == '_') c = ' ';
    }

    fin >> M;
    memset(adjCount, 0, sizeof(adjCount));

    for (int i = 0; i < M; i++) {
        int a, b, d;
        string road;
        fin >> a >> b >> d >> road;

        string roadType;
        if (road[0] == 'A') roadType = "A";
        else if (road[0] == 'E') roadType = "E";
        else if (road[0] == 'D' && road[1] == 'N') roadType = "DN";
        else if (road[0] == 'D' && road[1] == 'J') roadType = "DJ";
        else roadType = "DN";

        adj[a][adjCount[a]++] = {b, d, roadType, road};
        adj[b][adjCount[b]++] = {a, d, roadType, road};
    }

    fin.close();
    cout << "Date incarcate: " << N << " orase, " << M << " drumuri" << endl;
}

int findCity(const string& name) {
    for (int i = 0; i < N; i++)
        if (cities[i].name == name) return i;
    return -1;
}

// ===== ROAD INFO =====
Road* findRoad(int from, int to)
{
    for (int i = 0; i < adjCount[from]; i++)
        if (adj[from][i].to == to) return &adj[from][i];
    return nullptr;
}

int getSpeed(const string& roadType)
{
    if (roadType == "A")  return 130;
    if (roadType == "E")  return 100;
    if (roadType == "DN") return 90;
    if (roadType == "DJ") return 70;
    return 90;
}

// ===== BACKTRACKING =====
void INIT(int k)
{
    x[k] = -1;
}

int EXISTA(int k)
{
    if (x[k] < N - 1)
        return 1;
    return 0;
}

int VALID(int k)
{
    // niciun oras repetat
    for (int i = 1; i < k; i++)
        if (x[i] == x[k])
            return 0;

    // exista drum de la x[k-1] la x[k]
    for (int i = 0; i < adjCount[x[k-1]]; i++)
        if (adj[x[k-1]][i].to == x[k])
            return 1;

    return 0;
}

int SOLUTIE(int k)
{
    if (x[k] == bktDest)
        return 1;
    return 0;
}

void OPTIM(int k)
{
    if (pathCount >= MAX_PATHS) return;

    for (int i = 1; i <= k; i++)
        allPaths[pathCount].nodes[i - 1] = x[i];
    allPaths[pathCount].nodeCount = k;

    int d = 0;
    for (int i = 2; i <= k; i++) {
        Road* r = findRoad(x[i-1], x[i]);
        if (r) d += r->distance;
    }
    allPaths[pathCount].totalDist = d;
    pathCount++;
}

void BKT()
{
    int k = 2;
    INIT(k);

    while (k > 1 && pathCount < MAX_PATHS)
        if (EXISTA(k))
        {
            x[k]++;
            if (VALID(k))
            {
                if (SOLUTIE(k))
                    OPTIM(k);
                else
                {
                    k++;
                    INIT(k);
                }
            }
        }
        else
            k--;
}

void findAllPaths(int start, int dest)
{
    pathCount = 0;
    bktDest   = dest;
    x[1]      = start;
    BKT();
}

// ===== JSON BUILDERS =====
string escapeJSON(const string& s) {
    string result;
    for (char c : s) {
        if (c == '"')  result += "\\\"";
        else if (c == '\\') result += "\\\\";
        else result += c;
    }
    return result;
}

string buildPathJSON(const Path& p) {
    stringstream ss;
    ss << "{\"nodes\":[";
    for (int i = 0; i < p.nodeCount; i++) {
        if (i > 0) ss << ",";
        ss << "\"" << escapeJSON(cities[p.nodes[i]].name) << "\"";
    }
    ss << "],\"distance\":" << p.totalDist;

    ss << ",\"segmente\":[";
    double totalTime = 0;
    for (int i = 0; i < p.nodeCount - 1; i++) {
        Road* r = findRoad(p.nodes[i], p.nodes[i + 1]);
        if (i > 0) ss << ",";
        int speed = getSpeed(r->roadType);
        double segTime = (double)r->distance / speed;
        totalTime += segTime;
        ss << "{\"from\":\"" << escapeJSON(cities[p.nodes[i]].name) << "\""
           << ",\"to\":\"" << escapeJSON(cities[p.nodes[i + 1]].name) << "\""
           << ",\"distance\":" << r->distance
           << ",\"roadType\":\"" << r->roadType << "\""
           << ",\"roadFull\":\"" << r->roadFull << "\""
           << ",\"speed\":" << speed
           << ",\"time\":" << segTime << "}";
    }
    ss << "]";
    ss << ",\"totalTime\":" << totalTime;
    ss << ",\"nodeIndices\":[";
    for (int i = 0; i < p.nodeCount; i++) {
        if (i > 0) ss << ",";
        ss << p.nodes[i];
    }
    ss << "]}";
    return ss.str();
}

string handleAPI(const string& startName, const string& endName, int option) {
    int s = findCity(startName);
    int e = findCity(endName);

    if (s == -1 || e == -1)
        return "{\"error\":\"Oras negasit\"}";
    if (s == e)
        return "{\"error\":\"Selectati orase diferite\"}";

    std::string result;
    {
        std::lock_guard<std::mutex> lock(bktMutex);

        if (s != cacheStart || e != cacheEnd) {
            findAllPaths(s, e);
            if (pathCount == 0)
                return "{\"error\":\"Nu exista traseu intre cele doua orase\"}";
            sort(allPaths, allPaths + pathCount, [](const Path& a, const Path& b) {
                return a.totalDist < b.totalDist;
            });
            cacheStart = s;
            cacheEnd   = e;
        }

        if (pathCount == 0)
            return "{\"error\":\"Nu exista traseu intre cele doua orase\"}";

        stringstream ss;

        if (option == 1) {
            ss << "{\"option\":1,\"result\":" << buildPathJSON(allPaths[0]) << "}";
        }
        else if (option == 2) {
            ss << "{\"option\":2,\"results\":[";
            for (int i = 0; i < pathCount; i++) {
                if (i > 0) ss << ",";
                ss << buildPathJSON(allPaths[i]);
            }
            ss << "]}";
        }
        else if (option == 3) {
            ss << "{\"option\":3,\"result\":" << buildPathJSON(allPaths[0]) << "}";
        }
        else if (option == 4) {
            ss << "{\"option\":4,\"result\":" << buildPathJSON(allPaths[0]) << "}";
        }
        else {
            ss << "{\"error\":\"Optiune invalida\"}";
        }

        result = ss.str();
    }
    return result;
}

// ===== HTTP SERVER =====
string readFile(const string& path) {
    ifstream file(path, ios::binary);
    if (!file.is_open()) return "";
    stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

string getContentType(const string& path) {
    if (path.find(".html") != string::npos) return "text/html";
    if (path.find(".css")  != string::npos) return "text/css";
    if (path.find(".js")   != string::npos) return "application/javascript";
    if (path.find(".json") != string::npos) return "application/json";
    return "text/plain";
}

string urlDecode(const string& str) {
    string result;
    for (int i = 0; i < (int)str.size(); i++) {
        if (str[i] == '%' && i + 2 < (int)str.size()) {
            int hex;
            sscanf(str.substr(i + 1, 2).c_str(), "%x", &hex);
            result += (char)hex;
            i += 2;
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    return result;
}

string buildCitiesJSON() {
    stringstream ss;
    ss << "{\"cities\":[";
    for (int i = 0; i < N; i++) {
        if (i > 0) ss << ",";
        ss << "{\"name\":\"" << escapeJSON(cities[i].name) << "\""
           << ",\"lat\":" << cities[i].lat
           << ",\"lon\":" << cities[i].lon
           << ",\"index\":" << i << "}";
    }
    ss << "],\"edges\":[";
    int edgeCount = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < adjCount[i]; j++) {
            Road& r = adj[i][j];
            if (r.to > i) {
                if (edgeCount > 0) ss << ",";
                ss << "{\"from\":" << i << ",\"to\":" << r.to
                   << ",\"dist\":" << r.distance
                   << ",\"road\":\"" << r.roadFull << "\""
                   << ",\"type\":\"" << r.roadType << "\"}";
                edgeCount++;
            }
        }
    }
    ss << "]}";
    return ss.str();
}

void handleClient(SOCKET clientSocket) {
    char buffer[4096];
    int bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) { closesocket(clientSocket); return; }
    buffer[bytes] = '\0';

    string request(buffer);
    string method, path;
    istringstream reqStream(request);
    reqStream >> method >> path;

    string response;
    string body;
    string contentType = "text/html";

    if (path == "/api/cities") {
        body = buildCitiesJSON();
        contentType = "application/json";
    }
    else if (path.find("/api/route") == 0) {
        string startName, endName;
        int option = 1;

        size_t qpos = path.find('?');
        if (qpos != string::npos) {
            string query = path.substr(qpos + 1);
            istringstream qs(query);
            string param;
            while (getline(qs, param, '&')) {
                size_t eq = param.find('=');
                if (eq != string::npos) {
                    string key = param.substr(0, eq);
                    string val = urlDecode(param.substr(eq + 1));
                    if (key == "start") startName = val;
                    else if (key == "end") endName = val;
                    else if (key == "option") option = stoi(val);
                }
            }
        }

        body = handleAPI(startName, endName, option);
        contentType = "application/json";
    }
    else {
        // Whitelist static files only — prevents path traversal
        string filePath;
        if (path == "/" || path == "/index.html") filePath = "index.html";
        else if (path == "/style.css")            filePath = "style.css";
        else if (path == "/script.js")            filePath = "script.js";
        else {
            response = "HTTP/1.1 404 Not Found\r\nContent-Length: 9\r\n\r\nNot Found";
            send(clientSocket, response.c_str(), response.size(), 0);
            closesocket(clientSocket);
            return;
        }

        body = readFile(filePath);
        if (body.empty()) {
            response = "HTTP/1.1 404 Not Found\r\nContent-Length: 9\r\n\r\nNot Found";
            send(clientSocket, response.c_str(), response.size(), 0);
            closesocket(clientSocket);
            return;
        }
        contentType = getContentType(filePath);
    }

    response  = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: " + contentType + "; charset=utf-8\r\n";
    response += "Content-Length: " + to_string(body.size()) + "\r\n";
    response += "Access-Control-Allow-Origin: *\r\n";
    response += "\r\n";
    response += body;

    send(clientSocket, response.c_str(), response.size(), 0);
    closesocket(clientSocket);
}

int main() {
    loadData("data.txt");

#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Eroare creare socket" << endl;
        return 1;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    if (::bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Eroare bind" << endl;
        closesocket(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 10) == SOCKET_ERROR) {
        cerr << "Eroare listen" << endl;
        closesocket(serverSocket);
        return 1;
    }

    cout << "=== Google Maps Moldova ===" << endl;
    cout << "Server pornit pe http://localhost:8080" << endl;
    cout << "Ctrl+C pentru oprire" << endl;

    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket != INVALID_SOCKET) {
            std::thread([clientSocket]() {
                handleClient(clientSocket);
            }).detach();
        }
    }

    closesocket(serverSocket);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
