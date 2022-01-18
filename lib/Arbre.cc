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
 Version:  2.3.2 as of 17th Janauary 2022
*/


#include <cstring>
#include <fstream>
#include <sstream>
#include <time.h>

#include "Arbre.h"
#include "Noeud.h"
#include "LexicalEntry.h"

using namespace std;

/* uniquement l'arbre utilise des int32_t pour les caractères
 * On lit et ecrit UTF8 (stocké non-interpré dans des char * ou string
 * 
 */

const int maxUCS = 2000;

ArbreBinaire::ArbreBinaire(const char *lexfile, const char *similarletters, bool multipleEntries, set<int> *columns) {
    // allouer autant de Noeud que nécessaire

 
// #ifdef WITHACCENTS
//      // initialiser les groupes des letters similaires
//     Noeud::initLetterGroups();
// #endif


#ifdef SIMILIARLETTERS
    if (similarletters) {
        Noeud::similar = new SimilarLetters(similarletters, Calculator::penalty);
        //cout << *Noeud::similar << endl;
	Noeud::reduplicationPenalty = Noeud::similar->getReduplPen();
	Noeud::transpositionPenalty = Noeud::similar->getTranspPen();
    }
#endif    
    
    root = new Noeud(0);
    maxwordlength = 0;
    countforms = 0;
    ucs4buffer = new uint32_t [maxUCS];

    if (lexfile) {
        ifstream ifp(lexfile);
        if (!ifp) {
            cerr << "cannot open " << lexfile << endl;
        } else {
            time_t start = time(0);

            string line;
            while (!ifp.eof()) {
                getline(ifp, line);
                //ct++;
                if (!line.empty()) {
                    this->addWord(line, multipleEntries, columns);
                }
                //if (ct % 10000 == 0) cout << ct << " lines read" << endl;
            }
            time_t end = time(0);
            cerr << "loading time: " << (end - start) << "s" << endl;
            ifp.close();
            cerr << "Tree: nodes: " << Noeud::ndct << " of size " << sizeof (Noeud) << "),"
                    << " maximal Word Length: " << this->getMaxwordlength()
                    << ", words: " << this->getCountforms() << endl;
        }
    }
    delete [] ucs4buffer;
}

ArbreBinaire::~ArbreBinaire() {
    delete root;
    //delete [] ucs4buffer;
#ifdef SIMILIARLETTERS 
    delete Noeud::similar;
#endif
    //cerr << "** delete AbreBinaire " << this << endl;
}

void ArbreBinaire::dot(ostream &out) {
    out << "digraph\n{\n" << endl;
    root->dot(out);
    out << "}" << endl;
}

ostream& operator<<(ostream& out, const ArbreBinaire& a) {
    if (a.root)
        out << *a.root << endl;
    return out;
}

WordForm * ArbreBinaire::findWordExact(const Character *word) const {
    Noeud *n = root;
    const Character *p = word;

    bool found = false;
    while (*p) {
        //cout << "Char: " << *p << endl;// << *n << endl;
        Character key = *p;
        if (n->left && n->left->key_value == key) {
            n = n->left;
            //cout << "LEFT:" << *n << endl;
            p++;
        } else if (n->left && n->left->right) {
            found = false;
            Noeud *x = n->left->right;
            //cout << "X: " << *x << endl;
            while (x != 0) {
                //cout << "X: " << *x << " " << found << endl;
                if (x->key_value == key) {
                    n = x;
                    //cout << "RIGHT:" << *n << endl;
                    p++;
                    found = true;
                    break;
                }
                x = x->right;
            }
            if (!found) break; // nothing found
            //cout << "broke" << endl;
        } else {
            //cout << "break" << endl;
            break;
        }
    }
    if (*p == 0 && n->info != 0) {
        // on a parcouru l'arbre
        //cout << word << " found " << n->info << endl;
        return n->info;
    }
    return 0;
}

void ArbreBinaire::findWordCorrected(const Character *word, const distancetype maxdist, Calculator *table) const {
    // trouver mot avec acces de correction (Levenshtein-Damerau)
    parcourir(word, maxdist, table);
}

void ArbreBinaire::parcourir(const Character *word, const distancetype maxdist, Calculator *table) const {
    table->push(root->key_value);
    root->left->parcourir(word, maxdist, table, 1);
}


#undef DEBUG

void ArbreBinaire::addWord(const string &lexiconline, bool multipleEntries, set<int> *columns) {
    Noeud *n = root;

    WordForm *wf = new WordForm(lexiconline, multipleEntries, columns);
    maxwordlength = maxwordlength < wf->form.size() ? wf->form.size() : maxwordlength;
    countforms++;

#ifdef DEBUG
    int ucslength = 
#endif
        u8_toucs(ucs4buffer, maxUCS, wf->form.c_str(), -1);

#ifdef DEBUG
    cout << "ADDWORD " << wf->form << endl;
    for (int i = 0; i < ucslength; ++i) {
        cout << " 0x" << hex << ucs4buffer[i];
    }
    cout << dec << endl;
#endif
    for (const Character *p = ucs4buffer; *p != 0; ++p) {
        //for (const char *p = wf->form.c_str(); *p != 0; ++p) {
#ifdef DEBUG
        cout << "+ " << *p << endl;
#endif
        n = insert(*p, n);
    }

#ifdef DEBUG
    cout << "__" << *n << endl;
#endif

    //n->info = wf;
    if (n->info == 0) n->info = wf;
    else {
        n->info->addLexicalEntry(wf->entries);
        delete wf;
    }
}

Noeud *ArbreBinaire::insert(Character key, Noeud *n) {
    if (n->left == 0) {
        n->left = new Noeud(key);
        //cout << "A: " << *n->left << endl;
        return n->left;
    } else if (n->left->key_value == key) {
        //cout << "C: " << *n->left << endl;
        return n->left;
    }
        // different letter, add right node
    else if (n->left->right == 0) {
        n->left->right = new Noeud(key);
        //cout << "B: " << *n->left->right << endl;
        return n->left->right;
    }// there is already a right node: go through chain until we find our letter
    else {
        Noeud *x = n->left->right;
        while (true) {
            if (x->key_value == key) {
                return x;
            }
            if (x->right == 0) {
                x->right = new Noeud(key);
                return x->right;
            }

            x = x->right;
        }
        //cout << "ret " << *x << endl;
        return x;
    }
}

