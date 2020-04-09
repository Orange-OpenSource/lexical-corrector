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



if(CP_EXPORT)
   set(INTERFACE LexicalCorrector.i)
   set(WORDS ${PROJECT_SOURCE_DIR}/example/word.txt)
   set(SIMILARLETTERS ${PROJECT_SOURCE_DIR}/example/letters.txt)
   set(FREELING_TXT ${PROJECT_SOURCE_DIR}/downloaded_data/dictionary.txt)

   FIND_PACKAGE(SWIG)

   if (SWIG_FOUND)
      FIND_PACKAGE(PythonLibs)

      if(PYTHONLIBS_FOUND)
         set(PYTHON_CP_NAME LexCor)
         include_directories(${PYTHON_INCLUDE_PATH})
         include_directories(${CMAKE_SOURCE_DIR}/lib)
         include_directories(${CMAKE_CURRENT_SOURCE_DIR})
         #message("aaaaaa ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}")

         set(SWIGPY -py3)
         set(PYCOMMAND python3)

         add_custom_command(
            DEPENDS ${INTERFACE}
            OUTPUT lexicalcorrector_py.cc 
            COMMAND swig -python ${SWIGPY} -builtin -c++ -I${CMAKE_SOURCE_DIR}/lib -o lexicalcorrector_py.cc ${CMAKE_CURRENT_SOURCE_DIR}/${INTERFACE}
         )

         # Specify the lib
         add_library(${PYTHON_CP_NAME} SHARED
            lexicalcorrector_py.cc 
            LexicalCorrector.cc
         )

         set_target_properties(${PYTHON_CP_NAME} PROPERTIES PREFIX _ )
         target_link_libraries(${PYTHON_CP_NAME} lexicon)

         add_custom_target(testpython
            #COMMAND echo "bin path ${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND echo "PYTHON TEST ${PYCOMMAND} -u ${CMAKE_CURRENT_SOURCE_DIR}/testpython.py ${CMAKE_CURRENT_BINARY_DIR} ${FREELING_TXT} ${SIMILARLETTERS} ${WORDS}"
            COMMAND ${PYCOMMAND} -u ${CMAKE_CURRENT_SOURCE_DIR}/testpython.py ${CMAKE_CURRENT_BINARY_DIR}  ${FREELING_TXT} ${SIMILARLETTERS} ${WORDS}
         )
      
         #add_dependencies(testpython lexicon)

      else()
         message("pythonlibs not installed on your system")
      endif()
   else()
      message("swig >= 3.0 not installed on your system")
   endif()
endif()

