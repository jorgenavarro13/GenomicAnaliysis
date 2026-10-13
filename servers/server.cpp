// EXTERNAL LIBRARIES
#include "../libraries/httplib.h"
#include "../libraries/nlohmann/json.hpp"
#include <bits/stdc++.h>

// MY LIBRARIES
#include "./coincidence.h"
#include "./palindrome.h"
#include "./compare.h"
#include "./proteins.h"

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
    

    svr.Post("/findGenInSequence", [](const Request& req, Response &res) {

        res.set_header("Access-Control-Allow-Origin", "*"); 
        json request_data = json::parse(req.body);
        cout << "Received request data: " << request_data.dump() << endl;
        string gene_file = request_data["gene_file"];
        cout << "Gene: " << gene_file << endl;
        string sequence_file = request_data["sequence_file"];
        cout<< "Sequence: " << sequence_file << endl;

        vector<char> sequence = readFile(sequence_file);
        vector<char> gene = readFile(gene_file);

        pair<int,int> result = findGeneInSequence(sequence, gene);

        cout << "Result: " << result.first << ", " << result.second << endl;
        
        json found = json::array();
        for (int i=result.first; i<=result.second; i++) {
            found.push_back(string(1, sequence[i]));
        }

        json response_data;
        response_data["begin"] = result.first;
        response_data["end"] = result.second;
        response_data["found"]= found;
        cout << response_data.dump() << endl;
        res.set_content(response_data.dump(), "application/json");
    });


    svr.Post("/findPalindrome", [](const Request& req, Response &res) {
        res.set_header("Access-Control-Allow-Origin", "*"); 
        json request_data = json::parse(req.body);
        cout << "Received request data: " << request_data.dump() << endl;
        string sequence_file = request_data["sequence_file"];
        cout<< "Sequence: " << sequence_file << endl;   
        string sequence_name = request_data["sequence_name"];
        cout << "Sequence Name: " << sequence_name << endl;

        vector<char> sequence = readFile(sequence_file);

        pair<int,int> result = genPalindrome(sequence);
        
        string filename =  "../data/palindrome/" + sequence_name + ".txt";
        cout<< "Saving palindrome to file: " << filename << endl;
        saveToFile(filename, sequence, result);

        cout << "Result: " << result.first << ", " << result.second << endl;

        json palindrome = json::array();
        for (int i=result.first; i<=result.second; i++) {
            palindrome.push_back(string(1, sequence[i]));
        }
        // Parse the sequence
        json response_data;
        response_data["begin"] = result.first;
        response_data["end"] = result.second;
        response_data["palindrome"] = palindrome;
        cout << response_data.dump() << endl;
        res.set_content(response_data.dump(), "application/json");
    });


    svr.Post("/compareGenomes", [](const Request& req, Response &res) {
        res.set_header("Access-Control-Allow-Origin", "*");

        json request_data = json::parse(req.body);
        cout << "Received request data: " << request_data.dump() << endl;
        string sequence_file_a = request_data["sequence_file_a"];
        string sequence_file_b = request_data["sequence_file_b"];

        vector<char> sequence_a = readFile(sequence_file_a);
        vector<char> sequence_b = readFile(sequence_file_b);

        if (sequence_a.empty() || sequence_b.empty()) {
            res.status = 404; res.set_content(R"({"error": "Sequence file not found or empty"})", "application/json"); return;
        }

        ComparisonResult result = compareGenomes(sequence_a, sequence_b);
        cout << "Differences: " << result.differenceIndexes.size() << endl;

        json frames = json::array();
        for (auto & frame : result.frames) {
            json differences = json::array();
            for (auto & d : frame) {
                differences.push_back({
                    {"index", d.index},
                    {"codonStart", d.codonStart},
                    {"codonA", d.codonA},
                    {"codonB", d.codonB},
                    {"aminoA", d.aminoA},
                    {"aminoB", d.aminoB},
                    {"changesAminoacid", d.changesAminoacid}
                });
            }
            frames.push_back(differences);
        }

        json response_data;
        response_data["sequence_file_a"] = sequence_file_a;
        response_data["sequence_file_b"] = sequence_file_b;
        response_data["comparedLength"] = result.comparedLength;
        response_data["differences"] = result.differenceIndexes;
        response_data["frames"] = frames;
        response_data["longest"] = {
            {"begin", result.longestBegin},
            {"end", result.longestEnd},
            {"length", result.longestLength}
        };
        res.set_content(response_data.dump(), "application/json");
    });


    svr.Post("/findProteins", [](const Request& req, Response &res) {
        res.set_header("Access-Control-Allow-Origin", "*");

        // Without exceptions: an invalid body gives a discarded json, and we answer 400 instead of failing
        json request_data = json::parse(req.body, nullptr, false);
        if (request_data.is_discarded() ||
            !request_data.contains("sequence_file") || !request_data["sequence_file"].is_string() ||
            !request_data.contains("proteins_file") || !request_data["proteins_file"].is_string()) {
            res.status = 400;
            res.set_content(R"({"error": "Expected sequence_file and proteins_file"})", "application/json");
            return;
        }
        cout << "Received request data: " << request_data.dump() << endl;
        string sequence_file = request_data["sequence_file"];
        string proteins_file = request_data["proteins_file"];

        vector<char> genome = readFile(sequence_file);
        vector<Protein> proteins = readProteins(proteins_file);

        // Both readers return an empty vector when the file doesn't exist
        if (genome.empty() || proteins.empty()) {
            res.status = 404;
            res.set_content(R"({"error": "Sequence or proteins file not found or empty"})", "application/json");
            return;
        }

        vector<ProteinMatch> results = findProteins(genome, proteins);

        int n = genome.size(), found = 0, frameshifts = 0;
        json matches = json::array();
        for (ProteinMatch & m : results) {
            found += m.found;
            frameshifts += m.frameshift;

            // Frame of the second segment (counted on its own strand like m.frame) and the slip:
            // the second segment starts 3p bases after the first one, moved -1 (a base read twice) or +1 (a base skipped)
            int shiftFrame = 0, slip = 0;
            if (m.frameshift) {
                if (m.strand == '+') {
                    shiftFrame = m.shiftBegin % 3 + 1;
                    slip = m.shiftBegin - (m.begin + 3 * m.prefixLength);
                } else {
                    // On the reverse strand the genome indexes are mirrored with n - 1 - i
                    shiftFrame = (n - 1 - m.shiftBegin) % 3 + 1;
                    slip = (m.end - m.shiftBegin) - 3 * m.prefixLength;
                }
            }

            matches.push_back({
                {"name", m.name},
                {"length", m.length},
                {"found", m.found},
                {"strand", string(1, m.strand)},
                {"frame", m.frame},
                {"begin", m.begin},
                {"end", m.end},
                {"frameshift", m.frameshift},
                {"prefixLength", m.prefixLength},
                {"shiftBegin", m.shiftBegin},
                {"shiftFrame", shiftFrame},
                {"slip", slip},
                {"firstAminoacids", m.firstAminoacids},
                {"firstCodons", m.firstCodons}
            });
        }
        cout << "Proteins found: " << found << " / " << results.size() << endl;

        json response_data;
        response_data["sequence_file"] = sequence_file;
        response_data["proteins_file"] = proteins_file;
        response_data["genomeLength"] = n;
        response_data["total"] = results.size();
        response_data["found"] = found;
        response_data["frameshifts"] = frameshifts;
        response_data["proteins"] = matches;
        res.set_content(response_data.dump(), "application/json");
    });

    svr.listen("0.0.0.0", 8080);
}