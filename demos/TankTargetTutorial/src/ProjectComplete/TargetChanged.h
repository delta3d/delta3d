/* -*-c++-*-
 * TutorialLibrary - TargetChanged (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
 * @author Curtiss Murphy
 * @author Chris Osborn
 */

#ifndef __GM_TUTORIAL_TARGET_CHANGED__
#define __GM_TUTORIAL_TARGET_CHANGED__

#include "export.h"
#include <dtGame/message.h>
#include <dtGame/messagetype.h>
#include <dtGame/messagefactory.h>
#include <dtCore/uniqueid.h>

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DT_DECLARE_MESSAGE_TYPE_CLASS_BEGIN(TutorialMessageType, TUTORIAL_TANK_EXPORT)
      static const TutorialMessageType TANK_TARGET_CHANGED;
DT_DECLARE_MESSAGE_TYPE_CLASS_END()

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TargetChangedMessage : public dtGame::Message
{
   public:
      TargetChangedMessage();

   protected:
      virtual ~TargetChangedMessage();

   public:
      void SetNewTargetUniqueId(const dtCore::UniqueId& uniqueId);
      const dtCore::UniqueId& GetNewTargetUniqueId() const;
};

#endif // TARGET_CHANGED
