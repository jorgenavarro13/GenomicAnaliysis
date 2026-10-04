#include <bits/stdc++.h>
#include "coincidence.h"
using namespace std;

namespace {
    const unordered_set<char> set_a = {'A', 'C', 'G', 'T'};
}

// Function to read a file and return its content as a vector of characters
vector<char> readFile (string filename){
    ifstream file(filename);
    vector<char> sequence;

    if(!file.is_open()) {
        return {};
    }
    string line;
    size_t i_l = 0;

    while (getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
                line.pop_back();
        }
        if(i_l > 0){
            for(char c : line){
                if(set_a.find(c)!=set_a.end()){
                    sequence.push_back(c);      
                }
            }
        }
        i_l++;
    }
    return sequence;
}    