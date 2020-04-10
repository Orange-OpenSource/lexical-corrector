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
# Version: 2.0 as of 9th April 2020


import sys

if len(sys.argv) < 5:
    print("Error in %s" % sys.argv[0], file=sys.stderr)
    print("usage:\n  %s libdirname lexfile similarlettersfile testwordlist" % sys.argv[0], file=sys.stderr)
else:
    sys.path.append(sys.argv[1])

    import json
    import LexCor

    # dictionary, distance definitions, format
    # for format see README.md
    lc = LexCor.LexicalCorrector(sys.argv[2], sys.argv[3], 1)

    #res = lc.findWordExact("trompette")
    #j = json.loads(res)
    #json.dump(j, sys.stdout, indent=2)
    #print()

    #res = lc.findWordCorrected("mange", 1500)
    #j = json.loads(res)
    #json.dump(j, sys.stdout, indent=2)
    #print()

    #res = lc.findWordBest("manè", 1500)
    #j = json.loads(res)
    #json.dump(j, sys.stdout, indent=2)
    #print()


    ifp = open(sys.argv[4])

    distfactor = 1000

    allres = "["
    first = True
    for word in ifp:
        word = word.strip()
        if len(word) < 1:
            continue
        maxdistance = int(distfactor * (len(word) / 5))

        if (maxdistance < 1):
            maxdistance = 1000
        if not first:
            allres += ","
        first = False
        allres += '{ "word": "%s", "maxdist": %d, "results": ' % (word, maxdistance)

        res = lc.findWordCorrected(word, maxdistance)
        allres += res + "}"
    allres += "]"
    j = json.loads(allres)
    json.dump(j, sys.stdout, indent=2, ensure_ascii=False)
