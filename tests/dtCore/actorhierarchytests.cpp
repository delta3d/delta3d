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

#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/actorhierarchynode.h>
#include <dtCore/actorvisitor.h>
#include <dtCore/actorcomponent.h>

using namespace dtCore;
 


typedef std::vector<std::string> StrArray;

////////////////////////////////////////////////////////////////////////////////
// TEST CLASS
////////////////////////////////////////////////////////////////////////////////
class TestActorObject : public dtCore::BaseActor
{
public:
   typedef dtCore::BaseActor BaseClass;
   typedef dtCore::BaseActor::ObjType ObjType;
   typedef dtCore::BaseActor::ObjArray ObjArray;

   static int mDeleteCount;

   // Test variables
   dtCore::ObserverPtr<ObjType> mObjectAdded;
   dtCore::ObserverPtr<ObjType> mObjectRemoved;
   dtCore::ObserverPtr<ObjType> mParentAdded;
   dtCore::ObserverPtr<ObjType> mParentRemoved;
   dtCore::ObserverPtr<ObjType> mAddedToActor;
   dtCore::ObserverPtr<ObjType> mRemovedFromActor;
   int mCountAddedToActor;
   int mCountRemovedFromActor;

   TestActorObject()
   {
      ResetTestVars();
   }

   TestActorObject(const std::string& name)
   {
      ResetTestVars();
      SetName(name);
   }

   virtual bool IsPlaceable() const
   {
      return false;
   }

   virtual void OnChildAdded(ObjType& obj)
   {
      mObjectAdded = &obj;
   }

   virtual void OnChildRemoved(ObjType& obj)
   {
      mObjectRemoved = &obj;
   }

   virtual void OnParentAdded(ObjType& obj)
   {
      mParentAdded = &obj;
      ++mCountAddedToActor;
   }

   virtual void OnParentRemoved(ObjType& obj)
   {
      mParentRemoved = &obj;
      ++mCountRemovedFromActor;
   }

   void ResetTestVars()
   {
      mObjectAdded = NULL;
      mObjectRemoved = NULL;
      mParentAdded = NULL;
      mParentRemoved = NULL;
      mAddedToActor = NULL;
      mRemovedFromActor = NULL;
      mCountAddedToActor = 0;
      mCountRemovedFromActor = 0;
   }

protected:
   virtual ~TestActorObject()
   {
      ++mDeleteCount;
   }
};

int TestActorObject::mDeleteCount = 0;



////////////////////////////////////////////////////////////////////////////////
// TEST CLASS
////////////////////////////////////////////////////////////////////////////////
class TestActorComponent : public dtCore::ActorComponent
{
public:
   typedef dtCore::ActorComponent BaseClass;

   static const ACType TYPE;

   TestActorComponent(const ACType& type = TYPE)
      : BaseClass(type)
   {}

protected:
   virtual ~TestActorComponent()
   {}
};

const TestActorComponent::ACType TestActorComponent::TYPE
   = TestActorComponent::ACType( new dtCore::ActorType("TestActorComponent", "ActorComponents", "",
      dtCore::ActorComponent::BaseActorComponentType.get()));



////////////////////////////////////////////////////////////////////////////////
// TEST CLASS
////////////////////////////////////////////////////////////////////////////////
class TestActorVisitor : public dtCore::ActorVisitor
{
public:
   typedef dtCore::ActorVisitor BaseClass;

   StrArray mNameOrder;

   TestActorVisitor(BaseClass::Mode mode = BaseClass::DEFAULT_MODE)
      : BaseClass(mode)
   {}

   /*virtual*/ bool Handle(dtCore::BaseActor& actor)
   {
      mNameOrder.push_back(actor.GetName());
      return true;
   }

   void Reset()
   {
      mNameOrder.clear();
   }

protected:
   virtual ~TestActorVisitor()
   {}
};



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
class ActorHierarchyTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(ActorHierarchyTests);   
      CPPUNIT_TEST(TestConstructors);
      CPPUNIT_TEST(TestLinkAssignment);
      CPPUNIT_TEST(TestTwoWayLinkAssignment);
      CPPUNIT_TEST(TestObjectFilter);
      CPPUNIT_TEST(TestChildAccessFilters);
      CPPUNIT_TEST(TestMemoryLeaks);
      CPPUNIT_TEST(TestNotifications);
      CPPUNIT_TEST(TestActorAddRemoveChildren);
      CPPUNIT_TEST(TestActorRemoveChildrenByType);
      CPPUNIT_TEST(TestActorGetChildren);
      CPPUNIT_TEST(TestActorVisitorTraversal);
   CPPUNIT_TEST_SUITE_END();
   
   public:
      void setUp();
      void tearDown();  

      void TestConstructors();
      void TestLinkAssignment();
      void TestTwoWayLinkAssignment();
      void TestObjectFilter();
      void TestChildAccessFilters();
      void TestMemoryLeaks();
      void TestNotifications();
      void TestActorAddRemoveChildren();
      void TestActorRemoveChildrenByType();
      void TestActorGetChildren();
      void TestActorVisitorTraversal();

      bool FilterName(const dtCore::BaseActorObject& actor) const;

      typedef TestActorObject TestObj;
      typedef dtCore::RefPtr<TestObj> TestObjPtr;

      TestObjPtr mActor;
};

CPPUNIT_TEST_SUITE_REGISTRATION(ActorHierarchyTests);



