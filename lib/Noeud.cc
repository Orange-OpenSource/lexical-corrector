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

// TODO conf pour le score de reduplication
	


#include <cstring>
#include <sstream>

#include "Noeud.h"
#include "Calculator.h"

using namespace std;

//#define WITH_CASEINSENSITIVE

#ifdef WITH_CASEINSENSITIVE
#include <unicase.h>
#endif
        

unsigned int Noeud::ndct = 0;
int Noeud::reduplicationPenalty = 900;
int Noeud::transpositionPenalty = 900;


Noeud::Noeud(Character kv) : key_value(kv) {
    left = 0;
    right = 0;
    info=0;
    id = ndct++;

#ifdef DEBUG
    char utf[10];
    u8_wc_toutf8(utf, key_value);
    cout << "NOEUD " << id << " " << utf << endl;
#endif
     
}

Noeud::~Noeud() {
    //cout << "deleting " << id << " " << (char)key_value << endl;
    delete left;
    delete right;
    delete info;
}

ostream& operator<<(ostream& out, const Noeud& n) {
    char utf[10];
    u8_wc_toutf8(utf, n.key_value);
    out << n.id << ":" <<  hex << n.key_value << " '" << utf << "'";
    if (n.info) out << '"' << n.info->form << '"';
    if (n.left) cout << " L:" << (char) n.left->key_value;
    if (n.right) cout << " R:" << (char) n.right->key_value;

    //out << endl;

    //if (n.left) out << *n.left << endl;
    //if (n.right) out << *n.right << endl;

    if (n.left) out << "(" << n.id << ":" << *n.left << ")" << n.id << ":";
    if (n.right) out << "[" << n.id << ":" << *n.right << "]" << n.id << ":";

    return out;
}



void Noeud::dot(ostream & out) {
    ostringstream name;
    char utf[10];
    if (key_value == 0) name << "zero";
    else {
        u8_wc_toutf8(utf, key_value);
        //name << id << " (" << (Character) key_value << ")";
        name << id << " (" << utf << ") x" << hex << key_value ;
    }


    if (info) {
        name << "\\n" << info->form;
    }
    if (left) {
        ostringstream lname;
        u8_wc_toutf8(utf, left->key_value);
        //lname << left->id << " (" << (Character) left->key_value << ")";
        lname << left->id << " (" << utf << ") x"<< hex << left->key_value;
        if (left->info) lname << "\\n" << left->info->form;

        out << "\t" << '"' << name.str() << '"'
                << " -> " << '"' << lname.str() << "\" [label=\"suite\"];" << endl;
        left->dot(out);
    }
    if (right) {
        ostringstream rname;
        u8_wc_toutf8(utf, right->key_value);
        //rname << right->id << " (" << (Character) right->key_value << ")";
        rname << right->id << " (" << utf << ") x"<< hex << right->key_value; 
        if (right->info) rname << "\\n" << right->info->form;
        out << "\t" << '"' << name.str() << '"'
                << " -> " << '"' << rname.str() << "\" [label=\"alt\"];" << endl;
        right->dot(out);
    }
}

//#define DEBUG

