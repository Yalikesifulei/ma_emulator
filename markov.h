#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

#ifndef MARKOV_H
#define MARKOV_H


class Markov
{
private:
    string Name;
    string Alphabet;
    vector <pair<string, string>> Commands;
    vector<unsigned int> End_Commands;
public:
    Markov();
    Markov(string N, string A, vector <pair<string, string>> C, vector<unsigned int> EC):
            Name(N), Alphabet(A), Commands(C), End_Commands(EC){};
    void Export();
    string Process(string Input, int &count);
};

#endif // MARKOV_H
