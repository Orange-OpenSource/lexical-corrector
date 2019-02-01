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

import java.io.PrintStream;
import java.util.HashMap;
import java.util.Map;
import java.util.Stack;


public class Calculator {

    //public static int penalty = 1000;
    private int penalty = 1000;
    private final int tsize;
    private final int[][] table;
    private final Stack<Character> stack;
    Map<WordForm, Integer> solutions;

    final boolean debug = false;


    public Calculator(int tsize) {
        this.tsize = tsize;

        table = new int[this.tsize][this.tsize];
        solutions = new HashMap<>();
        stack = new Stack<>();
    }

    public void setDefaultPenalty(int d) {
        penalty = d;
    }

    public int getDefaultPenalty() {
        return penalty;
    }

    int getTsize() {
        return tsize;
    }

    public void init() {
        for (int col = 0; col < tsize; ++col) {
            table[0][col] = col * penalty;
            for (int row = 1; row < tsize; ++row) {
                table[row][col] = 0;
            }
        }

        for (int row = 1; row < tsize; ++row) {
            table[row][0] = row * penalty;
        }
        stack.clear();
        solutions.clear();
    }

    public int minNeighbour(int row, int col, int cost) {
        // if we get a OutOfBoundary exception here, increase tsize
        if (debug)  System.out.println("minneighbour " + row + " " + col + " " + cost);
        //System.err.println("TTT " +table.length + " " + table[0].length + " " + row + " "  +col);
        int x = table[row][col - 1] + penalty;
        int y = table[row - 1][col] + penalty;
        int z = table[row - 1][col - 1] + cost;
        return (x <= y) ? (x <= z) ? x : z : (y <= z) ? y : z;
    }

    public int lowest(int row, int maxcol) {
        int min = 1000000;
        //cout << "ROW: " << row << endl;
        for (int col = 0; col < maxcol; ++col) {
            min = table[row][col] < min ? table[row][col] : min;
            //System.out.println("AAAA " + col + " " + row + " " + table[row][col] + " min: " + min);
        }
        return min;
    }

    public void status(PrintStream out, String word, int maxrow, int maxcol) {
        for (int col = 0; col < maxcol; ++col) {
            // pour toutes les
            if (col == 0) {
                out.print("\t");
            }

            out.print("\t" + word.charAt(col));
        }
        out.println();

        for (int row = 0; row < maxrow; ++row) {
            // pour toutes les lignes
            for (int col = 0; col <= maxcol; ++col) {
            // pour toutes les colonnes

                if (col == 0) {
                    out.print(stack.elementAt(row) == 0 ? "0" : stack.elementAt(row));
                    out.print('\t');
                }
                out.print(table[row][col]);
                out.print("\t");
                //if (col == 0) cout << "\t";
            }
            out.println();
            if (row == 0) {
                out.println();
            }
        }
    }

    public int get(int row, int col) {
        return table[row][col];
    }

    public void set(int row, int col, int val) {
        table[row][col] = val;
    }

    void push(Character c) {
        stack.push(c);
    }

    /** l'avant-dernière élément du stack. Indéfini, si longueur de stack < 2 */
    Character getPenultime() {
        return stack.elementAt(stack.size()-2);
    }

    void pop() {
        stack.pop();
    }

    int stacksize() {
        return stack.size();
    }

    void addResult(WordForm wi, int distance) {
        solutions.put(wi, distance);
    }

    Map<WordForm, Integer> getresults() {
        return solutions;
    }

    void printstack() {
        System.out.println(" STACK" + stack);
    }

    class Result {
        int lowest;
        int row;
        int col;
        public Result(int l, int r, int c) {
            lowest = l;
            row = r;
            col = c;
        }
    }

    // unsuccessful trial to replace recursion by iteration
    /*
    void parcourir(Noeud root, String word, int maxdist) {
        Noeud start = root.left;
        Result r1 = firstpart(start, word, maxdist, 1);
        int depth = 1;
        Stack<Noeud> leftnodes = new Stack<>();
        if (r1.lowest <= maxdist) {

            // on continue...
            int curdist = this.get(r1.row, r1.col - 1);
            if (curdist <= maxdist && start.info != null) {

                if (debug) {
                    System.out.println("FOUND " + start.info + " " + curdist);
                }

                this.addResult(start.info, curdist); // il y a un mot
            }
            if (start.left != null) {
                start = start.left;
                //left.parcourir(word, maxdist, calc, depth + 1);
                r1 = firstpart(start, word, maxdist, depth+1);
            }

        }


        this.pop();
        if (start.right != null) {
           // right.parcourir(word, maxdist, calc, depth);
           r1 = firstpart(start.right, word, maxdist, depth+1);
        }


    }


    Result firstpart(Noeud node, String word, int maxdist, int depth) {
        this.push(node.key_value);

        if (debug) {
            System.out.println("KEY " + node.id + " " + node.key_value + " depth: " + depth);
            this.printstack();
        }

        int ix = 0;
        //char p = word.charAt(ix);
        int row = this.stacksize() - 1;
        int col = 1;
        while (ix < word.length() && col < this.getTsize()) {
            Character key = word.charAt(ix);

            // inserting values in current row
            //int cost = Calculator.penalty; // la transposition de deux caractère ne coute que 0.9 * cost
            int cost = this.getDefaultPenalty();
            if (key.equals(node.key_value)) {
                cost = 0;
            }



            else if (Noeud.similar != null) {
                Integer newpen = Noeud.similar.areSimilar(key, node.key_value);
                if (newpen != null) cost = newpen;
            }


            int val = this.minNeighbour(row, col, cost);

            if (Noeud.withDamerau) {
                if (row > 1 && col > 1
                        && key.equals(this.getPenultime()) // a[i] = b[j-1]
                        && node.key_value.equals(word.charAt(ix - 1))) { // a[i-1] = b[j]
                    int a = this.get(row - 2, col - 2) + ((int) (cost * .9)); // transposition de deux caractères
                    if (a < val) {
                        val = a;
                    }
                }
            }

            this.set(row, col, val);

            ++ix;
            ++col;
        }

        if (debug) {
            this.status(System.out, word, row + 1, col - 1);
        }

        // pas la peine de continuer si la distance > maxdist
        //unsigned short curdist = table->get(row, depth  < col - 1 ? depth  : col - 1);
        int lowest = this.lowest(row, col);

        if (debug) {
            System.out.println("LOWEST " + lowest + " depth:" + depth + " r:" + row + " c:" + (col - 1) + " maxdist" + maxdist);
        }
        return new Result(lowest, row, col);
    }
*/


}
