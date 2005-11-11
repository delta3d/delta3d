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
* @author Curtiss Murphy
*/
#ifndef DELTA_EXAMPLETESTPROPERTYPROXY
#define DELTA_EXAMPLETESTPROPERTYPROXY

#include <dtActors/deltaobjectactorproxy.h>
#include <dtActors/staticmeshactorproxy.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/enumeration.h>
#include <dtUtil/log.h>
#include <dtDAL/plugin_export.h>

using namespace dtActors;

namespace dtCore {
    class Scene;
}

class DT_PLUGIN_EXPORT ExampleTestPropertyProxy : public StaticMeshActorProxy {
public:
    ExampleTestPropertyProxy();
    virtual void BuildPropertyMap();

    //dtCore::Light has an enumeration.  We define our own enumeration here
    //which can be exported as a property to the editor.
    class DT_PLUGIN_EXPORT TestEnum : public dtUtil::Enumeration {
        DECLARE_ENUM(TestEnum);
    public:
        static TestEnum OPTION1;
        static TestEnum OPTION2;
        static TestEnum OPTION3;
        static TestEnum OPTION4;
        static TestEnum OPTION5;
        static TestEnum OPTION6;

    private:
        TestEnum(const std::string &name) : dtUtil::Enumeration(name) {
            AddInstance(this);
        }
    };

    /**
    * Sets the test Float
    * @param value Test value
    */
    void setTestFloat(float value)
    {
        myFloat = value;
    }

    /**
    * Gets the Test Float
    * @return the test value
    */
    float getTestFloat()
    {
        return myFloat;
    }

    /**
    * Sets the test bool
    * @param value Test value
    */
    void setTestBool(bool value)
    {
        myBool = value;
    }

    /**
    * Gets the Test bool
    * @return the test value
    */
    bool getTestBool()
    {
        return myBool;
    }

    /**
    * Sets the test Double
    * @param value Test value
    */
    void setTestDouble(double value)
    {
        myDouble = value;
    }

    /**
    * Gets the Test Value
    * @return the test value
    */
    double getTestDouble()
    {
        return myDouble;
    }

    /**
    * Sets the test value
    * @param value Test value
    */
    void setTestInt(int value)
    {
        myInt = value;
    }

    /**
    * Gets the Test value
    * @return the test value
    */
    int getTestInt()
    {
        return myInt;
    }

    /**
    * Sets the test value
    * @param value Test value
    */
    void setReadOnlyTestInt(int value)
    {
        myReadOnlyInt = value;
    }

    /**
    * Gets the Test value
    * @return the test value
    */
    int getReadOnlyTestInt()
    {
        return myReadOnlyInt;
    }

    /**
    * Sets the test value
    * @param value Test value
    */
    void setTestLong(long value)
    {
        myLong = value;
    }

    /**
    * Gets the Test value
    * @return the test value
    */
    long getTestLong()
    {
        return myLong;
    }

   /**
    * Sets the test value
    * @param value Test value
    */
    void setTestString(const std::string &value)
    {
        myString = value;
    }

    /**
    * Gets the Test value
    * @return the test value
    */
    std::string getTestString()
    {
        return myString;
    }

   /**
    * Sets the test value
    * @param value Test value
    */
    void setTestStringWithLength(const std::string &value)
    {
        myStringWithLength = value;
    }

    /**
    * Gets the Test value
    * @return the test value
    */
    std::string getTestStringWithLength()
    {
        return myStringWithLength;
    }

    /**
    * Sets the test value
    * @param value Test value
    */
    void setTestVec4(const osg::Vec4 &value)
    {
        myVec4 = value;
    }

    /**
    * Gets the Test value
    * @return the test value
    */
    osg::Vec4 getTestVec4()
    {
        return myVec4;
    }

    /**
    * Sets the test value
    * @param value Test value
    */
    void setTestVec4f(const osg::Vec4f &value)
    {
        myVec4f = value;
    }

    /**
    * Gets the Test value
    * @return the test value
    */
    osg::Vec4d getTestVec4d()
    {
        return myVec4d;
    }

    /**
    * Sets the test value
    * @param value Test value
    */
    void setTestVec4d(const osg::Vec4d &value)
    {
        myVec4d = value;
    }

