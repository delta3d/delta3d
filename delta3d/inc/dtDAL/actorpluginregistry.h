/*
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
#ifndef __ActorPluginRegistry__h
#define __ActorPluginRegistry__h

#include <vector>
#include <osg/Referenced>
#include <osg/ref_ptr>
#include <dtDAL/export.h>
#include <dtUtil/objectfactory.h>
#include "dtDAL/actortype.h"
#include "dtDAL/actorproxy.h"
#include "dtDAL/plugin_export.h"

namespace dtDAL
{

    /**
     * The ActorPluginRegistry is the base class that developers extend to
     * build their own registries.  Its main purpose is to serve as an
     * object factory which knows how to build ActorProxies using
     * ActorTypes as the tool by which to do so.
     * @note
     *      Registry objects should only be used with the osg::ref_ptr<>
     *      construct since they are reference counted objects.
     * @see ActorType
     * @see ActorProxy
     */
    class DT_DAL_EXPORT ActorPluginRegistry : public osg::Referenced
    {
    public:
        /**
         * Constructs the registry.  Sets the name and description for
         * this registry.
         */
        ActorPluginRegistry(const std::string &name,
            const std::string &desc = "") : mName(name), mDescription(desc)
        {
            mActorFactory = new dtUtil::ObjectFactory<osg::ref_ptr<ActorType>,
                ActorProxy,ActorType::RefPtrComp>;
        }

        /**
         * Empty destructor.  Usually this is made protected since registry
         * objects are smart pointers.  However, we need to manually free
         * pointers to the registry objects from their corresponding dynamic
         * library, therefore, we need access to the object's destructor.
         */
        virtual ~ActorPluginRegistry() { }

        /**
         * Registers the actor types that this registry knows how to create.
         * This method is the first method to get called by the LibraryManager
         * after it loads a dynamic library and gets a pointer to the
         * registry object it contains.
         */
        virtual void RegisterActorTypes() = 0;

        /**
         * Sets the name of this registry.
         * @param name Name to assign to the registry.
         */
        void SetName(const std::string &name) { mName = name; }

        /**
         * Gets the name currently assigned to this registry.
         */
        const std::string& GetName() const { return mName; }

        /**
         * Sets the description for this registry.
         * @param desc Couple sentence description for this actor registry.
         */
        void SetDescription(const std::string &desc) { mDescription = desc; }

        /**
         * Gets the description of this registry.
         */
        const std::string& GetDescription() const { return mDescription; }

        /**
         * Gets a list of actor types that this registry supports.
         */
        void GetSupportedActorTypes(std::vector<osg::ref_ptr<ActorType> > &actors);

        /**
         * Checks to see if this registry supports the given actor type.
         * @param type The type to check support for.
         * @return True if supported, false otherwise.
         */
        bool IsActorTypeSupported(osg::ref_ptr<ActorType> type);

        /**
         * Creates a new actor proxy based on the ActorType given.
         * @param type Type of actor to create.
         * @return Returns a smart pointer to the newly created
         * proxy object.
         * @throws ExceptionEnum::ObjectFactoryUnknownType
         */
        osg::ref_ptr<ActorProxy> CreateActorProxy(ActorType& type);

    protected:
        std::string mName;
        std::string mDescription;

        /**
         * Factory object which stores the actor types and knows how to
         * create proxy objects for each type.
         * @see ObjectFactory
         */
        osg::ref_ptr<dtUtil::ObjectFactory<osg::ref_ptr<ActorType>,
            ActorProxy,ActorType::RefPtrComp> > mActorFactory;
    };
}

#endif
