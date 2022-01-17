/** This library is under the 3-Clause BSD License

Copyright (c) 2017, Orange S.A.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice, 
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

  3. Neither the name of the copyright holder nor the names of its contributors 
     may be used to endorse or promote products derived from this software without
     specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 Author: Johannes Heinecke
 Version:  2.3.1 as of 17th January 2022
*/

#include "Util.h"
#include <cstring>


// the following functions are taken from
// https://github.com/JeffBezanson/cutef8/blob/master/utf8.c
// Basic UTF-8 manipulation routines  by Jeff Bezanson
// placed in the public domain Fall 2005
static const uint32_t offsetsFromUTF8[6] = {
    0x00000000UL, 0x00003080UL, 0x000E2080UL,
    0x03C82080UL, 0xFA082080UL, 0x82082080UL
};

static const char trailingBytesForUTF8[256] = {
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,  1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    2,2,2,2, 2,2,2,2, 2,2,2,2, 2,2,2,2,  3,3,3,3, 3,3,3,3, 4,4,4,4, 5,5,5,5
};

/** convert UTF-8 to UCS-4 (4-byte wide characters)
   srcsz = source size in bytes, or -1 if 0-terminated
   sz = dest size in # of wide characters
   returns # characters converted */
int u8_toucs(uint32_t *dest, int sz, const char *src, int srcsz) {
    uint32_t ch;
    const char *src_end = src + srcsz;
    int nb;
    int i=0;
   
    while (i < sz-1) {       
        nb = trailingBytesForUTF8[(unsigned char)*src];
        if (srcsz == -1) {
            if (*src == 0)
                goto done_toucs;
        }
        else {
            if (src + nb >= src_end)
                goto done_toucs;
        }
        ch = 0;
        switch (nb) {
            /* these fall through deliberately */
        case 3: ch += (unsigned char)*src++; ch <<= 6;
        case 2: ch += (unsigned char)*src++; ch <<= 6;
        case 1: ch += (unsigned char)*src++; ch <<= 6;
        case 0: ch += (unsigned char)*src++;
        }
        ch -= offsetsFromUTF8[nb];
        dest[i++] = ch;
    }
 done_toucs:
    dest[i] = 0;
    return i;
}

/* srcsz = number of source characters, or -1 if 0-terminated
   sz = size of dest buffer in bytes

   returns # characters converted
   dest will only be '\0'-terminated if there is enough space. this is
   for consistency; imagine there are 2 bytes of space left, but the next
   character requires 3 bytes. in this case we could NUL-terminate, but in
   general we can't when there's insufficient space. therefore this function
   only NUL-terminates if all the characters fit, and there's space for
   the NUL as well.
   the destination string will never be bigger than the source string.
*/
int u8_toutf8(char *dest, int sz, uint32_t *src, int srcsz)
{
    uint32_t ch;
    int i = 0;
    char *dest_end = dest + sz;

    while (srcsz<0 ? src[i]!=0 : i < srcsz) {
        ch = src[i];
        if (ch < 0x80) {
            if (dest >= dest_end)
                return i;
            *dest++ = (char)ch;
        }
        else if (ch < 0x800) {
            if (dest >= dest_end-1)
                return i;
            *dest++ = (ch>>6) | 0xC0;
            *dest++ = (ch & 0x3F) | 0x80;
        }
        else if (ch < 0x10000) {
            if (dest >= dest_end-2)
                return i;
            *dest++ = (ch>>12) | 0xE0;
            *dest++ = ((ch>>6) & 0x3F) | 0x80;
            *dest++ = (ch & 0x3F) | 0x80;
        }
        else if (ch < 0x110000) {
            if (dest >= dest_end-3)
                return i;
            *dest++ = (ch>>18) | 0xF0;
            *dest++ = ((ch>>12) & 0x3F) | 0x80;
            *dest++ = ((ch>>6) & 0x3F) | 0x80;
            *dest++ = (ch & 0x3F) | 0x80;
        }
        i++;
    }
    if (dest < dest_end)
        *dest = '\0';
    return i;
}

/* single character to UTF-8 */

int u8_wc_toutf8(char *dest, uint32_t ch)
{
    if (ch < 0x80) {
        dest[0] = (char)ch;
        dest[1] = 0;
        return 1;
    }
    if (ch < 0x800) {
        dest[0] = (ch>>6) | 0xC0;
        dest[1] = (ch & 0x3F) | 0x80;
         dest[2] = 0;
        return 2;
    }
    if (ch < 0x10000) {
        dest[0] = (ch>>12) | 0xE0;
        dest[1] = ((ch>>6) & 0x3F) | 0x80;
        dest[2] = (ch & 0x3F) | 0x80;
         dest[3] = 0;
        return 3;
    }
    if (ch < 0x110000) {
        dest[0] = (ch>>18) | 0xF0;
        dest[1] = ((ch>>12) & 0x3F) | 0x80;
        dest[2] = ((ch>>6) & 0x3F) | 0x80;
        dest[3] = (ch & 0x3F) | 0x80;
         dest[4] = 0;
        return 4;
    }
    return 0;
}

// author: johannes.heinecke@orange.com
void SplitNoEmptyFields(vector<string> &result,
        const string & chaine,
        const char * chaineRecherche) {
    result.clear();
    if (!chaineRecherche) return;
    string::size_type pos1 = 0;
    string::size_type pos2 = chaine.find(chaineRecherche, pos1);
    if (pos2 == string::npos) {
	// on n'a pas trouvé aucune instance de la chaineRecherche
	result.push_back(chaine);
	return;
    }
    
    while (pos2 != string::npos) {
        if (pos2 - pos1) {
            // on ne rajoute pas des éléments de longueur 0
            result.push_back(string(chaine, pos1, pos2 - pos1));
        }
        pos1 = pos2 + strlen(chaineRecherche);
        pos2 = chaine.find(chaineRecherche, pos1);
    }

    if (chaine.size() - pos1) {
        result.push_back(string(chaine, pos1, pos2 - pos1));
    }
}

// author: johannes.heinecke@orange.com
string Replace(const string & chaine, const char *ancien, const char *nouveau)
{
        string result = chaine;
        if (!ancien) return result;
        if (!nouveau) return result;
        if (strlen(ancien) == 0) return result;  // MODIF JHE 01/06/2006
        string::size_type pos = result.find(ancien,0);
        while (pos != string::npos)
        {
                result.replace(pos,strlen(ancien),nouveau);
                pos = result.find(ancien,pos+strlen(nouveau));
        }

        return(result);
}
