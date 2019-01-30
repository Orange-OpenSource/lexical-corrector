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
 @version 1.2 as of 30th January 2019
*/

package com.orange.labs.lexicon;


public class Noeud {
    Noeud left = null;
    Noeud right = null;
    WordForm info = null;
    int id;

    Character key_value;

    public static int ndct = 0; // to have a unique node id number
    final boolean debug = false;

    final boolean withDamerau = true;
    final boolean withAccents = false;
  
    
    //final static Collator collator = Collator.getInstance(Locale.FRENCH);
   //     collator.setStrength(Collator.PRIMARY);

    public static SimilarLetters similar = null;

    public Noeud(Character kv) {
//        if (debug) {
//            System.out.println("N: " + id + " " + kv);
//        }
        key_value = kv;
        id = ndct++;
      
        //System.out.println("NOEUD " + (id == 0 ? '0' : id) + " " + kv);
    }

    public String dot() {
        StringBuilder out = new StringBuilder();
        String name;

        if (key_value == 0) {
            name = "zero";
        } else {
            name = id + " (" + key_value + ")";
        }

        if (info != null) {
            name += "\\n" + info;
        }
        if (left != null) {
            String lname;
            lname = left.id + " (" + left.key_value + ")";
            if (left.info != null) {
                lname += "\\n" + left.info;
            }

            out.append("\t\"").append(name).append('"')
                    .append(" -> \"").append(lname).append("\" [label=\"suite\"];\n");
            out.append(left.dot());
        }
        if (right != null) {
            String rname;
            rname = right.id + " (" + right.key_value + ")";
            if (right.info != null) {
                rname += "\\n" + right.info;
            }
            out.append("\t\"").append(name).append('"')
                    .append(" -> \"").append(rname).append("\" [label=\"alt\"];\n");
            out.append(right.dot());
        }
        return out.toString();
    }

    void parcourir(String word, int maxdist, Calculator calc, int depth) {
        calc.push(key_value);

        if (debug) {
            System.out.println("KEY " + id + " " + key_value + " depth: " + depth);
            calc.printstack();
        }        

        int ix = 0;
        //char p = word.charAt(ix);
        int row = calc.stacksize() - 1;
        int col = 1;
        while (ix < word.length() && col < calc.getTsize()) {
            Character key = word.charAt(ix);

            // inserting values in current row
            //int cost = Calculator.penalty; // la transposition de deux caractère ne coute que 0.9 * cost
            int cost = calc.getDefaultPenalty();
            if (key.equals(key_value)) {
                cost = 0;
            }
            
            
            
            else if (Noeud.similar != null) {                
                Integer newpen = Noeud.similar.areSimilar(key, key_value);
                if (newpen != null) cost = newpen;
            }

      
              
            
//#define WITHACCENTS
//#ifdef WITHACCENTS        
//        else if (Noeud::identicalWithoutDiacritics(key, key_value)) cost = Calculator::penalty/2;
            //str = Normalizer.normalize(str, Form.NFC);
//str2 = Normalizer.normalize(str2, Form.NFC);
//#endif

            int val = calc.minNeighbour(row, col, cost);

            if (withDamerau) {
                if (row > 1 && col > 1
                        && key.equals(calc.getPenultime()) // a[i] = b[j-1]
                        && key_value.equals(word.charAt(ix - 1))) { // a[i-1] = b[j]
                    int a = calc.get(row - 2, col - 2) + ((int) (cost * .9)); // transposition de deux caractères
                    if (a < val) {
                        val = a;
                    }
                }
            }


            calc.set(row, col, val);
       
           // System.err.println("Word <" + word.substring(0, ix+1) + "> ix:" + ix+ " COST: " 
           //                    + cost + " r:" + row + " c:" + col + " v:" + val + " ");

            ++ix;
            ++col;

        }

        if (debug) {
            calc.status(System.out, word, row + 1, col - 1);
        }

        // pas la peine de continuer si la distance > maxdist
        //unsigned short curdist = table->get(row, depth  < col - 1 ? depth  : col - 1);
        int lowest = calc.lowest(row, col);

        if (debug) {
            System.out.println("LOWEST " + lowest + " depth:" + depth + " r:" + row + " c:" + (col - 1) + " maxdist" + maxdist);
        }

        if (lowest <= maxdist) {
            // on continue...
            //cout << "continue" << endl;
            int curdist = calc.get(row, col - 1);
            if (curdist <= maxdist && info != null) {

                if (debug) {
                    System.out.println("FOUND " + info + " " + curdist);
                }

                calc.addResult(info, curdist); // il y a un mot
            }
            if (left != null) {
                left.parcourir(word, maxdist, calc, depth + 1);
            }
        }
        calc.pop();
        if (right != null) {
            //cout << "alternative" << endl;
            right.parcourir(word, maxdist, calc, depth);
        }

    }

    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append(id).append(":").append(key_value);
        if (info != null) {
            sb.append('"').append(info).append('"');
        }
        if (left != null) {
            sb.append(" L:").append(left.key_value);
        }
        if (right != null) {
            sb.append(" R:").append(key_value);
        }

        if (left != null) {
            sb.append("(").append(id).append(":").append(left).append(")").append(id).append(":");
        }
        if (right != null) {
            sb.append("[").append(id).append(":").append(right).append("]").append(id).append(":");
        }
        return sb.toString();
    }

}
