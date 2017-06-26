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
      
        table = new int[tsize][tsize];
        solutions = new HashMap<>();
        stack = new Stack<>();
    }

    public void setDefaultPenalty(int d) {
        penalty = d;
    }
    
    public int getDefaultPenalty() {
        return penalty;
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
        if (debug) System.out.println("minneighbour " + row + " " + col);
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
    
    
}
