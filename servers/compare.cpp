#include <bits/stdc++.h>
#include "compare.h"
using namespace std;

// Contribution: Nicté Allelen
string getAminoAcid(const string& codon) {
    static const unordered_map<string, string> codonMap = {
        {"ATA", "Ile"}, {"ATC", "Ile"}, {"ATT", "Ile"}, {"ATG", "Met"},
        {"ACA", "Thr"}, {"ACC", "Thr"}, {"ACG", "Thr"}, {"ACT", "Thr"},
        {"AAC", "Asn"}, {"AAT", "Asn"}, {"AAA", "Lys"}, {"AAG", "Lys"},
        {"AGC", "Ser"}, {"AGT", "Ser"}, {"AGA", "Arg"}, {"AGG", "Arg"},
        {"CTA", "Leu"}, {"CTC", "Leu"}, {"CTG", "Leu"}, {"CTT", "Leu"},
        {"CCA", "Pro"}, {"CCC", "Pro"}, {"CCG", "Pro"}, {"CCT", "Pro"},
        {"CAC", "His"}, {"CAT", "His"}, {"CAA", "Gln"}, {"CAG", "Gln"},
        {"CGA", "Arg"}, {"CGC", "Arg"}, {"CGG", "Arg"}, {"CGT", "Arg"},
        {"GTA", "Val"}, {"GTC", "Val"}, {"GTG", "Val"}, {"GTT", "Val"},
        {"GCA", "Ala"}, {"GCC", "Ala"}, {"GCG", "Ala"}, {"GCT", "Ala"},
        {"GAC", "Asp"}, {"GAT", "Asp"}, {"GAA", "Glu"}, {"GAG", "Glu"},
        {"GGA", "Gly"}, {"GGC", "Gly"}, {"GGG", "Gly"}, {"GGT", "Gly"},
        {"TCA", "Ser"}, {"TCC", "Ser"}, {"TCG", "Ser"}, {"TCT", "Ser"},
        {"TTC", "Phe"}, {"TTT", "Phe"}, {"TTA", "Leu"}, {"TTG", "Leu"},
        {"TAC", "Tyr"}, {"TAT", "Tyr"}, {"TAA", "STOP"}, {"TAG", "STOP"},
        {"TGC", "Cys"}, {"TGT", "Cys"}, {"TGA", "STOP"}, {"TGG", "Trp"}
    };
    
    auto it = codonMap.find(codon);
    if (it != codonMap.end()) {
        return it->second;
    }
    return "Desconocido";
}



/*
    What does this function do?
    Asumes that the two genomes are aligned (same start, the shorter one may be cut at the end)
    and compares them base by base.

    For every base that differs we look at the codon that contains it in the 3 reading frames,
    because the same base belongs to a different codon depending on where we start reading
    (offset 0, 1 or 2). If two differences fall in the same codon, that codon is reported once per frame.

    Time complexity: O(n) where n is the length of the shorter genome.
    Each base is visited once and each difference is checked in 3 frames, O(1) each.
    Space complexity: O(d) where d is the number of differences found.
*/
ComparisonResult compareGenomes(const vector<char>& A, const vector<char>& B) {
    ComparisonResult result;
    int minLen = min(A.size(), B.size());
    result.comparedLength = minLen;

    int maxLen=0;
    int maxStartIndex=0;
    int currentLength=0;
    int currentStart=0;
    int lastCodon[3] = {-1, -1, -1};  // last codon reported in each frame

    for (int i=0;i<minLen;++i) {
        if (A[i]==B[i]) {
            if (currentLength==0) {
                currentStart=i;
            }
            currentLength++;

            if (currentLength>maxLen) {
                maxLen=currentLength;
                maxStartIndex=currentStart;
            }
            continue;
        }

        currentLength=0;
        result.differenceIndexes.push_back(i);

        for (int frame=0; frame<3; ++frame) {
            if (i<frame) continue;  // the base is before the first codon of this frame

            int codonStart = frame + 3*((i-frame)/3);
            if (codonStart+2>=minLen || codonStart==lastCodon[frame]) continue;
            lastCodon[frame] = codonStart;

            AminoacidDifference difference;
            difference.index = i;
            difference.codonStart = codonStart;
            difference.codonA = string(A.begin()+codonStart, A.begin()+codonStart+3);
            difference.codonB = string(B.begin()+codonStart, B.begin()+codonStart+3);
            difference.aminoA = getAminoAcid(difference.codonA);
            difference.aminoB = getAminoAcid(difference.codonB);
            difference.changesAminoacid = difference.aminoA != difference.aminoB;

            result.frames[frame].push_back(difference);
        }
    }

    result.longestLength = maxLen;
    result.longestBegin = maxStartIndex;
    result.longestEnd = maxStartIndex+maxLen-1;
    return result;
}

/*
    Alligns two genetic sequences using the Needleman–Wunsch algorithm,
    with this algorithm we're using DP to give penalties and rewards on each fragment found.
    This considers one of the worsts scenarios, where the data is not cleaned nor alligned and they may corrspond
    to a different genetic sequence

    Time complexity: O(A*B)  We traverse a matrix of length  lenght(A) * lenght(B)
    Space complexity : O(A*B) We have to store the matrix to compare

    Note: for two full genomes (~30,000 bases each) the matrix needs ~3.6 GB, so this function
    is for learning and documentation, it should not be called from the server with full genomes.

    ----- B - ----
    |
    |
    A
    |
    |

    Penalties: The simplest scoring schemes simply give a value for each match, mismatch and indel.
    This scoring scheme is simple, but it is not very accurate.
    There are many different scoring schemes
    More sophisticated scoring schemes take the evolutionary relationships between the sequences into account, and are based on empirical data.
*/

namespace {
    const int match_penalty = 1;
    const int mismatch_penalty = -1;
    const int gap_penalty = -1;
}

pair<string, string> allignSequences(const vector<char> &A, const vector<char> &B){
    size_t A_size = A.size(), B_size=B.size();
    vector<vector<int>> matrix(A_size+1, vector<int> (B_size+1, 0));
    for(size_t i=0; i<=A_size; i++){
        matrix[i][0]=i*gap_penalty;
    }
    for(size_t j=0; j<=B_size; j++){
        matrix[0][j]=j*gap_penalty;
    }

    for(size_t i=1; i<=A_size; i++){
        for(size_t j=1; j<=B_size; j++){
            int score_diagonal = matrix[i-1][j-1] + (A[i-1]==B[j-1]? match_penalty:mismatch_penalty);
            int score_up = matrix[i-1][j] + gap_penalty;
            int score_left = matrix[i][j-1] + gap_penalty;
            matrix[i][j]=max({score_diagonal, score_up, score_left});
        }
    }

    string aligned_A, aligned_B;
    size_t i=A_size, j=B_size;
    while(i>0 || j>0){
        if(i>0 && j>0 && matrix[i][j]==matrix[i-1][j-1] + (A[i-1]==B[j-1]? match_penalty:mismatch_penalty)){
            aligned_A+=A[i-1];
            aligned_B+=B[j-1];
            i--;
            j--;
        } else if(i>0 && matrix[i][j]==matrix[i-1][j] + gap_penalty){
            aligned_A+=A[i-1];
            aligned_B+='-';
            i--;
        } else {
            aligned_A+='-';
            aligned_B+=B[j-1];
            j--;
        }
    }

    reverse(aligned_A.begin(), aligned_A.end());
    reverse(aligned_B.begin(), aligned_B.end());

    return {aligned_A, aligned_B};
}
