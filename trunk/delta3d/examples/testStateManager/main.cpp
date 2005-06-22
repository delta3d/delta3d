#include "teststatemanager.h"     // for app class

#include "dtCore/dt.h"
#include "dtABC/dtabc.h"

#include <iostream>

void Usage()
{
   std::cout << "Proper usage is like this : <program> <config>" << std::endl;
}

int main(unsigned int argc, char* argv[])
{
   dtCore::SetNotifyLevel(dtCore::DEBUG_INFO);
//   dtCore::SetDataFilePathList( "..;" + dtCore::GetDeltaDataPathList() ); 
   dtCore::RefPtr<TestStateManager> app;

   if( argc > 1 )
      app = new TestStateManager(argv[1]);
   else
   {
      Usage();
      return -1;
   }

   dtCore::System::GetSystem()->Config();
   dtCore::System::GetSystem()->Run();

   return 0;
}
