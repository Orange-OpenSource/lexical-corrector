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
 @version 1.0.1 as of 13th December 2018
*/

package com.orange.labs.lexicon;

import com.orange.labs.lexicon.WordForm.LexicalEntry;
import java.util.Comparator;
import java.util.Set;
import java.util.TreeSet;

/**
 *
 * @author Johannes Heinecke <johannes.heinecke@orange.com>
 */
public class WordForm implements Comparable {
    String form;
    Set<LexicalEntry> entries;
    // regexp used ti split lines in the lexicon
    public static String FIELDSEPARATOR = "[ \t]+";

    /** une ligne du lexique à lire

    @param lexiconline fichier lexique
    @param multipleLemmasPerLine si vrai on attend des lignes à la freeling: forme lemma1 POS1 lemma2 POS2 ...
    si non on attend un lexique avec une entrée par ligne: forme [lemma [POS]]
    */
    public WordForm(String lexiconline, boolean multipleLemmasPerLine) {
        if (!multipleLemmasPerLine) {
            // line: forme [lemma [POS]]

            //form = lexiconline;
            String[] elems = lexiconline.split(FIELDSEPARATOR);
            form = elems[0];
            if (elems.length > 1) {
                entries = new TreeSet<>();
                if (elems.length > 2)
                    entries.add(new LexicalEntry(elems[1], elems[2]));
                else
                    entries.add(new LexicalEntry(elems[1], null));
            }
        } else {
            // on pense que la ligne lexicale est forme POS lemme POS Lemme ...
            String[] elems = lexiconline.split(FIELDSEPARATOR);
            form = elems[0];

            if (elems.length > 1 && elems.length % 2 == 1) {
                entries = new TreeSet<>();
                for (int i = 1; i < elems.length; i += 2) {
                    entries.add(new LexicalEntry(elems[i], elems[i + 1]));
                }
            }
        }
    }

    public String getForm() {
        return form;
    }

    public Set<LexicalEntry> getEntries() {
        return entries;
    }

    public void merge(WordForm wf2) {
        if (wf2.entries == null) return;
        //System.err.println("swwww " + wf2 + " " + entries);
        if (entries == null) entries = wf2.getEntries();
        else entries.addAll(wf2.getEntries());
    }

    public String toString() {
        StringBuilder sb = new StringBuilder(form);
        if (entries != null) {
            sb.append(" [");
            boolean first = true;
            //Collections.sort(entries, new SortLE());
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
            return form.compareTo(((WordForm) o).form);
        } else {
            return -1;
        }
    }




    public class LexicalEntry implements Comparable {
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

        @Override
        public int compareTo(Object o) {
            if (o instanceof LexicalEntry) {
                LexicalEntry other = (LexicalEntry)o;
                if (lemma.equals(other.lemma) && pos != null) {
                    return pos.compareTo(other.pos);
                } else {
                    return lemma.compareTo(other.lemma);
                }
            } else {
                return -1;
            }
        }
    }
    static class SortLE implements Comparator<LexicalEntry> {
            public int compare(LexicalEntry a, LexicalEntry b)  {
                return a.compareTo(b);
            }
        }


}