/////////////////////////////////////////////////////////////////////////
void ActorHierarchyTests::setUp()
{
   TestObj::mDeleteCount = 0;

   mActor = new TestObj();
}

//////////////////////////////////////////////////////////////////////////
void ActorHierarchyTests::tearDown()
{
   mActor = NULL;

   TestObj::mDeleteCount = 0;
}

//////////////////////////////////////////////////////////////////////////
void ActorHierarchyTests::TestConstructors()
{
   TestObjPtr other = new TestObj;

   // Test constructor with only the owner reference.
   CPPUNIT_ASSERT(other->GetParent() == NULL);
   CPPUNIT_ASSERT(other->GetChildCount() == 0);
   CPPUNIT_ASSERT( ! other->HasChild(*other));
   CPPUNIT_ASSERT( ! other->HasChild(*mActor));
}

//////////////////////////////////////////////////////////////////////////
void ActorHierarchyTests::TestLinkAssignment()
{
   TestObjPtr objA = new TestObj;
   TestObjPtr objB = new TestObj;
   TestObjPtr objC = new TestObj;
   TestObjPtr objD = new TestObj;

   CPPUNIT_ASSERT(mActor->GetParent() == NULL);
   CPPUNIT_ASSERT(mActor->GetChildCount() == 0);

   CPPUNIT_ASSERT(mActor->HasReference(*mActor));
   CPPUNIT_ASSERT( ! mActor->HasChild(*mActor));
   CPPUNIT_ASSERT( ! mActor->HasChild(*objA));
   CPPUNIT_ASSERT( ! mActor->HasChild(*objB));
   CPPUNIT_ASSERT( ! mActor->HasChild(*objC));
   CPPUNIT_ASSERT( ! mActor->HasChild(*objD));
   CPPUNIT_ASSERT( ! mActor->HasReference(*objA));
   CPPUNIT_ASSERT( ! mActor->HasReference(*objB));
   CPPUNIT_ASSERT( ! mActor->HasReference(*objC));
   CPPUNIT_ASSERT( ! mActor->HasReference(*objD));

   CPPUNIT_ASSERT( ! mActor->IsChildAllowed(*mActor));
   CPPUNIT_ASSERT(mActor->IsChildAllowed(*objA));
   CPPUNIT_ASSERT(mActor->IsChildAllowed(*objB));
   CPPUNIT_ASSERT(mActor->IsChildAllowed(*objC));
   CPPUNIT_ASSERT(mActor->IsChildAllowed(*objD));

   // Set the parent.
   mActor->SetParent(objC.get());
   CPPUNIT_ASSERT(mActor->GetParent() == objC.get());
   CPPUNIT_ASSERT(mActor->GetChildCount() == 0);
   CPPUNIT_ASSERT( ! mActor->HasChild(*objC));
   CPPUNIT_ASSERT(mActor->HasReference(*objC));
   // --- Ensure that the owner cannot be assigned as the parent.
   mActor->SetParent(mActor.get());
   CPPUNIT_ASSERT(mActor->GetParent() == objC.get());
   CPPUNIT_ASSERT(mActor->GetChildCount() == 0);
   CPPUNIT_ASSERT( ! mActor->HasChild(*objC));
   CPPUNIT_ASSERT(mActor->HasReference(*objC));
   // --- Verify that the parent can be changed.
   mActor->SetParent(objA.get());
   CPPUNIT_ASSERT(mActor->GetParent() == objA.get());
   CPPUNIT_ASSERT(mActor->GetChildCount() == 0);
   CPPUNIT_ASSERT(mActor->HasReference(*objA));
   CPPUNIT_ASSERT( ! mActor->HasChild(*objA));
   CPPUNIT_ASSERT( ! mActor->HasChild(*objC));
   CPPUNIT_ASSERT( ! mActor->HasReference(*objC));


   // Add the rest.
   CPPUNIT_ASSERT(mActor->AddChild(*objB));
   CPPUNIT_ASSERT(mActor->HasChild(*objB));
   CPPUNIT_ASSERT( ! mActor->HasChild(*objC));
   CPPUNIT_ASSERT( ! mActor->HasChild(*objD));
   CPPUNIT_ASSERT(mActor->GetChildCount() == 1); // children
   CPPUNIT_ASSERT( ! mActor->IsChildAllowed(*objA));
   CPPUNIT_ASSERT( ! mActor->IsChildAllowed(*objB));
   CPPUNIT_ASSERT(mActor->IsChildAllowed(*objC));
   CPPUNIT_ASSERT(mActor->IsChildAllowed(*objD));

   CPPUNIT_ASSERT(mActor->AddChild(*objC));
   CPPUNIT_ASSERT(mActor->AddChild(*objD));
   CPPUNIT_ASSERT( ! mActor->HasChild(*objA)); // parent
   CPPUNIT_ASSERT(mActor->HasChild(*objB));
   CPPUNIT_ASSERT(mActor->HasChild(*objC));
   CPPUNIT_ASSERT(mActor->HasChild(*objD));
   CPPUNIT_ASSERT(mActor->GetChildCount() == 3); // children
   CPPUNIT_ASSERT( ! mActor->IsChildAllowed(*objA));
   CPPUNIT_ASSERT( ! mActor->IsChildAllowed(*objB));
   CPPUNIT_ASSERT( ! mActor->IsChildAllowed(*objC));
   CPPUNIT_ASSERT( ! mActor->IsChildAllowed(*objD));
   CPPUNIT_ASSERT(mActor->HasReference(*objA));
   CPPUNIT_ASSERT(mActor->HasReference(*objB));
   CPPUNIT_ASSERT(mActor->HasReference(*objC));
   CPPUNIT_ASSERT(mActor->HasReference(*objD));

   // --- Verify adding an exiting reference fails.
   CPPUNIT_ASSERT( ! mActor->AddChild(*objA)); // parent
   CPPUNIT_ASSERT( ! mActor->AddChild(*objB));
   CPPUNIT_ASSERT( ! mActor->AddChild(*objC));
   CPPUNIT_ASSERT( ! mActor->AddChild(*objD));
   CPPUNIT_ASSERT(mActor->GetChildCount() == 3); // children


   // Test accessing all children.
   TestObj::ObjArray objArray;
   CPPUNIT_ASSERT(mActor->GetChildren(objArray) == 3);
   CPPUNIT_ASSERT(objArray.size() == 3);
   CPPUNIT_ASSERT(objArray[0] == objB);
   CPPUNIT_ASSERT(objArray[1] == objC);
   CPPUNIT_ASSERT(objArray[2] == objD);


   // Verify that the parent cannot be replaced by another object that is already linked.
   CPPUNIT_ASSERT(mActor->GetParent() == objA.get());
   mActor->SetParent(objC.get());
   CPPUNIT_ASSERT(mActor->GetParent() == objA.get());

   // Remove one child.
   CPPUNIT_ASSERT(mActor->RemoveChild(*objC));

   CPPUNIT_ASSERT( ! mActor->HasChild(*objA)); // parent
   CPPUNIT_ASSERT(mActor->HasChild(*objB));
   CPPUNIT_ASSERT( ! mActor->HasChild(*objC));
   CPPUNIT_ASSERT(mActor->HasChild(*objD));
   CPPUNIT_ASSERT(mActor->GetChildCount() == 2); // children

   // Ensure a second removal fails for the same object.
   CPPUNIT_ASSERT( ! mActor->RemoveChild(*objC));

   // Verify that the parent can be replaced by another object that is not linked.
   CPPUNIT_ASSERT(mActor->GetParent() == objA.get());
   mActor->SetParent(objC.get());
   CPPUNIT_ASSERT(mActor->GetParent() == objC.get());
   CPPUNIT_ASSERT( ! mActor->HasChild(*objA));

   // --- Ensure the parent cannot be added as a child while it is referenced as a parent.
   CPPUNIT_ASSERT( ! mActor->AddChild(*objC));
   CPPUNIT_ASSERT(mActor->GetParent() == objC.get());
   CPPUNIT_ASSERT( ! mActor->HasChild(*objA));
   CPPUNIT_ASSERT(mActor->HasChild(*objB));
   CPPUNIT_ASSERT( ! mActor->HasChild(*objC)); // parent
   CPPUNIT_ASSERT(mActor->HasChild(*objD));
   CPPUNIT_ASSERT(mActor->GetChildCount() == 2); // children


   // Ensure the parent cannot be removed by the remove method.
   CPPUNIT_ASSERT( ! mActor->RemoveChild(*objC));
   CPPUNIT_ASSERT(mActor->GetParent() == objC);

   // Test removing the parent.
   mActor->SetParent(NULL);
   CPPUNIT_ASSERT(mActor->GetParent() == NULL);
   CPPUNIT_ASSERT( ! mActor->HasChild(*objA));
   CPPUNIT_ASSERT(mActor->HasChild(*objB));
   CPPUNIT_ASSERT( ! mActor->HasChild(*objC)); // parent
   CPPUNIT_ASSERT(mActor->HasChild(*objD));
   CPPUNIT_ASSERT(mActor->GetChildCount() == 2); // children


   // Clear all children.
   mActor->SetParent(objA.get());
   CPPUNIT_ASSERT(mActor->GetParent() == objA.get());
   mActor->ClearChildren();
   CPPUNIT_ASSERT(mActor->GetParent() == objA.get());
   CPPUNIT_ASSERT(mActor->GetChildCount() == 0); // children
   CPPUNIT_ASSERT(mActor->HasReference(*objA)); // parent
   CPPUNIT_ASSERT( ! mActor->HasChild(*objA)); // parent
   CPPUNIT_ASSERT( ! mActor->HasChild(*objB));
   CPPUNIT_ASSERT( ! mActor->HasChild(*objC));
   CPPUNIT_ASSERT( ! mActor->HasChild(*objD));
}

