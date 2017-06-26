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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.Map;

/**
 *
 * @author Johannes Heinecke <johannes.heinecke@orange.com>
 */
public class SimilarLetters {

    Map<String, Integer> letterPairs; // "àa": penalty
    //int default_penalty;

    public SimilarLetters(String fn//, int defaultpenalty
    ) throws FileNotFoundException {
        FileInputStream fis = new FileInputStream(new File(fn));
       // this.default_penalty = defaultpenalty;
        letterPairs = new HashMap<>();

        //Construct BufferedReader from InputStreamReader
        try {
            BufferedReader br = new BufferedReader(new InputStreamReader(fis, "utf8"));
            //  PrintStream out = new PrintStream(System.out, true, "UTF-8");
            String line;
            int penalty = -1; //default_penalty;
            int ct = 0;
            while ((line = br.readLine()) != null) {
                ct++;

                line = line.trim();
                if (line.isEmpty()) {
                    continue;
                }
                if (line.charAt(0) == '#') {
                    continue;
                }
                if (line.startsWith("penalty")) {
                    penalty = Integer.parseInt(line.substring(8));
                    if (penalty == 0) {
                        System.err.println("Invalid penalty line " + ct + ": " + line);
                        System.err.println("Ignoring definitions until valid penalty line!");
                        penalty = -1; //default_penalty;
                    }
                } else {
                    if (penalty == -1) continue;
                    String[] fields = line.split(" ");
                    if (fields.length != 2) {
                        System.err.println("Invalid line " + ct + ": " + line);
                        continue;
                    }
                    if (fields[0].length() != 1) {
                        System.err.println("Invalid first character " + ct + ": " + line);
                        continue;
                    }

                    for (int pos = 0; pos < fields[1].length(); pos++) {
                        StringBuilder sb = new StringBuilder();
                        //sb.append(fields[0]);
                        //sb.append(fields[1].charAt(pos));

                        Character a = fields[0].charAt(0);
                        Character b = fields[1].charAt(pos);
                        if (a > b) {
                            // mettre le Character le plus petit dans a
                            Character tmp = b;
                            b = a;
                            a = tmp;
                        }
                        sb.append(a).append(b);
                        letterPairs.put(sb.toString(), penalty);

                    }
                }
            }

            System.out.println("" + letterPairs.size() + " weighted corrections loaded from " + fn);
//        
//            for (String p : letterPairs.keySet()) {
//                out.println("aa " + p + " " + letterPairs.get(p));
//            }
        } catch (IOException e) {
        }
    }

    /** return null if both are different else the similarity penalty */
    Integer areSimilar(Character a, Character b) {
        if (a > b) {
            // mettre le Character le plus petit dans a
            Character tmp = b;
            b = a;
            a = tmp;
        }

        StringBuilder sb = new StringBuilder();
        sb.append(a).append(b);

//        try {
//            PrintStream out = new PrintStream(System.out, true, "UTF-8");
//               out.println("AAA " + a + " " + b + ":: " + sb + " " + letterPairs.size());
//        } catch (UnsupportedEncodingException ex) {
//           
//        }
        //Integer penalty = letterPairs.get(couple);
        Integer penalty = letterPairs.get(sb.toString());
//        System.out.println(" --> "  + sb + ": "+ penalty);
        return penalty;
    }
}
