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

#include "ChangeStance.h"


namespace dtExample
{


/////////////////////////////////////////////////////////////////////////////
ChangeStance::ChangeStance()
  : BaseClass()
{
  AddAuthor("Bradley Anderegg");
}

/////////////////////////////////////////////////////////////////////////////
ChangeStance::~ChangeStance()
{
}

/////////////////////////////////////////////////////////////////////////////
void ChangeStance::Init(const dtDirector::NodeType& nodeType, dtDirector::DirectorGraph* graph)
{
  BaseClass::Init(nodeType, graph);

}

/////////////////////////////////////////////////////////////////////////////
void ChangeStance::BuildPropertyMap()
{
  BaseClass::BuildPropertyMap();
}

/////////////////////////////////////////////////////////////////////////////
bool ChangeStance::Update(float simDelta, float delta, int input, bool firstUpdate)
{
  return BaseClass::Update(simDelta, delta, input, firstUpdate);
}

///////////////////////////////////////////////////////////////////////////////
bool ChangeStance::CanConnectValue(dtDirector::ValueLink* link, dtDirector::ValueNode* value)
{
  return true;
}


//---------------------
}//namespace dtExample


//---------------------------------------------------------------------------------------------------------------------------------
// dtExample
// Originally created on 03/11/2014 by Bradley G Anderegg
// Copyright (C) 2013-2014 Caper Holdings LLC, all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------
// ChangeStance.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