//////////////////////////////////////////////////////////////////////////
void ActorHierarchyTests::TestTwoWayLinkAssignment()
{
   TestObjPtr other = new TestObj;

   // Validate start conditions.
   CPPUNIT_ASSERT( ! mActor->HasChild(*other));
   CPPUNIT_ASSERT(mActor->GetParent() == NULL);
   CPPUNIT_ASSERT(mActor->GetChildCount() == 0);
   CPPUNIT_ASSERT(other->GetParent() == NULL);
   CPPUNIT_ASSERT(other->GetChildCount() == 0);

   // Add other as parent and ensure other has a child reference back.
   mActor->SetParent(other.get());
   CPPUNIT_ASSERT( ! mActor->HasChild(*other));
   CPPUNIT_ASSERT(mActor->GetParent() == other.get());
   CPPUNIT_ASSERT(mActor->GetChildCount() == 0);

   CPPUNIT_ASSERT(other->HasChild(*mActor));
   CPPUNIT_ASSERT(other->GetParent() == NULL);
   CPPUNIT_ASSERT(other->GetChildCount() == 1);
   

   // Test removing other from being a parent, to clear for the next phase.
   mActor->SetParent(NULL);
   CPPUNIT_ASSERT( ! mActor->HasChild(*other));
   CPPUNIT_ASSERT(mActor->GetParent() == NULL);
   CPPUNIT_ASSERT(mActor->GetChildCount() == 0);
   CPPUNIT_ASSERT(other->GetParent() == NULL);
   CPPUNIT_ASSERT(other->GetChildCount() == 0);


   // Add other as a child and ensure other has a parent reference back.
   CPPUNIT_ASSERT(mActor->AddChild(*other));
   CPPUNIT_ASSERT(mActor->HasChild(*other));
   CPPUNIT_ASSERT(mActor->GetParent() == NULL);
   CPPUNIT_ASSERT(mActor->GetChildCount() == 1);

   CPPUNIT_ASSERT( ! other->HasChild(*mActor));
   CPPUNIT_ASSERT(other->GetParent() == mActor.get());
   CPPUNIT_ASSERT(other->GetChildCount() == 0);


   // Test that clearing the owner object also removes it
   // from other hierarchy node references.
   mActor->ClearChildren();
   CPPUNIT_ASSERT( ! mActor->HasChild(*other));
   CPPUNIT_ASSERT(mActor->GetParent() == NULL);
   CPPUNIT_ASSERT(mActor->GetChildCount() == 0);
   CPPUNIT_ASSERT(other->GetParent() == NULL);
   CPPUNIT_ASSERT(other->GetChildCount() == 0);
}

