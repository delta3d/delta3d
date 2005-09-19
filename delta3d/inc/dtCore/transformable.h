/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 2.1 of the License, or (at your option) 
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
*/

#ifndef DELTA_TRANSFORMABLE
#define DELTA_TRANSFORMABLE




#include "dtCore/deltadrawable.h"
#include "dtCore/transform.h"
#include "dtCore/refptr.h"
#include <osg/MatrixTransform>

namespace dtCore
{
   ///Anything that can be located and moved in 3D space
   
   /** The Transformable class is the base class of anything that can move in the 
     * virtual world and can be added to the Scene.
     * 
     * The default coordinate system of dtCore is +X to the right, +Y forward into
     * the screen, and +Z is up.  Therefore, heading is around the Z axis, pitch
     * is around the X axis, and roll is around the Y axis.  The angles are all
     * right-hand-rule.
     * 
     * The Transformable class creates a osg::MatrixTransform node for the
     * protected member mNode.  
     */
   class DT_EXPORT Transformable : virtual public  DeltaDrawable  
   {
      DECLARE_MANAGEMENT_LAYER(Transformable)
   public:
      enum CoordSysEnum{
         REL_CS, ///< The Transform coordinate system is relative to the parent
         ABS_CS  ///< The Transform coordinate system is absolute
      } ;

      Transformable();
      virtual ~Transformable();

      ///Add a DeltaDrawable child
      virtual bool AddChild( DeltaDrawable *child );
         
      ///Remove a DeltaDrawable child
      virtual void RemoveChild( DeltaDrawable *child );

      ///Set the Transform to reposition this Transformable
      virtual void SetTransform( Transform *xform, CoordSysEnum cs=ABS_CS );

      ///Get the current Transform of this Transformable
      virtual void GetTransform( Transform *xform, CoordSysEnum cs=ABS_CS  );

      ///convenience function to return back the internal matrix transform node
      virtual osg::MatrixTransform* GetMatrixNode(void)
      { return dynamic_cast<osg::MatrixTransform*>( mNode.get() ); }

      ///render method for an object which may not have geometry
      virtual void RenderProxyNode( bool enable = true );
      
     ///Get the world coordinate matrix from the supplied node
      static bool GetAbsoluteMatrix( osg::Node *node, osg::Matrix& wcMatrix );

      ///Automatically rescales normals if you scale your objects.
      void SetNormalRescaling( bool enable );

      ///Checks if we are rescaling normals for this object.
      bool GetNormalRescaling() const;

   private:

      class getWCofNodeVisitor : public osg::NodeVisitor
      {
         public:
            getWCofNodeVisitor( osg::Matrix& matrix ):
               osg::NodeVisitor(NodeVisitor::TRAVERSE_PARENTS),
               wcMatrix(matrix),
               done(false)
            {}

            virtual void apply(osg::Node &node)
            {
               if (!done)
               {
                  if ( 0 == node.getNumParents() )
                  {
                     wcMatrix.set( osg::computeLocalToWorld(this->getNodePath()) );
                     done = true;
                  }
                  traverse(node);
               }
            }

         private:
            osg::Matrix& wcMatrix;
            bool done;
      };

   };
};



#endif // DELTA_TRANSFORMABLE
