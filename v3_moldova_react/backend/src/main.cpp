#include "httplib.h"
#include "graph.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>

using namespace httplib;

Graf graf;

std::string traseuToJSON(const Traseu& t, const Graf& g) {
    std::ostringstream json;
    json << "{";

    json << "\"cities\":[";
    for (size_t i = 0; i < t.orase.size(); i++) {
        const Oras& oras = g.getOras(t.orase[i]);
        json << "{";
        json << "\"id\":" << t.orase[i] << ",";
        json << "\"name\":\"" << oras.nume << "\",";
        json << "\"lat\":" << std::fixed << std::setprecision(4) << oras.latitudine << ",";
        json << "\"lng\":" << oras.longitudine;
        json << "}";
        if (i < t.orase.size() - 1) json << ",";
    }
    json << "],";

    json << "\"segments\":[";
    for (size_t i = 0; i < t.orase.size() - 1; i++) {
        int o1 = t.orase[i];
        int o2 = t.orase[i + 1];
        json << "{";
        json << "\"from\":\"" << g.getOras(o1).nume << "\",";
        json << "\"to\":\"" << g.getOras(o2).nume << "\",";
        json << "\"distance\":" << g.getDistanta(o1, o2) << ",";
        json << "\"roadType\":\"" << g.getTipDrum(o1, o2) << "\",";
        json << "\"speedLimit\":" << g.getVitezaMax(o1, o2);
        json << "}";
        if (i < t.orase.size() - 2) json << ",";
    }
    json << "],";

    json << "\"totalDistance\":" << std::fixed << std::setprecision(1) << t.distanta << ",";
    json << "\"totalTime\":" << std::setprecision(2) << t.timp;
    json << "}";

    return json.str();
}

int main() {
    std::cout << "🚀 Initializing Moldova Navigator Backend..." << std::endl;
    graf.initializeazaDate();
    std::cout << "✓ Graph initialized with " << graf.getNrOrase() << " cities" << std::endl;

    Server svr;

    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    // CORS preflight
    svr.Options("/api/cities", [](const Request&, Response& res) { res.status = 204; });
    svr.Options("/api/calculate", [](const Request&, Response& res) { res.status = 204; });
    svr.Options("/api/health", [](const Request&, Response& res) { res.status = 204; });

    svr.Get("/api/cities", [](const Request& req, Response& res) {
        std::ostringstream json;
        json << "[";
        for (int i = 0; i < graf.getNrOrase(); i++) {
            const Oras& oras = graf.getOras(i);
            json << "{";
            json << "\"id\":" << i << ",";
            json << "\"name\":\"" << oras.nume << "\",";
            json << "\"lat\":" << std::fixed << std::setprecision(4) << oras.latitudine << ",";
            json << "\"lng\":" << oras.longitudine;
            json << "}";
            if (i < graf.getNrOrase() - 1) json << ",";
        }
        json << "]";

        res.set_content(json.str(), "application/json");
    });

    svr.Post("/api/calculate", [](const Request& req, Response& res) {
        std::string body = req.body;

        size_t sourcePos = body.find("\"source\":");
        size_t destPos = body.find("\"destination\":");

        if (sourcePos == std::string::npos || destPos == std::string::npos) {
            res.status = 400;
            res.set_content("{\"error\":\"Invalid request\"}", "application/json");
            return;
        }

        size_t srcVal = body.find_first_of("0123456789", sourcePos + 9);
        size_t dstVal = body.find_first_of("0123456789", destPos + 14);
        if (srcVal == std::string::npos || dstVal == std::string::npos) {
            res.status = 400;
            res.set_content("{\"error\":\"Invalid request\"}", "application/json");
            return;
        }
        int source = std::stoi(body.substr(srcVal));
        int destination = std::stoi(body.substr(dstVal));

        if (source < 0 || source >= graf.getNrOrase() || destination < 0 || destination >= graf.getNrOrase()) {
            res.status = 400;
            res.set_content("{\"error\":\"Invalid city id\"}", "application/json");
            return;
        }
        if (source == destination) {
            std::ostringstream json;
            json << "{\"success\":true,\"routesCount\":0,\"calculationTime\":0,\"routes\":[]}";
            res.set_content(json.str(), "application/json");
            return;
        }

        std::cout << "📍 Calculating routes: " << source << " → " << destination << std::endl;

        auto start = std::chrono::high_resolution_clock::now();
        std::vector<Traseu> trasee = graf.calculeazaTrasee(source, destination);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "✓ Found " << trasee.size() << " routes in " << duration.count() << "ms" << std::endl;

        std::ostringstream json;
        json << "{";
        json << "\"success\":true,";
        json << "\"routesCount\":" << trasee.size() << ",";
        json << "\"calculationTime\":" << duration.count() << ",";
        json << "\"routes\":[";

        int limit = std::min((int)trasee.size(), 50);
        for (int i = 0; i < limit; i++) {
            json << traseuToJSON(trasee[i], graf);
            if (i < limit - 1) json << ",";
        }

        json << "]}";

        res.set_content(json.str(), "application/json");
    });

    svr.Get("/api/health", [](const Request& req, Response& res) {
        res.set_content("{\"status\":\"ok\"}", "application/json");
    });

    std::cout << "\n🌐 Server running on http://localhost:8080" << std::endl;
    std::cout << "📡 Endpoints:" << std::endl;
    std::cout << "   GET  /api/cities     - Get all cities" << std::endl;
    std::cout << "   POST /api/calculate  - Calculate routes" << std::endl;
    std::cout << "   GET  /api/health     - Health check" << std::endl;
    std::cout << "\n⏹  Press Ctrl+C to stop\n" << std::endl;

    svr.listen("0.0.0.0", 8080);

    return 0;
}
