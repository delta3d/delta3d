#ifndef _DTDIS_HAS_PROPERTY_H_
#define _DTDIS_HAS_PROPERTY_H_

#include <vector>
#include <dtCore/refptr.h>
#include <dtDAL/actorproxy.h>
#include <string>
#include <dtDIS/dtdisexport.h>

namespace dtDIS
{
   namespace details
   {

      /// a utility that builds a vector of actors that support the property of interest
      class DT_DIS_EXPORT HasProperty
      {
      public:
         typedef std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > ActorVector;

         HasProperty(const std::string& prop_name);
         ~HasProperty();
         HasProperty(const HasProperty& hp);

         void operator ()(dtCore::RefPtr<dtDAL::ActorProxy>& proxy);

         ActorVector& GetPassedActors();
         const ActorVector& GetPassedActors() const;

      private:
         HasProperty& operator =(const HasProperty& hp);  // not implemented by design

         ActorVector mActors;

         std::string mPropName;
      };

   }
}

#endif  // _DTDIS_HAS_PROPERTY_H_
