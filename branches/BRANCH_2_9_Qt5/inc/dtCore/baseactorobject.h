/* -*-c++-*-
 * Delta3D
 * Copyright 2010, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Matthew W. Campbell
 * David Guthrie
 */

#ifndef BASEACTOROBJECT_H_
#define BASEACTOROBJECT_H_

#include <string>
#include <set>
#include <osg/Referenced>
#include <dtUtil/enumeration.h>
#include <dtUtil/refstring.h>
#include <dtUtil/deprecationmgr.h>
#include <dtCore/uniqueid.h>
#include <dtCore/refptr.h>
#include <dtCore/observerptr.h>
#include <dtCore/export.h>
#include <dtCore/propertycontainer.h>
#include <dtUtil/macros.h>


namespace dtCore
{
   class DeltaDrawable;
}

namespace dtCore
{
   class ActorType;
   class DataType;
   class ResourceDescriptor;
   class ActorProxyIcon;


   class PrototypeInfo : public osg::Referenced
   {
   public:
      std::string mPrototypeName;
      dtCore::UniqueId mPrototypeId;
   };

   /**
    * This is the base class for all actor.
    * @note
    *      Actor objects must be created through the ActorPluginRegistry or
    *      the ActorFactory. If they are not created in this fashion,
    *      the actor types will not be set correctly.
    */
   class DT_CORE_EXPORT BaseActorObject : public PropertyContainer
   {
      DECLARE_MANAGEMENT_LAYER(BaseActorObject)

   public:
      static const dtUtil::RefString PROPERTY_NAME;
      static const dtUtil::RefString PROPERTY_TYPE_CATEGORY;
      static const dtUtil::RefString PROPERTY_TYPE_NAME;
      static const dtUtil::RefString PROPERTY_CLASS_NAME;
      static const dtUtil::RefString PROPERTY_DESCRIPTION;
      static const dtUtil::RefString PROPERTY_ACTIVE;

      /**
       * This enumeration defines the different ways an actor can
       * be rendered in the level editor.  Based on the state of an actor,
       * it can choose to represent itself in these different ways.
       */
      class DT_CORE_EXPORT RenderMode : public dtUtil::Enumeration
      {
         DECLARE_ENUM(RenderMode);
      public:

         ///Renders only the actor.
         static const RenderMode DRAW_ACTOR;

         ///Renders a billboard to represent the position and orientation of the actor.
         static const RenderMode DRAW_BILLBOARD_ICON;

         ///Renders both the billboard and the actor at the same time.
         static const RenderMode DRAW_ACTOR_AND_BILLBOARD_ICON;

         /**
          * Lets the system determine how to properly render the actor.  This
          * basically means that if the underlying actor contains any geometry,
          * the RenderMode will be set to DRAW_ACTOR else it will be set to
          * DRAW_BILLBOARD_ICON.
          */
         static const RenderMode DRAW_AUTO;

      protected:

      private:
         RenderMode(const std::string& name) : Enumeration(name)
         {
            AddInstance(this);
         }
      };

      /**
       * Constructs the actor.
       */
      BaseActorObject();

      /**
       * Initializes the actor by calling
       * CreateDrawable
       * SetActorType
       * BuildPropertyMap
       *
       * Subclasses may override this to do more init that needs to depend on the actor or properties
       * but must happen before the creation is complete.
       *
       * This would be called from the constructor, but virtual methods may not be called from there.
       */
      virtual void Init(const dtCore::ActorType& actorType);

      /**
       * Gets the UniqueID object.
       * @return The UniqueID.
       */
      const dtCore::UniqueId& GetId() const;

      /**
       * Sets the UniqueID.
       * @param newId The new ID.
       */
      void SetId(const dtCore::UniqueId& newId);

      /**
       * Gets the name of the actor.  In previous versions, this value was stored on the delta drawable.
       * This is no longer the case.
       * @return A string identifier.
       */
      const std::string& GetName() const;

      /**
       * Sets the name of this actor.
       * @param name The new name to assign to this actor.
       */
      void SetName(const std::string& name);

      /**
       * Retrieve the class name
       * @note consider not using this, but just use the actor type.
       * @return The class name
       */
      const std::string& GetClassName() const;

