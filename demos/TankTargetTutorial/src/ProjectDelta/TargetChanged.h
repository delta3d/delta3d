/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation & MOVES Institute
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
 * @author Curtiss Murphy
 * @author Chris Osborn
 */

#ifndef __GM_TUTORIAL_TARGET_CHANGED__
#define __GM_TUTORIAL_TARGET_CHANGED__

#include "export.h"
#include <dtGame/message.h>
#include <dtGame/messagetype.h>
#include <dtCore/uniqueid.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TUTORIAL_TANK_EXPORT TutorialMessageType : public dtGame::MessageType
{
   DECLARE_ENUM(TutorialMessageType);

   public:

      // TUTORIAL - ADD STATIC INSTANCE OF TANK_TARGET_CHANGED MESSAGE TYPE HERE
      static const TutorialMessageType TANK_TARGET_CHANGED;

      static void RegisterMessageTypes(dtGame::MessageFactory& factory);

   protected:

      TutorialMessageType( const std::string &name, 
                           const std::string &category,
                           const std::string &description, 
                           const unsigned short messageId) :
         dtGame::MessageType(name, category, description, messageId)
      {
         AddInstance(this);
      }

      virtual ~TutorialMessageType() { }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TargetChangedMessage : public dtGame::Message
{
   public:
      TargetChangedMessage();
   protected:
      virtual ~TargetChangedMessage();
   public:
      void SetNewTargetUniqueId( const dtCore::UniqueId& uniqueId );
      const dtCore::UniqueId& GetNewTargetUniqueId() const;
};

#endif // TARGET_CHANGED
