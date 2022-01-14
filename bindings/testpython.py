#!/usr/bin/python3

# This library is under the Modified BSD License:
# 
# Copyright (c) 2017-2020, Orange S.A. All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification, 
# are permitted provided that the following conditions are met:
# 
#   * Redistributions of source code must retain the above copyright notice, 
#     this list of conditions and the following disclaimer.
# 
#   * Redistributions in binary form must reproduce the above copyright notice, 
#     this list of conditions and the following disclaimer in the documentation 
#     and/or other materials provided with the distribution.
# 
#   * Neither the name of the Orange S.A. nor the names of its contributors may be used
#     to endorse or promote products derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL ORANGE S.A. BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
# OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Author: Johannes Heinecke
# Version: 2.3.0 as of 14th January 2022


import sys
import collections

distfactor = 1000

def getword(word):
    maxdistance = int(distfactor * (len(word) / 5))

    if (maxdistance < 1):
        maxdistance = 1000
    first = False


    res = lc.findWordCorrected(word, maxdistance)
    return '{ "word": "%s", "maxdist": %d, "results": %s }' % (word, maxdistance, res)



import argparse
    
parser = argparse.ArgumentParser()
parser.add_argument("--libdir", "-d", required=True, type=str, help="directory of library")
parser.add_argument("--lexicon", "-l", required=True, type=str, help="lexicon file")
parser.add_argument("--similar", "-s", required=True, type=str, help="error definitions")
parser.add_argument("--test", default="-", type=str, help="file containing test words")
parser.add_argument("--singleEntry", "-e", default=True, action="store_false", help="lexicon file contains a single entry per line")


if len(sys.argv) < 2:
    parser.print_help()
else:
    args = parser.parse_args()



    sys.path.append(args.libdir)
    import json
    import LexCor

    def pp(res):
        j = json.loads(res)
        json.dump(j, sys.stdout, indent=2)
        print()

    print("version:", LexCor.__version__, file=sys.stderr)

    # dictionary, distance definitions, format
    # for format see README.md
    lc = LexCor.LexicalCorrector(args.lexicon, args.similar, args.singleEntry)
    print("version:", lc.getVersion(), file=sys.stderr)

    #res = lc.findWordExact("trompette")
    #pp(res)

    #res = lc.findWordCorrected("mange", 1500)
    #pp(res)

    #res = lc.findWordBest("manè", 1500)
    #pp(res)



    if args.test == "-":
        import readline
        line = input(">> ")
        while line:
            res = getword(line)
            print(json.dumps(json.loads(res), indent=2))
            line = input(">> ")
    else:
        ifp = open(args.test)

        allres = []
        for word in ifp:
            word = word.strip()
            if len(word) < 1:
                continue
            allres.append(getword(word))

        # before python3.6 apparently, python json does not keep the original order of items
        #print(allres)
        j = json.loads("[%s]" % ",".join(allres),
                       object_pairs_hook=collections.OrderedDict,
                       object_hook=collections.OrderedDict)
        #j = json.loads(allres)
        json.dump(j, sys.stdout, indent=2, sort_keys=False, ensure_ascii=False)
        print()

