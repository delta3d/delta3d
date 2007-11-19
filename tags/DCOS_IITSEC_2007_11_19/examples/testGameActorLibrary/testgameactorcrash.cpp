/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * William E. Johnson II
 */
#include "testgameactorcrash.h"
#include <dtUtil/exception.h>

////////////////////////////////////////////////////////////////////
// Proxy Code
////////////////////////////////////////////////////////////////////
TestGameActorCrashProxy::TestGameActorCrashProxy():ticksEnabled(false)
{
   SetClassName("TestGameActorCrash");
}

////////////////////////////////////////////////////////////////////
TestGameActorCrashProxy::~TestGameActorCrashProxy() 
{
}

////////////////////////////////////////////////////////////////////
void TestGameActorCrashProxy::BuildPropertyMap()
{
}

////////////////////////////////////////////////////////////////////
void TestGameActorCrashProxy::BuildInvokables()
{
}

////////////////////////////////////////////////////////////////////
void TestGameActorCrashProxy::CreateActor()
{
   SetActor(*new TestGameActorCrash(*this));
}

////////////////////////////////////////////////////////////////////
void TestGameActorCrashProxy::OnEnteredWorld()
{
   throw dtUtil::Exception("DON'T ADD ME TO THE SCENE!!!", __FILE__, __LINE__);
}

////////////////////////////////////////////////////////////////////
// Actor Code
////////////////////////////////////////////////////////////////////
TestGameActorCrash::TestGameActorCrash(dtGame::GameActorProxy& proxy): dtGame::GameActor(proxy)
{
}

////////////////////////////////////////////////////////////////////
TestGameActorCrash::~TestGameActorCrash()
{
}