//////////////////////////////////////////////////////////////////////////
bool ActorHierarchyTests::FilterName(const dtCore::BaseActorObject& actor) const
{
   return ! actor.GetName().empty();
}

//////////////////////////////////////////////////////////////////////////
void ActorHierarchyTests::TestObjectFilter()
{
   TestObjPtr objA = new TestObj;
   TestObjPtr objB = new TestObj("B");
   TestObjPtr objC = new TestObj;
   TestObjPtr objD = new TestObj("D");

   // Ensure that not having a filter will allow all actors to be linked.
   CPPUNIT_ASSERT( ! mActor->GetChildFilter().valid());
   CPPUNIT_ASSERT(mActor->IsChildAllowed(*objA));
   CPPUNIT_ASSERT(mActor->IsChildAllowed(*objB));
   CPPUNIT_ASSERT(mActor->IsChildAllowed(*objC));
   CPPUNIT_ASSERT(mActor->IsChildAllowed(*objD));

   TestObj::ObjectFilterFunc filter(this, &ActorHierarchyTests::FilterName);
   mActor->SetChildFilter(filter);

   // Ensure the filter exists and determines only actors with names as valid.
   CPPUNIT_ASSERT(mActor->GetChildFilter().valid());
   CPPUNIT_ASSERT( ! mActor->IsChildAllowed(*objA));
   CPPUNIT_ASSERT(mActor->IsChildAllowed(*objB));
   CPPUNIT_ASSERT( ! mActor->IsChildAllowed(*objC));
   CPPUNIT_ASSERT(mActor->IsChildAllowed(*objD));


   // Test that adding actors is affected by the filter.
   CPPUNIT_ASSERT( ! mActor->AddChild(*objA));
   CPPUNIT_ASSERT(mActor->AddChild(*objB));
   CPPUNIT_ASSERT( ! mActor->AddChild(*objC));
   CPPUNIT_ASSERT(mActor->AddChild(*objD));
   
   CPPUNIT_ASSERT( ! mActor->HasChild(*objA));
   CPPUNIT_ASSERT(mActor->HasChild(*objB));
   CPPUNIT_ASSERT( ! mActor->HasChild(*objC));
   CPPUNIT_ASSERT(mActor->HasChild(*objD));
   CPPUNIT_ASSERT(mActor->GetChildCount() == 2);


   // Ensure that the filter only affects children
   // but does not affect parent assignment.
   mActor->ClearChildren();
   CPPUNIT_ASSERT(mActor->GetChildCount() == 0);

   mActor->SetParent(objA.get());
   CPPUNIT_ASSERT(mActor->GetParent() == objA.get());

   mActor->SetParent(objB.get());
   CPPUNIT_ASSERT(mActor->GetParent() == objB.get());

   mActor->SetParent(objC.get());
   CPPUNIT_ASSERT(mActor->GetParent() == objC.get());

   mActor->SetParent(objD.get());
   CPPUNIT_ASSERT(mActor->GetParent() == objD.get());
}

//////////////////////////////////////////////////////////////////////////
// HELPER PREDS
struct FindActorPred
{
   std::string mName;

   bool IsMatch(const dtCore::BaseActorObject& obj) const
   {
      return mName == obj.GetName();
   }
};

