/** This library is under the 3-Clause BSD License

Copyright (c) 2017, Orange S.A.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice, 
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

  3. Neither the name of the copyright holder nor the names of its contributors 
     may be used to endorse or promote products derived from this software without
     specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 Author: Johannes Heinecke
 Version:  1.0.1 as of 14th December 2018
*/

#include <iostream>
#include <fstream>

#include <vector>
#include <algorithm>

#include <thread>

#include <cstring>
#include <time.h>

using namespace std;


#include "Arbre.h"
#include "Corrector.h"

//#define VERBOSE
//#define RESFILE


/// utiliser par le teste de performance
void testthread(bool exact, ArbreBinaire *ab, const vector<string> *words, int id) {
    // initialiser thread du correcteur
    Corrector cr(ab);
    int loop;

//#ifdef VERBOSE
    ostream *out = &cout;
    
#ifdef RESFILE
    char tmp[10];
    sprintf(tmp, "res_%d.txt", id);
    ofstream outfile(tmp);
    out = &outfile;
#endif
//#endif

    time_t start = time(0);
    if (exact) {
        loop = 1000000;
#ifdef VERBOSE
        loop = 20;
        const WordForm * s;
#endif

        for (int i = 0; i < loop; i++) {
            //cerr << i << endl;
            for (vector<string>::const_iterator it = words->begin(); it != words->end(); ++it) {
#ifdef VERBOSE
                *out << "thread " << id 
                     << " WORD: [" << *it << "]" << endl;
                s =
#endif  
                        cr.findWordExact(it->c_str());
#ifdef VERBOSE
                if (s) {
                    *out << "\tfound: " << *s;
                }
                *out << endl;
#endif
            }
        }
    } else {
        loop = 100;
#ifdef VERBOSE
        loop = 20;
        map<const WordForm *, unsigned int> *res;
#endif

        for (int i = 0; i < loop; i++) {
            //cerr << i << endl;
            for (vector<string>::const_iterator it = words->begin(); it != words->end(); ++it) {
#ifdef VERBOSE
                *out << "WORD: [" << *it << "]" << endl;
                res =
#endif  

                        cr.findWordCorrected(it->c_str(), 1000*it->size()/4);
#ifdef VERBOSE
                for (map<const WordForm *, unsigned int>::iterator it = res->begin(); it != res->end(); ++it) {
                    *out << "  " << *(it->first) << " (" << it->second << " faute" << (it->second != 1 ? "s)" : ")") << endl;
                }
                *out << res->size() << " Solutions" << endl;
#endif
            }
        }
    }
    time_t end = time(0);
    *out << "accessing " << (loop * words->size()) << " words: " << (end - start) << "s" << endl;
#ifdef RESFILE
    outfile.close();
#endif
}


bool speedtest(const char *lexfile, const char *similarletters, const char *wordfile, bool exact, bool multipleEntries) {
    // creer Arbre
    ArbreBinaire ab(lexfile, similarletters, multipleEntries);
   

    if (wordfile == 0) return false;



    // test with words from a given file
    ifstream ifp2(wordfile);
    string line;
    vector<string>words;
    if (ifp2) {
        cout << "Reading test wordfile '" << wordfile << "'" << endl;
        while (!ifp2.eof()) {
            getline(ifp2, line);
            if (!line.empty()) words.push_back(line);
        }
    } else {
        cerr << "cannot open " << wordfile << endl;
        return 10;
    }


    int numthreads = 4;
    cout << "threads " << numthreads << endl
         << "words " << words.size() << endl;
    
    std::thread *mythreads = new std::thread[numthreads];
    for (int i = 0; i < numthreads; ++i) {
        mythreads[i] = std::thread(testthread, exact, &ab, &words, i);
    }

    for (int i = 0; i < numthreads; ++i) {
        mythreads[i].join();
    }
    
    delete [] mythreads;
    return true;
}




