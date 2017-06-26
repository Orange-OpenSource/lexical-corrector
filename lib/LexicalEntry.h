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

/* 
 * File:   LexicalEntry.h
 * currently for freeling style dictionaries
 * form lemma POS [lemma POS [...]]
 */

#ifndef __LEXICALENTRY_H__
#define __LEXICALENTRY_H__

#include <iostream>
#include <string>
#include <vector>


using std::vector;
using std::ostream;
using std::string;

#include "Util.h"

class LexicalEntry {
public:
    string lemma;
    string pos;
    string type;
    string traits_m;
    string traits_s;
    string usems;
    
    LexicalEntry () {
    }
    
    LexicalEntry (const LexicalEntry &le) {
        //copy of constructor
        lemma = le.lemma;
        pos = le.pos;
        type = le.type;
        traits_m = traits_m;
        traits_s = le.traits_s;
        usems = usems;
    }
    
};


class WordForm {
public:
    /**
     * 
     * @param lexiconline line read from file
     * @param multipleEntries if true, we expect a line like "form lemma cat lemma cat ..." else lines are "form description"
     */
    WordForm(const string &lexiconline, bool multipleEntries = true);
    ~WordForm();
    string form;
    vector<LexicalEntry>entries;
    
   friend ostream& operator<<(ostream& out, const WordForm &wf);
   
   /** copy tje lexical entry to this word form */
   void addLexicalEntry(LexicalEntry &le) {
       entries.push_back(le);
   }
   void addLexicalEntry(const vector<LexicalEntry > &le) {
       entries.insert(entries.end(), le.begin(), le.end());
   }
};


#endif /* LEXICALENTRY_H */

