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
 Version:  1.0 as of 6th April 2017
*/

#include "SimilarLetters.h"
#include "Util.h"

#include <iostream>
#include <fstream>
#include <string>

#include <cstdlib>

using namespace std;

SimilarLetters::SimilarLetters(const char *filename, unsigned int penalty) :
    default_penalty(penalty) {   
    ifstream ifp(filename);
    uint32_t *ucs4bufferA = new uint32_t [200];
    uint32_t *ucs4bufferB = new uint32_t [200];
    if (!ifp) {
        cerr << "cannot open " << filename << endl;
    } else {
        // format:
        // # Nom
        // penalty 500
        // a àâä
        // c çćč

        string line;
        unsigned int penalty = default_penalty;
        unsigned int ct = 0;
	reduplicationPenalty = default_penalty*.9; /// penalty for reduplicated letters (default if not specified)
	transpositionPenalty = default_penalty*.9; /// penalty for transposed letters (default if not specified)

        while (!ifp.eof()) {
            getline(ifp, line);
            ct++;
            if (line.empty()) continue;
            if (line[0] == '#') continue;
            if (line.size() > 7 &&  line.substr(0, 7) == "penalty") {
		int p = atoi(line.substr(8).c_str());
		if (p == 0) {
		    cerr << "ERR 1 invalid penalty (" << ct << "): " << line;
		} else {
		    penalty = p;
		} 
	    } else if (line.size() > 13 &&  line.substr(0, 13) == "transposition") {
		int p = atoi(line.substr(13).c_str());
		if (p == 0) {
		    cerr << "ERR 1 invalid transposition penalty (" << ct << "): " << line;
		} else {
		    transpositionPenalty = p;
		} 
	    } else if (line.size() > 13 &&  line.substr(0, 13) == "reduplication") {
		int p = atoi(line.substr(13).c_str());
		if (p == 0) {
		    cerr << "ERR 1 invalid reduplication penalty (" << ct << "): " << line;
		} else {
		    reduplicationPenalty = p;
		} 

            } else {
                vector<string>tmp;
                SplitNoEmptyFields(tmp, line, " ");
                if (tmp.size() != 2) {
                    cerr << "ERR 2 invalid line (" << ct << "): " << line << endl;
                } else {
                    unsigned letterlen = u8_toucs(ucs4bufferA, 200, tmp[0].c_str(), -1);
                    unsigned alternatives = u8_toucs(ucs4bufferB, 200, tmp[1].c_str(), -1);
                    if (letterlen != 1) {
                        cerr << "ERR 3 invalid line (" << ct << "): "
			<< "first letter  is " << letterlen << " long: "
			 << line << endl;
                    } else if (penalty != default_penalty) {    
			//cerr << "line " << line << endl;
                        uint32_t a = ucs4bufferA[0];
                        for (unsigned int i = 0; i < alternatives; ++i) {
                            uint32_t b = ucs4bufferB[i];
                            if (a > b) {
                                // mettre le plus petit sur a
                                uint32_t t = a;
                                a = b;
                                b = t;
                            }
                            unsigned long couple = a;
                            couple <<= 32; // shift à gauche
                            couple |= b; // rajouter b
                            letterPairs[couple] = penalty;
			    //cerr << "adding: " << couple << endl;
                        }
                    }
                }
            }
        }
    }
    delete [] ucs4bufferA;
    delete [] ucs4bufferB;

    //cerr << *this << endl;
}


bool SimilarLetters::areSimilar(unsigned int *penalty, Character a, Character b) {
    Character tmp;
    if (a > b) {
        // mettre le Character le plus petit dans a
        // TODO ne fonctionne pas pour les lettres non accentué (voisin au clavier ...))
        tmp = b;
        b = a;
        a = tmp;
    }
    unsigned long couple = a;
    couple <<= 32; // shift à gauche
    couple |= b; // rajouter b
    
    map<unsigned long, int>::iterator it = letterPairs.find(couple);
    if (it != letterPairs.end()) {
        *penalty = it->second;
        return true;
    }
    *penalty = default_penalty;
    return false;
}

ostream& operator<<(ostream& out, const SimilarLetters &s) {
    uint32_t a;
    uint32_t b;
    char utfchar[10];

    out << s.letterPairs.size() << " pairs" << endl;

    unsigned int ct = 0;
    for (map<unsigned long, int>::const_iterator it = s.letterPairs.begin(); it != s.letterPairs.end(); ++it) {
        unsigned long tmp = it->first;
        b = tmp & 0xffffffff;
        a = tmp >> 32;
        u8_wc_toutf8(utfchar, b);
        out << ++ct << ": " << utfchar << ":";
        u8_wc_toutf8(utfchar, a);
        out << utfchar << "\t" << it->second << endl;
    }
    return out;
}
