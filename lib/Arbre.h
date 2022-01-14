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
 Version:  2.3.0 as of 14th Janauary 2022
*/

#ifndef __ARBRE_H
#define __ARBRE_H

#include <iostream>
#include <string>


#include "Noeud.h"
#include "Calculator.h"
#include "Util.h"

using std::ostream;
using std::string;

const string __version__ = "2.3.0";

class ArbreBinaire {
 public:
    /**
     * Créer un arbre binaire à partir d'un fichier de lexique
     * 
     * @param lexfile a file containing the forms of the lexicon (one entry per line)
     * @param similarletters a file defining which letters are considered similar (gives lower penality)
     * @param multipleEntries if true, we expect a line like "form lemma POS lemma POS ..." (space separated) else lines are "form [lemma [POS [type [morpho features [syntactic feature [semantics]]]]]] (tab separated)"
     */
    ArbreBinaire(const char *lexfile = 0, const char *similarletters = 0, bool multipleEntries = true);
    ~ArbreBinaire();

    /** rajouter un mots dans l'arbre 
     * @param lexiconline la ligne du lexique lu
     * @param multipleEntries if true, we expect a line like "form lemma cat lemma cat ..." else lines are "form description"
     */
    void addWord(const string &lexiconline, bool multipleEntries = true);
    
 
    /**
     * produire un fichier dot avec l'arbre (attention : ne pas faire pour des grands arbre !!)
     * @param out nom du fichier dot
     */
    void dot(ostream &out);
    
    /** lancer la rechercher avec correction dans l'arbre
     * 
     * @param word le mot à retrouver (ou une correction)
     * @param maxdist la distance de Levenshtein maximale permise
     * @param calc le Calculateur
     */
    void parcourir(const Character *word, distancetype maxdist, Calculator *calc) const;
    unsigned int getMaxwordlength() const { return maxwordlength; }
    unsigned int getCountforms() const { return countforms; }
    friend ostream& operator<<(ostream& out, const ArbreBinaire& a);

    const string& getVersion() const {
	return __version__;
    }
   
 private:
     // les instance de Corrector sont des amis et peuvent appeller les deux méthodes privées pour trouver des mots
     friend class Corrector;
     /**
     * trouver un mot sans correction. Cette méthode est privée, elle est appelé par une instance du
     * correcteur (ArbreBinaire est un friend)
     * @param w mot à trouver
     * @return l'entrée du lexique ou 0 
     */
    WordForm *findWordExact(const Character *w) const;

    /** chercher un mot a corriger. Le(s) résultats se trouvent dans le Calculator	
	@param word le mot
	@param maxdist la distance Levenshtein maximale
	@param le tableau pour calculer les résultats (préparer par le thread dans Corrector)
    */
    void findWordCorrected(const Character *word, const distancetype maxdist, Calculator *calc) const;
    
     
     /** mettre un nouveau moeud dans l'arbre
      * 
      * @param key le charactère du noeu
      * @param leaf le nouveau noeud
      * @return le nouveau noeud
      */
    Noeud *insert(Character key, Noeud *leaf);
    
    /// la racine de l'arbre (cet noeud ne contient un left sur la première lettre))
    Noeud *root;
    unsigned int maxwordlength;
    unsigned int countforms;
    
    // keeps lexicon words in UCS4 format during insertion, not used afterwards
    uint32_t *ucs4buffer; 
};

#endif
