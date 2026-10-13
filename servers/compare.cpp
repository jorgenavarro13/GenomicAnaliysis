#include <bits/stdc++.h>
using namespace std;

// Contribution: Nicté Allelen
string getAminoAcid(const string& codon) {
    static unordered_map<string, string> codonMap = {
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
    
    if (codonMap.find(codon) != codonMap.end()) {
        return codonMap[codon];
    }
    return "Desconocido";
}



/*
    What does this function do?
    Asumes that the two genomes are aligned and compares them base by base.

    Time complexity: O(n) where n is the length of the shorter genome.
    Space complexity: O(n) since we're storing differences in the sequence.
*/
void compareGenomes(const string& wuhan, const string& texas) {
    int minLen = min(wuhan.length(), texas.length());

    int maxLen=0;
    int maxStartIndex=0;
    int currentLength=0;
    int currentStart=0;
    int totalDifferences=0;

    for (int i=0;i<minLen;++i) {
        if (wuhan[i]==texas[i]) {
            if (currentLength==0) {
                currentStart=i;
            }
            currentLength++;
            
            if (currentLength>maxLen) {
                maxLen=currentLength;
                maxStartIndex=currentStart;
            }
        } else {
            currentLength=0;

            int startCodonIndex=i-(i%3);

            if (startCodonIndex+2<minLen) {
                string codonW=wuhan.substr(startCodonIndex,3);
                string codonT=texas.substr(startCodonIndex,3);
                
                string aminoW=getAminoAcid(codonW);
                string aminoT=getAminoAcid(codonT);

                cout<<"Diferencia encontrada en el indice: "<<i<<"\n";
                cout<<"Wuhan 2019 : Codon ["<<codonW<<"] produce Aminoacido ["<<aminoW<<"]\n";
                cout<<"Texas 2020 : Codon ["<<codonT<<"] produce Aminoacido ["<<aminoT<<"]\n";
                totalDifferences++;
                
                if (aminoW!=aminoT) {
                    cout<<"La mutacion altera el aminoacido.\n\n";
                } else {
                    cout<<"Mutacion silenciosa (el aminoacido se mantiene igual).\n\n";
                }
            }
        }
    }

    cout<<"Total de diferencias encontradas: "<<totalDifferences<<"\n";
    cout<<"\nCadena mas larga sin diferencias\n";
    cout<<"Longitud: "<<maxLen<<" bases.\n";
    cout<<"Inicia en el indice: "<<maxStartIndex<<" y termina en: "<<maxStartIndex+maxLen-1<< "\n";
}

/*
    Alligns two genetic sequences using the Needleman–Wunsch algorithm,
    with this algorithm we're using DP to give penalties and rewards on each fragment found.
    This considers one of the worsts scenarios, where the data is not cleaned nor alligned and they may corrspond
    to a different genetic sequence

    Time complexity: O(A*B)  We traverse a matrix of length  lenght(A) * lenght(B)
    Space complexity : O(A*B) We have to store the matrix to compare


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

#define match_penalty 1
#define mismatch_penalty -1
#define gap_penalty -1

pair<string, string> allignSequences(const vector<char> &A, const vector<char> &B){
    size_t A_size = A.size(), B_size=B.size();
    vector<vector<int>> matrix(A_size+1, vector<int> (B_size+1, 0));
    for(int i=0; i<=A_size; i++){
        matrix[i][0]=i*gap_penalty;
    }
    for(int j=0; j<=B_size; j++){
        matrix[0][j]=j*gap_penalty;
    }

    for(int i=1; i<=A_size; i++){
        for(int j=1; j<=B_size; j++){
            int score_diagonal = matrix[i-1][j-1] + (A[i-1]==B[j-1]? match_penalty:mismatch_penalty);
            int score_up = matrix[i-1][j] + gap_penalty;
            int score_left = matrix[i][j-1] + gap_penalty;
            matrix[i][j]=max({score_diagonal, score_up, score_left});
        }
    }

    string aligned_A, aligned_B;
    int i=A_size, j=B_size;
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

int main(){
    ifstream wuhanFile("../data/SARS-COV-2-MN908947.3.txt");
    ifstream texasFile("../data/SARS-COV-2-MT106054.1.txt");

    string wuhan,texas,linea;

    while(getline(texasFile,linea)){
        texas+=linea;
    }

    while(getline(wuhanFile,linea)){
        wuhan+=linea;
    }

    wuhanFile.close();
    texasFile.close();

    compareGenomes(wuhan,texas);
    
    return 0;
}