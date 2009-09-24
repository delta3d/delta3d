/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009, Alion Science and Technology, BMH Operation
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
 * Bradley Anderegg
 */
#ifndef AIUTILITY_AI_COMPONENT
#define AIUTILITY_AI_COMPONENT

#include <dtCore/refptr.h>
#include <dtAI/baseaicomponent.h>

namespace dtAI
{
   class AIPluginInterface;
}

class AIComponent : public dtAI::BaseAIComponent
{
public:
   static const std::string DEFAULT_NAME;

   /// Constructor
   AIComponent(const std::string& name = DEFAULT_NAME);

   /**
    * Handles incoming messages
    */
   /*virtual*/ void ProcessMessage(const dtGame::Message& message);

   void AddAIInterfaceToMap(const std::string& map);

protected:
   /*virtual*/ void CleanUp();
};

#endif //AIUTILITY_AI_COMPONENT
