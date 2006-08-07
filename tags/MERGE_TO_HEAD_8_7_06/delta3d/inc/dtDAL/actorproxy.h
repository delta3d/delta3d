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
 * @author Matthew W. Campbell
 */
#ifndef DELTA_ACTORPROXY
#define DELTA_ACTORPROXY

#include <string>
#include <map>
#include <vector>
#include <set>
#include <osg/Referenced>
#include <dtUtil/enumeration.h>
#include <dtCore/uniqueid.h>
#include <dtCore/refptr.h>
#include "dtDAL/export.h"

namespace dtCore
{
   class DeltaDrawable;
}

namespace dtDAL
{
   class ActorProperty;
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
   class DT_DAL_EXPORT ActorProxy : public osg::Referenced
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
               RenderMode(const std::string &name) : Enumeration(name)
               {
                  AddInstance(this);
               }
         };

         /**
          * Constructs the actor proxy.
          */
         ActorProxy();

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
         bool IsInstanceOf(const std::string &name) const
         {
            return mClassNameSet.find(name) != mClassNameSet.end();
         }

         /**
          * Returns an ordered list of strings corresponding to the inheritance
          * hierarchy for an instance of this class.
          * @return An STL set.
          */
         const std::set<std::string>& GetClassHierarchy() const { return mClassNameSet; }

         /**
          * This is a shortcut to avoid having to dynamic cast to a GameActorProxy.  
          * It should only be overridded by dtGame::GameActorProxy.
          * @return true if this proxy is an instance of dtGame::GameActorProxy
          */
         virtual bool IsGameActorProxy() const { return false; }

         /**
          * Adds a new property to the this proxy's list of properties.
          * @note
          *      Properties must have unique names, therefore, if a property
          *      is added that who's name collides with another, the property
          *      is not added and an error message is logged.
          */
         void AddProperty(ActorProperty *newProp);

         /**
          * Gets a property of the requested name.
          * @param name Name of the property to retrieve.
          * @return A pointer to the property object or NULL if it
          * is not found.
          */
         ActorProperty* GetProperty(const std::string &name);
           
         /**
          * Gets a property of the requested name. (const version)
          * @param name Name of the property to retrieve.
          * @return A pointer to the property object or NULL if it
          * is not found.
          */
         const ActorProperty* GetProperty(const std::string &name) const;

         /**
          * Gets a ResourceDescriptor of the requested property name.
          * @param name Name of the resouce to retrieve.
          * @return A pointer to the resource descripter or NULL if it
          * is not found.
          */
         ResourceDescriptor* GetResource(const std::string &name);

         /**
          * Gets a ResourceDescriptor of the requested property name.
          * @param name Name of the resouce to retrieve.
          * @return A pointer to the resource descripter or NULL if it
          * is not found.
          */
         const ResourceDescriptor* GetResource(const std::string &name) const;

         /**
          * Sets a resource in the map
          * @param name The name of the resouce
          * @param source The pointer to resource descriptor
          */
         void SetResource(const std::string &name, ResourceDescriptor *source);

         /**
          * Gets a property of the requested name.
          * @param name Name of the actor actor property to retrieve.
          * @return A pointer to the ActorProxy or NULL if it
          * is not found.
          */
         const ActorProxy* GetLinkedActor(const std::string &name) const;

         /**
          * Gets a property of the requested name.
          * @param name Name of the actor actor property to retrieve.
          * @return A pointer to the ActorProxy or NULL if it
          * is not found.
          */
         ActorProxy* GetLinkedActor(const std::string &name);

         /**
          * Sets an actor proxy in the map
          * @param name The name of the property to set
          * @param value The pointer to new proxy value
          */
         void SetLinkedActor(const std::string& name, ActorProxy* newValue);

         /**
          * Gets a list of the properties currently registered for this
          * actor proxy.
          */
         void GetPropertyList(std::vector<ActorProperty *>& propList);

         /**
          * Gets a const list of the properties currently registered for this
          * actor proxy.
          */
         void GetPropertyList(std::vector<const ActorProperty *> &propList) const;

         /**
          * Gets the actor type that represents this actor proxy.
          * @note
          *      The actor type is assigned to the proxy when it is created
          *      by the actor registry.
          */
         const ActorType& GetActorType() const { return *mActorType.get(); }

         /**
          * Gets the actor type that represents this actor proxy.
          * @note
          *      The actor type is assigned to the proxy when it is created
          *      by the actor registry.
          */
         ActorType& GetActorType() { return *mActorType.get(); }

         /**
          * Gets the actor who's properties are modeled by this proxy.
          * @note
          *      Actors in the Dynamic Actor Layer are actually synonyms
          *      for any Delta3D object.  All Delta3D objects that are
          *      drawable derive from dtCore::DeltaDrawable.  Therefore,
          *      an actor is any class that contains dtCore::DeltaDrawable
          *      in its inheritance tree.
          */
         virtual dtCore::DeltaDrawable* GetActor();

         /**
          * Gets the actor who's properties are modeled by this proxy.
          * @note
          *      Actors in the Dynamic Actor Layer are actually synonyms
          *      for any Delta3D object.  All Delta3D objects that are
          *      drawable derive from dtCore::DeltaDrawable.  Therefore,
          *      an actor is any class that contains dtCore::DeltaDrawable
          *      in its inheritance tree.
          */
         virtual const dtCore::DeltaDrawable* GetActor() const;

         /**
          * Sets the billboard icon used to represent this actor proxy.
          * @param icon The billboard to use.
          * @note This will only be rendered if the RenderMode is set to
          *  DRAW_BILLBOARD_ICON.
          */
         virtual void SetBillBoardIcon(ActorProxyIcon *icon);

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
          * This method is called by the actor registry which instructs the
          * proxy to create its properties.  Methods implementing this should
          * be sure to call their parent class's buildPropertyMap method to
          * ensure all properties in the proxy inheritance hierarchy are
          * correcly added to the property map.
          */
         virtual void BuildPropertyMap() = 0;

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
          * Creates a copy of this actor proxy and returns it.  The method uses the
          * library manager to create the clone and then iterates though the
          * current state of this proxy's property set and copies their values
          * to the newly created clone.
          * @return The cloned actor proxy.
          */
         virtual dtCore::RefPtr<ActorProxy> Clone();

         /**
          * Allow access to the ActorPluginRegistry.  This is so it can set the
          * ActorType of the proxy when it creates it and call any initialization
          * methods on the actor proxy.
          */
         friend class ActorPluginRegistry;

      protected:
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
         void SetClassName(const std::string &name);

         ///Pointer to the Delta3D object (Actor) this proxy is wrapping.
         dtCore::RefPtr<dtCore::DeltaDrawable> mActor;

         /**
          * Each actor proxy may have a billboard associated with it.  Billboards
          * are displayed in place of the actual actor if the actor has no
          * displable qualities.  For example, if a static mesh has no mesh
          * currently assigned to it, the billboard will display instead.
          */
         dtCore::RefPtr<ActorProxyIcon> mBillBoardIcon;

      private:
         ///Map of properties.
         std::map<std::string, dtCore::RefPtr<ActorProperty> > mPropertyMap;

         /// Map of propery names to resource values
         std::map<std::string, ResourceDescriptor> mResourceMap;

         /// Map of property names to actor proxy values
         std::map<std::string, dtCore::RefPtr<ActorProxy> > mActorProxyMap;

         /// Set of class names
         std::set<std::string> mClassNameSet;

         ///ActorType corresponding to this proxy.
         dtCore::RefPtr<ActorType> mActorType;

         /// The current class name
         std::string mClassName;

         ///Simple method for setting the actor type.
         void SetActorType(ActorType *type);

         ///Hidden copy constructor.
         ActorProxy(const ActorProxy&);

         ///Hidden assignment operator.
         ActorProxy& operator=(const ActorProxy&);
   };
}

#endif