    /**
    * Gets the Test value
    * @return the test value
    */
    osg::Vec4f getTestVec4f()
    {
        return myVec4f;
    }

    /**
    * Sets the test value
    * @param value Test value
    */
    void setTestVec2(const osg::Vec2 &value)
    {
        myVec2 = value;
    }

    /**
    * Gets the Test value
    * @return the test value
    */
    osg::Vec2 getTestVec2()
    {
        return myVec2;
    }

    /**
    * Sets the test value
    * @param value Test value
    */
    void setTestVec2f(const osg::Vec2f &value)
    {
        myVec2f = value;
    }

    /**
    * Gets the Test value
    * @return the test value
    */
    osg::Vec2f getTestVec2f()
    {
        return myVec2f;
    }

    /**
    * Sets the test value
    * @param value Test value
    */
    void setTestVec2d(const osg::Vec2d &value)
    {
        myVec2d = value;
    }

    /**
    * Gets the Test value
    * @return the test value
    */
    osg::Vec2d getTestVec2d()
    {
        return myVec2d;
    }

    /**
    * Sets the test value
    * @param value Test value
    */
    void setTestVec3(const osg::Vec3 &value)
    {
        myVec3 = value;
    }

    /**
    * Gets the Test value
    * @return the test value
    */
    osg::Vec3 getTestVec3()
    {
        return myVec3;
    }

    /**
    * Sets the test value
    * @param value Test value
    */
    void setTestVec3f(const osg::Vec3f &value)
    {
        myVec3f = value;
    }

    /**
    * Gets the Test value
    * @return the test value
    */
    osg::Vec3f getTestVec3f()
    {
        return myVec3f;
    }

    /**
    * Sets the test value
    * @param value Test value
    */
    void setTestVec3d(const osg::Vec3d &value)
    {
        myVec3d = value;
    }

    /**
    * Gets the Test value
    * @return the test value
    */
    osg::Vec3d getTestVec3d()
    {
        return myVec3d;
    }

    void setTestEnum(TestEnum &mode) 
    {
        //dtCore::Light *l = dynamic_cast<dtCore::Light *>(this->actor.get());
        myEnum = &mode;
    }

    TestEnum &getTestEnum() 
    {
        return *myEnum;
    }

    /**
     * Sets the test value
     * @param value Test value
     */
    void setTestColor(const osg::Vec4 &value)
    {
        myColor = value;
    }

    /**
     * Gets the Test value
     * @return the test value
     */
    osg::Vec4 getTestColor()
    {
        return myColor;
    }

    void setSoundResourceName(const std::string &fileName) { mySound = fileName;}

    void setTextureResourceName(const std::string &fileName) { myTexture = fileName; }

    dtCore::DeltaDrawable* GetTestActor() 
    {
       LOG_ALWAYS("ActorProxy get");
       
       ActorProxy* proxy = GetLinkedActor("Test_Actor");
       return proxy->GetActor();
    }

    void SetTestActor(ActorProxy* proxy) 
    {
        this->SetLinkedActor("Test_Actor", proxy);
        LOG_ALWAYS("ActorProxy set");
    }


    void loadFile(const std::string &fileName) {
    //    dtCore::Loadable *obj = dynamic_cast<dtCore::Loadable*>(actor.get());
    //    if (obj == NULL)
    //        EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Loadable");

    //    obj->LoadFile(fileName);
    }

protected:
    virtual ~ExampleTestPropertyProxy() { }

private:
    int myInt, myReadOnlyInt;
    float myFloat;
    double myDouble;
    long myLong;
    bool myBool;
    std::string myString;
    std::string myStringWithLength;
    osg::Vec2 myVec2;
    osg::Vec3 myVec3;
    osg::Vec4 myVec4;
    osg::Vec2 myVec2f;
    osg::Vec3 myVec3f;
    osg::Vec4 myVec4f;
    osg::Vec2 myVec2d;
    osg::Vec3 myVec3d;
    osg::Vec4 myVec4d;
    osg::Vec4 myColor;
    TestEnum *myEnum;
    std::string mySound;
    std::string myTexture;

    static const std::string GROUPNAME;

};


#endif
