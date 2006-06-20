/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * @author David Guthrie
 */

#include <dtCore/refptr.h>
#include <dtGame/gameapplication.h>
#include <dtGame/exceptionenum.h>
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

#ifndef __APPLE__
   /**
   dtUtil::FileInfo info = dtUtil::FileUtils::GetInstance().GetFileInfo(executable);
   if(info.fileType == dtUtil::FILE_NOT_FOUND)
   {
      LOG_ERROR(std::string("Unable to change to the directory of application \"")
         + executable + "\": file not found.");
   }
   else
   {
      LOG_ALWAYS(std::string("Changing to directory \"")
            + info.path + "\".");

      try
      {
         if (!info.path.empty())
            dtUtil::FileUtils::GetInstance().ChangeDirectory(info.path);
      }
      catch (dtUtil::Exception& ex)
      {
         ex.LogException(dtUtil::Log::LOG_ERROR);
      }
   }
   */
#endif

   try 
   {
      dtCore::RefPtr<dtGame::GameApplication> app = new dtGame::GameApplication(argc, argv);
      app->SetGameLibraryName(std::string(appToLoad));
      app->Config();
      app->Run();
      app = NULL;
   }
   catch (dtUtil::Exception &ex)
   {
      std::cout << "GameStart caught exception: " << ex.ToString() << std::endl;
   }
   return 0;
}
