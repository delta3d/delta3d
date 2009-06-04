/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * Matthew W. Campbell
 */

#ifndef DELTA_ACTORPROXY
#define DELTA_ACTORPROXY

//To get the windows.h undefs before anything else.
#include <dtUtil/macros.h>

#include <string>
#include <set>
#include <osg/Referenced>
#include <dtUtil/enumeration.h>
#include <dtUtil/refstring.h>
#include <dtCore/uniqueid.h>
#include <dtCore/refptr.h>
#include <dtDAL/export.h>
#include <dtDAL/propertycontainer.h>


namespace dtCore
{
   class DeltaDrawable;
}

namespace dtDAL
{
   class ActorType;
   class DataType;
   class ResourceDescriptor;
   class ActorProxyIcon;

   /**
    * This is the base class for all actor proxy objects.  The proxy's
    * main purpose is to expose the attributes of an actor class (or
    * Delta3D object) via a list of properties.
    * @note
    *      Proxy objects must be created through the ActorPluginRegistry or
    *      the LibraryManager. If they are not created in this fashion,
    *      the actor types will not be set correctly.
    *  @note
    *      Client-side applications may or may not wish to use the proxy
    *      interface for accessing actor objects.  However, the actor proxy
    *      is the only way to expose actors to the Delta3D Level Editor.
    */
   class DT_DAL_EXPORT ActorProxy : public PropertyContainer
   {
      public:

         /**
          * This enumeration defines the different ways an actor proxy can
          * be rendered in the level editor.  Based on the state of an actor
          * proxy, it can choose to represent itself in these different ways.
          */
         class DT_DAL_EXPORT RenderMode : public dtUtil::Enumeration
         {
            DECLARE_ENUM(RenderMode);
            public:

               ///Renders only the actor.
               static const RenderMode DRAW_ACTOR;

               ///Renders a billboard to represent the position and orientatino of the proxy.
               static const RenderMode DRAW_BILLBOARD_ICON;

               ///Renders both the billboard and the actor at the same time.
               static const RenderMode DRAW_ACTOR_AND_BILLBOARD_ICON;

               /**
                * Lets the system determine how to properly render the actor proxy.  This
                * basically means that if the underlying actor contains any geometry,
                * the RenderMode will be set to DRAW_ACTOR else it will be set to
                * DRAW_BILLBOARD_ICON.
                */
               static const RenderMode DRAW_AUTO;

            private:
               RenderMode(const std::string& name) : Enumeration(name)
               {
                  AddInstance(this);
               }
         };

         /**
          * Constructs the actor proxy.
          */
         ActorProxy();

         /**
          * Initializes the actor by calling
          * CreateActor
          * SetActorType
          * BuildPropertyMap
          *
          * Subclasses may override this to do more init that needs to depend on the actor or properties
          * but must happen before the creation is complete.
          *
          * This would be called from the constructor, but virtual methods may not be called from there.
          */
         virtual void Init(const dtDAL::ActorType& actorType);

         /**
          * Gets the UniqueID object assigned to this actor proxy.
          * @return The UniqueID.
          */
         const dtCore::UniqueId& GetId() const;

         /**
          * Sets the UniqueID mapped to this actor proxy.
          * @param newId The new ID.
          */
         void SetId(const dtCore::UniqueId& newId);

         /**
          * Gets the name of the actor proxy.  This is actually a wrapper which just
          * returns the name assigned to the underlying actor.
          * @return A string identifier.
          */
         const std::string& GetName() const;

         /**
          * Sets the name of this actor proxy.  In actuality, this is setting the
          * name of the underlying actor this proxy is holding.
          * @param name The new name to assign to this actor proxy.
          */
         void SetName(const std::string& name);

         /**
          * Retrieve the class name
          * @return The class name
          */
         const std::string& GetClassName() const { return mClassName; }

         /**
          * Returns if one object is an instance of another
          * @param name The name of the object to check against
          * @return true if it is, false if not
          */
         bool IsInstanceOf(const std::string& name) const
         {
            return mClassNameSet.find(name) != mClassNameSet.end();
         }