//////////////////////////////////////////////////////////////////////////
void ActorHierarchyTests::TestChildAccessFilters()
{
   TestObjPtr objA = new TestObj;
   TestObjPtr objB = new TestObj("B");
   TestObjPtr objC = new TestObj;
   TestObjPtr objD = new TestObj("D");

   // Test setup.
   CPPUNIT_ASSERT(mActor->AddChild(*objA));
   CPPUNIT_ASSERT(mActor->AddChild(*objB));
   CPPUNIT_ASSERT(mActor->AddChild(*objC));
   CPPUNIT_ASSERT(mActor->AddChild(*objD));
   CPPUNIT_ASSERT(mActor->HasChild(*objA));
   CPPUNIT_ASSERT(mActor->HasChild(*objB));
   CPPUNIT_ASSERT(mActor->HasChild(*objC));
   CPPUNIT_ASSERT(mActor->HasChild(*objD));
   CPPUNIT_ASSERT(mActor->GetChildCount() == 4);


   // Test filters.
   FindActorPred pred;
   TestObj::ObjectFilterFunc func(&pred, &FindActorPred::IsMatch);
   TestObj::ObjArray childArray;

   pred.mName = "D";
   CPPUNIT_ASSERT(mActor->GetChild(func) == objD.get());
   CPPUNIT_ASSERT(mActor->GetChildrenFiltered(childArray, func) == 1);
   CPPUNIT_ASSERT(childArray.size() == 1);
   CPPUNIT_ASSERT(childArray[0] == objD.get());
   childArray.clear();

   pred.mName = "B";
   CPPUNIT_ASSERT(mActor->GetChild(func) == objB.get());
   CPPUNIT_ASSERT(mActor->GetChildrenFiltered(childArray, func) == 1);
   CPPUNIT_ASSERT(childArray.size() == 1);
   CPPUNIT_ASSERT(childArray[0] == objB.get());
   childArray.clear();

   pred.mName = "";
   CPPUNIT_ASSERT(mActor->GetChild(func) == objA.get());
   CPPUNIT_ASSERT(mActor->GetChildrenFiltered(childArray, func) == 2);
   CPPUNIT_ASSERT(childArray.size() == 2);
   CPPUNIT_ASSERT(childArray[0] == objA.get());
   CPPUNIT_ASSERT(childArray[1] == objC.get());
   childArray.clear();
}

//////////////////////////////////////////////////////////////////////////
void ActorHierarchyTests::TestMemoryLeaks()
{
   TestObjPtr parent = new TestObj;
   TestObjPtr objA = new TestObj;
   TestObjPtr objB = new TestObj;


   // Test initial setup.
   mActor->SetParent(parent.get());
   mActor->AddChild(*objA);
   mActor->AddChild(*objB);
   CPPUNIT_ASSERT(mActor->GetParent() == parent.get());
   CPPUNIT_ASSERT( ! mActor->HasChild(*parent));
   CPPUNIT_ASSERT(mActor->HasReference(*parent));
   CPPUNIT_ASSERT(mActor->HasReference(*objA));
   CPPUNIT_ASSERT(mActor->HasReference(*objB));
   CPPUNIT_ASSERT(mActor->HasChild(*objA));
   CPPUNIT_ASSERT(mActor->HasChild(*objB));
   CPPUNIT_ASSERT(mActor->GetParent() == parent.get());
   CPPUNIT_ASSERT(mActor->GetChildCount() == 2);

   // --- Verify two-way linking.
   CPPUNIT_ASSERT( ! parent->HasChild(*parent)); // self
   CPPUNIT_ASSERT( ! parent->HasChild(*objA));
   CPPUNIT_ASSERT( ! parent->HasChild(*objB));
   CPPUNIT_ASSERT(parent->HasChild(*mActor)); // child
   
   CPPUNIT_ASSERT( ! objA->HasReference(*parent)); // grand parent
   CPPUNIT_ASSERT(objA->HasReference(*mActor)); // parent
   CPPUNIT_ASSERT(objA->HasReference(*objA)); // self
   CPPUNIT_ASSERT( ! objA->HasChild(*parent));
   CPPUNIT_ASSERT( ! objA->HasChild(*objA)); // self
   CPPUNIT_ASSERT( ! objA->HasChild(*objB));
   CPPUNIT_ASSERT(objA->GetParent() == mActor.get());
   
   CPPUNIT_ASSERT( ! objB->HasReference(*parent)); // grand parent
   CPPUNIT_ASSERT(objB->HasReference(*objB)); // self
   CPPUNIT_ASSERT(objB->HasReference(*mActor)); // parent
   CPPUNIT_ASSERT( ! objB->HasChild(*parent));
   CPPUNIT_ASSERT( ! objB->HasChild(*objA));
   CPPUNIT_ASSERT( ! objB->HasChild(*objB)); // self
   CPPUNIT_ASSERT(objB->GetParent() == mActor.get());


   // Ensure the parent is not held.
   parent = NULL;
   CPPUNIT_ASSERT(mActor->GetParent() == NULL);
   CPPUNIT_ASSERT(mActor->HasChild(*objA));
   CPPUNIT_ASSERT(mActor->HasChild(*objB));
   CPPUNIT_ASSERT(mActor->GetChildCount() == 2);
   CPPUNIT_ASSERT(TestObj::mDeleteCount == 1);

   // Ensure child A is still held, owned by the parent actor.
   dtCore::ObserverPtr<TestObj> tmpA = objA.get();
   objA = NULL;
   CPPUNIT_ASSERT(tmpA.valid());
   CPPUNIT_ASSERT(mActor->HasChild(*tmpA));
   CPPUNIT_ASSERT(mActor->HasChild(*objB));
   CPPUNIT_ASSERT(mActor->GetChildCount() == 2);
   CPPUNIT_ASSERT(TestObj::mDeleteCount == 1);

   CPPUNIT_ASSERT(objB->GetParent() == mActor.get());
   CPPUNIT_ASSERT(objB->HasReference(*mActor));

   // Ensure that removing the child allows it to delete.
   CPPUNIT_ASSERT(mActor->RemoveChild(*tmpA));
   CPPUNIT_ASSERT(TestObj::mDeleteCount == 2);

   // Ensure the remaining child B will not reference the owner when it is deleted.
   mActor = NULL;
   CPPUNIT_ASSERT(objB->GetParent() == NULL);
   CPPUNIT_ASSERT(objB->GetChildCount() == 0);
   CPPUNIT_ASSERT(TestObj::mDeleteCount == 3);
}

