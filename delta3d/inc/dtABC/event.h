#ifndef DELTA_EVENT
#define DELTA_EVENT

#include <string>
#include <dtCore/base.h>

namespace dtABC
{

   class DT_EXPORT Event : public dtCore::Base
   {
      public:

         Event();
         Event(const std::string& mt): mType(mt) {}
         virtual ~Event();
         virtual const std::string& GetType() const { return mType; }

      private:

        const std::string mType;

   };

}

#endif //DELTA_EVENT
