#include <dtABC/autotrigger.h>


namespace dtABC
{

IMPLEMENT_MANAGEMENT_LAYER(AutoTrigger)


AutoTrigger::AutoTrigger( const std::string& name /* = "AutoTrigger"  */)
{
   mTrigger = new Trigger("InternalAutoTrigger");

   RegisterInstance(this);

   mTrigger->SetEnabled(true);
   mTrigger->Fire();

}

} //namespace dtABC 

