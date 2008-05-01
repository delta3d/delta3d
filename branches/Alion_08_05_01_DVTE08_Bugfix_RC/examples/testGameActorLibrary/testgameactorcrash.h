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
#ifndef DELTA_TESTGAMEACTORCRASH_H_
#define DELTA_TESTGAMEACTORCRASH_H_

#include "export.h"
#include <dtGame/gameactor.h>

class TestGameActorCrash : public dtGame::GameActor
{
public:
	TestGameActorCrash(dtGame::GameActorProxy& proxy);
protected:
	virtual ~TestGameActorCrash();
};

class DT_EXAMPLE_EXPORT TestGameActorCrashProxy : public dtGame::GameActorProxy
{
public:

   /// Constructor
   TestGameActorCrashProxy();

   /// Destructor
   virtual ~TestGameActorCrashProxy();

   /**
    * Builds the properties associated with this proxy's actor
    */
   virtual void BuildPropertyMap();

   /**
    * Builds the invokable associated with this proxy.
    */
   virtual void BuildInvokables();

   /// @throws dtUtil::Exception Throws to test the GM except on entered world handler.
   virtual void OnEnteredWorld();

protected:
   virtual void CreateActor();
private:
   bool ticksEnabled;
};


#endif /*DELTA_TESTGAMEACTORCRASH_H_*/
