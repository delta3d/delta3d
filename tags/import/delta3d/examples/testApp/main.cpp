
#include "dt.h"
#include "application.h"

int main()
{
   dtABC::Application *app = new dtABC::Application("config.xml");
   app->Config();

   app->Run();

	return 0;
}
