#ifndef DELTA_EVENT
#define DELTA_EVENT

#include <string>
#include <dtCore/base.h>
#include <dtUtil/enumeration.h>

namespace dtABC
{

   class DT_EXPORT Event : public osg::Referenced
   {
      public:

         typedef dtUtil::Enumeration Type;

         Event( const Type* type ) : mType(type) {}
        
         virtual const Type* GetType() const { return mType; }

      protected:

         //pure virutal destructor to force abstract class
         virtual ~Event() = 0;

         const Type* mType;

   };

   //"implementation" of pure virual destructor so compiler is happy
   Event::~Event() 
   {
   }

};


#endif //DELTA_EVENT