//////////////////////////////////////////////////////////////////////////
void ActorHierarchyTests::TestNotifications()
{
   TestObjPtr parentA = new TestObj;
   TestObjPtr parentB = new TestObj;
   TestObjPtr objA = new TestObj;
   TestObjPtr objB = new TestObj;

   // Test add notifications.
   mActor->SetParent(parentA.get());
   CPPUNIT_ASSERT(mActor->mParentAdded == parentA.get());
   CPPUNIT_ASSERT(mActor->mParentRemoved == NULL);
   CPPUNIT_ASSERT(mActor->mObjectAdded == NULL);
   CPPUNIT_ASSERT(mActor->mObjectRemoved == NULL);
   mActor->mParentAdded = NULL;
   
   CPPUNIT_ASSERT(mActor->AddChild(*objA));
   CPPUNIT_ASSERT(mActor->mParentAdded == NULL);
   CPPUNIT_ASSERT(mActor->mParentRemoved == NULL);
   CPPUNIT_ASSERT(mActor->mObjectAdded == objA.get());
   CPPUNIT_ASSERT(mActor->mObjectRemoved == NULL);
   mActor->mObjectAdded = NULL;

   CPPUNIT_ASSERT(mActor->AddChild(*objB));
   CPPUNIT_ASSERT(mActor->mParentAdded == NULL);
   CPPUNIT_ASSERT(mActor->mParentRemoved == NULL);
   CPPUNIT_ASSERT(mActor->mObjectAdded == objB.get());
   CPPUNIT_ASSERT(mActor->mObjectRemoved == NULL);
   mActor->mObjectAdded = NULL;
   

   // Test parent change notifications (add and remove).
   mActor->SetParent(parentB.get());
   CPPUNIT_ASSERT(mActor->mParentAdded == parentB.get());
   CPPUNIT_ASSERT(mActor->mParentRemoved == parentA.get());
   CPPUNIT_ASSERT(mActor->mObjectAdded == NULL);
   CPPUNIT_ASSERT(mActor->mObjectRemoved == NULL);
   mActor->mParentAdded = NULL;
   mActor->mParentRemoved = NULL;


   // Test regular child object removal notifications.
   CPPUNIT_ASSERT(mActor->RemoveChild(*objB));
   CPPUNIT_ASSERT(mActor->mParentAdded == NULL);
   CPPUNIT_ASSERT(mActor->mParentRemoved == NULL);
   CPPUNIT_ASSERT(mActor->mObjectAdded == NULL);
   CPPUNIT_ASSERT(mActor->mObjectRemoved == objB.get());
   mActor->mObjectRemoved = NULL;

   CPPUNIT_ASSERT(mActor->RemoveChild(*objA));
   CPPUNIT_ASSERT(mActor->mParentAdded == NULL);
   CPPUNIT_ASSERT(mActor->mParentRemoved == NULL);
   CPPUNIT_ASSERT(mActor->mObjectAdded == NULL);
   CPPUNIT_ASSERT(mActor->mObjectRemoved == objA.get());
   mActor->mObjectRemoved = NULL;
}

