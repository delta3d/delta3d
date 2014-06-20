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
 * energonquest (forum id)
 */

#include <dtCore/refptr.h>
#include <dtGame/gameapplication.h>
#include <dtGame/gamemanager.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/exception.h>
#include <dtUtil/log.h>

#include <iostream>
#include <cstdlib>
#include <osg/ArgumentParser>
#include <sstream>

void ParseCLI(int& argc, char** argv, std::string& configFileName, std::string& appToLoad, std::string& logFileName)
{
   osg::ArgumentParser arguments(&argc, argv);

   arguments.getApplicationUsage()->setApplicationName("GameStart");
   arguments.getApplicationUsage()->setDescription("GameStart is the application that loads an entry point.");
   std::ostringstream msg;
   msg << arguments.getApplicationName() << " [AppLibraryName] [options] parameter ...\n"
         << "  Note: AppLibraryName should not have the extension or Unix library prefix on it.\n"
         << "  For example libMyApp.so, MyApp.dll, or libMyApp.dylib would all be loaded as 'GameStart MyApp'.";
   arguments.getApplicationUsage()->setCommandLineUsage(msg.str());

   arguments.getApplicationUsage()->addCommandLineOption("--configFileName <filename>","Load application configuration file, if not present default config.xml.");
   arguments.getApplicationUsage()->addCommandLineOption("--logFileName <filename>","The name of the log file to use.  Defaults to [AppLibraryName]_log.html");
   arguments.getApplicationUsage()->addCommandLineOption("--help","Show usage.");

   if (arguments.read("--help") == true)
   {
      arguments.getApplicationUsage()->write(std::cout);
      exit(1);
   }

   arguments.read("--configFileName", configFileName);
   arguments.read("--logFileName", logFileName);

   int pos;
   for (pos = 1; pos<arguments.argc(); ++pos)
   {
      if (arguments.isOption(pos) == false)
      {
         appToLoad = arguments[pos];
         arguments.remove(pos);
         break;
      }
   }

   if (appToLoad.empty())
   {
      arguments.getApplicationUsage()->write(std::cout);
      exit(1);
   }
}

int main(int argc, char** argv)
{
   std::string appToLoad;

   std::string configFileName("config.xml");

   std::string logFileName;

   ParseCLI(argc, argv, configFileName, appToLoad, logFileName);

   if (logFileName.empty())
   {
      logFileName = appToLoad + "_log.html";
   }

   dtUtil::LogFile::SetFileName(logFileName);
#ifdef _DEBUG
   dtUtil::LogFile::SetTitle(appToLoad + " Log File (Debug Libs)");
#else
   dtUtil::LogFile::SetTitle(appToLoad + " Log File");
#endif

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
