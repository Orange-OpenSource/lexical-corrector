# This library is under the Modified BSD License:
# 
# Copyright (c) 2017-2022, Orange S.A. All rights reserved.
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


cmake_minimum_required (VERSION 3.5)
project (lexicon)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fmax-errors=2 -pthread")


#find_package(Boost COMPONENTS regex REQUIRED)
#if (Boost_FOUND)
#	message(STATUS "Boost found: ${Boost_LIBRARIES}")
#	# include_directories(${BOOST_INCLUDE_DIR})
#endif()

# deactivate with cmake -DCP_EXPORT=False
set(CP_EXPORT TRUE)

if(CP_EXPORT)
    # needed to build Python bindings
    # swig needs that c/c++ code is compiled with -fPIC
    set (CMAKE_POSITION_INDEPENDENT_CODE TRUE)
endif()


add_subdirectory (lib)
add_subdirectory (example)


if(CP_EXPORT)
   add_subdirectory(bindings)
endif()


# activate this in order to avoid /usr/local/lib
set(CMAKE_INSTALL_PREFIX ../testinstall)