//////////////////////////////////////////////////////////////////////////
void ActorHierarchyTests::TestActorAddRemoveChildren()
{
   TestObjPtr objA = new TestObj;
   TestObjPtr objB = new TestObj;
   TestObjPtr objC = new TestObj;


   // Check the calls to OnAddedToActor and OnRemovedFromActor.
   CPPUNIT_ASSERT(objA->mCountAddedToActor == 0);
   CPPUNIT_ASSERT(objB->mCountAddedToActor == 0);
   CPPUNIT_ASSERT(objC->mCountAddedToActor == 0);
   CPPUNIT_ASSERT(objB->mAddedToActor == NULL);
   CPPUNIT_ASSERT(objC->mAddedToActor == NULL);
   CPPUNIT_ASSERT(objB->mRemovedFromActor == NULL);
   CPPUNIT_ASSERT(objC->mRemovedFromActor == NULL);
   CPPUNIT_ASSERT(objA->mCountRemovedFromActor == 0);
   CPPUNIT_ASSERT(objB->mCountRemovedFromActor == 0);
   CPPUNIT_ASSERT(objC->mCountRemovedFromActor == 0);


   // Test that the actor assignment.
   CPPUNIT_ASSERT( ! objA->AddChild(*objA));
   CPPUNIT_ASSERT(objA->AddChild(*objB));
   CPPUNIT_ASSERT(objA->AddChild(*objC));
   CPPUNIT_ASSERT( ! objA->HasChild(*objA));
   CPPUNIT_ASSERT(objA->HasChild(*objB));
   CPPUNIT_ASSERT(objA->HasChild(*objC));
   // --- Ensure adding cannot happen twice.
   CPPUNIT_ASSERT( ! objA->AddChild(*objB));
   CPPUNIT_ASSERT( ! objA->AddChild(*objC));
   CPPUNIT_ASSERT(objA->HasChild(*objB));
   CPPUNIT_ASSERT(objA->HasChild(*objC));


   // Check the calls to OnAddedToActor and OnRemovedFromActor.
   CPPUNIT_ASSERT(objA->mCountAddedToActor == 0);
   CPPUNIT_ASSERT(objB->mCountAddedToActor == 1);
   CPPUNIT_ASSERT(objC->mCountAddedToActor == 1);
   CPPUNIT_ASSERT(objB->mParentAdded == objA.get());
   CPPUNIT_ASSERT(objC->mParentAdded == objA.get());
   CPPUNIT_ASSERT(objB->mParentRemoved == NULL);
   CPPUNIT_ASSERT(objC->mParentRemoved == NULL);
   CPPUNIT_ASSERT(objA->mCountRemovedFromActor == 0);
   CPPUNIT_ASSERT(objB->mCountRemovedFromActor == 0);
   CPPUNIT_ASSERT(objC->mCountRemovedFromActor == 0);
   objB->ResetTestVars();
   objC->ResetTestVars();


   // Test removal.
   CPPUNIT_ASSERT(objA->RemoveChild(*objB));
   CPPUNIT_ASSERT(objA->RemoveChild(*objC));
   CPPUNIT_ASSERT( ! objA->HasChild(*objB));
   CPPUNIT_ASSERT( ! objA->HasChild(*objC));
   // --- Ensure second removal fails and does not crash.
   CPPUNIT_ASSERT( ! objA->RemoveChild(*objB));
   CPPUNIT_ASSERT( ! objA->RemoveChild(*objC));


   // Check the calls to OnAddedToActor and OnRemovedFromActor.
   CPPUNIT_ASSERT(objA->mCountAddedToActor == 0);
   CPPUNIT_ASSERT(objB->mCountAddedToActor == 0);
   CPPUNIT_ASSERT(objC->mCountAddedToActor == 0);
   CPPUNIT_ASSERT(objB->mParentAdded == NULL);
   CPPUNIT_ASSERT(objC->mParentAdded == NULL);
   CPPUNIT_ASSERT(objB->mParentRemoved == objA.get());
   CPPUNIT_ASSERT(objC->mParentRemoved == objA.get());
   CPPUNIT_ASSERT(objA->mCountRemovedFromActor == 0);
   CPPUNIT_ASSERT(objB->mCountRemovedFromActor == 1);
   CPPUNIT_ASSERT(objC->mCountRemovedFromActor == 1);
}

//////////////////////////////////////////////////////////////////////////
void ActorHierarchyTests::TestActorRemoveChildrenByType()
{
   typedef dtCore::RefPtr<TestActorComponent> TestActCompPtr;
   TestActCompPtr objA = new TestActorComponent;
   TestActCompPtr objB = new TestActorComponent;
   TestActCompPtr objC = new TestActorComponent;

   CPPUNIT_ASSERT(mActor->AddChild(*objA));
   CPPUNIT_ASSERT(mActor->AddChild(*objB));
   CPPUNIT_ASSERT(mActor->AddChild(*objC));
   CPPUNIT_ASSERT(mActor->HasChild(*objA));
   CPPUNIT_ASSERT(mActor->HasChild(*objB));
   CPPUNIT_ASSERT(mActor->HasChild(*objC));
   CPPUNIT_ASSERT(mActor->GetChildCount() == 3);

   dtCore::BaseActorArray children;
   CPPUNIT_ASSERT(mActor->RemoveChildrenByType(
      *dtCore::ActorComponent::BaseActorComponentType, &children, false) == 3);

   CPPUNIT_ASSERT( ! mActor->HasChild(*objA));
   CPPUNIT_ASSERT( ! mActor->HasChild(*objB));
   CPPUNIT_ASSERT( ! mActor->HasChild(*objC));
   CPPUNIT_ASSERT(mActor->GetChildCount() == 0);
   CPPUNIT_ASSERT(children.size() == 3);
   
   CPPUNIT_ASSERT(children[0] == objA.get());
   CPPUNIT_ASSERT(children[1] == objB.get());
   CPPUNIT_ASSERT(children[2] == objC.get());
}

//////////////////////////////////////////////////////////////////////////
void ActorHierarchyTests::TestActorGetChildren()
{
   TestObjPtr objA = new TestObj;
   TestObjPtr objB = new TestObj;
   dtCore::RefPtr<TestActorComponent> compA = new TestActorComponent;
   dtCore::RefPtr<TestActorComponent> compB = new TestActorComponent;
   compA->Init(*TestActorComponent::TYPE);
   compB->Init(*TestActorComponent::TYPE);

   CPPUNIT_ASSERT(mActor->AddChild(*objA));
   CPPUNIT_ASSERT(mActor->AddChild(*objB));
   CPPUNIT_ASSERT(mActor->AddChild(*compA));
   CPPUNIT_ASSERT(mActor->AddChild(*compB));
   CPPUNIT_ASSERT(mActor->GetChildCount() == 4);

   const dtCore::ActorType& BASE_COMP_TYPE = *dtCore::ActorComponent::BaseActorComponentType;

   // Test the default actor type filter;
   TestObj::ActorTypeFilter filter(BASE_COMP_TYPE);
   CPPUNIT_ASSERT(filter.IsMatchType(compA->GetActorType()));
   CPPUNIT_ASSERT(filter.IsMatch(*compA));
   CPPUNIT_ASSERT( ! filter.IsExactMatchEnabled());
   
   // Test that all child actors can be accessed.
   dtCore::BaseActorArray actors;
   CPPUNIT_ASSERT(mActor->GetChildren(actors) == 4);
   CPPUNIT_ASSERT(actors.size() == 4);
   actors.clear();

   // Test that specific actors of a specified type can be accessed, such as actor components.
   CPPUNIT_ASSERT(mActor->GetChildrenByType(BASE_COMP_TYPE, actors) == 2);
   CPPUNIT_ASSERT(actors.size() == 2);
   CPPUNIT_ASSERT(actors[0] == compA.get());
   CPPUNIT_ASSERT(actors[1] == compB.get());
   actors.clear();

   // Test accessing the first match of a specified type.
   BaseActorObject* match = mActor->GetChildByType(BASE_COMP_TYPE);
   CPPUNIT_ASSERT(match == compA.get());
}

