
#include "dt.h"
#include "dtabc.h"


int main()
{
   dtABC::Application *app = new dtABC::Application("config.xml");
   app->Config();

   app->Run();

	return 0;
}