      /**
       * Returns if one object is an instance of another
       *
       * @note Consider using the similar functionality where actor types can have parent types instead of this.
       #
       * @param name The name of the object to check against
       * @return true if it is, false if not
       */
      bool IsInstanceOf(const dtUtil::RefString& name) const;

      /**
       * Returns an alphabetically ordered list of strings corresponding to the list of
       * ancestor classes from which this derives.
       * @deprecated look on the actor type
       * @return a const STL set by value.
       */
      DEPRECATE_FUNC const std::set<std::string> GetClassHierarchy() const;

      /// Call IsGameActor
      DEPRECATE_FUNC bool IsGameActorProxy() const { return IsGameActor(); }

      /**
       * This is a shortcut to avoid having to dynamic cast to a ActorObject.
       * It should only be overridden by dtGame::GameActorProxy.
       * @return true if this actor is an instance of dtGame::GameActorProxy
       */
      virtual bool IsGameActor() const { return false; }

      /**
       * If this actor represents a high level system component.  The code may chose to handle this differently.
       */
      virtual bool IsSystemComponent() const { return false; }

      /**
       * Gets the actor type that represents this actor.
       * @note
       *      The actor type is assigned to the actor when it is created
       *      by the actor registry.
       */
      const ActorType& GetActorType() const;
      const ObjectType& GetObjectType() const;

      /**
       * Gets the drawable associated with this actor.
       */
      dtCore::DeltaDrawable* GetDrawable();

      /// Templated version of GetDrawable that returns the drawable with a dynamic cast to the type requested.
      template <typename TPtr>
      TPtr* GetDrawable() { return dynamic_cast<TPtr*>(GetDrawable()); }

      /** Templated version of GetDrawable() that dynamic casts the actor to the type passed in.
       *  @note Make sure the supplied pointer is of the correct type which
       *  matches this actor!
       * @code
       * dtCore::InfiniteLight *light;
       * actor->GetDrawable(light);
       * @endcode
       */
      template <typename TPtr>
      void GetDrawable(TPtr& drawableType)
      {
         drawableType = dynamic_cast<TPtr>(GetDrawable());
      }

      /** Templated version of GetDrawable() that static casts the actor to the type passed in.
       *  @note Make sure the supplied ref pointer is of the correct type which
       *  matches the actor!
       * @code
       * dtCore::RefPtr<dtCore::InfiniteLight> light;
       * actor->GetDrawable(light);
       * @endcode
       */
      template <typename T>
      void GetDrawable(dtCore::RefPtr<T>& drawable)
      {
         drawable = dynamic_cast<T*>(GetDrawable());
      }

      /// Templated version of GetDrawable that returns the drawable with a dynamic cast to the type requested.
      template <typename TPtr>
      const TPtr* GetDrawable() const { return dynamic_cast<const TPtr*>(GetDrawable()); }

      /// Const version of GetDrawable()
      const dtCore::DeltaDrawable* GetDrawable() const;

      /// Templated version of GetDrawable() const that static casts the actor to the type passed in.
      template <typename TPtr>
      void GetDrawable(TPtr& drawable) const
      {
         drawable = dynamic_cast<TPtr>(GetDrawable());
      }


      /**
       * Sets the billboard icon used to represent this actor.
       * @param icon The billboard to use.
       * @note This will only be rendered if the RenderMode is set to
       *  DRAW_BILLBOARD_ICON.
       */
      virtual void SetBillBoardIcon(ActorProxyIcon* icon);

      /**
       * Gets the billboard icon currently assigned to this actor.
       * @return The billboard icon.
       */
      virtual ActorProxyIcon* GetBillBoardIcon();

      /**
       * Gets the render mode currently active on this actor.
       * @return The current render mode.
       */
      virtual const RenderMode& GetRenderMode();

      /**
       * This method is called init, which instructs the
       * actor to create its properties.  Methods implementing this should
       * be sure to call their parent class's buildPropertyMap method to
       * ensure all properties in the actor inheritance hierarchy are
       * correctly added to the property map.
       *
       * @see GetDeprecatedProperty to handle old properties that need
       *       to be removed.
       */
      virtual void BuildPropertyMap();


