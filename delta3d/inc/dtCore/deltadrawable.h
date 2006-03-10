/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
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

#include <dtCore/base.h>
#include <dtUtil/log.h>
#include <dtCore/refptr.h>
#include <osg/Node>

namespace dtCore
{
   //forward declaration
   class Scene;

   ///A renderable object. 

   /**
    * A Drawable is a virtual base class which cannot be
    * created, but rather must be derived.  The derived class must instantiate 
    * the mNode protected variable which is some type of osg::Node. 
    * A Drawable instance must be added to the Scene for it to be rendered.
    *
    * @see Scene::AddDrawable
    */
   class DT_CORE_EXPORT DeltaDrawable : public Base
   {
      DECLARE_MANAGEMENT_LAYER(DeltaDrawable)

      public:

         ///Get the internal node
         /**
          * Returns this object's OpenSceneGraph node.
          * @return the OpenSceneGraph node
          */
         virtual osg::Node* GetOSGNode() 
         {
            return mNode.get();
         }
         
         virtual const osg::Node* GetOSGNode() const
         {
            return mNode.get();
         }

         ///Supply the Scene this Drawable has been added to
         virtual void AddedToScene( Scene* scene );

         ///Override function for derived object to know when attaching to scene
         virtual void SetParent(DeltaDrawable* parent) {mParent=parent;}

         DeltaDrawable* GetParent() { return mParent; }
         
         ///Get a pointer to the Scene this Drawable has been added to
         Scene* GetSceneParent();

         ///Add a child to this DeltaDrawable
         virtual bool AddChild( DeltaDrawable *child );

         ///Remove a DeltaDrawable child
         virtual void RemoveChild( DeltaDrawable *child );

         ///Remove this DeltaDrawable from it's parent
         void Emancipate();

         ///Return the number of DeltaDrawable children added
         inline unsigned int GetNumChildren() { return mChildList.size(); }

         ///Get the child specified by idx (0 to number of children-1)
         DeltaDrawable* GetChild( unsigned int idx ) {return mChildList[idx].get();}

         ///Get the index number of child
         /** Return a value between
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

         ///Check if the supplied DeltaDrawable can actually be a chil of this instance.
         /** 
          * @param child : The child to test
          * @return true if the supplied parameter can be a child
          */
         bool CanBeChild( DeltaDrawable *child );

         virtual void RenderProxyNode( bool enable = true )
         {  
            if( mProxyNode == 0 )
               LOG_WARNING("Proxy node is not implemented, overwrite RenderProxyNode." );
         };

   protected:
      DeltaDrawable( const std::string& name = "DeltaDrawable");
      virtual ~DeltaDrawable();

      RefPtr<osg::Node> mNode; ///< The node to store anything
      DeltaDrawable* mParent; ///<Any immediate parent of this instance

      typedef std::vector<RefPtr<DeltaDrawable> > ChildList;
      ChildList mChildList;      ///<List of children DeltaDrawable added

      Scene* mParentScene; ///<The Scene this Drawable was added to

      RefPtr<osg::Node> mProxyNode;
      bool mRenderingProxy;
   };
};


#endif // DELTA_DELTA_DRAWABLE
