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
 Version:  2.0.0 as of 9th April 2020
*/

#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>

#include "LexicalEntry.h"
#include "Util.h"

using std::vector;
using std::istringstream;
using std::istream_iterator;
using std::ostringstream;

/** A wordform found in the lexicon
 * 
 * @param lexiconline
 * @param multipleEntries
 */
WordForm::WordForm(const string &lexiconline, bool multipleEntries) {
    if (multipleEntries) {
        istringstream iss(lexiconline);
        istream_iterator<string> begin(iss);
        istream_iterator<string> end;

        //putting all the tokens in the vector
        vector<string> tmp(begin, end);
        vector<string>::iterator vi = tmp.begin();
        form = *vi;

        //std::cerr << "eee " << tmp << std::endl;
        int s = tmp.size();
        if (s > 1 && s % 2 == 1) {
            ++vi;
            entries.reserve((tmp.size() - 1) / 2);
            while (vi != tmp.end()) {
                //LexicalEntry *le = new LexicalEntry();
                LexicalEntry le;

                le.lemma = *vi;
                ++vi;
                le.pos = *vi;
                ++vi;
                entries.push_back(le);
            }
        }
        tmp.clear();
    } else {
        // lire lexique produit avec acceslexique --dump --tab
        string::size_type space = lexiconline.find('\t');
        //std::cout << space << " " << lexiconline << std::endl;
        if (space == string::npos) {
            space = lexiconline.find(' ');
        }
        if (space != string::npos) {
            form = lexiconline.substr(0, space);
                                 
            //LexicalEntry *le = new LexicalEntry();
            LexicalEntry le;
            
            vector<string>elems;
            SplitNoEmptyFields(elems, lexiconline, "\t");
            if (elems.size() > 1) {
		// line is a TSV line
                le.lemma = elems[1];
		if (elems.size() > 2) {
		    le.pos = elems[2];
		    if (elems.size() > 3) {
			le.type = elems[3];
			if (elems.size() > 4) {
			    le.traits_m = elems[4];
			    if (elems.size() > 5) {
				le.traits_s = elems[5];
				if (elems.size() > 6) {
				    le.usems = elems[6];
				}
			    }
			}
		    }
		}
            } else {
		// line has form BLANK other info
                le.lemma = lexiconline.substr(space+1);
            }
            entries.push_back(le);
            //std::cout << "[" << form << "] [" << le->lemma << "]" << std::endl;
        } else {
            form = lexiconline;
        }
    }
}

WordForm::~WordForm() {
//    for (vector<LexicalEntry *>::iterator it = entries.begin(); it != entries.end(); ++it) {
//        delete *it;
//    }
}

bool LESort(const LexicalEntry & a,
	    const LexicalEntry & b) {  
    if (a.lemma == b.lemma) {
	if (a.pos < b.pos) return true;
    } else {
	if (a.lemma < b.lemma) return true;
    }
    return false;
}


string WordForm::toJson() const {
    ostringstream out;
    out << '{'
	<< "\"form\": \"" << form << "\","
	<< "\"lemmas\": [";
    vector<LexicalEntry>ee = entries;
    sort(ee.begin(), ee.end(), &LESort);
    for (vector<LexicalEntry>::const_iterator it = ee.begin(); it != ee.end(); ++it) {
	if (it != ee.begin()) out << ",";
        out << "{"<< "\"lemma\": \"" << (it)->lemma << "\"";
        if (!(it)->pos.empty()) out << ", \"pos\": \"" << (it)->pos << "\"";
        //if (!(it)->type.empty()) out << "\n\t\ttype: " << (it)->type;
        //if (!(it)->traits_m.empty()) out << "\n\t\tmorpho: " << (it)->traits_m;
        //if (!(it)->traits_s.empty()) out << "\n\t\tsyntax: " << (it)->traits_s;
        //if (!(it)->usems.empty()) out << "\n\t\tsem: " << (it)->usems;
	out << "}";
    }
    out << "]}";
    return out.str();
}



ostream& operator<<(ostream& out, const WordForm &wf) {
    out << wf.form;
    unsigned int ct = 1;
    vector<LexicalEntry>ee = wf.entries;
    sort(ee.begin(), ee.end(), &LESort);
    for (vector<LexicalEntry>::const_iterator it = ee.begin(); it != ee.end(); ++it, ++ct) {
        //out << " L:" << (*it)->lemma << " P:" << (*it)->pos;
        out << "\n\t"<< ct << " lemma: " << (it)->lemma;
        if (!(it)->pos.empty()) out << "\n\t\tpos: " << (it)->pos;
        if (!(it)->type.empty()) out << "\n\t\ttype: " << (it)->type;
        if (!(it)->traits_m.empty()) out << "\n\t\tmorpho: " << (it)->traits_m;
        if (!(it)->traits_s.empty()) out << "\n\t\tsyntax: " << (it)->traits_s;
        if (!(it)->usems.empty()) out << "\n\t\tsem: " << (it)->usems;
    }
    return out;
}

