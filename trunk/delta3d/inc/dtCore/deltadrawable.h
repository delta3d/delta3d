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

#ifndef DELTA_DELTA_DRAWABLE
#define DELTA_DELTA_DRAWABLE

// deltadrawable.h: Declaration of the DeltaDrawable class.
//
//////////////////////////////////////////////////////////////////////

#include "dtCore/base.h"
#include "dtCore/export.h"
//#include "dtCore/scene.h"
#include "osg/Node"

namespace dtCore
{
   //forward declaration
   class Scene;

   /**
    * A drawable object.
    */
   class DT_EXPORT DeltaDrawable : virtual public Base
   {
      public:

         /**
          * Returns this object's OpenSceneGraph node.
          *
          * @return the OpenSceneGraph node
          */
         virtual osg::Node* GetOSGNode() 
         {
            return (mNode.get());
         }
         

         /**
          * Notifies this drawable object that it has been added to
          * a scene.
          *
          * @param scene the scene to which this drawable object has
          * been added
          */
         virtual void AddedToScene( Scene* scene );

         ///Override function for derived object to know when attaching to scene
         virtual void SetParent(DeltaDrawable* parent) {mParent=parent;}
         DeltaDrawable* GetParent(void)  {return mParent.get();}
         
         Scene* GetSceneParent(void);

         ///Add a DeltaDrawable child
         virtual void AddChild( DeltaDrawable *child );

         ///Remove a DeltaDrawable child
         virtual void RemoveChild( DeltaDrawable *child );

         ///Return the number of Transformable children added
         inline unsigned int GetNumChildren() { return mChildList.size(); }

         ///Get the child specified by idx (0 to number of children-1)
         DeltaDrawable* GetChild( unsigned int idx ) {return mChildList[idx].get();}

         /** Get the index number of child, return a value between
         * 0 and the number of children-1 if found, if not found then
         * return the number of children.
         */
         inline unsigned int GetChildIndex( const DeltaDrawable* child ) const
         {
            for (unsigned int childNum=0;childNum<mChildList.size();++childNum)
            {
               if (mChildList[childNum]==child) return childNum;
            } 
            return mChildList.size(); // node not found.
         }

         ///Test to see if child
         bool CanBeChild( DeltaDrawable *child );

   protected:
      DeltaDrawable(std::string name = "DeltaDrawable");
      virtual ~DeltaDrawable() {};

      osg::ref_ptr<osg::Node> mNode;
      osg::ref_ptr<DeltaDrawable> mParent; ///<Any immediate parent of this instance
      typedef std::vector<osg::ref_ptr<DeltaDrawable> > ChildList;
      ChildList mChildList;      ///<List of children DeltaDrawable added
      osg::ref_ptr<Scene> mParentScene;
   };

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
   typedef DeltaDrawable Drawable;
#endif
};


#endif // DELTA_DELTA_DRAWABLE
