#include <bits/stdc++.h>
#include "coincidence.h"
using namespace std;

namespace {
    unordered_map<char,char> oppositeMapping = {
        {'A','T'},
        {'G','C'},
        {'T', 'A'},
        {'C', 'G'}
    };
}

/*
    The structure of this algorithm is based on the manacher's algorithm but with some modifications due 
    the fact that a palindrome in genetics refers that the corresponding other has to be the opposite

    Taking that in mind this algorithm has the following complexities:

    Time Complexity: O(n)
    We do a single pass throught the string, we are reusing data (dp) from the previous computed parts, making this actually O(n)

    Space Complexity: O(n)
    We have to arrays of aproximate size 2*n, (values and actual manacher string) which is reduced to O(n)
*/

pair<int,int> genPalindrome(vector<char> gene){

    // Create the manacher array
    vector<char> manacherArray;
    manacherArray.push_back('@');
    for(char nucleotid : gene ){
        manacherArray.push_back('$');
        manacherArray.push_back(nucleotid);
    }
    manacherArray.push_back('$'); manacherArray.push_back('#');
    


    int center = 0, limit = 0, n= manacherArray.size();
    int max_center = 0;
    int max_lenght = 0;

    vector<int> P (n, 0);

    for(int i=1; i<n-1; i++){
        if(i<limit){
            int simetric = 2 * center - i;
            P[i] = min(limit -i, P[simetric]);
        }

        int gap = P[i]+1;
        while(manacherArray[i-gap] == oppositeMapping[manacherArray[i+gap]] || (manacherArray[i-gap] == '$' && manacherArray[i+gap] == '$')){
            P[i]++;
            gap++;
        }

        if(P[i]> max_lenght){
            max_lenght = P[i];
            max_center = i;
        }

        if(i+P[i]>limit){
            limit = i+P[i];
            center = i;
        }
    }

    int start_index = (max_center - max_lenght)/2;
    int end_index = start_index + max_lenght - 1;

    return {start_index, end_index};
}

void saveToFile(string filename, vector<char> & sequence, pair<int,int> & result){
    ofstream file(filename);
    for(int i = result.first; i <= result.second; i++){
        file<<sequence[i];
    }
    file.close();
}

/*
  vector<char> test = {
        'A','T', 'G', 'C', 'T', 'G', 'A','T', 'C', 'T', 'A','T'
    };

vector<char> convert(string s){
    vector<char> c;
    for(char l : s) c.push_back(l);
    return c;
}

    int main(){
        
        vector<char> sequence = test;
        pair<int,int> result = genPalindrome(sequence);
        saveToFile("../data/palindromes/palindrome_result.txt", sequence, result);
        return 0;
    }

*/