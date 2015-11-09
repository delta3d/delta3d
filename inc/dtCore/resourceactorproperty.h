/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005-2009, BMH Associates, Inc.
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
 * William E. Johnson II
 */

#ifndef resourceactorproperty_h__
#define resourceactorproperty_h__

#include <dtCore/export.h>
#include <dtCore/actorproperty.h>
#include <dtUtil/deprecationmgr.h>
#include <dtUtil/functor.h>

namespace dtCore
{
   class ResourceDescriptor;
   class BaseActorObject;

   struct OldRDData;
   /**
    * @class ResourceActorProperty
    *
    * This actor property represents a resource
    * @code
    * class MyActor
    * {
    *    ...
    *    void SetResource(const dtCore::ResourceDescriptor& res);
    *    dtCore::ResourceDescriptor GetResource();
    *    ...
    * };
    * 
    * class Proxy : public dtCore::BaseActorObject
    * {
    *   void BuildPropertyMap()
    *   {
    *      AddProperty(new dtCore::ResourceActorProperty(NULL, dtCore::DataType::SHADER,
    *                   "Resource", "A Resource", 
    *                   dtCore::ResourceActorProperty::SetFuncType(&ga, &MyActor::SetResource),
    *                   dtCore::ResourceActorProperty::GetFuncType(&ga, &MyActor::GetResource),
    *                   "Example of setting a resource", "example group"));
    *    }
    *   ...
    * };
    * @endcode
    */
   class DT_CORE_EXPORT ResourceActorProperty : public ActorProperty
   {
   public:
      typedef dtUtil::Functor<void, TYPELIST_1(const std::string&)> SetFuncType;
      typedef dtUtil::Functor<std::string, TYPELIST_0()> GetFuncType;
      typedef dtUtil::Functor<void, TYPELIST_1(const dtCore::ResourceDescriptor&)> SetDescFuncType;
      typedef dtUtil::Functor<dtCore::ResourceDescriptor, TYPELIST_0()> GetDescFuncType;

      /**
       * Preferred constructor that allows you to specify both Get and Set
       * callback functions, both passing the ResourceDescriptor.
       *
       * @note Will not store the ResourceDescriptor in the BaseActorObject.  BaseActorObject::GetResource()
       * will return NULL.  It is preferred to store the ResourceDescriptor in
       * the actor itself.
       *
       * @param type The type of resource this contains.
       * @param name The name of the property.
       * @param label The label displayed for this property in the UI.
       * @param Set The Setter callback function that receives a ResourceDescriptor.
       * @param Get The Getter callback function that returns a ResourceDescriptor.
       * @param desc The description of the property.
       * @param groupName The group that this property will fall under.
       * @param editorType a string specifying what type of editor to use for property.
       */
      ResourceActorProperty(DataType& type,
            const dtUtil::RefString& name,
            const dtUtil::RefString& label,
            SetDescFuncType Set,
            GetDescFuncType Get,
            const dtUtil::RefString& desc = "",
            const dtUtil::RefString& groupName = "",
            const std::string& editorType = "");

      /**
       * Our original constructor, only allows you to specify a Set function
       * callback that sets the value via resource path name.
       *
       * @param actor The actor object.  This exists for legacy reasons.  It is not used.
       * @param type The type of resource this contains.
       * @param name The name of the property.
       * @param label The label displayed for this property in the UI.
       * @param Set The Setter callback function that receives a string value.
       * @param desc The description of the property.
       * @param groupName The group that this property will fall under.
       * @param editorType a string specifying what type of editor to use for property.
       */
      ResourceActorProperty(BaseActorObject& actor,
            DataType& type,
            const dtUtil::RefString& name,
            const dtUtil::RefString& label,
            SetFuncType Set,
            const dtUtil::RefString& desc = "",
            const dtUtil::RefString& groupName = "",
            const std::string& editorType = "");
      /**
       * Same as previous but without the unused actor
       */
      ResourceActorProperty(
            DataType& type,
            const dtUtil::RefString& name,
            const dtUtil::RefString& label,
            SetFuncType Set,
            const dtUtil::RefString& desc = "",
            const dtUtil::RefString& groupName = "",
            const std::string& editorType = "");

      /**
       * Modified constructor that allows you to specify both Get and Set
       * callback functions, both passing the resource path string.
       *
       * @param actor The actor object.  This exists for legacy reasons.  It is not used.
       * @param type The type of resource this contains.
       * @param name The name of the property.
       * @param label The label displayed for this property in the UI.
       * @param Set The Setter callback function that receives a string value.
       * @param Get The Getter callback function that returns a string value.
       * @param desc The description of the property.
       * @param groupName The group that this property will fall under.
       * @param editorType a string specifying what type of editor to use for property.
       */
      ResourceActorProperty(BaseActorObject& actor,
            DataType& type,
            const dtUtil::RefString& name,
            const dtUtil::RefString& label,
            SetFuncType Set,
            GetFuncType Get,
            const dtUtil::RefString& desc = "",
            const dtUtil::RefString& groupName = "",
            const std::string& editorType = "");


      /**
       * Copies a ResourceActorProperty value to this one from the property
       * specified. This method fails if otherProp is not a ResourceActorProperty.
       * @param otherProp The property to copy the value from.
       */
      virtual void CopyFrom(const ActorProperty& otherProp);

      /**
       * Sets the value of this property by calling the set functor
       * assigned to this property.
       * Hack for the resource class
       * @param value the value to set or NULL to clear it.  The passed in pointer is
       * not stored.  The values are extracted and stored in a separate object.
       */
      void SetValue(const ResourceDescriptor& value);

      /**
       * Gets the value of this property be calling the get functor
       * assigned to this property.
       * Hack for the resource class
       * @return the currently set ResourceDescriptor for this property.
       */
      ResourceDescriptor GetValue() const;

      /**
       * Sets the value of the property based on a string.
       * The string should be the both the unique id and the display string separated by a comma.
       * @note Returns false it the property is read only
       * @param value the value to set.
       * @return false of the value is not valid.
       */
      virtual bool FromString(const std::string& value);

      /**
       * @return a string version of the data.  This value can be used when calling FromString.
       * @see #FromString
       */
      virtual const std::string ToString() const;

      /**
       * Retrieves a human readable version of the property's value.
       */
      virtual std::string GetValueString() const;

      /**
       * Retrieves the editor type for this property.
       */
      const std::string& GetEditorType() const { return mEditorType; };


      /**
       * Convenient logging handler so everyone doesn't have to do the conversions by hand.
       */
      static std::string GetResourcePath(const dtCore::ResourceDescriptor& rd);

   private:
      OldRDData* mData;

      SetDescFuncType   SetDescPropFunctor;
      GetDescFuncType   GetDescPropFunctor;

      std::string mEditorType;

   protected:
      virtual ~ResourceActorProperty();
   };

} //namespace dtCore

#endif // resourceactorproperty_h__
