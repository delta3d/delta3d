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
#include "testappmessages.h"
#include "testappconstants.h"



namespace dtExample
{
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString MotionModelChangedMessage::PARAM_NAME_NEW_MOTION_MODEL_TYPE("NewMotionModelType");

   MotionModelChangedMessage::MotionModelChangedMessage()
   {      
      mNewMotionModelParam = new dtGame::EnumMessageParameter(PARAM_NAME_NEW_MOTION_MODEL_TYPE, dtExample::MotionModelType::NONE.GetName());
      AddParameter(mNewMotionModelParam);
   }

   const dtExample::MotionModelType& MotionModelChangedMessage::GetNewMotionModelType() const 
   {
      return *dtExample::MotionModelType::GetValueForName(mNewMotionModelParam->GetValue());
   }

   void MotionModelChangedMessage::SetNewMotionModelType(const dtExample::MotionModelType& motionModelType)
   {
      mNewMotionModelParam->SetValue(motionModelType.GetName());
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_MESSAGE_BEGIN(RequestAttachMessage)

      DT_ADD_PARAMETER(dtCore::UniqueId, ActorId)

   DT_IMPLEMENT_MESSAGE_END()



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_MESSAGE_BEGIN(RequestTimeOffsetMessage)

      DT_ADD_PARAMETER(float, Offset)

   DT_IMPLEMENT_MESSAGE_END()



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_MESSAGE_BEGIN(UIMessage)

      DT_ADD_PARAMETER(std::string, UIName)

   DT_IMPLEMENT_MESSAGE_END()



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_MESSAGE_BEGIN(UIHideMessage)
   DT_IMPLEMENT_MESSAGE_END()



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_MESSAGE_BEGIN(UIShowMessage)
   DT_IMPLEMENT_MESSAGE_END()



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_MESSAGE_BEGIN(UIToggleMessage)
   DT_IMPLEMENT_MESSAGE_END()

}
