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
#include <dtGame/gamemanager.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/exception.h>
#include <dtUtil/log.h>

#include <iostream>
#include <cstdlib>

void ShowUsageAndExit()
{
   std::cerr << "Usage: GameStart [--configFileName <file>] <AppLibraryName> [App Library args]\n";
   std::cerr << "   Note: AppLibraryName should not have the extension or Unix library prefix on it.\n";
   std::cerr << "   For example libMyApp.so, MyApp.dll, or libMyApp.dylib would all be loaded as 'GameStart MyApp'." << std::endl;
   exit(1);
}

int main(int argc, char** argv)
{
   bool showUsage = false;

   std::string appToLoad;

   int curArg = 1;
   std::string configFileName("config.xml");

   while (!showUsage && appToLoad.empty() && curArg < argc)
   {
      std::string curArgv = argv[curArg];
      if (!curArgv.empty())
      {
         if (curArgv == "--configFileName")
         {
            ++curArg;
            if (curArg < argc)
            {
               configFileName = argv[curArg];
               LOG_ALWAYS ( "Using configuration file " + configFileName );
            }
            else
            {
               showUsage = true;
            }
         }
         else if (curArgv[0] == '-')
         {
            std::cerr << "Unknown option: " << curArgv << std::endl;
            showUsage = true;
         }
         else
         {
            appToLoad = curArgv;
         }
      }
      ++curArg;
   }

   //The loop always overruns by one, so subtract it back off.
   --curArg;

   if (appToLoad.empty() || showUsage)
   {
      ShowUsageAndExit();
   }

   argc -= curArg;
   argv = argv + curArg;
   try
   {
      dtGame::GameApplicationLoader loader(argc, argv);
      loader.SetGameLibraryName(appToLoad);
      loader.Config(configFileName);
      loader.Run();

      std::cerr << "The Game Manager is now shutting down ... " << std::endl;
   }
   catch (const dtUtil::Exception& ex)
   {
      LOG_ERROR("GameStart caught exception: ");
      ex.LogException(dtUtil::Log::LOG_ERROR);
      return -1;
   }
   return 0;
}
