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


#include <iostream>
#include <vector>
using namespace std;

#include "Calculator.h"
#include "Noeud.h"

unsigned int Calculator::penalty = 1000;

Calculator::Calculator(unsigned int size) {
    tsize = size;

    tmp = new unsigned [tsize * tsize];
    table = new unsigned * [tsize];
    for (unsigned int i=0; i<tsize; ++i) {
	table[i] = &tmp[tsize*i];
    }    
    correctiontype = CORRECT_ALL;
}

Calculator::~Calculator() {
    delete [] tmp;
    delete [] table;
}

void Calculator::init() {
    for (unsigned int col=0; col < tsize; ++col) {
	table[0][col] = col*penalty;
	for (unsigned int row=1; row < tsize; ++row) {
	    table[row][col] = 0;
	}
    }

    for (unsigned int row=1; row < tsize; ++row) {
	table[row][0] = row*penalty;
    }
    stack.clear();
    solutions.clear();
}


//#define DAMERAU
unsigned int Calculator::minNeighbour (unsigned int row, unsigned int col, int cost) {
#ifdef DEBUG
    cout << "minneighbour " << row << " " << col << endl;
#endif
    unsigned x = table[row][col-1]+Calculator::penalty; // insertion
    unsigned y = table[row-1][col]+Calculator::penalty; // deletion
    unsigned z = table[row-1][col-1]+cost; // substitution
    
    return (x <= y) ? (x <= z) ? x : z : (y <= z)? y : z;
}


// la valeur la plus basse de la ligne row
unsigned Calculator::lowest(unsigned int row, unsigned int maxcol) {
    unsigned min = 1000000;
    //cout << "ROW: " << row << endl;
    for (unsigned int col=0; col < maxcol; ++col) {
	min = table[row][col] < min ? table[row][col] : min;
	//cout << col << " " << table[row][col] << " min: " << min << endl;
    }    
    return min;
}

ostream& operator<<(ostream& out, const Calculator& t) {
    for (unsigned int row=0; row < t.tsize; ++row) {
	for (unsigned int col=0; col < t.tsize; ++col) {
	    out << t.table[row][col] << "\t";
	    if (col == 0) out << "\t";
	}
	out << endl;
	if (row == 0) out << endl;
    }
    return out;
}

void Calculator::status(ostream &out, const Character *word, unsigned maxrow, unsigned maxcol) {
    char utf[5];
    for (unsigned int col=0; col < maxcol; ++col) {
	// pour toutes les 
	if (col == 0) out << "\t";
        u8_wc_toutf8(utf, word[col]);
	out << "\t" << utf; //word[col];
    }
    out << endl;

    for (unsigned int row=0; row < maxrow; ++row) {
	// pour toutes les lignes
	for (unsigned int col=0; col <= maxcol; ++col) {
	    // pour toutes les colonnes
             u8_wc_toutf8(utf, stack[row]);
	    if (col == 0) out << (stack[row] == 0 ? "0" : utf) << '\t';
	    out << table[row][col] << "\t";
	    //if (col == 0) cout << "\t";
	}
	out << endl;
	if (row == 0) out << endl;
    }
}
