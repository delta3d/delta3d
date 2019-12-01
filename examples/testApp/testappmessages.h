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

#ifndef TEST_APP_MESSAGES_H
#define TEST_APP_MESSAGES_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtGame/message.h>
#include <dtGame/messagemacros.h>
#include "export.h"
#include "testappconstants.h"



namespace dtExample
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   DT_DECLARE_MESSAGE_BEGIN(MotionModelChangedMessage, dtGame::Message, TEST_APP_EXPORT)
      
      static const dtUtil::RefString PARAM_NAME_NEW_MOTION_MODEL_TYPE;

      const dtExample::MotionModelType& GetNewMotionModelType() const;
      void SetNewMotionModelType(const dtExample::MotionModelType& motionModelType);
      
   private:
      dtCore::RefPtr<dtGame::EnumMessageParameter> mNewMotionModelParam;

   DT_DECLARE_MESSAGE_END()



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   DT_DECLARE_MESSAGE_BEGIN(RequestAttachMessage, dtGame::Message, TEST_APP_EXPORT)

      DECLARE_PARAMETER_INLINE(dtCore::UniqueId, ActorId)

   DT_DECLARE_MESSAGE_END()



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   DT_DECLARE_MESSAGE_BEGIN(RequestTimeOffsetMessage, dtGame::Message, TEST_APP_EXPORT)

      DECLARE_PARAMETER_INLINE(float, Offset)

   DT_DECLARE_MESSAGE_END()



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   DT_DECLARE_MESSAGE_BEGIN(UIMessage, dtGame::Message, TEST_APP_EXPORT)

      DECLARE_PARAMETER_INLINE(std::string, UIName)

   DT_DECLARE_MESSAGE_END()



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   DT_DECLARE_MESSAGE_BEGIN(UIHideMessage, UIMessage, TEST_APP_EXPORT)
   DT_DECLARE_MESSAGE_END()



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   DT_DECLARE_MESSAGE_BEGIN(UIShowMessage, UIMessage, TEST_APP_EXPORT)
   DT_DECLARE_MESSAGE_END()



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   DT_DECLARE_MESSAGE_BEGIN(UIToggleMessage, UIMessage, TEST_APP_EXPORT)
   DT_DECLARE_MESSAGE_END()

}

#endif
