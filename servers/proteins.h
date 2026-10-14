#ifndef PROTEINS_H
#define PROTEINS_H

#include <bits/stdc++.h>
using namespace std;

struct Protein {
    string name;
    vector<char> sequence;
};

struct ProteinMatch {
    string name;
    int length;
    bool found;
    char strand;                  
    int frame;                    
    int begin, end;               
    bool frameshift;
    int prefixLength, shiftBegin;
    string firstAminoacids;
    vector<string> firstCodons;
};

vector<Protein> readProteins(string filename);
vector<char> reverseComplement(vector<char> & dna);
vector<char> translateFrame(vector<char> & dna, int frame);
vector<int> findAllMatches(vector<char> & text, vector<char> & pattern);
vector<ProteinMatch> findProteins(vector<char> & genome, vector<Protein> & proteins);

#endif