         /**
          * Returns an alphabetically ordered list of strings corresponding to the list of
          * ancestor classes from which this derives.
          * @return a const STL set by value.
          */
         const std::set<std::string> GetClassHierarchy() const;

         /**
          * This is a shortcut to avoid having to dynamic cast to a GameActorProxy.
          * It should only be overridded by dtGame::GameActorProxy.
          * @return true if this proxy is an instance of dtGame::GameActorProxy
          */
         virtual bool IsGameActorProxy() const { return false; }

         /**
          * Gets a ResourceDescriptor of the requested property name.
          * @param name Name of the resource to retrieve.
          * @return A pointer to the resource descripter or NULL if it
          * is not found.
          */
         ResourceDescriptor* GetResource(const std::string& name);

         /**
          * Gets a ResourceDescriptor of the requested property name.
          * @param name Name of the resource to retrieve.
          * @return A pointer to the resource descripter or NULL if it
          * is not found.
          */
         const ResourceDescriptor* GetResource(const std::string& name) const;

         /**
          * Sets a resource in the map
          * @param name The name of the resource
          * @param source The pointer to resource descriptor
          */
         void SetResource(const std::string& name, ResourceDescriptor* source);

         /**
          * Gets a property of the requested name.
          * @param name Name of the actor actor property to retrieve.
          * @return A pointer to the ActorProxy or NULL if it
          * is not found.
          */
         const ActorProxy* GetLinkedActor(const std::string& name) const;

         /**
          * Gets a property of the requested name.
          * @param name Name of the actor actor property to retrieve.
          * @return A pointer to the ActorProxy or NULL if it
          * is not found.
          */
         ActorProxy* GetLinkedActor(const std::string& name);

         /**
          * Sets an actor proxy in the map
          * @param name The name of the property to set
          * @param value The pointer to new proxy value
          */
         void SetLinkedActor(const std::string& name, ActorProxy* newValue);

         /**
          * Gets the actor type that represents this actor proxy.
          * @note
          *      The actor type is assigned to the proxy when it is created
          *      by the actor registry.
          */
         const ActorType& GetActorType() const;

         /**
          * Gets the actor who's properties are modeled by this proxy.
          * @note
          *      Actors in the Dynamic Actor Layer are actually synonyms
          *      for any Delta3D object.  All Delta3D objects that are
          *      drawable derive from dtCore::DeltaDrawable.  Therefore,
          *      an actor is any class that contains dtCore::DeltaDrawable
          *      in its inheritance tree.
          */
         dtCore::DeltaDrawable* GetActor();

         /** Templated version of GetActor() that static casts the actor to the type passed in.
          *  @note Make sure the supplied pointer is of the correct type which
          *  matches the proxy!
          * @code
          * dtCore::InfiniteLight *light;
          * proxy->GetActor(light);
          * @endcode
          */
         template <typename TActorPtr>
         void GetActor(TActorPtr& actorType)
         {
            actorType = static_cast<TActorPtr>(GetActor());
         }

         /**
          * Gets the actor who's properties are modeled by this proxy.
          * @note
          *      Actors in the Dynamic Actor Layer are actually synonyms
          *      for any Delta3D object.  All Delta3D objects that are
          *      drawable derive from dtCore::DeltaDrawable.  Therefore,
          *      an actor is any class that contains dtCore::DeltaDrawable
          *      in its inheritance tree.
          */
         const dtCore::DeltaDrawable* GetActor() const;

         /// Templated version of GetActor() const that static casts the actor to the type passed in.
         template <typename TActorPtr>
         void GetActor(TActorPtr& actorType) const
         {
            actorType = static_cast<TActorPtr>(GetActor());
         }

         /**
          * Sets the billboard icon used to represent this actor proxy.
          * @param icon The billboard to use.
          * @note This will only be rendered if the RenderMode is set to
          *  DRAW_BILLBOARD_ICON.
          */
         virtual void SetBillBoardIcon(ActorProxyIcon* icon);

