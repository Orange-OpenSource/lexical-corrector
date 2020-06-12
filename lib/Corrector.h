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
 Version:  2.2.0 as of 12th June 2020
*/

#ifndef __CORRECTOR_H__
#define __CORRECTOR_H__


#include <stdint.h>

#include "Arbre.h"
#include "Calculator.h"

/** correcteur qui utilise l'arbre binaire.
    il faut un correcteur par thread
 */
class Corrector {
public:
    /**
     * Créer un correcteur (par exemple dans un thread)
     * @param ab l'arbre binaire construite avec le lexique
     */
    Corrector(const ArbreBinaire *ab);
    ~Corrector();

    /**
     * trouver un mot sans correction
     * @param w mot à trouver
     * @return l'entrée du lexique ou 0 
     */
    const WordForm *findWordExact(const char *word);

    /** chercher un mot a corriger. Le(s) résultats se trouvent dans le Calculator	
       @param word le mot
       @param maxdist la distance Levenshtein maximale
       @return pointer sur map avec les résultats dans le calculateur (peut être vide) 
     */
    map<const WordForm *, distancetype> * findWordCorrected(const char *word, distancetype maxdist);

    /** chercher un mot exact ou à corriger si le mot exact n'existe pas. Le(s) résultats se trouvent dans le Calculator	
       @param word le mot
       @param maxdist la distance Levenshtein maximale
       @return pointer sur map avec les résultats dans le calculateur (peut être vide) 
     */
    map<const WordForm *, distancetype> * findWordBest(const char *word, distancetype maxdist);
    
    inline void setCorrectionType(unsigned int t) {
        calculator->setCorrectionType(t);
    }
    
    /** rendre la taille du mot (UTF8) en nombre des charactères unicode
      @param word le mot (en UTF8)
     */
    unsigned int unicodePoints(const char *word);
    
    inline unsigned int getCorrectionType() {
        return calculator->getCorrectionType();
    }
    
private:
    /**
     * initaliser le calculator avant chaque recherche
     */
    void init();

    /// l'arbre binaire avec les données (en lecture)
    const ArbreBinaire *arbre;
    /// le calculateur qui fait les calculations de la distance Levenshtein et qui stocke les résultats
    Calculator *calculator;
    /// keeps lexicon words in UCS4 format during search
    uint32_t *ucs4buffer; 
    
  
};

#endif
