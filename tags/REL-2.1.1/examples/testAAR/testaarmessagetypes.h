/* -*-c++-*-
* testAAR - testaarmessagetypes (.h & .cpp) - Using 'The MIT License'
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
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* @author William E. Johnson II
*/
#ifndef DELTA_TEST_AAR_MESSAGE_TYPES
#define DELTA_TEST_AAR_MESSAGE_TYPES

#include <dtGame/messagetype.h>
#include "export.h"

class TEST_AAR_EXPORT TestAARMessageType : public dtGame::MessageType
{
   public:

      DECLARE_ENUM(TestAARMessageType);

      static TestAARMessageType PLACE_ACTOR;
      static TestAARMessageType RESET;
      static TestAARMessageType REQUEST_ALL_CONTROLLER_UPDATES;
      static TestAARMessageType PRINT_TASKS;
      static TestAARMessageType UPDATE_TASK_CAMERA;
      static TestAARMessageType PLACE_IGNORED_ACTOR;

   private:

      /// Constructor
      TestAARMessageType(const std::string &name, 
                            const std::string &category = "", 
                            const std::string &desc = "", 
                            const unsigned short id = 0) : dtGame::MessageType(name, category, desc, id)
      {
         AddInstance(this);
      }
};

#endif