/// trier un map selon les valeurs
bool resultSort(std::pair<const WordForm *, unsigned int> a,
                std::pair<const WordForm *, unsigned int> b) {  
    if (a.second == b.second) {
	if (a.first->form < b.first->form) return true;
    } else {
	if (a.second < b.second) return true;
    }
    return false;
}

#include "SimilarLetters.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        cerr << "usage :" << argv[0] << " [options] in:lexfile [in:wordfile]" << endl;
        cerr << " options:" << endl
             << " --similar  <file>  similar letter definition" << endl
             << " --singleEntry      one entry per lexicon file (form TAB other information)" << endl
             << " --speed            execute speedtest (needs wordfile)" << endl
             << " --exact            (speed)test with exact access" << endl
	     << " --maxdist <n>      factor used to calculate maximal allowed distance (n * wordLength / 5)" << endl
	     << " --defdist <n>      default distance (1000)" << endl
	    // << " --quiet            only output the best correction (or the original word)" << endl
	    ;
        return 1;
    }

    
    bool multipleEntries = true;
    bool speed = false;
    int optct = 1;
    bool exact = false;
    const char *similarletters = 0;
    int distfactor = 1000;
    //bool firstOnly = false;
    for(; optct<argc-1; ++optct) {
        if (argv[optct][0] != '-') break;
        if (strcmp(argv[optct], "--singleEntry") == 0) {
            multipleEntries = false;
        }
        else if (strcmp(argv[optct], "--speed") == 0) {
            speed = true;
        }
        else if (strcmp(argv[optct], "--exact") == 0) {
            exact = true;
        }
        //else if (strcmp(argv[optct], "--quiet") == 0) {
        //    firstOnly = true;
        //}
        else if (strcmp(argv[optct], "--similar") == 0) {
            similarletters = argv[++optct];
	}
        else if (strcmp(argv[optct], "--maxdist") == 0) {
	    int t = atoi(argv[++optct]);
	    if (t > 0) {
		distfactor = t;
	    } else {
		cerr << "value for --maxdist must be > 0" << endl;
	    }
	}
        else if (strcmp(argv[optct], "--defdist") == 0) {
	    int t = atoi(argv[++optct]);
	    if (t > 0) Calculator::penalty = t;
	    else 
		cerr << "value for --defist must be > 0" << endl;
        } 
	else if (argv[optct][0] == '-') {
	    cerr << "invalid option " << argv[optct] << endl;
	    return 3;
        }
    }
   
    const char *lexfile = argv[optct];

    const char *textfile = 0;
    if (argc > optct+1) {
	textfile = argv[optct+1];
    }

    if (speed && textfile != 0) {          
        speedtest(lexfile, similarletters, textfile, exact, multipleEntries);
        return 0;
    }
    
    // creer Arbre  
    ArbreBinaire ab(lexfile, similarletters, multipleEntries);
   

    // create a dot-file to visualise the tree
    //if (argc > optct+2) {
    //    ofstream tr(argv[optct+2]);
    //    ab.dot(tr);
    //    tr.close();
    //}

    // initialiser thread du correcteur
    Corrector cr(&ab);
    //return 0;


    if (textfile == 0) {
	// interactive mode
	string mot;
	while (true) {
	    //if (!firstOnly) {
	    cout << "enter word> ";
	    //}

	    cin >> mot;
	    if (mot.empty() || cin.eof() || mot == "q") return 0;
	    if (!mot.empty() && mot[0] == '>') {
		// TODO: meme cateégorie
		if (mot == ">ALL") cr.setCorrectionType(CORRECT_ALL);
		else if (mot == ">CASE") cr.setCorrectionType(CORRECT_CASE);
		else if (mot == ">DIA") cr.setCorrectionType(CORRECT_DIACRITICS);
		else if (mot == ">TYPOS") cr.setCorrectionType(CORRECT_TYPOS);
		else if (mot == ">+CASE") cr.setCorrectionType(cr.getCorrectionType() | CORRECT_CASE);
		else if (mot == ">+DIA") cr.setCorrectionType(cr.getCorrectionType() | CORRECT_DIACRITICS);
		else if (mot == ">+TYPOS") cr.setCorrectionType(cr.getCorrectionType() | CORRECT_TYPOS);
		cout << cr.getCorrectionType() << endl;
		continue;
	    }
        
	    unsigned wordLength = cr.unicodePoints(mot.c_str());
        
	    int distance = distfactor * wordLength / 5;

	    if (distance < 1) distance = 1000;
	    //if (!firstOnly) {
	    cout << mot << " (max penalty allowed: " << distance << ")" << endl;
	    //}

	    map<const WordForm *, unsigned int> *res;
	    if (!exact) res = cr.findWordCorrected(mot.c_str(), distance);
	    else res = cr.findWordBest(mot.c_str(), distance);

	    // trier en fonction de la distance levenshtein
	    vector<std::pair<const WordForm *, unsigned int> > resVec(res->begin(), res->end());
	    sort(resVec.begin(), resVec.end(), &resultSort);

	    cout << res->size() << " solutions:" << endl;
	    //cout << '[';
	    for (vector<std::pair<const WordForm *, unsigned int> >::iterator it = resVec.begin(); 
		 it != resVec.end(); ++it) {
		cout << "  " << it->second << ": " << *(it->first) << endl;
		//if (it != resVec.begin()) cout << ", ";
		//cout << "{ \"dist\": " << it->second
		//     << ", \"word\": " << (it->first)->toJson() << "}";
	    }
	    //cout << ']' << endl;
	}
    } else {
	// correct textfile
	ifstream ifp(textfile);
	string word;
	if (ifp) {
	    cerr << "Reading text from '" << textfile << "'" << endl;
	    cout << '[';
	    bool first = true;
	    while (!ifp.eof()) {
		ifp >> word;
		//cerr << "<<" << word <<"||" << word.size() << ">>" << endl;
		if (word.size() < 1) continue;
		unsigned wordLength = cr.unicodePoints(word.c_str());
		int distance = distfactor * wordLength / 5;
		if (distance < 1) distance = 1000;

#define CORRECT
#ifdef CORRECT
		map<const WordForm *, unsigned int> *res = cr.findWordCorrected(word.c_str(), distance);
#else
		map<const WordForm *, unsigned int> *res = cr.findWordBest(word.c_str(), distance);
#endif

		
		// trier en fonction de la distance levenshtein
		vector<std::pair<const WordForm *, unsigned int> > resVec(res->begin(), res->end());
		sort(resVec.begin(), resVec.end(), &resultSort);


		if (!first) cout << ',' << endl;
		first = false;
		cout << "{ \"word\": \"" << word
		     << "\", \"maxdist\": " << distance
		     << ", \"results\": [";

		for (vector<std::pair<const WordForm *, unsigned int> >::iterator it = resVec.begin();
		     it != resVec.end(); ++it) {
		    //cout << "  " << it->second << ": " << *(it->first) << endl;
		    if (it != resVec.begin()) cout << ", ";
		    cout << "{ \"dist\": " << it->second
			 << ", \"entry\": " << (it->first)->toJson() << "}";
		}
		cout << "]}";


// 		if (!resVec.empty()) {
// 		    const WordForm *wf = resVec.begin()->first;
// 		    if (
// #ifdef CORRECT
// 			true ||
// #endif
// 			word != wf->form) {
// 			//cout << word << "\t>>\t" << wf->form;
// 			cout << word << "\t>>";
// 			for (vector<std::pair<const WordForm *, unsigned int> >::iterator it = resVec.begin();
// 			     it != resVec.end(); ++it) {
// 			    cout << "\t" << it->first->form
// #ifdef CORRECT
// 				 << ":" << it->second
// #endif
// 				;
// 			}
// 		    } else
// 			cout << word;
// 		} else {
// 		    cout << word;
// 		}
// 		cout << endl;

		word = ""; // to avoid reading the same word if empty line
	    }
	    cout << ']';
	} else {
	    cerr << "cannot open " << textfile << endl;
	    return 10;
	}
    }

    return 0;

}
