#include <bits/stdc++.h>
#include "proteins.h"
#include "coincidence.h"
using namespace std;

namespace {
    // Standard genetic code. Index = 16*b1 + 4*b2 + b3, with T=0, C=1, A=2, G=3. '*' is a stop codon
    const char CODON_TABLE[] =
        "FFLLSSSSYY**CC*W"   // T--
        "LLLLPPPPHHQQRRRR"   // C--
        "IIIMTTTTNNKKSSRR"   // A--
        "VVVVAAAADDEEGGGG";  // G--

    int baseIndex(char nucleotide){
        switch (nucleotide) {
            case 'T': return 0;
            case 'C': return 1;
            case 'A': return 2;
            case 'G': return 3;
            default:  return -1;
        }
    }

    // Translates the codon that starts at position i, 'X' if any base is unknown
    char translateCodon(vector<char> & dna, int i){
        int first = baseIndex(dna[i]), second = baseIndex(dna[i+1]), third = baseIndex(dna[i+2]);
        if(first < 0 || second < 0 || third < 0) return 'X';
        return CODON_TABLE[16*first + 4*second + third];
    }

    // Complement of a nucleotide, 'N' if unknown (it translates to 'X' later)
    char complement(char nucleotide){
        switch (nucleotide) {
            case 'A': return 'T';
            case 'T': return 'A';
            case 'C': return 'G';
            case 'G': return 'C';
            default:  return 'N';
        }
    }

    /*
        Converts a protein found in a translated frame to indexes of the original genome.
        strand '+': frame is the offset in the genome, the protein starts at aminoacidIndex.
        strand '-': we found it in the reverse complement, so we map positions back with N-1-i
        and swap the ends, to keep begin < end.
    */
    pair<int,int> aminoacidsToGenome(char strand, int genomeSize, int frame, int aminoacidIndex, int length){
        int start = frame + 3 * aminoacidIndex;
        int finish = start + 3 * length - 1;

        if(strand == '+') return {start, finish};
        return {genomeSize - 1 - finish, genomeSize - 1 - start};
    }

    const int ANCHOR_LENGTH = 20;         // aminoacids used as anchor to find the second segment
    const int CUT_WINDOW = 5;             // how many cut points we try after the longest extension
    const int MIN_FRAMESHIFT_LENGTH = 100; // shorter proteins are not searched with frameshift

    // Checks that the codons that start at dna[start] produce the first "length" aminoacids
    bool prefixMatchesAt(vector<char> & dna, int start, vector<char> & sequence, int length){
        if(start < 0 || start + 3 * length > (int)dna.size()) return false;
        for(int k = 0; k < length; k++){
            if(translateCodon(dna, start + 3 * k) != sequence[k]) return false;
        }
        return true;
    }
}

/*
    Translates a DNA sequence to aminoacids reading codons of 3 bases. frame is the offset
    where we start reading (0, 1 or 2), so frame 1 is the reading frame +2.

    Time complexity: O(n)
    We visit each codon once, and translating a codon takes O(1) with the table.

    Space complexity: O(n)
    The result has about n/3 aminoacids.
*/
vector<char> translateFrame(vector<char> & dna, int frame){
    vector<char> aminoacids;
    int n = dna.size();
    aminoacids.reserve(n/3 + 1);

    for(int i = frame; i + 2 < n; i += 3){
        aminoacids.push_back(translateCodon(dna, i));
    }
    return aminoacids;
}

/*
    Builds the reverse complement of a DNA sequence: we read it from the end to the beginning
    and complement each base. This is the other strand of the DNA read in its own direction.

    Time complexity: O(n)
    One pass over the sequence.

    Space complexity: O(n)
    We store the new sequence.
*/
vector<char> reverseComplement(vector<char> & dna){
    vector<char> result;
    int n = dna.size();
    result.reserve(n);

    for(int i = n - 1; i >= 0; i--){
        result.push_back(complement(dna[i]));
    }
    return result;
}

