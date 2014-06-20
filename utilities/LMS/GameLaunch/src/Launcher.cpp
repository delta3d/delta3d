/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author Christopher DuBuc
 */

#include "Launcher.h"
#include "ParameterParser.h"
#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>

Launcher::Launcher(int argc, char** argv)
{
   mArgc = argc;
   mArgv = argv;

   //extract named parameters from command line
   mParser = new ParameterParser(argc, argv);

   //set working directory and add it to the front of the system path
   std::string workingDir = mParser->GetWorkingDirectory();
   if(!workingDir.empty())
   {
      //set working directory
      if (SetWorkingDirectory(workingDir))
      {
         //prepend working directory to system path
         PrependWorkingDirToPath(workingDir);
      }
      else
      {
         //cout << "Invalid working directory specified in launch parameters: " << parser->GetWorkingDirectory() << endl;
      }
   }
}

bool Launcher::SetWorkingDirectory(const std::string &workingDirectory)
{
   try
   {
      dtUtil::FileUtils::GetInstance().ChangeDirectory(workingDirectory);
      return true;
   }
   catch(const dtUtil::Exception &e)
   {
      e.LogException(dtUtil::Log::LOG_ERROR);
      return false;
   }
}

bool Launcher::SetWorkingDirectory(const char* workingDirectory)
{
   return SetWorkingDirectory(std::string(workingDirectory));
}

void Launcher::PrependWorkingDirToPath(const std::string &workingDirectory)
{
   PrependWorkingDirToPath(workingDirectory.c_str());
}

void Launcher::PrependWorkingDirToPath(const char* workingDirectory)
{
   const std::string path = dtUtil::GetEnvironment("PATH");

   std::string newPath;
   newPath += workingDirectory;
#ifdef DELTA_WIN32
   newPath += ";";
#else
   newPath += ":";
#endif
   newPath += path;

   dtUtil::SetEnvironment("PATH", newPath);
}

void Launcher::Launch()
{
   //set Delta3D data path
   dtUtil::SetDataFilePathList(mParser->GetDataDirectory());

   dtGame::GameApplicationLoader loader(mArgc, mArgv);
   loader.SetGameLibraryName(mParser->GetApplicationLibraryFile());
   loader.Config();
   loader.Run();
}

Launcher::~Launcher()
{
   delete mParser;
}
