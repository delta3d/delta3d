#ifndef DELTA_ENVEFFECT
#define DELTA_ENVEFFECT

#include "base.h"
#include "sg.h"
#include <osg/Group>

namespace dtCore
{
   ///A base class for all Environmental Effects
   class DT_EXPORT EnvEffect : public dtCore::Base
   {
   public:
      DECLARE_MANAGEMENT_LAYER(EnvEffect)

      EnvEffect(const std::string name=0);
      virtual ~EnvEffect(void);

      /// Must override this to supply the repainting routine
      virtual void Repaint(sgVec3 skyColor, sgVec3 fogColor,
                           double sunAngle, double sunAzimuth,
                           double visibility) = 0;

      virtual osg::Group *GetNode(void) = 0;

   };
}

#endif // DELTA_ENVEFFECT
