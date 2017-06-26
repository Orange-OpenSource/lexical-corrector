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

 @author Johannes Heinecke
 @version 1.0 as of 6th April 2017
*/

package com.orange.labs.lexicon;

import java.util.Map;


public class Corrector {

    private final ArbreBinaire arbre;
    private final Calculator calc;

    /**
     * Créer un correcteur (par exemple dans un thread)
     * @param ab l'arbre binaire construite avec le lexique
     */
    public Corrector(ArbreBinaire a) {
        arbre = a;
        calc = new Calculator(arbre.maxwordlength + 10);
    }

    public void setDefaultPenalty(int d) {
        calc.setDefaultPenalty(d);
    }
    
      public int getDefaultPenalty() {
        return calc.getDefaultPenalty();
    }
    
    /** chercher un mot a corriger. Le(s) résultats se trouvent dans le Calculator	
     @param word le mot
     @param maxdist la distance Levenshtein maximale
     @return pointer sur map avec les résultats (peut être vide mais pas null) 
     */
    public Map<WordForm, Integer> findWordCorrected(String word, int maxdist) {
        calc.init();
        //int ucslength = 

        arbre.findWordCorrected(word, maxdist, calc);
        return calc.getresults();
    }

    /** trouver un mot sans correction
     @param word mot à trouver
     @return l'entrée du lexique ou null
     */
    public WordForm findWordExact(String word) {
        //int ucslength = 

        return arbre.findWordExact(word);
    }

    /** chercher un mot exact ou à corriger si le mot exact n'existe pas. Le(s) résultats se trouvent dans le Calculator	
     @param word le mot
     @param maxdist la distance Levenshtein maximale
     @return pointer sur map avec les résultats dans le calculateur (peut être vide) 
     */
    public Map<WordForm, Integer> findWordBest(String word, int maxdist) {
        calc.init();
        WordForm wf = arbre.findWordExact(word);
        if (wf != null) {           
            calc.addResult(wf, 0);
            return calc.getresults();
        }

        arbre.findWordCorrected(word, maxdist, calc);
        return calc.getresults();
    }
}
