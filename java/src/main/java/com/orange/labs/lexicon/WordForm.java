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

import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author Johannes Heinecke <johannes.heinecke@orange.com>
 */
public class WordForm implements Comparable {
    String form;
    List<LexicalEntry> entries;

    /** une ligne du lexique à lire
    
    @param lexiconline fichier lexique
    @param multipleLemmasPerLine si vrai on attend des lignes à la freeling: forme POS1 lemme1 POS2 lemme2 ...
    si non on attend un lexique avec une entrée par ligne: forme [POS [lemme]]
    */
    public WordForm(String lexiconline, boolean multipleLemmasPerLine) {
        if (!multipleLemmasPerLine) {
            // ligne: forme [POS [lemme]]

            //form = lexiconline;
            String[] elems = lexiconline.split("[ \t]+");
            form = elems[0];
           
            if (elems.length > 1) {
                entries = new ArrayList<>();
                if (elems.length > 2)
                    entries.add(new LexicalEntry(elems[1], elems[2]));
                else
                    entries.add(new LexicalEntry(elems[1], null));
            }
        } else {
            // on pense que la ligne lexicale est forme POS lemme POS Lemme ...
            String[] elems = lexiconline.split("[ \t]+");
            form = elems[0];

            if (elems.length > 1 && elems.length % 2 == 1) {
                entries = new ArrayList<>();
                for (int i = 1; i < elems.length; i += 2) {
                    entries.add(new LexicalEntry(elems[i], elems[i + 1]));
                }
            }
        }
    }

    public String getForm() {
        return form;
    }

    public List<LexicalEntry> getEntries() {
        return entries;
    }

    public void merge(WordForm wf2) {
        entries.addAll(wf2.getEntries());
    }
    
    public String toString() {
        StringBuilder sb = new StringBuilder(form);
        if (entries != null) {
            sb.append(" [");
            boolean first = true;
            for (LexicalEntry le : entries) {
                if (!first) sb.append(", "); else first = false;
                sb.append(le.lemma);
                if (le.pos != null) sb.append(" ").append(le.pos);
            }
            sb.append("]");
        }
        return sb.toString();
    }

    @Override
    public int compareTo(Object o) {
        if (o instanceof WordForm) {
            return -1;
        } else {
            return form.compareTo(((WordForm) o).form);
        }
    }

    public class LexicalEntry {
        String lemma;
        String pos;

        public LexicalEntry(String l, String p) {
            lemma = l;
            pos = p;
        }

        public String getLemma() {
            return lemma;
        }

        public String getPos() {
            return pos;
        }
        
        
    }
}
