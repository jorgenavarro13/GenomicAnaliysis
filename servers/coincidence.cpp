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


// Longes prefix suffix for the pattern / gene in this case we're searching
vector<int> lps(vector<char> & pattern){
    int m = pattern.size();
    vector<int> lpsArray(m, 0);

    int j=0, i=1;
    
    while(i<m){
        if(pattern[i]==pattern[j]){
            j+=1;
            lpsArray[i]=j;
            i+=1;
        }
        else{
            if(j!=0){
                j= lpsArray[j-1];
            }
            else{
                lpsArray[i]=0;
                i+=1;
            }
        }
    }
    return lpsArray;
}

/*
This funcition uses kmp to search in the gene the coresponding sequence, reducing the time complexity against the brute force
aproach wich is O(n2)

    Time complexity n +m : 
    Creating the LPS array takes O(m) and the search takes O(n), so the total time complexity is O(n+m)

    Space complexity: m
    We only store the LPS array of size m

*/

// Here the gene is smaller and is supossed to be inside the sequence
pair<int,int> findGeneInSequence(vector<char> & sequence, vector<char> & gene ){
    pair<int,int> indexes = {-1,-1};

    int n = sequence.size(), m = gene.size();

    vector<int> LPS = lps(gene);
    int max_coincidence = 0;

    int i = 0, j = 0;
    while(i < n){
        if(sequence[i] == gene[j]){
            j++; 
            i++;
            max_coincidence = max(j, max_coincidence); 
            if(j == m) indexes = {i-j, i-1};
        }
        else{
            if(j > 0){
                j = LPS[j-1];
            }
            else{
                i++;
            }
        }
        
        if(j == m){
            j = LPS[j-1];
        }
    }
    return indexes;
}