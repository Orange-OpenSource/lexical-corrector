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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;


public class ArbreBinaire {

    int maxwordlength = 0;
    int countforms = 0;
    Noeud root = null;
    final boolean debug = false;

    /** créer l'arbre binaire avec le nom du fichier qui contient le lexique en format
     "forme [lemme pos [lemme pos [...]]]"
     @param lexfile fichier du lexiaue
     @throws IOException
     */
    public ArbreBinaire(String lexfile) throws IOException {
        this(lexfile, false);
    }

    /** créer l'arbre binaire avec le nom du fichier qui contient un mot par ligne (y compris locutions)
     @param lexfile fichier du lexiaue
     @param multipleLemmasPerLine si vrai on attend des lignes à la freeling: forme POS1 lemme1 POS2 lemme2 ...
    si non on attend un lexique avec une entrée par ligne: forme [POS [lemme]]
     @throws IOException
     */
    public ArbreBinaire(String lexfile, boolean multipleLemmasPerLine) throws IOException {
        root = new Noeud('\u0000');
        addFile(lexfile, multipleLemmasPerLine);
//        FileInputStream fis = new FileInputStream(new File(lexfile));
//
//        //Construct BufferedReader from InputStreamReader
//        BufferedReader br = new BufferedReader(new InputStreamReader(fis, "utf8"));
//
//        String line;
//        while ((line = br.readLine()) != null) {
//            addWord(line, multipleLemmasPerLine);
//        }
//
//
//        if (debug) {
//            System.out.println("ROOT: " + root);
//        }
//        br.close();
//
//        System.err.println("Tree: nodes: " + Noeud.ndct + ", maximal word length: " + maxwordlength
//                + ", words: " + countforms);
    }

    public void addFile(String lexfile, boolean multipleLemmasPerLine) throws IOException {
        FileInputStream fis = new FileInputStream(new File(lexfile));
        BufferedReader br = new BufferedReader(new InputStreamReader(fis, "utf8"));

        String line;
        while ((line = br.readLine()) != null) {
            addWord(line, multipleLemmasPerLine);
        }

        if (debug) {
            System.out.println("ROOT: " + root);
        }
        br.close();

        System.err.println("Tree: nodes: " + Noeud.ndct + ", maximal word length: " + maxwordlength
                + ", words: " + countforms);
    }

    public void addPenaltyFile(String penaltyFileName) throws FileNotFoundException {
        Noeud.similar = new SimilarLetters(penaltyFileName);
    }

    public String dot() {
        String res = "digraph\n{\n" + root.dot() + "}\n";
        return res;
    }

    WordForm findWordExact(String word) {
        Noeud n = root;
        int ix = 0;
        //char p = word.charAt(ix);
        boolean found = false;

        while (ix < word.length()) {
            //cout << "Char: " << *p << endl;// << *n << endl;
            Character key = word.charAt(ix);
            if (n.left != null && n.left.key_value.equals(key)) {
                n = n.left;
                //cout << "LEFT:" << *n << endl;
                ix++;
            } else if (n.left != null && n.left.right != null) {
                found = false;
                Noeud x = n.left.right;
                //cout << "X: " << *x << endl;
                while (x != null) {
                    //cout << "X: " << *x << " " << found << endl;
                    if (x.key_value.equals(key)) {
                        n = x;
                        //cout << "RIGHT:" << *n << endl;
                        ix++;
                        found = true;
                        break;
                    }
                    x = x.right;
                }
                if (!found) {
                    break; // nothing found
                }            //cout << "broke" << endl;
            } else {
                //cout << "break" << endl;
                break;
            }
        }
        if (ix >= word.length() && n.info != null) {
            // on a parcouru l'arbre
            //cout << word << " found " << n->info << endl;
            return n.info;
        }
        return null;
    }

    void findWordCorrected(String word, int maxdist, Calculator table) {
        // trouver mot avec acces de correction (Levenshtein-Damerau)
        parcourir(word, maxdist, table);
    }

    private void parcourir(String word, int maxdist, Calculator table) {
        table.push(root.key_value);
        root.left.parcourir(word, maxdist, table, 1);
    }

    /** rajouter un mot dans l'arbre. Si oneWordPerLine == true, on prend toute la ligne comme mot
     sinon on l'interpète comme "form lemma pos lemma pos ..."
     @param lexiconline
     @param multipleLemmasPerLine si vrai on attend des lignes à la freeling: forme POS1 lemme1 POS2 lemme2 ...
    si non on attend un lexique avec une entrée par ligne: forme [POS [lemme]]
     */
    private void addWord(String lexiconline, boolean multipleLemmasPerLine) {
        //System.out.println("DDD " + lexiconline);
        Noeud n = root;

        WordForm wf = new WordForm(lexiconline, multipleLemmasPerLine);
        maxwordlength = maxwordlength < wf.form.length() ? wf.form.length() : maxwordlength;
        countforms++;

        if (debug) {
            System.out.println("ADDWORD " + wf.form);
        }

        int len = wf.form.length();
        for (int i = 0; i < len; ++i) {
            char p = wf.form.charAt(i);

            if (debug) {
                System.out.println(" + " + p);
            }

            n = insert(p, n);
        }

//#ifdef DEBUG
        if (debug) {
            System.out.println("__" + n);
        }
//    cout << "__" << *n << endl;
//#endif
        if (!multipleLemmasPerLine) {
            if (n.info == null)  n.info = wf;
            else {
                n.info.merge(wf);
            }
        } else {
            n.info = wf;
        }
    }

    Noeud insert(Character key, Noeud n) {
        if (n.left == null) {
            n.left = new Noeud(key);
            //	cout << "A: " << *n->left << endl;
            return n.left;
        } else if (n.left.key_value.equals(key)) {
            //cout << "C: " << *n->left << endl;
            return n.left;
        }// different letter, add right node
        else if (n.left.right == null) {
            n.left.right = new Noeud(key);
            //cout << "B: " << *n->left->right << endl;
            return n.left.right;
        }// there is already I right node: go through chain until we find our letter
        else {
            Noeud x = n.left.right;
            while (true) {
                if (x.key_value.equals(key)) {
                    return x;
                }
                if (x.right == null) {
                    x.right = new Noeud(key);
                    return x.right;
                }

                x = x.right;
            }
        }
    }
}
