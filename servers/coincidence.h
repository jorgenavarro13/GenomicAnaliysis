#include <bits/stdc++.h>
using namespace std;

vector<char> readFile(string filename);
pair<int,int> findGeneInSequence(vector<char> & sequence, vector<char> & gene );
vector<int> lps(vector<char> & pattern);