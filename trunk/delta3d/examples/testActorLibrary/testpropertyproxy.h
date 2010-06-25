/* -*-c++-*-
 * testActorLibrary - testpropertyproxy (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Curtiss Murphy
 */
#ifndef DELTA_EXAMPLETESTPROPERTYPROXY
#define DELTA_EXAMPLETESTPROPERTYPROXY

#include <dtActors/staticmeshactorproxy.h>

#include <dtDAL/actorproxy.h>
#include <dtDAL/arrayactorproperty.h>
#include <dtDAL/gameevent.h>
#include <dtDAL/namedparameter.h>
#include <dtDAL/plugin_export.h>

#include <dtUtil/enumeration.h>
#include <dtUtil/getsetmacros.h>

using namespace dtActors;

namespace dtCore
{
    class Scene;
}

class DT_PLUGIN_EXPORT ExampleTestPropertyProxy : public StaticMeshActorProxy
{
public:
   ExampleTestPropertyProxy();
   virtual void BuildPropertyMap();

   //dtCore::Light has an enumeration.  We define our own enumeration here
   //which can be exported as a property to the editor.
   class DT_PLUGIN_EXPORT TestEnum : public dtUtil::Enumeration
   {
      DECLARE_ENUM(TestEnum);
   public:
      static TestEnum OPTION1;
      static TestEnum OPTION2;
      static TestEnum OPTION3;
      static TestEnum OPTION4;
      static TestEnum OPTION5;
      static TestEnum OPTION6;

   private:
      TestEnum(const std::string &name) : dtUtil::Enumeration(name)
      {
         AddInstance(this);
      }
   };

   struct DT_PLUGIN_EXPORT testStruct
   {
      osg::Vec3 vector;
      int       value;
   };

   DECLARE_PROPERTY_INLINE(float, TestFloat);
   DECLARE_PROPERTY_INLINE(bool, TestBool);
   DECLARE_PROPERTY_INLINE(double, TestDouble);
   DECLARE_PROPERTY_INLINE(int, TestInt);
   DECLARE_PROPERTY_INLINE(int, ReadOnlyTestInt);
   DECLARE_PROPERTY_INLINE(long, TestLong);
   DECLARE_PROPERTY_INLINE(std::string, TestString);
   DECLARE_PROPERTY_INLINE(std::string, TestStringWithLength);
   DECLARE_PROPERTY_INLINE(osg::Vec2, TestVec2);
   DECLARE_PROPERTY_INLINE(osg::Vec2f, TestVec2f);
   DECLARE_PROPERTY_INLINE(osg::Vec2d, TestVec2d);
   DECLARE_PROPERTY_INLINE(osg::Vec3, TestVec3);
   DECLARE_PROPERTY_INLINE(osg::Vec3f, TestVec3f);
   DECLARE_PROPERTY_INLINE(osg::Vec3d, TestVec3d);
   DECLARE_PROPERTY_INLINE(osg::Vec4, TestVec4);
   DECLARE_PROPERTY_INLINE(osg::Vec4f, TestVec4f);
   DECLARE_PROPERTY_INLINE(osg::Vec4d, TestVec4d);
   DECLARE_PROPERTY_INLINE(osg::Vec4, TestColor);
   DECLARE_PROPERTY_INLINE(dtUtil::EnumerationPointer<TestEnum>, TestEnum);
   DECLARE_PROPERTY_INLINE(dtCore::UniqueId, TestActor);
   DECLARE_PROPERTY_INLINE(dtCore::RefPtr<dtDAL::GameEvent>, TestGameEvent);

   DECLARE_PROPERTY_INLINE(dtDAL::ResourceDescriptor, TestSoundResource);
   DECLARE_PROPERTY_INLINE(dtDAL::ResourceDescriptor, TestTextureResource);

// void SetTestGameEvent(dtDAL::GameEvent* event) { mTestGameEvent = event; }
// dtDAL::GameEvent* GetTestGameEvent() { return mTestGameEvent; }

   // Can't use a DECLARE_PROPERTY because it returns a ref ptr to the value because it expects you may generate the group
   // data on the fly.
   void SetTestGroup(const dtDAL::NamedGroupParameter& groupParam) { mGroupParam = new dtDAL::NamedGroupParameter(groupParam); }
   dtCore::RefPtr<dtDAL::NamedGroupParameter> GetTestGroup() { return mGroupParam; }

   // Array of strings
   void StringArraySetIndex(int index);
   std::string StringArrayGetDefault();
   std::vector<std::string> StringArrayGetValue();
   void StringArraySetValue(const std::vector<std::string>& value);
   void SetStringArrayValue(const std::string& value);
   std::string GetStringArrayValue();

   // Array of colors
   void ContainerArraySetIndex(int index);
   testStruct ContainerArrayGetDefault();
   std::vector<testStruct> ContainerArrayGetValue();
   void ContainerArraySetValue(const std::vector<testStruct>& value);
   void SetVecContainerValue(const osg::Vec3& value);
   osg::Vec3 GetVecContainerValue();
   void SetIntContainerValue(int value);
   int GetIntContainerValue();

   // Array of Arrays of Ints.
   void IntArraySetIndex(int index);
   int IntArrayGetDefault();
   std::vector<int> IntArrayGetValue();
   void IntArraySetValue(const std::vector<int>& value);
   void ArrayArraySetIndex(int index);
   std::vector<int> ArrayArrayGetDefault();
   std::vector<std::vector<int> > ArrayArrayGetValue();
   void ArrayArraySetValue(const std::vector<std::vector<int> >& value);
   void SetIntArrayValue(int value);
   int GetIntArrayValue();

protected:
   virtual ~ExampleTestPropertyProxy() { }

private:
// dtCore::RefPtr<dtDAL::GameEvent> mTestGameEvent;
   dtCore::RefPtr<dtDAL::NamedGroupParameter> mGroupParam;
   std::vector<std::string>       mStringArray;
   int                            mStringArrayIndex;
   std::vector<testStruct>        mStructArray;
   int                            mContainerArrayIndex;
   std::vector<std::vector<int> > mArrayIntArray;
   int                            mIntArrayIndex;
   int                            mArrayArrayIndex;

   static const std::string GROUPNAME;
};

#endif // DELTA_EXAMPLETESTPROPERTYPROXY