         /**
          * Gets the billboard icon currently assigned to this actor proxy.
          * @return The billboard icon.
          */
         virtual ActorProxyIcon* GetBillBoardIcon();

         /**
          * Gets the render mode currently active on this actor proxy.
          * @return The current render mode.
          */
         virtual const RenderMode& GetRenderMode();

         /**
          * This method is called init, which instructs the
          * proxy to create its properties.  Methods implementing this should
          * be sure to call their parent class's buildPropertyMap method to
          * ensure all properties in the proxy inheritance hierarchy are
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
          * if this returns true, this proxy will not be saved into the map
          * useful for having proxies that own other proxies
          */
         virtual bool IsGhostProxy() const;


         /**
          * Called when this actor is removed from a map
          */
         virtual void OnRemove() const;

         /**
          *  Called when this ActorProxy has finished loading from a Map and
          *  all Property values have been assigned.  Overwrite for custom
          *  behavior.
          */
         virtual void OnMapLoadEnd();

         /**
          * Creates a copy of this actor proxy and returns it.  The method uses the
          * library manager to create the clone and then iterates though the
          * current state of this proxy's property set and copies their values
          * to the newly created clone.
          * @return The cloned actor proxy.
          */
         virtual dtCore::RefPtr<ActorProxy> Clone();

         /**
          * Returns true if called from STAGE
          * @return true if STAGE is running, false if not
          */
         const bool IsInSTAGE() const;

         /**
          * Allow access to the ActorPluginRegistry.  This is so it can set the
          * ActorType of the proxy when it creates it and call any initialization
          * methods on the actor proxy.
          */
         friend class ActorPluginRegistry;

         bool operator == (const ActorProxy& rhs) const
         {
            return GetId() == rhs.GetId();
         }

         bool operator != (const ActorProxy& rhs) const
         {
            return !(*this == rhs);
         }

      protected:

         /**
          * Sets the actor on this proxy. This should be only called from subclasses
          * @param actor The actor to set
          */
         void SetActor(dtCore::DeltaDrawable& actor);

         ///Keep the destructor protected since we use dtCore::RefPtr to
         ///track any object created.
         virtual ~ActorProxy();

         /**
          * Initializes the actor.
          */
         virtual void CreateActor() = 0;

         /**
          * Set the class name
          * @param name The class name to set
          */
         void SetClassName(const std::string& name);

         /**
          * Each actor proxy may have a billboard associated with it.  Billboards
          * are displayed in place of the actual actor if the actor has no
          * displayable qualities.  For example, if a static mesh has no mesh
          * currently assigned to it, the billboard will display instead.
          */
         dtCore::RefPtr<ActorProxyIcon> mBillBoardIcon;

      private:
         typedef std::map<dtUtil::RefString, ResourceDescriptor> ResourceMapType;
         typedef std::map<dtUtil::RefString, dtCore::RefPtr<ActorProxy> > ActorProxyMapType;
         typedef std::set<dtUtil::RefString> ClassHierarchyType;

         ///Pointer to the Delta3D object (Actor) this proxy is wrapping.
         dtCore::RefPtr<dtCore::DeltaDrawable> mActor;

         /// Map of property names to resource values
         ResourceMapType mResourceMap;

         /// Map of property names to actor proxy values
         ActorProxyMapType mActorProxyMap;

         /// Set of class names
         ClassHierarchyType mClassNameSet;

         ///ActorType corresponding to this proxy.
         dtCore::RefPtr<const ActorType> mActorType;

         /// The current class name
         dtUtil::RefString mClassName;

         ///Simple method for setting the actor type.
         void SetActorType(const ActorType& type);

         ///Hidden copy constructor.
         ActorProxy(const ActorProxy&);

         ///Hidden assignment operator.
         ActorProxy& operator=(const ActorProxy&);
   };
}

#endif
