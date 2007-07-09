#ifndef DELTA_STAGE_TYPEDEFS
#define DELTA_STAGE_TYPEDEFS

#include <QtCore/QMetaType>
#include <dtCore/refptr.h>
#include <dtDAL/actorproxy.h>
#include <vector>

typedef dtCore::RefPtr<dtDAL::ActorProxy> ActorProxyRefPtr;
typedef std::vector< ActorProxyRefPtr > ActorProxyRefPtrVector;
typedef dtCore::RefPtr<dtDAL::ActorProperty> ActorPropertyRefPtr;

Q_DECLARE_METATYPE(dtCore::RefPtr<dtDAL::ActorProxy>);
Q_DECLARE_METATYPE(dtCore::RefPtr<dtDAL::ActorProperty>);

#endif
