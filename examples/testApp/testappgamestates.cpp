/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
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
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "testappgamestates.h"



namespace dtExample
{
   //////////////////////////////////////////////////////////////////////////
   // GAME STATE TYPE CODE
   //////////////////////////////////////////////////////////////////////////
   TestAppGameState TestAppGameState::STATE_GAME("STATE_GAME");
   TestAppGameState TestAppGameState::STATE_GAME_OPTIONS("STATE_GAME_OPTIONS", true);
   TestAppGameState TestAppGameState::STATE_GAME_QUIT("STATE_GAME_QUIT", true);
   TestAppGameState TestAppGameState::STATE_TITLE("STATE_TITLE", true);



   //////////////////////////////////////////////////////////////////////////
   // TRANSITION CODE
   //////////////////////////////////////////////////////////////////////////
   TestAppTransition TestAppTransition::TRANSITION_MENU("TRANSITION_MENU");

} // END - namespace dtExample
