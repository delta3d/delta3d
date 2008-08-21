/* -*-c++-*-
* GameStart - main (.h & .cpp) - Using 'The MIT License'
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
* David Guthrie
*/

#include <dtCore/refptr.h>
#include <dtGame/gameapplication.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/gamemanager.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/exception.h>
#include <dtUtil/log.h>

int main(int argc, char** argv)
{
   if(argc < 2)
   {
      LOG_ERROR("A valid application library is required to run GameStart. Please specify it as the first command line parameter.");
      return -1;
   }

   std::string executable(argv[0]);
   char *appToLoad = argv[1];
   argv[1] = argv[0];
   argv++;
   --argc;

   try 
   {
      dtCore::RefPtr<dtGame::GameApplication> app = new dtGame::GameApplication(argc, argv);
      app->SetGameLibraryName(std::string(appToLoad));
      app->Config();
      app->Run();
      app = NULL;
   }
   catch (const dtUtil::Exception &ex)
   {
      LOG_ERROR("GameStart caught exception: ");
      ex.LogException(dtUtil::Log::LOG_ERROR);
      return -1;
   }
   return 0;
}