void Noeud::parcourir(const Character *word, const unsigned int maxdist, Calculator *calc, unsigned int depth) {
    calc->push(key_value);
#ifdef DEBUG 
    cout << "KEY " << id << " " << (Character) key_value << " depth:" << depth << endl;
    calc->printstack(cout);
#endif    
    const Character *p = word;
    unsigned int row = calc->stacksize() - 1;
    unsigned int col = 1;
    unsigned int similarpenalty;
    while (*p) {
        
        Character key = *p;
#ifdef DEBUG         
        cout << "ROW " << row << " COL " << col << " " << *p << " " << endl;
#endif
        // inserting values in current row

        int cost = Calculator::penalty; 
    
        if ((calc->getCorrectionType() & CORRECT_TYPOS) == 0) {
            // on ne corrige pas des typos           
            cost = maxdist+1;
        }

        //if (key != key_value) {
	//    cout << "key " << key << " p-1 " << *(p-1) << " " << p << " "  << word << endl;
	//}


        //else if (p > word && *(p-1) == key_value && key == key_value) {
	//    cout << "eeee " << key << endl;
	//  cost = 333;
	//} else


        if (key == key_value) cost = 0;
        

        
#ifdef SIMILIARLETTERS 
        else if (Noeud::similar && Noeud::similar->areSimilar(&similarpenalty, key, key_value)) {
             if (calc->getCorrectionType() & CORRECT_DIACRITICS) {
                cost = similarpenalty;
            } else {
                // pas de correction accents
                cost = Calculator::penalty+1;
            }
        }
#endif        
        
        
#ifdef WITH_CASEINSENSITIVE
        else {
            int result = 0;
            u32_casecmp(&key, 1, &key_value, 1, "en", 0, &result);
            if (result == 0) { 
                if (calc->getCorrectionType() & CORRECT_CASE) {
                    cost = Calculator::penalty/100;
                } else {
                    cost = Calculator::penalty+1;
                }
            }

        }
#endif
        //cout << "cc " << cost << endl;
        // TODO si la différence est uniquement la suppression/ajout d'un accent on prend penalty/2
        unsigned short val = calc->minNeighbour(row, col, cost);

#define DAMERAU
#ifdef DAMERAU
        if (row > 1 && col > 1 
            && key == calc->getPenultime() // a[i] = b[j-1]
            && *(p-1) == key_value) { // a[i-1] = b[j]
            //unsigned int a = calc->get(row-2, col-2) + (cost*.9); // transposition de deux caractères
	    unsigned int a = calc->get(row-2, col-2) + (transpositionPenalty); // transposition de deux caractères
            if (a<val) val=a;
        }
#endif
        //cout << "cd " << cost << endl;
        calc->set(row, col, val);
#ifdef DEBUG         
        cout << "COST: " << cost << " r:" << row << " c:" << col << " v:" << val << endl;
#endif
        ++p;
        ++col;
        //prevcol++;
    }

#ifdef DEBUG 
    calc->status(cout, word, row + 1, col - 1);
#endif

    // pas la peine de continuer si la distance > maxdist
    //unsigned short curdist = table->get(row, depth  < col - 1 ? depth  : col - 1);
    unsigned short lowest = calc->lowest(row, col);
#ifdef DEBUG 
    cout << "LOWEST " << lowest << " depth:" << depth << " r:" <<row << " c:" << col-1 << " maxdist" << maxdist << endl;
#endif

    if (lowest <= maxdist) {
        // on continue...
#ifdef DEBUG 
        cout << "continue" << endl;
#endif
        unsigned short curdist = calc->get(row, col - 1);
        if (curdist <= maxdist && info) {
#ifdef DEBUG
            cout << "FOUND " << *info << " " << curdist << endl;
#endif
            calc->addResult(info, curdist); // il y a un mot
        }
        if (left) {
            left->parcourir(word, maxdist, calc, depth+1);
        }
    }
    calc->pop();
    if (right) {
        //cout << "alternative" << endl;
        right->parcourir(word, maxdist, calc, depth);
    }

}

#ifdef SIMILIARLETTERS 
SimilarLetters *Noeud::similar = 0;
#endif


// #ifdef WITHACCENTS
// map<Character, char> Noeud::letterGroups; // letters which are similar
// bool Noeud::identicalWithoutDiacritics(const Character a, const Character b) {
//     map<Character, char>::iterator itA = Noeud::letterGroups.find(a);
//     if (itA != Noeud::letterGroups.end()) {
//         map<Character, char>::iterator itB = Noeud::letterGroups.find(b);
//         if (itB != Noeud::letterGroups.end()) {
//             if (itA->second == itB->second) return true;
//         }
//     }
//     return false;
    
//     // alternative mettre Character a et b dans un long le Chracter "plus petit" en premiere position (aà)
//     // et mettre ça dans une map<long, penalty>
// }

