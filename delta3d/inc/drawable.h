// drawable.h: Declaration of the Drawable class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DELTA_DRAWABLE
#define DELTA_DRAWABLE

#include "export.h"

#include "osg/Node"

namespace dtCore
{
   class Scene;
   
     
   /**
    * A drawable object.
    */
   class DT_EXPORT Drawable
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
};

#endif // DELTA_DRAWABLE
