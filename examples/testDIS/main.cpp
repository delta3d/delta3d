#include <dtCore/refptr.h>
#include "testdisapp.h"
#include <dtUtil/datapathutils.h>

int main(int argc, char** argv)
{
   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(dataPath + ";" +
      dtUtil::GetDeltaRootPath() + "/examples/data/" + ";" +
      dtUtil::GetDeltaRootPath() + "/examples/testDIS");

   std::string connectionConfigFile = dtUtil::FindFileInPathList("Connection.xml");
   std::string actorTypeMapFile = dtUtil::FindFileInPathList("actormap.xml");

   dtCore::RefPtr<TestDISApp> app = new TestDISApp(connectionConfigFile, actorTypeMapFile);

   app->Config();
   app->Run();

  return 0;
}