// void Noeud::initLetterGroups() {
//     if (!Noeud::letterGroups.empty()) return;
//     // TODO mettre uint32_t dans second ?
//     Noeud::letterGroups[0x41] = 'A';
//     Noeud::letterGroups[0xc0] = 'A';
//     Noeud::letterGroups[0xc1] = 'A';
//     Noeud::letterGroups[0xc3] = 'A';
//     Noeud::letterGroups[0xc4] = 'A';
//     Noeud::letterGroups[0xc5] = 'A';

//     Noeud::letterGroups[0x43] = 'C';
//     Noeud::letterGroups[0xc7] = 'C';

//     Noeud::letterGroups[0x45] = 'E';
//     Noeud::letterGroups[0xc8] = 'E';
//     Noeud::letterGroups[0xc9] = 'E';
//     Noeud::letterGroups[0xCa] = 'E';
//     Noeud::letterGroups[0xcb] = 'E';

//     Noeud::letterGroups[0x49] = 'I';
//     Noeud::letterGroups[0xcc] = 'I';
//     Noeud::letterGroups[0xcd] = 'I';
//     Noeud::letterGroups[0xcd] = 'I';
//     Noeud::letterGroups[0xcf] = 'I';

//     Noeud::letterGroups[0x4e] = 'N';
//     Noeud::letterGroups[0xd1] = 'N';

//     Noeud::letterGroups[0x4f] = 'O';
//     Noeud::letterGroups[0xd2] = 'O';
//     Noeud::letterGroups[0xd3] = 'O';
//     Noeud::letterGroups[0xd4] = 'O';
//     Noeud::letterGroups[0xd5] = 'O';
//     Noeud::letterGroups[0xd6] = 'O';

//     Noeud::letterGroups[0x55] = 'U';
//     Noeud::letterGroups[0xd9] = 'U';
//     Noeud::letterGroups[0xda] = 'U';
//     Noeud::letterGroups[0xdb] = 'U';
//     Noeud::letterGroups[0xdc] = 'U';

//     Noeud::letterGroups[0x59] = 'Y';
//     Noeud::letterGroups[0xdd] = 'Y';


//     Noeud::letterGroups[0x61] = 'a';
//     Noeud::letterGroups[0xe0] = 'a';
//     Noeud::letterGroups[0xe1] = 'a';
//     Noeud::letterGroups[0xe2] = 'a';
//     Noeud::letterGroups[0xe3] = 'a';
//     Noeud::letterGroups[0xe4] = 'a';
//     Noeud::letterGroups[0xe5] = 'a';

//     Noeud::letterGroups[0x63] = 'c';
//     Noeud::letterGroups[0xe7] = 'c';

//     Noeud::letterGroups[0x65] = 'e';
//     Noeud::letterGroups[0xe8] = 'e';
//     Noeud::letterGroups[0xe9] = 'e';
//     Noeud::letterGroups[0xea] = 'e';
//     Noeud::letterGroups[0xeb] = 'e';

//     Noeud::letterGroups[0x69] = 'i';
//     Noeud::letterGroups[0xec] = 'i';
//     Noeud::letterGroups[0xed] = 'i';
//     Noeud::letterGroups[0xee] = 'i';
//     Noeud::letterGroups[0xef] = 'i';

//     Noeud::letterGroups[0x6e] = 'n';
//     Noeud::letterGroups[0xf1] = 'n';

//     Noeud::letterGroups[0x6f] = 'o';
//     Noeud::letterGroups[0xf2] = 'o';
//     Noeud::letterGroups[0xf3] = 'o';
//     Noeud::letterGroups[0xf4] = 'o';
//     Noeud::letterGroups[0xf5] = 'o';
//     Noeud::letterGroups[0xf6] = 'o';

//     Noeud::letterGroups[0x75] = 'u';
//     Noeud::letterGroups[0xf9] = 'u';
//     Noeud::letterGroups[0xfa] = 'u';
//     Noeud::letterGroups[0xfb] = 'u';
//     Noeud::letterGroups[0xfc] = 'u';

//     Noeud::letterGroups[0x79] = 'y';
//     Noeud::letterGroups[0xff] = 'y';
// }
// #endif
