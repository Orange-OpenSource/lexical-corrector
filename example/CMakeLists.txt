# This library is under the 3-Clause BSD License
# 
# Copyright (c) 2017, Orange S.A.
# 
# Redistribution and use in source and binary forms, with or without modification, 
# are permitted provided that the following conditions are met:
# 
#   1. Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimer.
# 
#   2. Redistributions in binary form must reproduce the above copyright notice, 
#      this list of conditions and the following disclaimer in the documentation
#      and/or other materials provided with the distribution.
# 
#   3. Neither the name of the copyright holder nor the names of its contributors 
#      may be used to endorse or promote products derived from this software without
#      specific prior written permission.
# 
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
#  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
#  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
#  Author: Johannes Heinecke
#  Version:  1.0 as of 6th April 2017


add_executable (lexiconAccess main.cc)

set_property(TARGET lexiconAccess PROPERTY CXX_STANDARD 11)


target_link_libraries (lexiconAccess LINK_PUBLIC
			lexicon
			unistring)


set(FREELING_VERB_URL "https://raw.githubusercontent.com/TALP-UPC/FreeLing/master/data/fr/dictionary/entries/lefff.verb")
set(FREELING_NOUN_URL "https://raw.githubusercontent.com/TALP-UPC/FreeLing/master/data/fr/dictionary/entries/lefff.nom")

set(FREELING_VERB_TXT "${PROJECT_SOURCE_DIR}/downloaded_data/lefff.verb")
set(FREELING_NOUN_TXT "${PROJECT_SOURCE_DIR}/downloaded_data/lefff.nom")
set(FREELING_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# download information on Unicode data
if (NOT EXISTS "${FREELING_VERB_TXT}")
    file(DOWNLOAD "${FREELING_VERB_URL}" "${FREELING_VERB_TXT}")
endif()

if (NOT EXISTS "${FREELING_NOUN_TXT}")
    file(DOWNLOAD "${FREELING_NOUN_URL}" "${FREELING_NOUN_TXT}")
endif()


set(WORDS ${PROJECT_SOURCE_DIR}/example/word.txt)
set(SIMILARLETTERS ${PROJECT_SOURCE_DIR}/example/letters.txt)
set(FREELING_TXT "${PROJECT_SOURCE_DIR}/downloaded_data/dictionary.txt")

add_custom_target(create_dict
	COMMAND cat ${FREELING_VERB_TXT} ${FREELING_NOUN_TXT} > ${FREELING_TXT}
)

add_custom_target(speed_test
	COMMAND ./lexiconAccess --speed --exact
			--similar ${SIMILARLETTERS}
			${FREELING_VERB_TXT}
			${WORDS}
	COMMAND ./lexiconAccess --speed 
			--similar ${SIMILARLETTERS}
			${FREELING_VERB_TXT}
			${WORDS}
	)

add_custom_target(mini_test
	COMMAND cat ${PROJECT_SOURCE_DIR}/example/word2.txt | 
			./lexiconAccess
			--similar ${SIMILARLETTERS}
			${FREELING_TXT} | tee result.txt
	COMMAND echo
	COMMAND echo "comparing: result.txt ${PROJECT_SOURCE_DIR}/example/result_reference.txt"
	COMMAND meld result.txt ${PROJECT_SOURCE_DIR}/example/result_reference.txt
	
	)
	
add_custom_target(valgrind_test
	COMMAND cat ${WORDS} | valgrind --log-file=${CMAKE_CURRENT_BINARY_DIR}/val.log
				--track-origins=yes -v --num-callers=25 
				--tool=memcheck
				--leak-check=full 
				--leak-resolution=high
				--show-reachable=yes
				 ./lexiconAccess --similar ${SIMILARLETTERS} --exact ${FREELING_VERB_TXT}
	COMMAND echo "valgrind log: ${CMAKE_CURRENT_BINARY_DIR}/val.log"
		)




add_dependencies(speed_test lexiconAccess create_dict)
add_dependencies(mini_test lexiconAccess create_dict)
add_dependencies(valgrind_test lexiconAccess create_dict)

