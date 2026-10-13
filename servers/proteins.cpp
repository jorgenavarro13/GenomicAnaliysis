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
        int translation;     // frame of the first segment: 0..2 direct, 3..5 reverse (same order as translations[])
        int firstStart;      // index in the strand where the first segment starts
        int secondStart;     // index in the strand where the second segment starts
        int prefixLength;    // aminoacids of the first segment
    };

    /*
        KMP over the whole text, keeping the state at every position: states[x] is the length of
        the longest beginning of the pattern that ends exactly at text[x].

        Time complexity: O(n + m)
        Space complexity: O(n)
    */
    vector<int> prefixEndingAt(vector<char> & text, vector<char> & pattern){
        int n = text.size(), m = pattern.size();
        vector<int> states(n, 0);
        if(m == 0) return states;

        vector<int> LPS = lps(pattern);
        int j = 0;
        for(int i = 0; i < n; i++){
            while(j > 0 && (j == m || text[i] != pattern[j])) j = LPS[j - 1];
            if(text[i] == pattern[j]) j++;
            states[i] = j;
        }
        return states;
    }

    /*
        The same idea backwards: states[y] is the length of the longest ending of the pattern
        that starts exactly at text[y]. We run prefixEndingAt with both reversed, a match that
        ends at reversed[n-1-y] starts at y in the original text.

        Time complexity: O(n + m)
        Space complexity: O(n + m)
    */
    vector<int> suffixStartingAt(vector<char> & text, vector<char> & pattern){
        vector<char> reversedText(text.rbegin(), text.rend());
        vector<char> reversedPattern(pattern.rbegin(), pattern.rend());
        vector<int> reversedStates = prefixEndingAt(reversedText, reversedPattern);
        return vector<int>(reversedStates.rbegin(), reversedStates.rend());
    }

    // Checks that text[from .. from+length-1] is equal to sequence[begin .. begin+length-1]
    bool sameAminoacids(vector<char> & text, int from, vector<char> & sequence, int begin, int length){
        for(int k = 0; k < length; k++){
            if(text[from + k] != sequence[begin + k]) return false;
        }
        return true;
    }

    /*
        Looks for a protein that cannot be found in a single frame because the ribosome shifts
        frame in the middle: the first p aminoacids are read in one frame and the rest in the frame
        shifted one base (slip -1: the ribosome goes back one base and reads it twice, slip +1: it
        skips one). Both segments are on the same strand.

        For every frame we know, at each position, the longest beginning of the protein that ends
        there (a, prefixEndingAt) and the longest ending that starts there (b, suffixStartingAt).
        If the first segment ends at codon x, the second one starts 3 + slip bases later, and the
        shift is only possible when a + b >= L. Then any cut p between L - b and a is a candidate,
        and we verify it. There is no fixed anchor or window: the shift can be at any aminoacid,
        from the second to the last one, and the protein can have any length >= 2.

        When several cuts explain the protein (the aminoacids around the shift are the same in
        both frames) we report the shortest first segment, the earliest point where the shift can be.

        Time complexity: O(N + L) per protein
        6 forward and 6 backward KMP passes over texts of about N/3 aminoacids. Each candidate costs
        O(L) to verify, and a candidate needs a + b >= L, which in a real genome only happens where
        the protein really is. A very repetitive genome and protein (like AAAA...) could produce many
        candidates, up to O(N * L).

        Space complexity: O(N + L)
        The KMP states of the 6 frames.
    */
    FrameshiftMatch findFrameshift(vector<char> translations[], vector<char> & sequence){
        FrameshiftMatch result;
        result.found = false;
        result.translation = -1;
        result.firstStart = result.secondStart = -1;
        result.prefixLength = 0;

        int L = sequence.size();
        vector<int> prefixes[6], suffixes[6];
        for(int t = 0; t < 6; t++){
            prefixes[t] = prefixEndingAt(translations[t], sequence);
            suffixes[t] = suffixStartingAt(translations[t], sequence);
        }

        for(int t = 0; t < 6; t++){
            int strandOffset = (t < 3) ? 0 : 3;
            int frame = t % 3;

            for(int x = 0; x < (int)translations[t].size(); x++){
                int a = prefixes[t][x];
                if(a == 0) continue;

                for(int slip : {-1, 1}){
                    // The first segment ends with the codon that starts at frame + 3x,
                    // the second one starts 3 bases later, moved by the slip
                    int secondStart = frame + 3 * x + 3 + slip;
                    int secondFrame = secondStart % 3;
                    int y = (secondStart - secondFrame) / 3;
                    int second = strandOffset + secondFrame;
                    if(y >= (int)translations[second].size()) continue;

                    int b = suffixes[second][y];
                    if(a + b < L) continue;

                    for(int p = max(1, L - b); p <= min(a, L - 1); p++){
                        if(sameAminoacids(translations[t], x - p + 1, sequence, 0, p) &&
                           sameAminoacids(translations[second], y, sequence, p, L - p)){
                            result.found = true;
                            result.translation = t;
                            result.firstStart = frame + 3 * (x - p + 1);
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
        if(!match.found){
            FrameshiftMatch shift = findFrameshift(translations, protein.sequence);
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

    // Same as readFile: an empty result means the file could not be read
    if(!file.is_open()){
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
