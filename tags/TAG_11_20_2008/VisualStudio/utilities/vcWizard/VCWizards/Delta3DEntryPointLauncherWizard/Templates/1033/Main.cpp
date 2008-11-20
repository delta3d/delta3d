/* -*-c++-*-
 Simple application used to launch a Delta3D GameEntryPoint library.

 Usage: [!output PROJECT_NAME].exe <GameEntryPoint library base name>
 */

#include <dtCore/refptr.h>
#include <dtGame/gameapplication.h>
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
