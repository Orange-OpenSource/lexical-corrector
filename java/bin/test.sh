#!/bin/bash

MYDIR=$(dirname $0)
cat $MYDIR/../../example/word2.txt \
    | java -jar $MYDIR/../target/lexicon-1.0.1-jar-with-dependencies.jar \
      $MYDIR/../../downloaded_data/dictionary.txt \
      $MYDIR/../../example/letters.txt > $MYDIR/../../build/javares.txt

meld $MYDIR/../../build/javares.txt $MYDIR/../../example/result_reference.txt
#tkdiff $MYDIR/../../build/javares.txt $MYDIR/../../example/result_reference.txt

