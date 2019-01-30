#!/bin/bash

MYDIR=$(dirname $0)

NEWESTJAR=$(ls  $MYDIR/../target/lexicon-*-jar-with-dependencies.jar | tail -1)
echo $NEWESTJAR

cat $MYDIR/../../example/word2.txt \
    | java -jar $NEWESTJAR \
      $MYDIR/../../downloaded_data/dictionary.txt \
      $MYDIR/../../example/letters.txt > $MYDIR/../../build/javares.txt

meld $MYDIR/../../build/javares.txt $MYDIR/../../example/result_reference.txt
#tkdiff $MYDIR/../../build/javares.txt $MYDIR/../../example/result_reference.txt

