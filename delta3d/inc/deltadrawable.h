#ifndef DELTA_DELTA_DRAWABLE
#define DELTA_DELTA_DRAWABLE

// deltadrawable.h: Declaration of the DeltaDrawable class.
//
//////////////////////////////////////////////////////////////////////


#include "export.h"

#include "osg/Node"

namespace dtCore
{
   class Scene;
   
     
   /**
    * A drawable object.
    */
   class DT_EXPORT DeltaDrawable
   {
      public:

         /**
          * Returns this object's OpenSceneGraph node.
          *
          * @return the OpenSceneGraph node
          */
         virtual osg::Node* GetOSGNode() = 0;
         
         /**
          * Notifies this drawable object that it has been added to
          * a scene.
          *
          * @param scene the scene to which this drawable object has
          * been added
          */
         virtual void AddedToScene(Scene* scene) {}
   };

#ifdef _WIN32
   typedef DeltaDrawable Drawable;
#endif
};


#endif // DELTA_DELTA_DRAWABLE
