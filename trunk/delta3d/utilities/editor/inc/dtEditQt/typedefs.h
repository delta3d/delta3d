#ifndef DELTA_STAGE_TYPEDEFS
#define DELTA_STAGE_TYPEDEFS

#include <dtCore/refptr.h>
#include <dtDAL/actorproxy.h>
#include <vector>

typedef dtCore::RefPtr<dtDAL::ActorProxy> ActorProxyRefPtr;
typedef std::vector< ActorProxyRefPtr > ActorProxyRefPtrVector;
typedef dtCore::RefPtr<dtDAL::ActorProperty> ActorPropertyRefPtr;

#endif
