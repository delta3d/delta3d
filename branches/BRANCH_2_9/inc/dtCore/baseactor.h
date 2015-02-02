/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef DELTA_BASE_ACTOR_H
#define DELTA_BASE_ACTOR_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtCore/baseactorobject.h>
#include <dtCore/hierarchyobject.h>



namespace dtCore
{
   class ActorComponent;
}

namespace dtCore
{
   class BaseActor;

   /////////////////////////////////////////////////////////////////////////////
   // TYPE DEFINITIONS
   /////////////////////////////////////////////////////////////////////////////
   typedef std::vector<dtCore::RefPtr<dtCore::BaseActor> > BaseActorArray;
   typedef std::vector<dtCore::RefPtr<dtCore::BaseActorObject> > BaseActorObjectArray;

   typedef HierarchyObject<dtCore::BaseActor> ActorHierarchyObject;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   template<class T_Object>
   class ObjectTypeFilter
   {
   public:
      typedef T_Object ObjType;

      ObjectTypeFilter(const dtCore::ActorType& objType)
         : mObjectType(&objType)
         , mExactMatch(false)
      {}

      void SetExactMatchEnabled(bool match)
      {
         mExactMatch = match;
      }

      bool IsExactMatchEnabled() const
      {
         return mExactMatch;
      }

      virtual bool IsMatchType(const dtCore::ActorType& objType) const
      {
         return &objType == mObjectType || ( ! mExactMatch && objType.InstanceOf(*mObjectType));
      }

      virtual bool IsMatch(const ObjType& obj) const
      {
         return IsMatchType(obj.GetActorType());
      }

   private:
      const dtCore::ActorType* mObjectType;
      bool mExactMatch;
   };


   /**
    * This is the base class for all actor.
    * @note
    *      Actor objects must be created through the ActorPluginRegistry or
    *      the LibraryManager. If they are not created in this fashion,
    *      the actor types will not be set correctly.
    */
   class DT_CORE_EXPORT BaseActor : public dtCore::BaseActorObject,
      virtual public ActorHierarchyObject
   {
   public:
      typedef dtCore::BaseActorObject BaseClass;
      typedef ActorHierarchyObject HeirarchyBaseClass;
      typedef ObjectTypeFilter<BaseActor> ActorTypeFilter;
      typedef ActorHierarchyObject::ObjectFilterFunc ActorFilterFunc;

      /**
       * Constructs the actor.
       */
      BaseActor(const ActorType* actorType = NULL);

      bool operator == (const BaseActor& rhs) const
      {
         return GetId() == rhs.GetId();
      }

      bool operator != (const BaseActor& rhs) const
      {
         return !(*this == rhs);
      }

      BaseActor* GetChildByType(const dtCore::ActorType& actorType) const;

      int GetChildrenByType(const dtCore::ActorType& actorType, BaseActorArray& outActors, bool exactMatch = false) const;

      dtCore::ActorComponent* GetComponentByType(const dtCore::ActorType& compType) const;

      int GetComponentsByType(const dtCore::ActorType& actorType, BaseActorArray& outActors, bool exactMatch = false) const;

      /**
       * Get a component by type.
       * NOTE: Class must specify a constant ActorType reference named TYPE.
       * Usage:
       * @code
       * MyComponentClass* component = NULL;
       * myComponentBase->GetComponent(component);
       * @endcode
       * @param compType pointer to be set to component
       * @return True if component of this type exists, else false
       */
      template <typename T_Comp>
      bool GetComponent(T_Comp*& compType) const
      {
         compType = dynamic_cast<T_Comp*>(GetChildByType(*T_Comp::TYPE));
         return compType != NULL;
      }

      /**
       * Get a component by type.
       * NOTE: Class must specify a constant ActorType reference named TYPE.
       * Usage:
       * @code
       * dtCore::RefPtr<MyComponentClass> component;
       * myComponentBase->GetComponent(component);
       * @endcode
       * @param compType pointer to be set to component
       * @return True if component of this type exists, else false
       */
      template <typename T_Comp>
      bool GetComponent(dtCore::RefPtr<T_Comp>& compType) const
      {
         T_Comp* ptr = NULL;
         GetComponent(ptr);
         compType = ptr;
         return compType.valid();
      }

      /**
       * Gets the first component of the desired type.
       * NOTE: Class must specify a constant ActorType reference named TYPE.
       * Usage:
       * @code
       * MyComponentClass* component = myComponentBase->GetComponent<MyComponentClass>();
       * @endcode
       * @return The first ActorComponent of the templated parameter type, or NULL if it doesn't exist
       */
      template <typename T_Comp>
      T_Comp* GetComponent() const
      {
         T_Comp* component = NULL;
         GetComponent(component);
         return component;
      }

      int GetAllComponents(BaseActorArray& outComponents) const;

      int RemoveChildrenByType(const dtCore::ActorType& actorType, BaseActorArray* outActors = NULL, bool exactMatch = false);
      
      /**
       * This method walks the child actor components to see if one of them
       * can handle the deprecated property.
       * @param name Name of the property to find.
       * @return Property that matches the specified name, or NULL if not found.
       */
      /*virtual*/ dtCore::RefPtr<dtCore::ActorProperty>
         GetDeprecatedProperty(const std::string& name);

      /**
       * Overrideable method to be called when the actor is added to the world.
       */
      virtual void OnEnteredWorld();

      /**
       * Overrideable method to be called when the actor is removed from the world.
       */
      virtual void OnRemovedFromWorld();

      /**
       * Sets a flag that determines if the actor is in the world/GameManager.
       */
      bool IsInGM() const;
      void SetInGM(bool value);

      /**
       * If this actor is queued to be deleted.
       */
      bool IsDeleted() const;
      void SetDeleted(bool deleted);

      // TEMP:
      void AddComponentProperties();

      // TEMP:
      void RemoveComponentProperties();

      /**
       * Gets a property of the requested name, either directly
       * from the actor or from any contained actor components.
       *
       * @param name Name of the property to retrieve.
       * @return Property object or NULL if not found.
       */
      /*virtual*/ ActorProperty* FindProperty(const std::string& name);
      /*virtual*/ const ActorProperty* FindProperty(const std::string& name) const;
      
      /**
       * Gets all the properties contained in this actor and its immediate actor components.
       * @param propList Container to capture all the properties.
       */
      /*virtual*/ void GetDeepPropertyList(PropertyVector& propList);
      /*virtual*/ void GetDeepPropertyList(PropertyConstVector& propList) const;

   protected:

      ///Keep the destructor protected since we use dtCore::RefPtr to
      ///track any object created.
      virtual ~BaseActor();

      /*virtual*/ void OnChildAdded(BaseActor& actor);

      /*virtual*/ void OnChildRemoved(BaseActor& actor);

   private:

      ///Hidden copy constructor.
      BaseActor(const BaseActor&);

      ///Hidden assignment operator.
      BaseActor& operator=(const BaseActor&);

      bool mIsInGM;
      bool mDeleted;

   };
}

#endif
