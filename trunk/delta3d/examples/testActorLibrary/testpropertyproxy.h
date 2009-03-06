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
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/namedparameter.h>
#include <dtDAL/arrayactorproperty.h>
#include <dtDAL/plugin_export.h>
#include <dtDAL/gameevent.h>
#include <dtUtil/enumeration.h>

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

      /**
       * Sets the test Float
       * @param value Test value
       */
      void SetTestFloat(float value)
      {
         mFloat = value;
      }

      /**
       * Gets the Test Float
       * @return the test value
       */
      float GetTestFloat()
      {
         return mFloat;
      }

      /**
       * Sets the test bool
       * @param value Test value
       */
      void SetTestBool(bool value)
      {
         mBool = value;
      }

      /**
       * Gets the Test bool
       * @return the test value
       */
      bool GetTestBool()
      {
         return mBool;
      }

      /**
       * Sets the test Double
       * @param value Test value
       */
      void SetTestDouble(double value)
      {
         mDouble = value;
      }

      /**
      * Gets the Test Value
      * @return the test value
      */
      double GetTestDouble()
      {
         return mDouble;
      }

      /**
       * Sets the test value
       * @param value Test value
       */
      void SetTestInt(int value)
      {
         mInt = value;
      }

      /**
       * Gets the Test value
       * @return the test value
       */
      int GetTestInt()
      {
         return mInt;
      }

      /**
       * Sets the test value
       * @param value Test value
       */
      void SetReadOnlyTestInt(int value)
      {
         mReadOnlyInt = value;
      }

      /**
       * Gets the Test value
       * @return the test value
       */
      int GetReadOnlyTestInt()
      {
         return mReadOnlyInt;
      }

      /**
      * Sets the test value
      * @param value Test value
      */
      void SetTestLong(long value)
      {
         mLong = value;
      }

      /**
       * Gets the Test value
       * @return the test value
       */
      long GetTestLong()
      {
         return mLong;
      }

      /**
       * Sets the test value
       * @param value Test value
       */
      void SetTestString(const std::string &value)
      {
         mString = value;
      }

      /**
       * Gets the Test value
       * @return the test value
       */
      std::string GetTestString()
      {
        return mString;
      }

      /**
       * Sets the test value
       * @param value Test value
       */
      void SetTestStringWithLength(const std::string &value)
      {
         mStringWithLength = value;
      }

      /**
       * Gets the Test value
       * @return the test value
       */
      std::string GetTestStringWithLength()
      {
         return mStringWithLength;
      }

      /**
       * Sets the test value
       * @param value Test value
       */
      void SetTestVec4(const osg::Vec4 &value)
      {
         mVec4 = value;
      }

      /**
       * Gets the Test value
       * @return the test value
       */
      osg::Vec4 GetTestVec4()
      {
         return mVec4;
      }

      /**
       * Sets the test value
       * @param value Test value
       */
      void SetTestVec4f(const osg::Vec4f &value)
      {
         mVec4f = value;
      }

      /**
       * Gets the Test value
       * @return the test value
       */
      osg::Vec4d GetTestVec4d()
      {
         return mVec4d;
      }

      /**
       * Sets the test value
       * @param value Test value
       */
      void SetTestVec4d(const osg::Vec4d &value)
      {
         mVec4d = value;
      }

      /**
       * Gets the Test value
       * @return the test value
       */
      osg::Vec4f GetTestVec4f()
      {
         return mVec4f;
      }

      /**
       * Sets the test value
       * @param value Test value
       */
      void SetTestVec2(const osg::Vec2 &value)
      {
         mVec2 = value;
      }

      /**
       * Gets the Test value
       * @return the test value
       */
      osg::Vec2 GetTestVec2()
      {
         return mVec2;
      }

      /**
       * Sets the test value
       * @param value Test value
       */
      void SetTestVec2f(const osg::Vec2f &value)
      {
         mVec2f = value;
      }

      /**
       * Gets the Test value
       * @return the test value
       */
      osg::Vec2f GetTestVec2f()
      {
         return mVec2f;
      }

      /**
       * Sets the test value
       * @param value Test value
       */
      void SetTestVec2d(const osg::Vec2d &value)
      {
         mVec2d = value;
      }

      /**
       * Gets the Test value
       * @return the test value
       */
      osg::Vec2d GetTestVec2d()
      {
         return mVec2d;
      }

      /**
       * Sets the test value
       * @param value Test value
       */
      void SetTestVec3(const osg::Vec3 &value)
      {
         mVec3 = value;
      }

      /**
       * Gets the Test value
       * @return the test value
       */
      osg::Vec3 GetTestVec3()
      {
         return mVec3;
      }

      /**
       * Sets the test value
       * @param value Test value
       */
      void SetTestVec3f(const osg::Vec3f &value)
      {
         mVec3f = value;
      }

      /**
       * Gets the Test value
       * @return the test value
       */
      osg::Vec3f GetTestVec3f()
      {
         return mVec3f;
      }

      /**
       * Sets the test value
       * @param value Test value
       */
      void SetTestVec3d(const osg::Vec3d &value)
      {
         mVec3d = value;
      }

      /**
       * Gets the Test value
       * @return the test value
       */
      osg::Vec3d GetTestVec3d()
      {
         return mVec3d;
      }

      void SetTestEnum(TestEnum &mode)
      {
         //dtCore::Light *l = dynamic_cast<dtCore::Light *>(this->actor.get());
         mEnum = &mode;
      }

      TestEnum &GetTestEnum()
      {
         return *mEnum;
      }

      /**
       * Sets the test value
       * @param value Test value
       */
      void SetTestColor(const osg::Vec4 &value)
      {
         mColor = value;
      }

      /**
       * Gets the Test value
       * @return the test value
       */
      osg::Vec4 GetTestColor()
      {
         return mColor;
      }

      void SetSoundResourceName(const std::string &fileName) { mSound = fileName;}

      void SetTextureResourceName(const std::string &fileName) { mTexture = fileName; }

      dtCore::DeltaDrawable* GetTestActor()
      {
         //LOG_ALWAYS("ActorProxy Get");

         ActorProxy* proxy = GetLinkedActor("Test_Actor");
         return proxy->GetActor();
      }

      void SetTestActor(ActorProxy* proxy)
      {
         this->SetLinkedActor("Test_Actor", proxy);
         //LOG_ALWAYS("ActorProxy Set");
      }

      void loadFile(const std::string &fileName) 
      {
      //    dtCore::Loadable *obj = dynamic_cast<dtCore::Loadable*>(actor.get());
      //    if (obj == NULL)
      //        EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Loadable");

      //    obj->LoadFile(fileName);
      }

      void SetTestGameEvent(dtDAL::GameEvent *event) { mTestGameEvent = event; }
      dtDAL::GameEvent *GetTestGameEvent() { return mTestGameEvent.get(); }

      void SetTestGroup(const dtDAL::NamedGroupParameter& groupParam) { mGroupParam = new dtDAL::NamedGroupParameter(groupParam); }
      dtCore::RefPtr<dtDAL::NamedGroupParameter> GetTestGroup() { return mGroupParam; }

      // Array of strings
      void StringArraySetIndex(int index);
      std::string StringArrayGetDefault();
      std::vector<std::string>& StringArrayGetValue();
      void SetStringArrayValue(const std::string& value);
      std::string GetStringArrayValue();

      // Array of colors
      void ColorArraySetIndex(int index);
      osg::Vec4 ColorArrayGetDefault();
      std::vector<osg::Vec4>& ColorArrayGetValue();
      void SetColorArrayValue(const osg::Vec4& value);
      osg::Vec4 GetColorArrayValue();

      // Array of Arrays of Ints.
      void IntArraySetIndex(int index);
      int IntArrayGetDefault();
      std::vector<int>& IntArrayGetValue();
      void ArrayArraySetIndex(int index);
      std::vector<int> ArrayArrayGetDefault();
      std::vector<std::vector<int> >& ArrayArrayGetValue();
      void SetIntArrayValue(int value);
      int GetIntArrayValue();

   protected:
       virtual ~ExampleTestPropertyProxy() { }

   private:

      int mInt, mReadOnlyInt;
      float mFloat;
      double mDouble;
      long mLong;
      bool mBool;
      std::string mString;
      std::string mStringWithLength;
      osg::Vec2 mVec2;
      osg::Vec3 mVec3;
      osg::Vec4 mVec4;
      osg::Vec2 mVec2f;
      osg::Vec3 mVec3f;
      osg::Vec4 mVec4f;
      osg::Vec2 mVec2d;
      osg::Vec3 mVec3d;
      osg::Vec4 mVec4d;
      osg::Vec4 mColor;
      TestEnum *mEnum;
      std::string mSound;
      std::string mTexture;
      dtCore::RefPtr<dtDAL::GameEvent> mTestGameEvent;
      dtCore::RefPtr<dtDAL::NamedGroupParameter> mGroupParam;
      std::vector<std::string>      mStringArray;
      int                           mStringArrayIndex;
      std::vector<osg::Vec4>        mColorArray;
      int                           mColorArrayIndex;
      std::vector<std::vector<int> > mArrayIntArray;
      int                           mIntArrayIndex;
      int                           mArrayArrayIndex;

      static const std::string GROUPNAME;
};


#endif
