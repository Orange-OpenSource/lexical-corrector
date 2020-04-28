# Rapid lexicon access with correction

Library for rapid lexicon access including correction (based on Levenshtein's distance).
In correction mode all words up to a maximal Levenshtein distance are returned. 
Different errors can trigger differing distance (see [example/letters.txt](example/letters.txt)). for instance
missing diacritics count less, then lower/upper case errors or spelling errors with adjacent letters
(like "housr", where "r" is just next to "e")

The Corrector is thread save.

## License

This library is under the [3-Clause BSD Licence](https://opensource.org/licenses/BSD-3-Clause). See [LICENSE.md](LICENSE.md)


## Author

Johannes Heinecke


## Requirements

* GNU libunistring unicode library (Ubuntu: `sudo apt install libunistring-dev`)
* for tests only
  * `jq` json parser  (Ubuntu: `sudo apt install jq`)
  * `meld` graphical file differ  (Ubuntu: `sudo apt install meld`)
  * `valgrind`   (Ubuntu: `sudo apt install valgrind`)

## Compiling

### C++ library

For the tests a partial lexicon is downloaded from the FreeLing project
(https://github.com/TALP-UPC/FreeLing/tree/master/data)

    mkdir build
    cd build
    cmake  -DCMAKE_BUILD_TYPE=Release  [-DCMAKE_INSTALL_PREFIX=/usr/local]  ..	

(you can also use `Debug`, especially for test_valgrind)

    make [-j 4]
    make mini_test
    make speed_test
    make valgrind_test
    make python_test
    make pythonbasic_test

### Java library

The java version needs maven to compile

    cd java
    mvn install

## Using

### C++ tool

    example/lexiconAccess [options] in:lexfile [in:wordfile]

options:

    --similar <file>  similar letter definition
    --singleEntry     one entry per lexicon file (form TAB other information)
    --speed           execute speedtest (needs wordfile)
    --exact           speedtest with exact access
    --maxdist <n>     factor used to calculate maximal allowed distance (n * wordLength / 5)
    --defdist <n>     default distance unless changed via similar letter definition (1000)

for instance (correction with respect to a standard lexicon)

    ./build/example/lexiconAccess  --similar example/letters.txt  downloaded_data/dictionary.txt [testfile.txt]

The following configuration replaces `?` into letter with an diacritic symbol (like accented letters). This can be used in order to reaccentuate a text, which has been corrupted by an invalid encoding conversion

    ./build/example/lexiconAccess  --similar example/correct-diacritics.txt  downloaded_data/dictionary.txt

The format of the lexfile is **space separated** list of
```
form lemma POS [lemma2 POS [...]]
```

or

```
form lemma POS
form lemma2 POS
...
```

if the option `--singleEntry` is used, the format is a **tab-separated** list of
```
form [lemma [POS [type [morpho features [syntactic feature [semantics]]]]]]"
```

### Java tool

    java -jar java/target/lexicon-n.m.o-jar-with-dependencies.jar [options] in:lexfile [in:letter_definition]

for instance

    java -jar java/target/lexicon-n.m.o-jar-with-dependencies.jar downloaded_data/dictionary.txt example/letters.txt 

If the lexicon file is very large, it may be necessary to increase the JVM stacksize using the `-Xmss` option

## Install library

    cd build
    make install

By default, the library will be installed in `/usr/local`. To have it installed in a different location, uncomment modify the line `set(CMAKE_INSTALL_PREFIX ../testinstall)` in [CMakeLists.txt](CMakeLists.txt)


## API

### C++
create an instance of the data tree:

    ArbreBinaire ab = new ArbreBinaire(string:lexfile, string:similarlettersFile);

for each thread, create a corrector instance:

    Corrector cr = new Corrector(ab);

get corrections. fo find the best guesses up to a certain edit distance (finds also exact matches)

    map<const WordForm *, unsigned int> *res = cr->findWordBest(const char *word, int max_distance);

get exact match (or null, if word is not in the data:

    const WordForm *wf = cr->findWordExact(const char *word);

get corrections. fo find the best guesses up to a certain edit distance

    map<const WordForm *, unsigned int> *res = cr->findWordCorrected(const char *word, int max_distance);


The the default distance for one different character is 1000. The file [example/letters.txt](example/letters.txt) can be used to define different distance values for some kind of errors.


See [example/main.cc](example/main.cc) for more information. Do not forget to add `-I/usr/local/include/lexicon` and `-llexicon` to your c++ compiler

### Java

create an instance of the data tree:

    ArbreBinaire ab = new ArbreBinaire(String lexfile, boolean multipleLemmasPerLine);

for each thread, create a corrector instance:

    Corrector cr = new Corrector(ab);

get corrections. fo find the best guesses up to a certain edit distance (finds also exact matches)

    Map<WordForm, Integer>wf = cr.findWordBest(String word, int maxdist);


get exact match (or null, if word is not in the data:

    WordForm wf = cr.findWordExact(String word);

get corrections. fo find the best guesses up to a certain edit distance

    Map<WordForm, Integer> wf = cr.findWordCorrected(String word, int maxdist);



See [java/src/main/java/com/orange/labs/lexicon/Lexicon.java](java/src/main/java/com/orange/labs/lexicon/Lexicon.java) for more
information.

### Python3

In difference to Java and C++, the Python3 API returns JSON objects (this was done to avoid fiddling too much with Swig's interfacing)

    import sys
    import json
    import LexCor

    #                            lexicon file, error definitions, lexicon format
    lc = LexCor.LexicalCorrector(lexiconfile,  errordefinitions,  1)
    #                          word to correct, maximal Levenshtein-distance)
    res = lc.findWordCorrected("violinn",       1500)
    j = json.loads(res)
    json.dump(j, sys.stdout, indent=2, ensure_ascii=False)

See also [bindings/testpython.py](bindings/testpython.py).

`LexCor.LexicalCorrector()` is not threadsafe! In order to have multiple threads correct words using the same dictionary, either
* create an instance of `LexCor.LexicalCorrector` for each thread
* create an empty `LexCor.LexicalCorrector` and use an instance of `LexCor.AbreBinaire(dictionaryfile, letters, 1)` for all data and an instance of `LexCor.Corrector()`for each thread. 

for instance:

    import json
    import LexCor

    # creating empty LexicalCorrector
    lc = LexCor.LexicalCorrector()

    # creating data tree. The AbreBinaire is threadsafe (read-only)
    ab = LexCor.ArbreBinaire(sys.argv[2], sys.argv[3], True)

    # creating two correctors (one for each thread, if we head threads here)
    # a Corrector instance must no be used by two threads at the same time
    # in order to multithread, create an individual instance of Corrector for each thread
    cr = LexCor.Corrector(ab) # for one thread
    cr2 = LexCor.Corrector(ab # for a second thread
    # ...
 
    # give Corrector instance to `findWord...()`methods
    res = lc.findWordCorrected("tandu", 1500, cr2) 

See also [bindings/testpython_basic.py](bindings/testpython_basic.py).

The the default distance for one different character is 1000. The file [example/letters.txt](example/letters.txt) can be used to define different distance values for some kind of errors.

