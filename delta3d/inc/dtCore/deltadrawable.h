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
#include <dtCore/refptr.h>

#include <osg/BoundingBox>
#include <osg/Vec3>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Node;
}
/// @endcond

namespace dtCore
{
   class Scene;
   class DeltaDrawablePrivate;
   class Transformable;

   /**
    * A renderable object. A Drawable is a virtual base class which cannot be
    * created, but rather must be derived.  The derived class must instantiate
    * the mNode protected variable which is some type of osg::Node.
    * A Drawable instance must be added as a child to the Scene for it to be rendered.
    *
    * @see Scene::AddChild
    */
   class DT_CORE_EXPORT DeltaDrawable : public Base
   {
      DECLARE_MANAGEMENT_LAYER(DeltaDrawable)

   public:
      /**
       * Get the internal node. Returns this object's OpenSceneGraph node.
       *
       * @return the OpenSceneGraph node
       */
      virtual osg::Node* GetOSGNode() = 0;
      virtual const osg::Node* GetOSGNode() const = 0;

      ///Supply the Scene this Drawable has been added to
      virtual void AddedToScene(Scene* scene);

      ///Supply the Scene this Drawable has been removed from
      virtual void RemovedFromScene(Scene* scene);

      ///Override function for derived object to know when attaching to scene
      virtual void SetParent(DeltaDrawable* parent);

      DeltaDrawable* GetParent();
      const DeltaDrawable* GetParent() const;

      ///Get a pointer to the Scene this Drawable has been added to
      Scene* GetSceneParent();
      const Scene* GetSceneParent() const;

      ///Convert 'this' into a dtCore::Transformable if it is one, otherwise return NULL
      virtual dtCore::Transformable* AsTransformable() { return NULL; }

      ///Add a child to this DeltaDrawable
      virtual bool AddChild(DeltaDrawable* child);

      ///Remove a DeltaDrawable child
      virtual void RemoveChild(DeltaDrawable* child);

      ///Remove this DeltaDrawable from it's parent
      void Emancipate();

      ///Return the number of DeltaDrawable children added
      unsigned int GetNumChildren() const;

      /**
       * Temporary convenience method for retrieving all child drawables from the transform of
       * this actor's drawable.
       * @param outDrawables List container to capture all the drawables.
       * @return Number of drawable retrieved.
       */
      typedef std::vector<dtCore::DeltaDrawable* > DrawableList;
      unsigned int GetChildren(DrawableList& outDrawables);

      /**
       * Get the child specified by index (0 to number of children-1)
       *
       * @param idx : The index number of the DeltaDrawable to be retrieved
       * @return DeltaDrawable* : The DeltaDrawable at idx, or NULL if that
       * index does not exist.
       */
      DeltaDrawable* GetChild(unsigned int idx);

      /**
       * Get the const child specified by index (0 to number of children-1)
       *
       * @param idx : The index number of the DeltaDrawable to be retrieved
       * @return const DeltaDrawable* : The DeltaDrawable at idx, or NULL if that
       * index does not exist.
       */
      const DeltaDrawable* GetChild(unsigned int idx) const;

      /**
       * Get the index number of child. Return a value between
       * 0 and the number of children-1 if found, if not found then
       * return the number of children.
       */
      unsigned int GetChildIndex(const DeltaDrawable* child) const;

      /**
       * Determines if the supplied drawable is a child of this drawable.
       */
      bool HasChild(const DeltaDrawable* child) const;

      /**
       * Check if the supplied DeltaDrawable can actually be a child of this instance.
       *
       * @param child : The child to test
       * @return true if the supplied parameter can be a child
       */
      bool CanBeChild(DeltaDrawable* child) const;

      /**
       * Render the "proxy" node for this DeltaDrawable, which represents an
       * alternative graphical representation for this object.  Some objects,
       * such as Cameras, don't have an actual rendered geometry.  This is a
       * means to view such objects.  Derivatives will need to overwrite and
       * implement their own custom proxy node rendering.
       * @param enable To enable or disable the rendering of this DeltaDrawable
       */
      virtual void RenderProxyNode(bool enable = true);

      /**
       * Check if this DeltaDrawable is currently rendering it's proxy node.
       * Derivatives might need to overwrite this method to provide custom
       * checking.
       * @return If this DeltaDrawable is currently rendering it's proxy node
       */
      virtual bool GetIsRenderingProxyNode() const;

      /**
       * Get the bounding sphere information for this Drawable.
       * @param center : pointer to fill out with the sphere's center position
       * @param radius : float pointer to fill out with the sphere's radius
       */
      virtual void GetBoundingSphere(osg::Vec3& center, float& radius);

      /**
       * Get the bounding box information for this Drawable.
       *
       * @return BoundingBox that encloses the Drawable.
       */
      virtual osg::BoundingBox GetBoundingBox();

      /**
       * Make this DeltaDrawable "active" or "inactive".  The default
       * DeltaDrawalbe behavior will set the node mask of the node returned
       * from GetOSGNode() to 0x0 when false.  This will disable its rendering and
       * other scene graph processing.  Any previously set node mask will be
       * restored upon passing true.
       * Overwrite in derived classes for custom active/inactive behavior.
       * @param enable : true to activate, false to deactivate
       */
      virtual void SetActive(bool enable);

      /**
       * Query if this DeltaDrawable is currently active and its scene graph
       * is being traversed.
       * Overwrite in derived classes for custom active/inactive behavior.
       * @return true if active, false if not active
       */
      virtual bool GetActive() const;

      /**
       * Allows a description to be added to this DeltaDrawable for
       * application purposes.
       * @param description The string used to describe this object
       * @see GetDescription()
       */
      void SetDescription(const std::string& description);

      /**
       * Get a previously set description string of this object.
       * @return The description string (could be empty)
       * @see SetDescription()
       */
      const std::string& GetDescription() const;

      /**
       * Sets the shader group on the game actor.  This implementation uses
       * the default shader in the group to shade the game actor.
       * @param groupName The name of the shader group.
       */
      virtual void SetShaderGroup(const std::string& groupName);

      /**
       * Gets the current shader group assigned to this game actor.
       * @return The name of the group.
       */
      std::string GetShaderGroup() const;

      /**
       * Method to override to pick up on changes to shader assignment.
       */
      virtual void OnShaderGroupChanged();
      
      /**
       * Method to actually apply the currently specified shader group.
       * This can be used to reapply a shader if it has been recompiled
       * or if the drawable has new geometry that needs the current shader applied.
       */
      void ApplyShaderGroup();

      /**
       * Adds tangents to any osg::drawables that are under this object.
       */
      bool GenerateTangents();


   protected:
      DeltaDrawable(const std::string& name = "DeltaDrawable");
      virtual ~DeltaDrawable();

      osg::Node* GetProxyNode();
      const osg::Node* GetProxyNode() const;
      void SetProxyNode(osg::Node* proxyNode);

      /// Callback which is called when this
      /// drawable's parent is removed
      virtual void OnOrphaned();

   private:
      // Disallowed to prevent compile errors on VS2003. It apparently
      // creates this functions even if they are not used, and if
      // this class is forward declared, these implicit functions will
      // cause compiler errors for missing calls to "ref".
      DeltaDrawable& operator=(const DeltaDrawable&);
      DeltaDrawable(const DeltaDrawable&);

      DeltaDrawablePrivate* mPvt; ///<private pointer to implementation
   };

} // namespace dtCore

#endif // DELTA_DELTA_DRAWABLE
