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
#include "testappmessagetypes.h"
#include "testappmessages.h"



namespace dtExample
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   DT_IMPLEMENT_MESSAGE_TYPE_CLASS(TestAppMessageType);

   const TestAppMessageType TestAppMessageType::MOTION_MODEL_CHANGED("Motion Model Changed", "TestApp",
      "Message to signal to the rest of the system that the motion model has changed.",
      TEST_APP_MESSAGE_TYPE_ID, DT_MSG_CLASS(MotionModelChangedMessage));

   const TestAppMessageType TestAppMessageType::REQUEST_ATTACH("Request Attach", "TestApp",
      "Generic message to request that the camera motion model attach to a specified actor.",
      TEST_APP_MESSAGE_TYPE_ID + 2, DT_MSG_CLASS(RequestAttachMessage));

   const TestAppMessageType TestAppMessageType::REQUEST_TIME_OFFSET("Request Time Offset", "TestApp",
      "Message to request that the current time of day be offset by a specified amount.",
      TEST_APP_MESSAGE_TYPE_ID + 3, DT_MSG_CLASS(RequestTimeOffsetMessage));

   const TestAppMessageType TestAppMessageType::UI_HIDE("UI Hide", "TestApp",
      "Generic message to command the UI to hide a UI element.",
      TEST_APP_MESSAGE_TYPE_ID + 4, DT_MSG_CLASS(UIHideMessage));

   const TestAppMessageType TestAppMessageType::UI_SHOW("UI Show", "TestApp",
      "Generic message to command the UI to show a UI element.",
      TEST_APP_MESSAGE_TYPE_ID + 5, DT_MSG_CLASS(UIShowMessage));

   const TestAppMessageType TestAppMessageType::UI_TOGGLE("UI Toggle", "TestApp",
      "Generic message to command the UI to toggle a UI element.",
      TEST_APP_MESSAGE_TYPE_ID + 6, DT_MSG_CLASS(UIToggleMessage));
  
}
