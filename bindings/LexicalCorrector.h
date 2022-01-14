/** This library is under the 3-Clause BSD License

Copyright (c) 2017-2022, Orange S.A.

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
 Version:  2.3.0 as of 14th January 2022
*/


#ifndef __LEXICAL_CORRECTOR_H__
#define __LEXICAL_CORRECTOR_H__

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "Arbre.h"
#include "Corrector.h"


class LexicalCorrector {
 public:
    // create lexical corrector with all data
    LexicalCorrector(const char *lexfile, const char *similarletters, int multipleEntries);
    // create empty lexical corrector (AbreBinaire and Corrector must be created elsewhere and given to find*() commands
    LexicalCorrector();

    ~LexicalCorrector();
    
    
    // returns the correct word or an empty string
    string findWordExact(const char *word, Corrector *c = 0) const;

    // find corrections of a word. result is a json string 
    string findWordCorrected(const char *word, distancetype maxdist, Corrector *c = 0) const;

    // find corrections of a word with lowest distance 
    string findWordBest(const char *word, unsigned int maxdist, Corrector *c = 0) const;

    const string& getVersion() const;

 private:
    // helper function to create json output
    void getWFs(std::ostream &out, map<const WordForm *, distancetype> *res) const;

    ArbreBinaire *ab = 0;
    Corrector *cr = 0;

};

#endif

