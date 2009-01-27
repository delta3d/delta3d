#ifndef unittestapplication_h__
#define unittestapplication_h__

#include <dtABC/application.h>

///simple derivative used as the "global" application for the unit tests
class UnitTestApplication : public dtABC::Application
{
public:
	UnitTestApplication();

protected:
   virtual ~UnitTestApplication();
	
};
#endif // unittestapplication_h__

