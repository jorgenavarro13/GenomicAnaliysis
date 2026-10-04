// EXTERNAL LIBRARIES
#include "../libraries/httplib.h"
#include "../libraries/nlohmann/json.hpp"
#include <bits/stdc++.h>

// MY LIBRARIES
#include "./coincidence.h"

using json = nlohmann::json;
using namespace std;
using namespace httplib;

int main() {
    Server svr;

    svr.Options(R"(/(.*))", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "http://localhost:8000");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS, PUT, DELETE");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204; // No Content (Éxito estándar para preflights)
    });


    svr.Get("/", [](const Request&, Response& res) {
        res.set_content("Hello, World!", "text/plain");
    });

    
    svr.Get("/hi", [](const Request &req, Response &res) {
        res.set_content("Hello!", "text/plain");
    });
    
    svr.Post("/sequence", [](const Request& req, Response &res) {
         res.set_header("Access-Control-Allow-Origin", "*"); 
        json request_data = json::parse(req.body);
        cout << "Received request data: " << request_data.dump() << endl;
        string file = request_data["file"];
        cout << "File: " << file << endl;

        vector<char> sequence = readFile(file);

        // Parse the sequence
        json j = json::array();
        for (char base : sequence) {
            j.push_back(string(1, base));
        }

        json response_data;
        response_data["file"] = file;
        response_data["sequence"] = j;
        cout << response_data.dump() << endl;
        res.set_content(response_data.dump(), "application/json");
    });
    

    svr.listen("0.0.0.0", 8080);
}