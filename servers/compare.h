#include <bits/stdc++.h>
using namespace std;

// A codon that changes between the two genomes, read in one reading frame
struct AminoacidDifference {
    int index;              // first base that differs inside the codon
    int codonStart;         // index where the codon starts
    string codonA, codonB;
    string aminoA, aminoB;
    bool changesAminoacid;  // false means a silent mutation
};

struct ComparisonResult {
    int comparedLength;                             // length of the shorter genome
    vector<int> differenceIndexes;                  // every base that differs
    array<vector<AminoacidDifference>, 3> frames;   // frames[f]: codons read starting at offset f
    int longestBegin, longestEnd, longestLength;    // longest run without differences
};

string getAminoAcid(const string& codon);
ComparisonResult compareGenomes(const vector<char>& A, const vector<char>& B);
pair<string, string> allignSequences(const vector<char>& A, const vector<char>& B);