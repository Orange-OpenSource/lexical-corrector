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

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintStream;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class Lexicon {
    ArbreBinaire ab;
    Corrector cr;
    boolean multipleLemmasPerLine;

    /** create new Lexicon
     @param lexfile name fo the lexicon file
     @param multipleLemmasPerLine si vrai on attend des lignes à la freeling: forme POS1 lemme1 POS2 lemme2 ...
     si non on attend un lexique avec une entrée par ligne: forme [POS [lemme]]
     */
    public Lexicon(String lexfile, boolean multipleLemmasPerLine) throws IOException {
        this.multipleLemmasPerLine = multipleLemmasPerLine;
        ab = new ArbreBinaire(lexfile, multipleLemmasPerLine);
        //PrintStream out = new PrintStream(System.out, true, "UTF-8");
        //out.println(ab.dot());
        cr = new Corrector(ab);
    }

    public static <K  extends Comparable<? super K> , V extends Comparable<? super V>> Map<K, V> sortBMapValue(Map<K, V> map) {
        List<Map.Entry<K, V>> list = new LinkedList<>(map.entrySet());
        Collections.sort(list, new Comparator<Map.Entry<K, V>>() {
                     @Override
                     public int compare(Map.Entry<K, V> o1, Map.Entry<K, V> o2) {
                         if (o1.getValue().equals(o2.getValue())) {
                             return (o1.getKey()).compareTo(o2.getKey());
                         }
                         return (o1.getValue()).compareTo(o2.getValue());
                     }
                 });

        Map<K, V> result = new LinkedHashMap<>();
        for (Map.Entry<K, V> entry : list) {
            result.put(entry.getKey(), entry.getValue());
        }
        return result;
    }

    /** add more lexicon files */
    public void addFile(String fn) throws IOException {
        ab.addFile(fn, multipleLemmasPerLine);
        
        // new file may have longer words than initially seen, so it is needed to recreate Corrector here
         cr = new Corrector(ab);
    }

    /**
     Add a penalty file which modifies penalties for certain errors
     @param penaltyFileName
     @throws FileNotFoundException
     */
    public void addPenaltyFile(String penaltyFileName) throws FileNotFoundException {
        ab.addPenaltyFile(penaltyFileName);
    }

    public void setDefaultDist(Integer d) {
        //Calculator.penalty = d;
        cr.setDefaultPenalty(d);
    }

    /** non threaded interface (for API doc see Correctoer.java) */

    public Map<WordForm, Integer> findWordCorrected(String word, int maxdist) {
        return cr.findWordCorrected(word, maxdist);
    }

    public WordForm findWordExact(String word) {
        return cr.findWordExact(word);
    }

    public Map<WordForm, Integer> findWordBest(String word, int maxdist) {
         return cr.findWordBest(word, maxdist);
    }

    public class CorrectorThread implements Runnable {
        ArbreBinaire ab;
        Corrector cr;
        List<String> wordlist;
        boolean exact;
        boolean logtest;
        int id;

        public CorrectorThread(int id, ArbreBinaire ab, List<String> wordlist, boolean exact, boolean logtest) {
            this.id = id;
            this.logtest = logtest;
            this.ab = ab;
            this.wordlist = wordlist;
            cr = new Corrector(ab);
            this.exact = exact;
            System.out.println("Creating thread: " + exact + " " + ab + " " + cr);
        }

        @Override
        public void run() {

            try {
                BufferedWriter bw = null;
                if (logtest) {
                    bw = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(new File("" + id + ".out")), "UTF-8"));
                }

                long start = System.currentTimeMillis();
                int loop;
                if (exact) {
                    loop = 1000000;
                    for (int i = 0; i < loop; i++) {
                        for (String word : wordlist) {
                            //System.out.println("aaa " + word);
                            WordForm res = cr.findWordExact(word);
                            if (bw != null) {
                                try {
                                    bw.write(word + ": " + res + "\n");
                                } catch (IOException ex) {
                                }
                            }
                        }
                    }
                } else {
                    loop = 100;
                    for (int i = 0; i < loop; i++) {
                        for (String word : wordlist) {
                            Map<WordForm, Integer> res = cr.findWordBest(word, 1000 * word.length() / 4);
                            if (bw != null) {
                                Map<WordForm, Integer> res_sorted = Lexicon.sortBMapValue(res);
                                bw.write(word + ":\n");
                                for (WordForm wf : res_sorted.keySet()) {
                                    bw.write("\t" + res.get(wf) + ": " + wf + "\n");
                                }

                            }
                        }
                    }
                }
                if (bw != null) {
                    bw.close();
                }

                long end = System.currentTimeMillis();
                System.out.printf("accessing %d words: %.2fs\n", loop * wordlist.size(), ((end - start) / 1000.0));
            } catch (IOException ex) {
                System.err.println("cannot open " + id + ".out " + ex.getMessage());
            }
        }
    }

    void perftest(List<String> wordlist, boolean exact, boolean logtest) {
        List<Thread> threads = new ArrayList<>();

        for (int i = 0; i < 4; ++i) {
            CorrectorThread ct = new CorrectorThread(i, ab, wordlist, exact, logtest);
            Thread thread = new Thread(ct);
            threads.add(thread);
            thread.start();
        }

        try {
            for (Thread th : threads) {
                th.join();
            }
        } catch (InterruptedException e) {
            System.err.println("interlanguage link read failed");
        }

    }

    private class SortByLemma implements Comparator<WordForm.LexicalEntry> {
        public int compare(WordForm.LexicalEntry a, WordForm.LexicalEntry b) {
            return a.lemma.compareTo(b.lemma);
        }
    }

    public void test(String word, Integer maxpenalty, Boolean silent) {
        try {
            PrintStream out = new PrintStream(System.out, true, "UTF-8");
            Map<WordForm, Integer> res = cr.findWordCorrected(word, maxpenalty);
            if (!silent) {
                out.println(res.size() + " solutions:");
            }

            Map<WordForm, Integer> res_sorted = Lexicon.sortBMapValue(res);
            if (!res_sorted.isEmpty()) {
                WordForm best = res_sorted.keySet().iterator().next();
                if (silent) {
                    out.print(word);
                    if (!word.equals(best.form)) {
                        out.print("\t>>");
                        for (WordForm wf : res_sorted.keySet()) {
                            out.print("\t");
                            out.print(wf.form);
                        }
                    }
                    out.println();
                } else {
                    for (WordForm wf : res_sorted.keySet()) {
                        //out.println("  " + res.get(wf) + ": " + wf);
                        out.println("  " + res.get(wf) + ": " + wf.getForm());
                        int ct = 1;

                        Set<WordForm.LexicalEntry> les = wf.getEntries();
                        //Collections.sort(les, new SortByLemma());
                        //Collections.sort(les, new SortLE());
                        if (les != null) {
                            for (WordForm.LexicalEntry entry : les) {
                                out.format("\t%d lemma: %s\n\t\tpos: %s\n", ct++, entry.lemma, entry.pos);
                            }
                        }
                    }
                }
            } else {
                if (silent) {
                    out.println(word);
                }
            }
        } catch (UnsupportedEncodingException ex) {
            ex.printStackTrace();
        }
    }

    public String testtext(String text, Integer maxpenalty, Boolean silent) {
        StringBuilder sb = new StringBuilder();
        String[] words = text.split("[\\s,;:\\.-]+");
        for (String word : words) {
            sb.append(test2(word, maxpenalty, silent)); //.append("\n");
        }
        return sb.toString();
    }

    public String test2(String word, Integer maxpenalty, Boolean silent) {
        StringBuilder sb = new StringBuilder();
        //PrintStream out = new PrintStream(System.out, true, "UTF-8");
        if (maxpenalty == -1) {
            int maxdistfactor = 1000;
            maxpenalty = maxdistfactor * word.length() / 4;
        }
        Map<WordForm, Integer> res = cr.findWordCorrected(word, maxpenalty);
        if (!silent) {
            sb.append(word + ": " + res.size() + " solutions:\n");
        }

        Map<WordForm, Integer> res_sorted = Lexicon.sortBMapValue(res);
        if (!res_sorted.isEmpty()) {
            WordForm best = res_sorted.keySet().iterator().next();
            if (silent) {
                sb.append(word);
                if (!word.equals(best.form)) {
                    sb.append("\t>>");
                    for (WordForm wf : res_sorted.keySet()) {
                        sb.append("\t");
                        sb.append(wf.form);
                    }
                }
                sb.append('\n');
            } else {
                for (WordForm wf : res_sorted.keySet()) {
                    sb.append("  " + res.get(wf) + ": " + wf + "\n");
                }
            }
        } else {
            if (silent) {
                sb.append(word + "\n");
            }
        }
        return sb.toString();
    }

    enum Text {
        NONE, TESTCORR, TESTEXACT, TEXT
    };

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        if (args.length == 0) {
            System.out.println("usage: Lexicon.jar [options] lexfile [letters.txt]");
            System.out.println("   --multipleLemmas|-m ");
            System.out.println("   --text textfile|--testcorr wordlist|--testexact wordlist [--log]");
            System.out.println("   --defaultdist <n>");
            System.out.println("   --maxdistfactor <n>");
            System.out.println("   --sep <regex>   field separator");
            System.out.println("");
        } else {

            try {
                //Lexicon l = new Lexicon("/home/johannes/rhaglenni/cpp/projets/levenshtein/word2b.txt");
                boolean multipleLemmasPerLine = false;
                Text texttype = Text.NONE;
                //boolean testcorr = false;
                //boolean testexact = false;
                boolean logtest = false;
                int maxdistfactor = 1000;
                int defaultdistance = 1000;
                String wordlist = null;
                String sep = null;
                int shift = 0;
//                if (args.length > 1 && (args[0].equals("--multipleLemmas") || args[0].equals("-m"))) {
//                    multipleLemmasPerLine = true;
//                    shift = 1;
//                }

                for (int i = 0; i < args.length; ++i) {
                    if (args[i].equals("--multipleLemmas") || args[i].equals("-m")) {
                        multipleLemmasPerLine = true;
                        shift++;
                    } else if (args[i].equals("--testcorr") && texttype == Text.NONE) {
                        texttype = Text.TESTCORR;
                        shift++;
                        wordlist = args[shift++];
                    } else if (args[i].equals("--text") && texttype == Text.NONE) {
                        texttype = Text.TEXT;
                        shift++;
                        wordlist = args[shift++];
                    } else if (args[i].equals("--testexact") && texttype == Text.NONE) {
                        texttype = Text.TESTEXACT;
                        shift++;
                        wordlist = args[shift++];
                    } else if (args[i].equals("--sep")) {
                        shift++;
                        sep = args[shift++];
                    } else if (args[i].equals("--defaultdist")) {
                        shift++;
                        int defdist = Integer.parseInt(args[shift++]);
                        if (defdist > 0) {
                            //Calculator.penalty = defdist;
                            defaultdistance = defdist;
                        }
                    } else if (args[i].equals("--maxdistfactor")) {
                        shift++;
                        int tmp = Integer.parseInt(args[shift++]);
                        if (tmp > 0) {
                            maxdistfactor = tmp;
                        }

                    } else if (args[i].equals("--logtest")) {
                        logtest = true;
                        shift++;
                    } else if (args[i].startsWith("--")) {
                        System.err.println("invalid option: " + args[i]);
                        System.exit(1);
                    }

                }

                if (sep != null) {
                    WordForm.FIELDSEPARATOR = sep;
                }
                Lexicon lexicon = new Lexicon(args[shift], multipleLemmasPerLine);

                if (args.length == 2 + shift) {
                    lexicon.addPenaltyFile(args[1 + shift]);
                }

                lexicon.setDefaultDist(defaultdistance);

                if (texttype == Text.TESTCORR || texttype == Text.TESTEXACT) {
                    List<String> wl = new ArrayList<>();
                    BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream(new File(wordlist)), "UTF-8"));
                    String line;
                    while ((line = br.readLine()) != null) {
                        wl.add(line.trim());
                    }
                    lexicon.perftest(wl, texttype == Text.TESTEXACT, logtest);

                } else if (texttype == Text.TEXT) {
                    BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream(new File(wordlist)), "UTF-8"));
                    String line;
                    while ((line = br.readLine()) != null) {
                        String[] elems = line.split("\\s+");
                        for (String w : elems) {
                            int maxpenalty = maxdistfactor * w.length() / 4;
                            lexicon.test(w, maxpenalty, true);
                        }
                    }
                } else {
                    //interactive

                    BufferedReader br = new BufferedReader(new InputStreamReader(System.in, "utf8"));

                    String input;
                    int maxpenalty;
                    System.out.print("enter word> ");

                    while ((input = br.readLine()) != null) {
                        if (input.isEmpty()) {
                            break;
                        }
                        maxpenalty = maxdistfactor * input.length() / 5;
                        System.out.println(input + " (max penalty allowed: " + maxpenalty + ")");
                        lexicon.test(input, maxpenalty, false);
                        System.out.print("enter word> ");
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
                System.err.println("ERROR: " + e.getMessage());
            }

        }

    }
}