/*
    KMP search that returns every position where the pattern appears in the text. The lps array
    (from coincidence.cpp) tells us how much of the pattern we can keep after a mismatch, so
    we never move back in the text.

    Time complexity: O(n + m)
    Building lps takes O(m) and the search goes through the text only once, O(n).

    Space complexity: O(m)
    We only store the lps array (the list of matches is the output).
*/
vector<int> findAllMatches(vector<char> & text, vector<char> & pattern){
    vector<int> matches;
    int n = text.size(), m = pattern.size();
    if(m == 0 || m > n) return matches;

    vector<int> LPS = lps(pattern);
    int i = 0, j = 0;

    while(i < n){
        if(text[i] == pattern[j]){
            i++;
            j++;
            if(j == m){
                matches.push_back(i - j);
                j = LPS[j - 1];      // keep looking, matches can overlap
            }
        }
        else if(j > 0){
            j = LPS[j - 1];
        }
        else{
            i++;
        }
    }
    return matches;
}

namespace {
    // Result of looking for a protein that shifts frame in the middle
    struct FrameshiftMatch {
        bool found;
        int translation;     // 0..2 direct frames, 3..5 reverse frames (same order as translations[])
        int firstStart;      // index in the strand where the first segment starts
        int secondStart;     // index in the strand where the second segment starts
        int prefixLength;    // aminoacids of the first segment
    };

    /*
        Looks for a protein that cannot be found in a single frame because the ribosome shifts
        frame in the middle. We search the last ANCHOR_LENGTH aminoacids in the 6 frames,
        extend that match backwards while it still agrees with the protein, and then check that
        the missing beginning (the prefix) is produced by the codons right before,
        shifted one base (slip -1: the ribosome goes back one base, slip +1: it skips one).
        It only works inside the same strand.

        Time complexity: O(N + L)
        One KMP search of the anchor in each frame, the extension takes at most L steps, and we
        verify a constant number of cut points (each one costs at most L).

        Space complexity: O(L)
        We only copy the anchor.
    */
    FrameshiftMatch findFrameshift(vector<char> translations[], vector<char> & genome, vector<char> & reverseStrand, vector<char> & sequence){
        FrameshiftMatch result;
        result.found = false;
        result.translation = -1;
        result.firstStart = result.secondStart = -1;
        result.prefixLength = 0;

        int L = sequence.size();
        vector<char> anchor(sequence.end() - ANCHOR_LENGTH, sequence.end());

        for(int t = 0; t < 6; t++){
            vector<char> & dna = (t < 3) ? genome : reverseStrand;
            int frame = t % 3;
            vector<int> hits = findAllMatches(translations[t], anchor);

            for(int h : hits){
                // Extend the anchor backwards while the aminoacids still match
                int extension = 0;
                while(extension < L - ANCHOR_LENGTH && h - extension - 1 >= 0 &&
                      translations[t][h - extension - 1] == sequence[L - ANCHOR_LENGTH - extension - 1]){
                    extension++;
                }
                int shortestPrefix = L - ANCHOR_LENGTH - extension;

                // Try the cut points: the second segment starts at aminoacid p of the protein
                for(int p = max(1, shortestPrefix); p <= shortestPrefix + CUT_WINDOW && p <= L - ANCHOR_LENGTH; p++){
                    int secondStart = frame + 3 * (h - (L - ANCHOR_LENGTH - p));

                    for(int slip : {-1, 1}){
                        int firstStart = secondStart - 3 * p - slip;
                        if(prefixMatchesAt(dna, firstStart, sequence, p)){
                            result.found = true;
                            result.translation = t;
                            result.firstStart = firstStart;
                            result.secondStart = secondStart;
                            result.prefixLength = p;
                            return result;
                        }
                    }
                }
            }
        }
        return result;
    }
}

