/* -*-c++-*-
* testAnim - export (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2007-2008, Alion Science and Technology Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
* 
* Bradley Anderegg
*/
#ifndef DELTA_TEST_ANIM_EXPORT_H
#define DELTA_TEST_ANIM_EXPORT_H


#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  ifdef TEST_ANIM_LIBRARY
#    define TEST_ANIM_EXPORT __declspec(dllexport)
#  else
#    define TEST_ANIM_EXPORT __declspec(dllimport)
#  endif
#else
#   ifdef TEST_ANIM_LIBRARY
#      define TEST_ANIM_EXPORT __attribute__ ((visibility("default")))
#   else
#      define TEST_ANIM_EXPORT
#   endif 
#endif

#endif
