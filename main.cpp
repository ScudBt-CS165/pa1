#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <unordered_set>
#include <unordered_map>
#include <time.h>
#include <ctime>
#include <cmath>
#include <iomanip>
using namespace std;

int main(int argc, char* argv[]) {
    // input training data: into collection and dictionary
    vector<vector<string>> collection; //collection[n] is a document 
    unordered_set<string> dictionary;
    string line;
    ifstream input(argv[1]);
    if (!input.is_open()) 
        cout << "Unable to open file";        
    while (getline(input, line)) {
        vector<string> document;
        int l = 0;
        for (int r = 1; r < line.size(); r++) {
            if (line.substr(r,1) == " ") {
                string word = line.substr(l,r-l);
                document.push_back(word);
                if (dictionary.find(word) == dictionary.end()) {
                    dictionary.insert(word); 
                }
                l = r+1;
            } else if (line.substr(r,1) == ",") {
                document.push_back(line.substr(r+1,1));
            }
        }
        collection.push_back(document);
    }
    input.close();

    // process training data: cpt, priorProb, and n
    unordered_map<string, pair<int, int>> cpt;
    for (string vocab : dictionary) {
        cpt.insert(pair<string, pair<int, int>>(vocab, pair<int, int>(0,0))); // need to insert pair into map
    }
    for (vector<string> document : collection) {
        for (int i = 0; i < document.size()-1; i++) {
            if (document[document.size()-1] == "1"){
                cpt[document[i]].first++;
            } else {
                cpt[document[i]].second++;
            }
        }
    }
    int n0 = 0;
    int n1 = 0;
    int nw0 = 0;
    int nw1 = 0;
    for (vector<string> document : collection) {
        if (document[document.size()-1] == "1") {
            n1++;
            for (string word : document) 
                nw1++;
        } else {
            n0++;
            for (string word : document) 
                nw0++;
        }
    }
    double p0 = double(n0) / (n0 + n1);
    double p1 = double(n1) / (n0 + n1);  
    nw0 -= n0;
    nw1 -= n1;
    
    // for (vector<string> document : collection) {
    //     for (string word : document) {
    //         cout << word << " ";
    //     }
    //     cout << endl;
    // }
    // for (pair<string, pair<int, int>> entree : cpt) {
    //     cout << entree.first << " " << entree.second.first << " " << entree.second.second << endl;
    // }
    // cout << "dictionary size: " << dictionary.size() << endl;
    // cout << "nw0: " << nw0 << " nw1: " << nw1 << endl;

    // testing

    int a = 1;
    int successes = 0; 
    int fails = 0;  
    clock_t t1 = clock();
    ifstream train(argv[1]);
    if (!train.is_open()) 
        cout << "Unable to open file";        
    while (getline(train, line)) {
        vector<string> document;
        int l = 0;
        long double cp0 = log (p0);
        long double cp1 = log (p1);
        for (int r = 1; r < line.size(); r++) {
            if (line.substr(r,1) == " ") {
                string word = line.substr(l,r-l);
                if (dictionary.find(word) != dictionary.end()) {
                    double cpi0 = log (double(cpt[word].second + a) / (nw0 + cpt[word].first + a * dictionary.size()));
                    double cpi1 = log (double(cpt[word].first + a) / (nw1 + cpt[word].first + a * dictionary.size()));
                    cp0 += cpi0;
                    cp1 += cpi1;
                }
                l = r + 1;
            } else if (line.substr(r,1) == ",") {
                //cout << "cp0" << cp0 << " cp1" << cp1 << endl;
                if (cp0 > cp1) {
                    if (line.substr(r+1,1) == "0") {
                        successes++;
                    } else {
                        fails++;
                    }
                } else {
                    if (line.substr(r+1,1) == "1") {
                        successes++;
                    } else {
                        fails++;
                    }
                }
            }
        }
    }
    train.close();
    double accuracy1 =  double(successes) / (successes + fails) ;
    int time1 = (clock()-t1) / CLOCKS_PER_SEC;

    successes = 0; 
    fails = 0;
    clock_t t2 = clock();
    ifstream test(argv[2]);
    if (!test.is_open()) 
        cout << "Unable to open file";        
    while (getline(test, line)) {
        vector<string> document;
        int l = 0;
        long double cp0 = log (p0);
        long double cp1 = log (p1);
        //cout << "cp0" << cp0 << " cp1" << cp1 << endl;
        for (int r = 1; r < line.size(); r++) {
            if (line.substr(r,1) == " ") {
                string word = line.substr(l,r-l);
                if (dictionary.find(word) != dictionary.end()) {
                    double cpi0 = log (double(cpt[word].second + a) / (nw0 + cpt[word].first + a * dictionary.size()));
                    double cpi1 = log (double(cpt[word].first + a) / (nw1 + cpt[word].first + a * dictionary.size()));
                    cp0 += cpi0;
                    cp1 += cpi1;
                    //cout << word << " cp0" << cp0 << " cp1" << cp1 << endl;
                }
                l = r + 1; // i accidentally forgot this and it costed me three hours to debug...
            } else if (line.substr(r,1) == ",") {
                if (cp0 > cp1) {
                    cout << "0" << endl;
                    if (line.substr(r+1,1) == "0") {
                        successes++;
                    } else {
                        fails++;
                    }
                } else {
                    cout << "1" << endl;
                    if (line.substr(r+1,1) == "1") {
                        successes++;
                    } else {
                        fails++;
                    }
                }
            }
        }
    }
    test.close();
    double accuracy2 =  double(successes) / (successes + fails) ;
    int time2 = (clock()-t2) / CLOCKS_PER_SEC;
    
    cout << setprecision(3) << time1 << " seconds (training)" << endl;
    cout << setprecision(3) << time2 << " seconds (labeling)" << endl;
    cout << accuracy1 << " (training)" << endl;
    cout << accuracy2 << " (testing)" << endl;
    return 0;
}