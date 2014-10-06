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
 * David Guthrie
 */

#ifndef DELTA_NAMED_GROUP_PARAMETER
#define DELTA_NAMED_GROUP_PARAMETER

#include <dtCore/namedparameter.h>

#include <dtCore/datatype.h>

#include <dtCore/refptr.h>
#include <dtUtil/assocvector.h>

#include <vector>

namespace dtCore
{
   class DT_CORE_EXPORT NamedGroupParameter : public NamedParameter
   {
      public:
         typedef dtUtil::AssocVector<dtUtil::RefString, dtCore::RefPtr<NamedParameter> > ParameterList;

         NamedGroupParameter(const dtUtil::RefString& name);
         NamedGroupParameter(const NamedGroupParameter& toCopy);

         template <typename ParameterContainer >
         NamedGroupParameter(const dtUtil::RefString& name, const ParameterContainer& parameters)
         : NamedParameter(dtCore::DataType::GROUP, name, false)
         {
            typename ParameterContainer::const_iterator i, iend;
            i = parameters.begin();
            iend = parameters.end();
            for (; i != iend; ++i)
            {
               NamedParameter& cur = **i;

               dtCore::RefPtr<NamedParameter> newParameter =
                        AddParameter(cur.GetName(), cur.GetDataType(), cur.IsList());

               newParameter->CopyFrom(cur);
            }
         }

         virtual void ToDataStream(dtUtil::DataStream& stream) const;

         virtual bool FromDataStream(dtUtil::DataStream& stream);

         virtual const std::string ToString() const;

         virtual bool FromString(const std::string& value);

         virtual void CopyFrom(const NamedParameter& otherParam);


         /**
          * Include namedgroupparameter.inl to use this function
          */
         template <typename T>
         inline void AddValue(const dtUtil::RefString& name, const T& value);

         /**
          * Include namedgroupparameter.inl to use this function
          */
         template <typename T>
         inline void SetValue(const dtUtil::RefString& name, const T& value);

         /**
          * Include namedgroupparameter.inl to use this function
          */
         template <typename T>
         inline const T& GetValue(const dtUtil::RefString& name, const T& defaultVal) const;

         /**
          * Adds a parameter to the group
          * @param name The name of the parameter to add
          * @param type The type of parameter it is, corresponding with dtCore::DataType
          * @param createAsList true if the parameter should be a list parameter.
          * @return A pointer to the parameter
          * @see dtCore::DataType
          * @throws dtUtil::Exception if the name specified is already used.
          */
         NamedParameter* AddParameter(const dtUtil::RefString& name, dtCore::DataType& type, bool createAsList = false);

         /**
          * Removes a parameter to the group
          * @param name The name of the parameter to remove
          * @return A ref pointer to the parameter removed or NULL if nothing was removed.  It returns a ref ptr in case
          *         this group is the only object holding on to a reference.
          */
         dtCore::RefPtr<NamedParameter> RemoveParameter(const dtUtil::RefString& name);

         /// Simply clears all parameters in this group parameter.
         void RemoveAllParameters();

         /**
          * Adds a parameter to the group
          * @param param the new parameter to add
          * @see dtCore::DataType
          * @throws dtUtil::Exception if the name specified is already used.
          */
         void AddParameter(NamedParameter& newParam);

         /**
          * Retrieves the parameter for this group parameter with the given name.
          * @param name The name of the parameter to retrieve
          * @return A pointer to the parameter or NULL if no such parameter exists
          */
         NamedParameter* GetParameter(const dtUtil::RefString& name);

         /**
          * Retrieves the pointer to the parameter for this group parameter with the given name.
          * @param name The name of the parameter to retrieve
          * @param outParam outputs a pointer to the parameter or NULL if no such parameter exists or the param is not that type.
          */
         template <typename T>
         void GetParameter(const dtUtil::RefString& name, T*& outParam)
         {
            outParam = dynamic_cast<T*>(GetParameter(name));
         }

         /**
          * Retrieves const pointer to the parameter for this group parameter with the given name.
          * @param name The name of the parameter to retrieve
          * @return A const pointer to the parameter or NULL if no such parameter exists
          */
         const NamedParameter* GetParameter(const dtUtil::RefString& name) const;

         /**
          * Retrieves const pointer to the parameter for this group parameter with the given name.
          * @param name The name of the parameter to retrieve
          * @param outParam outputs a const pointer to the parameter or NULL if no such parameter exists or the param is not that type.
          */
         template <typename T>
         void GetParameter(const dtUtil::RefString& name, const T*& outParam) const
         {
            outParam = dynamic_cast<const T*>(GetParameter(name));
         }

         /**
          * Retrieves all of the parameters in this group.
          * @param toFill The vector to fill with the parameters
          */
         void GetParameters(std::vector<NamedParameter*>& toFill);

         /**
          * Retrieves all of the parameters in this group as const.
          * @param toFill The vector to fill with the parameters
          */
         void GetParameters(std::vector<const NamedParameter*>& toFill) const;

         template <class UnaryFunction>
         void ForEachParameter(UnaryFunction function) const
         {
            /*std::for_each(mParameterList.begin(), mParameterList.end(),
                  std::compose1(std::select2nd<std::pair<std::string, dtCore::RefPtr<NamedParameter> > >(),
                        function));*/
            ParameterList::const_iterator i = mParameterList.begin();
            ParameterList::const_iterator end = mParameterList.end();
            for (; i != end; ++i)
            {
               function(i->second);
            }
         }

         /**
          * Return the amount of iters in the group list
          * @return the size of the internal messageparam map size
          */
         unsigned int GetParameterCount() const;

         /**
          * Sets the message parameter's value from the actor property's value
          */
         virtual void SetFromProperty(const dtCore::ActorProperty& property);

         /**
          * Sets the actor property's value from the message parameter's value
          */
         virtual void ApplyValueToProperty(dtCore::ActorProperty& property) const;

         virtual bool operator==(const NamedParameter& toCompare) const;
         virtual bool operator==(const ActorProperty& toCompare) const;

      protected:
         NamedGroupParameter(dtCore::DataType& newDataType, const dtUtil::RefString& name);
      private:
         ParameterList mParameterList;
   };
}

#endif //DELTA_NAMED_GROUP_PARAMETER
