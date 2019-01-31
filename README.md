# Rapid lexicon acces with correction 

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

GNU libunistring unicode library


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

