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

#include <dtActors/gamemeshactor.h>

#include <dtCore/actorproxy.h>
#include <dtCore/arrayactorproperty.h>
#include <dtCore/gameevent.h>
#include <dtCore/namedgroupparameter.h>
#include <dtCore/plugin_export.h>
#include <dtCore/resourcedescriptor.h>

#include <dtUtil/enumeration.h>
#include <dtUtil/getsetmacros.h>

#include "testpropertycontainer.h"

namespace dtCore
{
    class Scene;
}

class DT_PLUGIN_EXPORT ExampleTestPropertyProxy : public dtActors::GameMeshActor
{
public:
   typedef dtActors::GameMeshActor BaseClass;

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
      TestEnum(const std::string& name) : dtUtil::Enumeration(name)
      {
         AddInstance(this);
      }
   };

   struct DT_PLUGIN_EXPORT testStruct
   {
      osg::Vec3 vector;
      int       value;
   };

   DT_DECLARE_ACCESSOR_INLINE(float, TestFloat)
   DT_DECLARE_ACCESSOR_INLINE(bool, TestBool)
   DT_DECLARE_ACCESSOR_INLINE(double, TestDouble)
   DT_DECLARE_ACCESSOR_INLINE(int, TestInt)
   DT_DECLARE_ACCESSOR_INLINE(int, ReadOnlyTestInt)
   DT_DECLARE_ACCESSOR_INLINE(long, TestLong)
   DT_DECLARE_ACCESSOR_INLINE(std::string, TestString)
   DT_DECLARE_ACCESSOR_INLINE(std::string, TestStringWithLength)
   DT_DECLARE_ACCESSOR_INLINE(unsigned int, TestBitmask)
   DT_DECLARE_ACCESSOR_INLINE(osg::Vec2, TestVec2)
   DT_DECLARE_ACCESSOR_INLINE(osg::Vec2f, TestVec2f)
   DT_DECLARE_ACCESSOR_INLINE(osg::Vec2d, TestVec2d)
   DT_DECLARE_ACCESSOR_INLINE(osg::Vec3, TestVec3)
   DT_DECLARE_ACCESSOR_INLINE(osg::Vec3f, TestVec3f)
   DT_DECLARE_ACCESSOR_INLINE(osg::Vec3d, TestVec3d)
   DT_DECLARE_ACCESSOR_INLINE(osg::Vec4, TestVec4)
   DT_DECLARE_ACCESSOR_INLINE(osg::Vec4f, TestVec4f)
   DT_DECLARE_ACCESSOR_INLINE(osg::Vec4d, TestVec4d)
   DT_DECLARE_ACCESSOR_INLINE(osg::Vec4, TestColor)
   DT_DECLARE_ACCESSOR_INLINE(dtUtil::EnumerationPointer<TestEnum>, TestEnum)
   DT_DECLARE_ACCESSOR_INLINE(dtCore::UniqueId, TestActor)
   DT_DECLARE_ACCESSOR_INLINE(dtCore::RefPtr<dtCore::GameEvent>, TestGameEvent)

   DT_DECLARE_ACCESSOR_INLINE(dtCore::ResourceDescriptor, TestSoundResource)
   DT_DECLARE_ACCESSOR_INLINE(dtCore::ResourceDescriptor, TestTextureResource)

   DT_DECLARE_ACCESSOR(dtCore::RefPtr<TestPropertyContainer>, TestPropertyContainer)

// void SetTestGameEvent(dtCore::GameEvent* event) { mTestGameEvent = event; }
// dtCore::GameEvent* GetTestGameEvent() { return mTestGameEvent; }

   // Can't use a DT_DECLARE_ACCESSOR because it returns a ref ptr to the value because it expects you may generate the group
   // data on the fly.
   void SetTestGroup(const dtCore::NamedGroupParameter& groupParam) { mGroupParam = new dtCore::NamedGroupParameter(groupParam); }
   dtCore::RefPtr<dtCore::NamedGroupParameter> GetTestGroup() { return mGroupParam; }

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

   void GetBitMasks(std::vector<std::string>& names, std::vector<unsigned int>& values);

protected:
   virtual ~ExampleTestPropertyProxy() { }

private:
// dtCore::RefPtr<dtCore::GameEvent> mTestGameEvent;
   dtCore::RefPtr<dtCore::NamedGroupParameter> mGroupParam;
   std::vector<std::string>       mStringArray;
   int                            mStringArrayIndex;
   std::vector<testStruct>        mStructArray;
   int                            mContainerArrayIndex;
   std::vector<std::vector<int> > mArrayIntArray;
   int                            mIntArrayIndex;
   int                            mArrayArrayIndex;

   static const dtUtil::RefString GROUPNAME;
};

#endif // DELTA_EXAMPLETESTPROPERTYPROXY