/*
    Looks for every protein in the 6 reading frames of the genome: +1, +2, +3 on the direct
    strand and -1, -2, -3 on the reverse complement. We translate each frame once and reuse
    it for all the proteins. The first frame that contains the protein is reported (order:
    +1, +2, +3, -1, -2, -3). Proteins that cross a frameshift are searched with findFrameshift
    when they are not found in a single frame.

    Time complexity: O(N + P * N + S)
    Translating 6 frames is O(N). For each of the P proteins we run KMP on 6 texts of N/3
    aminoacids, which is O(N + m) per protein. S is the sum of the protein lengths.

    Space complexity: O(N)
    The reverse complement and the 6 translations are about 2N characters in total.
*/
vector<ProteinMatch> findProteins(vector<char> & genome, vector<Protein> & proteins){
    int n = genome.size();
    vector<char> reverseStrand = reverseComplement(genome);

    // translations[0..2] are the frames +1..+3, translations[3..5] are -1..-3
    vector<char> translations[6];
    for(int f = 0; f < 3; f++){
        translations[f] = translateFrame(genome, f);
        translations[3 + f] = translateFrame(reverseStrand, f);
    }

    vector<ProteinMatch> results;
    for(Protein & protein : proteins){
        ProteinMatch match;
        match.name = protein.name;
        match.length = protein.sequence.size();
        match.found = false;
        match.strand = '?';
        match.frame = 0;
        match.begin = match.end = -1;
        match.frameshift = false;
        match.prefixLength = 0;
        match.shiftBegin = -1;

        for(int t = 0; t < 6 && !match.found; t++){
            vector<int> hits = findAllMatches(translations[t], protein.sequence);
            if(hits.empty()) continue;

            char strand = (t < 3) ? '+' : '-';
            int frame = t % 3;
            int aminoacidIndex = hits[0];

            pair<int,int> range = aminoacidsToGenome(strand, n, frame, aminoacidIndex, match.length);
            match.found = true;
            match.strand = strand;
            match.frame = frame + 1;
            match.begin = range.first;
            match.end = range.second;

            // The codons are shown as they are read on their strand
            vector<char> & dna = (strand == '+') ? genome : reverseStrand;
            int firstCodon = frame + 3 * aminoacidIndex;
            for(int k = 0; k < 4 && k < match.length; k++){
                int from = firstCodon + 3 * k;
                match.firstAminoacids.push_back(protein.sequence[k]);
                match.firstCodons.push_back(string(dna.begin() + from, dna.begin() + from + 3));
            }
        }

        // Not in a single frame: maybe the ribosome shifts frame in the middle
        if(!match.found && match.length > MIN_FRAMESHIFT_LENGTH){
            FrameshiftMatch shift = findFrameshift(translations, genome, reverseStrand, protein.sequence);
            if(shift.found){
                char strand = (shift.translation < 3) ? '+' : '-';
                vector<char> & dna = (strand == '+') ? genome : reverseStrand;
                int first = shift.firstStart, second = shift.secondStart, p = shift.prefixLength;
                int last = second + 3 * (match.length - p) - 1;

                match.found = true;
                match.strand = strand;
                match.frame = first % 3 + 1;
                match.frameshift = true;
                match.prefixLength = p;
                if(strand == '+'){
                    match.begin = first;
                    match.end = last;
                    match.shiftBegin = second;
                }
                else{
                    match.begin = n - 1 - last;
                    match.end = n - 1 - first;
                    match.shiftBegin = n - 1 - second;   // genome index of the first base read of the second segment
                }

                for(int k = 0; k < 4 && k < match.length; k++){
                    int from = (k < p) ? first + 3 * k : second + 3 * (k - p);
                    match.firstAminoacids.push_back(protein.sequence[k]);
                    match.firstCodons.push_back(string(dna.begin() + from, dna.begin() + from + 3));
                }
            }
        }

        results.push_back(match);
    }
    return results;
}

