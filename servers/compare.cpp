#include <bits/stdc++.h>
using namespace std;

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