//////////////////////////////////////////////////////////////////////////
void ActorHierarchyTests::TestActorVisitorTraversal()
{
   TestObjPtr objA = new TestObj("A");
   TestObjPtr objB = new TestObj("B");
   TestObjPtr objC = new TestObj("C");
   TestObjPtr objD = new TestObj("D");
   TestObjPtr objE = new TestObj("E");
   TestObjPtr objF = new TestObj("F");
   TestObjPtr objG = new TestObj("G");

   CPPUNIT_ASSERT(objA->AddChild(*objB));
   CPPUNIT_ASSERT(objA->AddChild(*objC));
   CPPUNIT_ASSERT(objB->AddChild(*objD));
   CPPUNIT_ASSERT(objB->AddChild(*objE));
   CPPUNIT_ASSERT(objC->AddChild(*objF));
   CPPUNIT_ASSERT(objC->AddChild(*objG));

   // Ensure the test hierarchy
   CPPUNIT_ASSERT(objA->HasChild(*objB));
   CPPUNIT_ASSERT(objA->HasChild(*objC));
   CPPUNIT_ASSERT(objB->HasChild(*objD));
   CPPUNIT_ASSERT(objB->HasChild(*objE));
   CPPUNIT_ASSERT(objC->HasChild(*objF));
   CPPUNIT_ASSERT(objC->HasChild(*objG));


   dtCore::RefPtr<TestActorVisitor> visitor = new TestActorVisitor;
   StrArray& order = visitor->mNameOrder;

   visitor->SetMode(TestActorVisitor::IteratorType::DEPTH_FIRST);
   visitor->Traverse(*objA);
   CPPUNIT_ASSERT(order.size() == 7);
   CPPUNIT_ASSERT(order[0] == "D");
   CPPUNIT_ASSERT(order[1] == "E");
   CPPUNIT_ASSERT(order[2] == "B");
   CPPUNIT_ASSERT(order[3] == "F");
   CPPUNIT_ASSERT(order[4] == "G");
   CPPUNIT_ASSERT(order[5] == "C");
   CPPUNIT_ASSERT(order[6] == "A");


   visitor->Reset();
   visitor->SetMode(TestActorVisitor::IteratorType::BREADTH_FIRST);
   visitor->Traverse(*objA);
   CPPUNIT_ASSERT(order.size() == 7);
   CPPUNIT_ASSERT(order[0] == "A");
   CPPUNIT_ASSERT(order[1] == "B");
   CPPUNIT_ASSERT(order[2] == "C");
   CPPUNIT_ASSERT(order[3] == "D");
   CPPUNIT_ASSERT(order[4] == "E");
   CPPUNIT_ASSERT(order[5] == "F");
   CPPUNIT_ASSERT(order[6] == "G");


   visitor->Reset();
   visitor->SetMode(TestActorVisitor::IteratorType::LEFT_FIRST);
   visitor->Traverse(*objA);
   CPPUNIT_ASSERT(order.size() == 7);
   CPPUNIT_ASSERT(order[0] == "A");
   CPPUNIT_ASSERT(order[1] == "B");
   CPPUNIT_ASSERT(order[2] == "D");
   CPPUNIT_ASSERT(order[3] == "E");
   CPPUNIT_ASSERT(order[4] == "C");
   CPPUNIT_ASSERT(order[5] == "F");
   CPPUNIT_ASSERT(order[6] == "G");


   visitor->Reset();
   visitor->SetMode(TestActorVisitor::IteratorType::RIGHT_FIRST);
   visitor->Traverse(*objA);
   CPPUNIT_ASSERT(order.size() == 7);
   CPPUNIT_ASSERT(order[0] == "A");
   CPPUNIT_ASSERT(order[1] == "C");
   CPPUNIT_ASSERT(order[2] == "G");
   CPPUNIT_ASSERT(order[3] == "F");
   CPPUNIT_ASSERT(order[4] == "B");
   CPPUNIT_ASSERT(order[5] == "E");
   CPPUNIT_ASSERT(order[6] == "D");


   visitor->Reset();
   visitor->SetMode(TestActorVisitor::IteratorType::ANCESTORS);
   visitor->Traverse(*objE);
   CPPUNIT_ASSERT(order.size() == 3);
   CPPUNIT_ASSERT(order[0] == "E");
   CPPUNIT_ASSERT(order[1] == "B");
   CPPUNIT_ASSERT(order[2] == "A");
}
