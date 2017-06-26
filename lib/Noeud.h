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

#ifndef __NOEUD_H__
#define	__NOEUD_H__


#include <iostream>
#include <string>
#include <map>

#include "Util.h"
#include "Calculator.h"
#include "SimilarLetters.h"

using std::ostream;
using std::string;
using std::map;

const unsigned int CORRECT_TYPOS =      0x01;
const unsigned int CORRECT_DIACRITICS = 0x02;
const unsigned int CORRECT_CASE =       0x04;

const unsigned int CORRECT_ALL = CORRECT_TYPOS | CORRECT_DIACRITICS | CORRECT_CASE;

//#define WITHACCENTS
#define SIMILIARLETTERS

class Noeud {
 public:
    /**
     * créer un nouveau noeud pour lun caractère 
     * @param kv caractère du noeud
     */
    Noeud(Character kv);
    ~Noeud();
    
    /**
     * pour sortir l'arbre pour dot
     * @param out flux du fichier dot
     */
    void dot(ostream &out);
    
    /**
     * Parcourir ce noeud en recherche d'un mot à corriger
     * @param word le mot pour lequelle on cherche une correction
     * @param maxdist distance Levenshtein maximale permise
     * @param calc le calculateur
     * @param depth profondeur actuelle (de recursion)
     */
    void parcourir(const Character *word, const unsigned int maxdist, Calculator *calc, unsigned int depth);
    
    friend ostream& operator<<(ostream& out, const Noeud& n);

    /// caractèer du noeud
    Character key_value;
    /// Noeud du caractère suivant
    Noeud *left; 
    /// alternative du noeud actuel
    Noeud *right; 
    
    /// les autres infos sur l'entree lexicale 
    WordForm *info; 
    
    /// identifian unique
    unsigned int id;
    
    /// compteur (statique ! crée des numéros différent si on a plusieurs arbre en parallèle)
    static unsigned int ndct; // count nodes
    
    //#ifdef WITHACCENTS
    //    /// pour pouvoir compter les erreurs d'accents différement
    //    static map<Character, char>letterGroups; // Character: "e" pour savoir si deux lettres sont similaires
    //
    //    /** initialiser le tableau des lettres "proches" */
    //    static void initLetterGroups(); 
    //       
    //    /**
    //     * comparer deux caractères pour voir si ils sont "similaire"
    //     * @param a
    //     * @param b
    //     * @return true si c'est comme 'é' et 'e' ou 'â' et 'ä', sinon false
    //     */
    //    static bool identicalWithoutDiacritics(const Character a, const Character b);
    //#endif 
    
#ifdef SIMILIARLETTERS 
    static SimilarLetters *similar;
#endif

    static int reduplicationPenalty;
    static int transpositionPenalty;
};



#endif	/* __NOEUD_H__ */