      /**
       * Not all objects are "placeable" in the scene.  For example, an infinite
       * light does not really have a position in the scene, therefore, it is
       * not a placeable actor.  Non-placeable actors are still added to a
       * Delta3D scene, they are just not able to be moved around and placed
       * throughout a scene in the same way as a placeable object.  Actor proxies
       * of this type are referred to as "global actors".
       */
      virtual bool IsPlaceable() const = 0;


      /**
       * if this returns true, this actor will not be saved into the map
       * useful for having actors that own other actors
       */
      virtual bool IsGhost() const;


      /**
       * Called when this actor is removed from a map
       */
      virtual void OnRemove() const;

      /**
       * Called when this BaseActorObject has been created during a map load
       * before any properties have been assigned.  Overwrite for custom
       * behavior.
       */
      virtual void OnMapLoadBegin();

      /**
       *  Called when this BaseActorObject has finished loading from a Map and
       *  all Property values have been assigned.  Overwrite for custom
       *  behavior.
       */
      virtual void OnMapLoadEnd();

      /**
       * Creates a copy of this actor and returns it.  The method uses the
       * library manager to create the clone and then iterates though the
       * current state of this actor's property set and copies their values
       * to the newly created clone.
       * @return The cloned actor.
       */
      virtual RefPtr<BaseActorObject> Clone();

      /**
       * Returns true if called from STAGE
       * @return true if STAGE is running, false if not
       */
      bool IsInSTAGE() const;

      /**
       * Allow access to the ActorPluginRegistry.  This is so it can set the
       * ActorType of the actor when it creates it and call any initialization
       * methods on the actor.
       */
      friend class ActorPluginRegistry;

      bool operator == (const BaseActorObject& rhs) const
      {
         return GetId() == rhs.GetId();
      }

      bool operator != (const BaseActorObject& rhs) const
      {
         return !(*this == rhs);
      }

      /// This api may be temporary just to remove it from the drawable.
      /// This may be the object from which it was cloned, or it may be a prefab
      BaseActorObject* GetPrototype();
      const BaseActorObject* GetPrototype() const;
      void SetPrototype(BaseActorObject* proto);

      /// Temporary method to determine if this object is a descendant of ActorComponent.
      /// Remove this method when ActorComponent has been promoted to dtCore.
      virtual bool IsActorComponent() const { return false; }

   protected:

      /**
       * Sets the actor on this actor. This should be only called from subclasses
       * @param actor The actor to set
       */
      void SetDrawable(dtCore::DeltaDrawable& drawable);

      ///Keep the destructor protected since we use dtCore::RefPtr to
      ///track any object created.
      virtual ~BaseActorObject();

      /**
       * Create the drawable this BaseActorObject is managing.  It's typical to overwrite
       * this method, instantiate the actor, and supply it to SetActor().
       * @code
       * void MyProxy::CreateDrawable()
       * {
       *    MyDrawable* draw = new MyDrawable();
       *    assert(draw);
       *    SetDrawable(*Drawable);
       * }
       * @endcode
       */
      virtual void CreateDrawable();
      DEPRECATE_FUNC virtual void CreateActor() {}

      /**
       * Set the class name
       * @param name The class name to set
       */
      void SetClassName(const std::string& name);

      /**
       * Each actor may have a billboard associated with it.  Billboards
       * are displayed in place of the actual actor if the actor has no
       * displayable qualities.  For example, if a static mesh has no mesh
       * currently assigned to it, the billboard will display instead.
       */
      RefPtr<ActorProxyIcon> mBillBoardIcon;

   private:


      ///Pointer to the Delta3D object (Actor) this actor is wrapping.
      RefPtr<DeltaDrawable> mDrawable;
      ///ActorType corresponding to this actor.
      RefPtr<const ActorType> mActorType;
      // This needs to be converted to a reference
      ObserverPtr<BaseActorObject> mPrototype;
      dtUtil::RefString mName;
      UniqueId mId;



      ///Simple method for setting the actor type.
      void SetActorType(const ActorType& type);

      ///Hidden copy constructor.
      BaseActorObject(const BaseActorObject&);

      ///Hidden assignment operator.
      BaseActorObject& operator=(const BaseActorObject&);

   };

   typedef dtCore::RefPtr<BaseActorObject> ActorPtr;
   typedef std::vector<BaseActorObject*> ActorPtrVector;
   typedef std::vector<ActorPtr> ActorRefPtrVector;
}

#endif /* BASEACTOROBJECT_H_ */
