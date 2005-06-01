#ifndef DELTA_EVENT
#define DELTA_EVENT

#include <string>
#include <dtCore/export.h>

namespace dtABC
{

   class DT_EXPORT Event
   {
      public:

         Event();
         virtual ~Event();
         virtual const std::string& GetType() const { return mType; }
      
      private:

         const std::string mType;
         
   };

}

#endif //DELTA_EVENT
