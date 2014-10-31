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

#ifndef DELTA_UNIFORMACTCOMP_H
#define DELTA_UNIFORMACTCOMP_H

#include <dtRender/dtrenderexport.h>

#include <dtGame/actorcomponent.h>

#include <dtUtil/refstring.h>

#include <dtUtil/assocvector.h>

namespace dtCore
{
   class BaseActorObject;
   class ShaderParameter;

}

namespace dtGame
{
   class GameActorProxy;
}
namespace osg
{
   class StateSet;
}

namespace dtRender
{

   /////////////////////////////////////////////////////////////////////////////
   // Class:
   /////////////////////////////////////////////////////////////////////////////
   class DT_RENDER_EXPORT UniformActComp: public dtGame::ActorComponent
   {
      public:
         static const dtGame::ActorComponent::ACType TYPE;

         static const dtUtil::RefString PROPERTY_UNIFORM_COMP_NAME;


      public:
         // constructors
         UniformActComp();
         
         void SetName(const dtUtil::RefString& n);
         const dtUtil::RefString& GetName() const;

         /*virtual*/ void OnEnteredWorld();
         /*virtual*/ void OnRemovedFromWorld();

         /*virtual*/ void OnAddedToActor(dtCore::BaseActorObject& /*actor*/);
         /*virtual*/ void OnRemovedFromActor(dtCore::BaseActorObject& /*actor*/);

         
         // build property maps, for editor.
         virtual void BuildPropertyMap();

         
         /**
         * Binds a uniform parameter to this node.
         * @param newParam The new uniform parameter .
         * @note An exception is thrown if a parameter with the same name already
         *    exists.
         * @throw DuplicateShaderGroupException
         */
         void AddParameter(dtCore::ShaderParameter& newParam);

         /**
         * Removes an existing parameter from the node.
         * @param param The parameter to remove.
         */
         void RemoveParameter(dtCore::ShaderParameter& param);

         /**
         * Removes an existing parameter from this node.
         * @param name The name of the uniform parameter to remove.
         */
         void RemoveParameter(const std::string& name);

         /**
         * Clears list of parameters.
         */
         void RemoveAllParameters();

         /**
         * Looks up a uniform parameter with the given name. (Const version).
         * @return A const pointer to the uniform parameter or NULL if it could not be found.
         */
         const dtCore::ShaderParameter* FindParameter(const std::string& name) const;

         /**
         * Looks up a uniform parameter with the given name. (Non-const version).
         * @return A pointer to the uniform parameter or NULL if it could not be found.
         */
         dtCore::ShaderParameter* FindParameter(const std::string &name);

         /**
         * Gets the number of parameters.
         * @return The parameter count.
         */
         unsigned int GetNumParameters() const { return mParameters.size(); }

         /**
         * Fills the specified vector with all the uniform parameters.
         * @param toFill The vector to fill.  Note, this is cleared first.
         */
         void GetParameterList(std::vector<dtCore::RefPtr<dtCore::ShaderParameter> >& toFill) const;

         /***
         *  Set the state set this uniform will attach to
         *
         */
         void SetStateSet(osg::StateSet&);
         osg::StateSet* GetStateSet();
         const osg::StateSet* GetStateSet() const;

      protected:
         ~UniformActComp();

         void BindAllUniforms();
         void ClearUniformBinding();
         void CleanUp();

         //For the actor property
         void SetNameByString(const std::string& name);
         //For the actor property
         const std::string& GetNameAsString() const;

         /***
         * @return false if the state set is null or the parameter could not be bound
         *
         */
         virtual bool AttachUniformToStateSet(dtCore::ShaderParameter&);
         virtual bool DetachUniformFromStateSet(dtCore::ShaderParameter&);

      private:
         /// hiding copy constructor and operator=
         UniformActComp(const UniformActComp&);
         /// hiding copy constructor and operator=
         const UniformActComp& operator=(const UniformActComp&);


         dtUtil::RefString mName;
         dtCore::ObserverPtr<osg::StateSet> mStateSet;

         typedef dtUtil::AssocVector<std::string,dtCore::RefPtr<dtCore::ShaderParameter> > ParameterListType;
         ParameterListType mParameters;



   };

} // namespace


#endif /* DELTA_UNIFORMACTCOMP_H */
