/** This library is under the 3-Clause BSD License

Copyright (c) 2017-2020, Orange S.A.

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
 Version:  2.0 as of 9th April 2020
*/

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "LexicalCorrector.h"


using namespace std;


LexicalCorrector::LexicalCorrector(const char *lexfile, const char *similarletters, int multipleEntries) {
    cerr << "loading " << lexfile << " " << similarletters << endl;
    ab = new ArbreBinaire(lexfile, similarletters, (multipleEntries == 0 ? false : true));
    cr = new Corrector(ab);
}

LexicalCorrector::~LexicalCorrector() {
    if (cr != 0) delete cr;
    if (ab != 0) delete ab;
}


// returns the correct word or an empty string
string LexicalCorrector::findWordExact(const char *word) {
    const WordForm *wf = cr->findWordExact(word);
    return wf->toJson();
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



void LexicalCorrector::getWFs(ostream &out, map<const WordForm *, unsigned int> *res) {
    // trier en fonction de la distance levenshtein
    vector<std::pair<const WordForm *, unsigned int> > resVec(res->begin(), res->end());
    sort(resVec.begin(), resVec.end(), &resultSort);

    out << '[';
    for (vector<std::pair<const WordForm *, unsigned int>>::iterator it = resVec.begin();
	 it != resVec.end(); ++it) {
	if (it != resVec.begin()) out << ", ";
	out << "{ \"dist\": " << it->second
	    << ", \"entry\": " << (it->first)->toJson() << "}";
    }
    out << ']';
}

// find corrections of a word. result is a json string 
string LexicalCorrector::findWordCorrected(const char *word, unsigned int maxdist) {
    map<const WordForm *, unsigned int> *res = cr->findWordCorrected(word, maxdist);
    ostringstream out;

    getWFs(out, res);
    return out.str();
}

// find corrections of a word with lowest distance 
string LexicalCorrector::findWordBest(const char *word, unsigned int maxdist) {
    map<const WordForm *, unsigned int> *res = cr->findWordBest(word, maxdist);
    ostringstream out;

    getWFs(out, res);
    return out.str();
}

