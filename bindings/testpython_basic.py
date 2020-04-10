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

# testing the original API (AbreBinaire and Corrector)

import sys

if len(sys.argv) < 5:
    print("Error in %s" % sys.argv[0], file=sys.stderr)
    print("usage:\n  %s libdirname lexfile similarlettersfile testwordlist" % sys.argv[0], file=sys.stderr)
else:
    sys.path.append(sys.argv[1])

    import json
    import LexCor


    def pp(res):
        j = json.loads(res)
        json.dump(j, sys.stdout, indent=2)
        print()

    print("version:", LexCor.__version__, file=sys.stderr)

    # creating empty LexicalCorrector
    lc = LexCor.LexicalCorrector()

    # creating data tree. The AbreBinaire is threadsafe (read-only)
    ab = LexCor.ArbreBinaire(sys.argv[2], sys.argv[3], True)

    # creating two correctors (one for each thread, if we head threads here)
    # a Corrector instance must no be used by two threads at the same time
    # in order to multithread, create an individual instance of Corrector for each thread
    cr = LexCor.Corrector(ab)
    cr2 = LexCor.Corrector(ab)

    # call to empty lc gives error
    res = lc.findWordExact("correction")
    pp(res)
    
    # give corrector to thread, since lc itself is empty
    res = lc.findWordExact("mange", cr)
    pp(res)

    res = lc.findWordCorrected("tandu", 1500, cr2)
    pp(res)

    res = lc.findWordBest("manè", 1500, cr)
    pp(res)

