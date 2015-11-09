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

#ifndef DELTA_ACTOR_PLUGIN_REGISTRY
#define DELTA_ACTOR_PLUGIN_REGISTRY

#include <vector>
#include <dtUtil/objectfactory.h>
#include <dtCore/actortype.h>
#include <dtCore/baseactorobject.h>
#include <dtCore/export.h>

namespace dtCore
{
   /**
    * The ActorPluginRegistry is the base class that developers extend to
    * build their own Actor registries.  Its main purpose is to serve as an
    * object factory which knows how to build ActorProxies using
    * ActorTypes as the tool by which to do so.
    * @note
    *      Registry objects should only be used with the dtCore::RefPtr<>
    *      construct since they are reference counted objects.
    * @see ActorType
    * @see BaseActorObject
    * @note The dynamic library file containing the Actor Registry must contain
    *  two exported "C" methods, to create and destroy the ActorPluginRegistry.
    *
    * Example:
    * @code
    * extern "C" DT_EXAMPLE_EXPORT dtCore::ActorPluginRegistry* CreatePluginRegistry()
    * {
    *    return new TestGameActorLibrary;
    * }
    *
    * extern "C" DT_EXAMPLE_EXPORT void DestroyPluginRegistry(dtCore::ActorPluginRegistry* registry)
    * { 
    *   if (registry != NULL)
    *   delete registry;
    * }
    * @endcode
    */
   class DT_CORE_EXPORT ActorPluginRegistry
   {
      public:

         /**
          * Constructs the registry.  Sets the name and description for
          * this registry.
          */
         ActorPluginRegistry(const std::string& name, const std::string& desc = "");

         /**
          * Empty destructor. This class is not reference counted since we need
          * to manually free pointers to the registry objects from their
          * corresponding dynamic library, therefore, we need access to the
          * object's destructor.
          */
         virtual ~ActorPluginRegistry();

         /**
          * Registers the actor types that this registry knows how to create.
          * This method is the first method to get called by the ActorFactory
          * after it loads a dynamic library and gets a pointer to the
          * registry object it contains.
          * Overwrite and use the inherited mActorFactor to register the mapping
          * of concrete BaseActorObject to ActorType.
          * @code
          * dtCore::RefPtr<dtCore::ActorType> MY_ACTOR_TYPE(new dtCore::ActorType("My Actor", "My category", "An Actor"));
          * void MyRegistry::RegisterActorTypes()
          * {
          *    mActorFactory->RegisterType<MyActorProxy>(MY_ACTOR_TYPE.get());
          * }
          * @endcode
          */
         virtual void RegisterActorTypes() = 0;

         /**
          * Sets the name of this registry.
          * @param name Name to assign to the registry.
          */
         void SetName(const std::string& name);

         /**
          * Gets the name currently assigned to this registry.
          */
         const std::string& GetName() const;

         /**
          * Sets the description for this registry.
          * @param desc Couple sentence description for this actor registry.
          */
         void SetDescription(const std::string& desc);

         /**
          * Gets the description of this registry.
          */
         const std::string& GetDescription() const;

         /**
          * Gets a list of actor types that this registry supports.
          */
         virtual void GetSupportedActorTypes(std::vector<dtCore::RefPtr<const ActorType> >& actors);

         /** 
           * Container of <old, new> ActorType names.  First entry is the full name of the
           * old ActorType.  Second entry is the full name of the new ActorType to
           * use instead.
           */
         typedef std::vector<std::pair<std::string, std::string> > ActorTypeReplacements;

         /** 
          * Get the ActorTypeReplacements for this ActorPluginRegistry.  This list
          * is used to provide some backwards compatibility with applications or maps
          * referring to older, deprecated ActorTypes.  Override in derived classes
          * if previous ActorTypes have been modified and backwards compatibility is 
          * desired.
          * @param The container to fill out with ActorType replacements
          */
         virtual void GetReplacementActorTypes(ActorTypeReplacements& replacements) const;

         /**
          * Checks to see if this registry supports the given actor type.
          * @param type The type to check support for.
          * @return True if supported, false otherwise.
          */
         virtual bool IsActorTypeSupported(const ActorType& type) const;

         /**
          * Finds the actor type for the given name and category
          * @return the actor type found or NULL if not found.
          */
         virtual const ActorType* GetActorType(const std::string& category, const std::string& name) const;

         /**
          * Creates a new actor object based on the ActorType given.
          * One can override this to change the behavior.
          * @param type Type of actor to create.
          * @return Returns a smart pointer to the newly created
          * proxy object.
          * @throws ExceptionEnum::ObjectFactoryUnknownType
          */
         virtual dtCore::RefPtr<BaseActorObject> CreateActor(const ActorType& type);

      private:
         std::string mName;
         std::string mDescription;

      protected:

         /**
          * Factory object which stores the actor types and knows how to
          * create proxy objects for each type.
          * @see ObjectFactory
          */
         typedef dtUtil::ObjectFactory<dtCore::RefPtr<const ActorType>, BaseActorObject, ActorType::RefPtrComp> FactoryType;
         dtCore::RefPtr<FactoryType> mActorFactory;
   };
} // namespace dtCore

#endif // DELTA_ACTOR_PLUGIN_REGISTRY
