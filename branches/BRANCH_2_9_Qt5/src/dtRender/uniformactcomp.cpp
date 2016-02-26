/* -*-c++-*-
 * dtRender
 * Copyright 2014, Caper Holdings LLC
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
 * Bradley Anderegg
 */


#include <dtRender/uniformactcomp.h>
#include <dtGame/gameactor.h>
#include <dtCore/enginepropertytypes.h>

#include <dtCore/shaderparameter.h>


namespace dtRender
{
   const dtGame::ActorComponent::ACType UniformActComp::TYPE(new dtCore::ActorType("UniformActComp", "ActorComponents",
      "An actor component which makes it easy to store and access uniforms on an actor, also useful for storing on the main scene for rendering globals like HDR.",
          dtGame::ActorComponent::BaseActorComponentType));

   const dtUtil::RefString UniformActComp::PROPERTY_UNIFORM_COMP_NAME("Uniform Component Name");
   
   
   /////////////////////////////////////////////////////////////////////////////
   UniformActComp::UniformActComp()
   : dtGame::ActorComponent(UniformActComp::TYPE)
   , mName(PROPERTY_UNIFORM_COMP_NAME)
   {
   }

   UniformActComp::~UniformActComp()
   {
      // Cleanup now happens in OnRemovedFromWorld.  It can't happen here because it's virtual.
   }

   void UniformActComp::SetName(const dtUtil::RefString& n)
   {
      mName = n;
   }

   const dtUtil::RefString& UniformActComp::GetName() const
   {
      return mName;
   }

   
   void UniformActComp::OnEnteredWorld()
   {
      dtGame::GameActorProxy* act = NULL;
      GetOwner(act);

      if(act->GetDrawable() != NULL )
      {
         SetStateSet(*act->GetDrawable()->GetOSGNode()->getOrCreateStateSet());
         BindAllUniforms();
      }
      else
      {
         LOG_ERROR("Unable to find stateset on base actor, drawable is NULL.");
      }
   }

   void UniformActComp::OnRemovedFromWorld()
   {
      CleanUp();
   }

   void UniformActComp::CleanUp()
   {
      RemoveAllParameters();
   }

   void UniformActComp::OnAddedToActor(dtCore::BaseActorObject& actor)
   {
      
   }

   void UniformActComp::OnRemovedFromActor(dtCore::BaseActorObject& actor)
   {
      ClearUniformBinding();
   }


   
   void UniformActComp::BuildPropertyMap()
   {
      static const dtUtil::RefString GROUP("UniformActComp");

   
   }


   void UniformActComp::SetNameByString(const std::string& name)
   {
      mName = name;
   }

   const std::string& UniformActComp::GetNameAsString() const
   {
      return mName;
   }

   void UniformActComp::RemoveAllParameters()
   {
      mParameters.clear();
   }

   void UniformActComp::AddParameter(dtCore::ShaderParameter& newParam)
   {
      ParameterListType::iterator itor =
         mParameters.find(newParam.GetName());

      if (itor != mParameters.end())
      {
         LOG_ERROR("Shader parameters must have unique names.");
         return;
      }

      mParameters.insert(std::make_pair(newParam.GetName(),&newParam));

      AttachUniformToStateSet(newParam);
   }

   void UniformActComp::RemoveParameter(dtCore::ShaderParameter& param)
   {
      ParameterListType::iterator itor =
         mParameters.find(param.GetName());

      if (itor == mParameters.end())
      {
         LOG_WARNING("Could not remove shader parameter: " + param.GetName() +
            "  Parameter is not bound to node: " + GetName());
      }
      else
      {
         dtCore::ShaderParameter* sp = (*itor).second;
         DetachUniformFromStateSet(*sp);
         mParameters.erase(itor);
      }
   }

   void UniformActComp::RemoveParameter(const std::string& name)
   {
      ParameterListType::iterator itor =
         mParameters.find(name);

      if (itor == mParameters.end())
      {
         LOG_WARNING("Could not remove shader parameter: " + name +
            "  Parameter is not bound to node: " + GetName());
      }
      else
      {
         dtCore::ShaderParameter* sp = (*itor).second;

         DetachUniformFromStateSet(*sp);
         mParameters.erase(itor);
      }
   }

   const dtCore::ShaderParameter *UniformActComp::FindParameter(const std::string& name) const
   {
      ParameterListType::const_iterator itor =
         mParameters.find(name);

      if (itor != mParameters.end())
      {
         return itor->second.get();
      }
      else
      {
         return NULL;
      }
   }

   dtCore::ShaderParameter* UniformActComp::FindParameter(const std::string& name)
   {
      ParameterListType::iterator itor =
         mParameters.find(name);

      if (itor != mParameters.end())
      {
         return itor->second.get();
      }
      else
      {
         return NULL;
      }
   }

   void UniformActComp::GetParameterList(std::vector<dtCore::RefPtr<dtCore::ShaderParameter> >& toFill) const
   {
      ParameterListType::const_iterator itor;

      toFill.clear();
      for (itor=mParameters.begin(); itor!=mParameters.end(); ++itor)
      {
         toFill.push_back(itor->second);
      }
   }

   void UniformActComp::ClearUniformBinding()
   {
      
      if (mStateSet.valid())
      {
         ParameterListType::iterator params = mParameters.begin();
         ParameterListType::iterator paramsEnd = mParameters.end();

         for (; params != paramsEnd; ++params)
         {
            dtCore::ShaderParameter* sp = (*params).second;

            DetachUniformFromStateSet(*sp);
         }

      }
   
   }

   void UniformActComp::BindAllUniforms()
   {

      if (mStateSet.valid())
      {
         ParameterListType::iterator params = mParameters.begin();
         ParameterListType::iterator paramsEnd = mParameters.end();

         for (; params != paramsEnd; ++params)
         {
            dtCore::ShaderParameter* sp = (*params).second;
            AttachUniformToStateSet(*sp);
         }

      }

   }

   bool UniformActComp::AttachUniformToStateSet( dtCore::ShaderParameter& p)
   {
      if(mStateSet.valid())
      {
         p.AttachToRenderState(*mStateSet);
         return true;
      }

      return false;
   }

   bool UniformActComp::DetachUniformFromStateSet( dtCore::ShaderParameter& p)
   {
      if(mStateSet.valid())
      {
         p.AttachToRenderState(*mStateSet);
         return true;
      }

      return false;
   }

   void UniformActComp::SetStateSet( osg::StateSet& ss)
   {
      if(mStateSet.valid())
      {
         ClearUniformBinding();
      }

      mStateSet = &ss;

      BindAllUniforms();
   }

   osg::StateSet* UniformActComp::GetStateSet()
   {
      return mStateSet.get();
   }

   const osg::StateSet* UniformActComp::GetStateSet() const
   {
      return mStateSet.get();
   }


} // namespace dtRender