/*
    Reads a FASTA file with several proteins. A line starting with '>' opens a new protein
    (its name is the first word of the header); the following lines are appended to its
    sequence, so a sequence can be split in several lines.

    Time complexity: O(n)
    We read every character of the file once.

    Space complexity: O(n)
    We store the sequences read.
*/
vector<Protein> readProteins(string filename){
    vector<Protein> proteins;
    ifstream file(filename);

    if(!file.is_open()){
        cerr << "Could not open " << filename << endl;
        return proteins;
    }

    string line;
    while(getline(file, line)){
        if(!line.empty() && line.back() == '\r'){
            line.pop_back();
        }
        if(line.empty()) continue;

        if(line[0] == '>'){
            Protein protein;
            for(size_t i = 1; i < line.size(); i++){
                if(line[i] == ' ' || line[i] == '\t') break;
                protein.name.push_back(line[i]);
            }
            proteins.push_back(protein);
        }
        else if(!proteins.empty()){
            for(char c : line){
                if(c >= 'a' && c <= 'z') c = c - 'a' + 'A';
                if(c >= 'A' && c <= 'Z') proteins.back().sequence.push_back(c);
            }
        }
    }
    return proteins;
}

#ifdef PROTEINS_MAIN
int main(){
    vector<char> genome = readFile("../data/SARS-COV-2-MN908947.3.txt");
    vector<Protein> proteins = readProteins("../data/seq-proteins.txt");

    vector<ProteinMatch> results = findProteins(genome, proteins);
    cout << "Proteins in SARS-CoV-2 Wuhan genome (MN908947.3), indexes are 0-based" << endl << endl;
    cout << left << setw(13) << "Protein" << setw(21) << "Length (aminoacids)" << setw(7) << "Frame"
         << setw(17) << "Genome indexes" << setw(20) << "First 4 aminoacids" << "Codons" << endl;
    cout << string(95, '-') << endl;

    int found = 0, frameshifts = 0;
    for(ProteinMatch & m : results){
        cout << left << setw(13) << m.name << setw(21) << m.length;
        if(!m.found){
            cout << "not found" << endl;
            continue;
        }
        found++;
        string frame = string(1, m.strand) + to_string(m.frame);
        string indexes = to_string(m.begin) + " - " + to_string(m.end);
        string codons;
        for(string & codon : m.firstCodons) codons += codon;
        cout << setw(7) << frame << setw(17) << indexes << setw(20) << m.firstAminoacids << codons << endl;

        if(m.frameshift){
            frameshifts++;
            if(m.strand == '+'){
                cout << string(13, ' ') << "-> frameshift -1: aminoacids 1-" << m.prefixLength << " from "
                     << m.begin << "-" << m.begin + 3 * m.prefixLength - 1 << " (frame " << frame << "), aminoacids "
                     << m.prefixLength + 1 << "-" << m.length << " from " << m.shiftBegin << "-" << m.end
                     << " (frame +" << m.shiftBegin % 3 + 1 << "), base " << m.shiftBegin << " is read twice" << endl;
            }
        }
    }
    cout << string(95, '-') << endl;
    cout << "Proteins found: " << found << " / " << results.size() << " (" << frameshifts << " with frameshift)" << endl;

    // Check: exact hits per strand, searching every protein in the 6 frames
    vector<char> rc = reverseComplement(genome);
    vector<char> direct[3], inverse[3];
    for(int f = 0; f < 3; f++){
        direct[f] = translateFrame(genome, f);
        inverse[f] = translateFrame(rc, f);
    }

    int forwardHits = 0, reverseHits = 0;
    for(Protein & p : proteins){
        for(int f = 0; f < 3; f++){
            forwardHits += findAllMatches(direct[f], p.sequence).size();
            reverseHits += findAllMatches(inverse[f], p.sequence).size();
        }
    }
    cout << "Exact matches in direct strand (+1, +2, +3): " << forwardHits << endl;
    cout << "Exact matches in reverse strand (-1, -2, -3): " << reverseHits << endl;
    return 0;
}
#endif
