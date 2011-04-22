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
#include "dtDAL/actorproxy.h"
#include "dtDAL/log.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtDAL/actorproperty.h"
#include "dtDAL/datatype.h"
#include "dtDAL/librarymanager.h"
#include "dtDAL/actorproxyicon.h"
#include <dtCore/scene.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/transformable.h>
#include <dtCore/uniqueid.h>
#include <sstream>

namespace dtDAL
{

    //////////////////////////////////////////////////////////////////////////
    IMPLEMENT_ENUM(ActorProxy::RenderMode);
    const ActorProxy::RenderMode ActorProxy::RenderMode::DRAW_ACTOR("DRAW_ACTOR");
    const ActorProxy::RenderMode ActorProxy::RenderMode::DRAW_BILLBOARD_ICON("DRAW_BILLBOARD_ICON");
    const ActorProxy::RenderMode ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON("DRAW_ACTOR_AND_BILLBOARD_ICON");
    const ActorProxy::RenderMode ActorProxy::RenderMode::DRAW_AUTO("DRAW_AUTO");
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    ActorProxy::ActorProxy()
    {
        mActor = NULL;
        mActorType = NULL;
        mBillBoardIcon = NULL;
    }

    //////////////////////////////////////////////////////////////////////////
    ActorProxy::ActorProxy(const ActorProxy& rhs)
    {
    }

    //////////////////////////////////////////////////////////////////////////
    ActorProxy::~ActorProxy()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    ActorProxy& ActorProxy::operator=(const ActorProxy& rhs)
    {
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    void ActorProxy::SetClassName(const std::string& name)
    {
        mClassName = name;
        mClassNameSet.insert(mClassName);
    }

    //////////////////////////////////////////////////////////////////////////
    const dtCore::UniqueId& ActorProxy::GetId() const
    {
        return GetActor()->GetUniqueId();
    }

    //////////////////////////////////////////////////////////////////////////
    void ActorProxy::SetId(const dtCore::UniqueId& newId)
    {
        GetActor()->SetUniqueId(newId);
    }


    //////////////////////////////////////////////////////////////////////////
    const std::string& ActorProxy::GetName() const
    {
        return GetActor()->GetName();
    }

    //////////////////////////////////////////////////////////////////////////
    void ActorProxy::SetName(const std::string& name)
    {
        GetActor()->SetName(name);
		if (mBillBoardIcon.valid())
			mBillBoardIcon->GetDrawable()->SetName(name);
    }

    //////////////////////////////////////////////////////////////////////////
    ResourceDescriptor* ActorProxy::GetResource(const std::string &name)
    {
        std::map<std::string, ResourceDescriptor>::iterator itor = mResourceMap.find(name);
        return itor != mResourceMap.end() ? &itor->second : NULL;
    }

    //////////////////////////////////////////////////////////////////////////
    void ActorProxy::SetResource(const std::string &name, ResourceDescriptor *source)
    {
        // first, remove the current setting from the map.
        std::map<std::string, ResourceDescriptor>::iterator itor = mResourceMap.find(name);
        if(itor != mResourceMap.end())
            mResourceMap.erase(itor);

        // insert the new value, if we have one.
        if(source != NULL)
        {
            mResourceMap.insert(std::make_pair(name, *source));
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void ActorProxy::AddProperty(ActorProperty *newProp)
    {
        std::map<std::string,osg::ref_ptr<ActorProperty> >::iterator itor =
            mPropertyMap.find(newProp->GetName());
        if(itor != mPropertyMap.end())
        {
            std::ostringstream ss;
            ss << "Could not add new property " << newProp->GetName() << " because "
                << "a property with that name already exists.";
            LOG_ERROR(ss.str());
        }
        else
        {
            mPropertyMap.insert(std::make_pair(newProp->GetName(),newProp));
        }
    }

    //////////////////////////////////////////////////////////////////////////
    ActorProperty* ActorProxy::GetProperty(const std::string &name)
    {
        std::map<std::string,osg::ref_ptr<ActorProperty> >::iterator itor =
            mPropertyMap.find(name);

        if(itor == mPropertyMap.end())
            return NULL;
        else
            return itor->second.get();
    }

    //////////////////////////////////////////////////////////////////////////
    void ActorProxy::GetPropertyList(std::vector<const ActorProperty *> &propList) const
    {
        propList.clear();
        std::map<std::string,osg::ref_ptr<ActorProperty> >::const_iterator itor =
            mPropertyMap.begin();

        propList.reserve(mPropertyMap.size());
        while(itor != mPropertyMap.end())
        {
            propList.push_back(itor->second.get());
            ++itor;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    void ActorProxy::GetPropertyList(std::vector<ActorProperty *> &propList)
    {
        propList.clear();
        std::map<std::string,osg::ref_ptr<ActorProperty> >::iterator itor =
            mPropertyMap.begin();

        propList.reserve(mPropertyMap.size());
        while(itor != mPropertyMap.end())
        {
            propList.push_back(itor->second.get());
            ++itor;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    const ActorProxy::RenderMode& ActorProxy::GetRenderMode()
    {
        return RenderMode::DRAW_ACTOR;
    }

    //////////////////////////////////////////////////////////////////////////
    void ActorProxy::SetBillBoardIcon(ActorProxyIcon *icon)
    {
        mBillBoardIcon = icon;
    }

    //////////////////////////////////////////////////////////////////////////
    ActorProxyIcon *ActorProxy::GetBillBoardIcon()
    {
        if(!mBillBoardIcon.valid())
           mBillBoardIcon = new ActorProxyIcon();

        return mBillBoardIcon.get();
    }

	//////////////////////////////////////////////////////////////////////////
    dtCore::DeltaDrawable *ActorProxy::GetActor()
    {
        return mActor.get();
    }

	//////////////////////////////////////////////////////////////////////////
    const dtCore::DeltaDrawable *ActorProxy::GetActor() const
    {
        return mActor.get();
    }

    //////////////////////////////////////////////////////////////////////////
    osg::ref_ptr<ActorProxy> ActorProxy::Clone()
    {
        std::ostringstream error;

        //First tell the library manager to create a new mActor using this
        // actors actor type.
        osg::ref_ptr<ActorProxy> copy = NULL;

        try
        {
            copy = LibraryManager::GetInstance().CreateActorProxy(*mActorType);
        }
        catch(dtDAL::Exception &e)
        {
            error << "Clone of actor proxy: " << GetName() << " failed. Reason was: " << e.What();
            LOG_ERROR(error.str());
            return NULL;
        }

        //The names should be the same until
        //the user changes them.
        copy->SetName(GetName());

        //Now copy all of the properties from this proxy to the clone.
        std::map<std::string,osg::ref_ptr<ActorProperty> >::iterator myPropItor,copyPropItor;
        copyPropItor = copy->mPropertyMap.begin();
        myPropItor = mPropertyMap.begin();
        while(myPropItor != mPropertyMap.end() && copyPropItor != copy->mPropertyMap.end())
        {
            copyPropItor->second->CopyFrom(myPropItor->second.get());
            ++copyPropItor;
            ++myPropItor;
        }

        //Now copy all the resource descriptors from this proxy to the clone.
        copy->mResourceMap = mResourceMap;

        return copy;
    }

    //////////////////////////////////////////////////////////////////////////
    void ActorProxy::SetActorType(ActorType *type)
    {
        mActorType = type;
    }
}