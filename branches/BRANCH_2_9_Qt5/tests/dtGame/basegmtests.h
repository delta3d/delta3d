/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2015, Caper Holdings, LLC
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
 */

#ifndef TESTS_DTGAME_BASEGMTESTS_H_
#define TESTS_DTGAME_BASEGMTESTS_H_

#include <cppunit/extensions/HelperMacros.h>
#include <dtGame/gamemanager.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/testcomponent.h>
#include <dtCore/actorfactory.h>

namespace dtGame
{
   class BaseGMTestFixture : public CPPUNIT_NS::TestFixture
   {
   public:
      typedef std::vector<std::string> NameVector;
      BaseGMTestFixture();
      virtual ~BaseGMTestFixture();
      virtual void setUp();
      virtual void tearDown();
   protected:

      virtual void GetRequiredLibraries(NameVector& namesOut)
      {
         namesOut.push_back(dtCore::ActorFactory::DEFAULT_ACTOR_LIBRARY);
         namesOut.push_back(mTestActorLibrary);
         namesOut.push_back(mTestGameActorLibrary);
      }

      dtCore::RefPtr<GameManager> mGM;
      dtCore::RefPtr<DefaultMessageProcessor> mDefMsgProc;
      dtCore::RefPtr<TestComponent> mTestComp;
      dtUtil::Log* mLogger;

      static const std::string mTestGameActorLibrary;
      static const std::string mTestActorLibrary;

   };
}


#endif /* TESTS_DTGAME_BASEGMTESTS_H_ */
