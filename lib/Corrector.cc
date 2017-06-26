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

#include "Corrector.h"

using namespace std;

const int maxUCS = 1000; // max word length (in unicode characters)
Corrector::Corrector(const ArbreBinaire *ab) : arbre(ab) {
    // tableau pour la correction Levenshtein
    // il faut avoir deux dimensions, sur x les lettres du mot recherché
    // sur y les mettres sorties de l'arbre
    // 
    //		mot du texte
    //		l	e	x	0

    //	0	1	2	3	4
    //l	1	0	1	2	3
    //e	2	1	0	1	2
    //s	3	2	1	1	2
    //0	4	3	2	2	1 <-- faute de 1 parce que c'est le minimum apres avoir vu tout

    calculator = new Calculator(ab->getMaxwordlength()+10);
    ucs4buffer = new uint32_t [maxUCS];
   
}



Corrector::~Corrector() {
    delete calculator;
    delete [] ucs4buffer;
}

unsigned int Corrector::unicodePoints(const char *word) {
    return u8_toucs(ucs4buffer, maxUCS, word, -1);
}

map<const WordForm *, unsigned int> *Corrector::findWordCorrected(const char *word, unsigned int maxdist) {    
    calculator->init();
    //int ucslength = 
    u8_toucs(ucs4buffer, maxUCS, word, -1);
    
    arbre->findWordCorrected(ucs4buffer, maxdist, calculator);
    return calculator->getresults();
}

const WordForm *Corrector::findWordExact(const char *word) {
    //int ucslength = 
    u8_toucs(ucs4buffer, maxUCS, word, -1);
    return arbre->findWordExact(ucs4buffer);
}

 map<const WordForm *, unsigned int> * Corrector::findWordBest(const char *word, unsigned int maxdist) {
    calculator->init();
    u8_toucs(ucs4buffer, maxUCS, word, -1);
    WordForm *wf = arbre->findWordExact(ucs4buffer);
    if (wf != 0) {
        // on a trouvé un mot
        calculator->addResult(wf, 0);
        return calculator->getresults();
    } 
        
    arbre->findWordCorrected(ucs4buffer, maxdist, calculator);
    return calculator->getresults();
 }
