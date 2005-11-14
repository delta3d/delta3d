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
#include "testPropertyProxy.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtActors/deltaobjectactorproxy.h"
#include "dtCore/scene.h"
#include "dtCore/object.h"
#include <sstream>

using namespace dtCore;
using namespace dtDAL;
using namespace dtActors;

const std::string ExampleTestPropertyProxy::GROUPNAME("Example Test");

    IMPLEMENT_ENUM(ExampleTestPropertyProxy::TestEnum);
    ExampleTestPropertyProxy::TestEnum ExampleTestPropertyProxy::TestEnum::OPTION1("My First Option");
    ExampleTestPropertyProxy::TestEnum ExampleTestPropertyProxy::TestEnum::OPTION2("Cool Stuff");
    ExampleTestPropertyProxy::TestEnum ExampleTestPropertyProxy::TestEnum::OPTION3("123");
    ExampleTestPropertyProxy::TestEnum ExampleTestPropertyProxy::TestEnum::OPTION4("Why a spoon cousin?  Because it'll hurt more.");
    ExampleTestPropertyProxy::TestEnum ExampleTestPropertyProxy::TestEnum::OPTION5("733T H@X0R !N D37T@3D!");
    ExampleTestPropertyProxy::TestEnum ExampleTestPropertyProxy::TestEnum::OPTION6("Mind what you have learned, save you it can");

    ///////////////////////////////////////////////////////////////////////////////
    ExampleTestPropertyProxy::ExampleTestPropertyProxy()
        : myInt(0), myReadOnlyInt(5), myFloat(0.0), myDouble(0.0), myLong(0), myBool(0), 
        myString(""), myEnum(&TestEnum::OPTION1) 
    {
        //static int count = 0;
        //std::ostringstream ss;

        myEnum = &TestEnum::OPTION1;

        //ss << "ExampleObject" << count++;
        //getActor()->SetName(ss.str());
        //loadFile("data/blackhawk.ive");

        SetClassName("dtCore::ExampleTestPropertyProxy");
    }

    void ExampleTestPropertyProxy::BuildPropertyMap() {
        DeltaObjectActorProxy::BuildPropertyMap();

        Object *obj = dynamic_cast<Object*>(GetActor());
        if(obj == NULL)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Object");

        AddProperty(new BooleanActorProperty("Test_Boolean", "Test Boolean",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTestBool),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getTestBool),
            "Holds a test Boolean property", GROUPNAME));

        AddProperty(new IntActorProperty("Test_Int", "Test Int",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTestInt),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getTestInt),
            "Holds a test Int property", GROUPNAME));

        dtDAL::IntActorProperty *i = new IntActorProperty("Test_Read_Only_Int", "Test_Read_Only_Int",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setReadOnlyTestInt),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getReadOnlyTestInt),
            "Holds a test Read Only Int property", GROUPNAME);
        i->SetReadOnly(true);

        AddProperty(i);

        AddProperty(new LongActorProperty("Test_Long", "Test Long",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTestLong),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getTestLong),
            "Holds a test Long property", GROUPNAME));

        AddProperty(new FloatActorProperty("Test_Float", "Test Float",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTestFloat),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getTestFloat),
            "Holds a test Float property", GROUPNAME));

        AddProperty(new DoubleActorProperty("Test_Double", "Test Double",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTestDouble),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getTestDouble),
            "Holds a test Double property", GROUPNAME));

        AddProperty(new Vec3ActorProperty("Test_Vec3", "Test Vector3",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTestVec3),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getTestVec3),
            "Holds a test Vector3 Property", GROUPNAME));

        AddProperty(new Vec2ActorProperty("Test_Vec2", "Test Vector2",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTestVec2),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getTestVec2),
            "Holds a test Vector2 Property", GROUPNAME));

        AddProperty(new Vec4ActorProperty("Test_Vec4", "Test Vector4",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTestVec4),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getTestVec4),
            "Holds a test Vector4 Property", GROUPNAME));

        AddProperty(new Vec3fActorProperty("Test_Vec3f", "Test Vector3f",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTestVec3f),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getTestVec3f),
            "Holds a test Vector3f Property", GROUPNAME));

        AddProperty(new Vec2fActorProperty("Test_Vec2f", "Test Vector2f",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTestVec2f),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getTestVec2f),
            "Holds a test Vector2f Property", GROUPNAME));

        AddProperty(new Vec4fActorProperty("Test_Vec4f", "Test Vector4f",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTestVec4f),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getTestVec4f),
            "Holds a test Vector4f Property", GROUPNAME));

        AddProperty(new Vec3dActorProperty("Test_Vec3d", "Test Vector3d",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTestVec3d),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getTestVec3d),
            "Holds a test Vector3d Property", GROUPNAME));

        AddProperty(new Vec2dActorProperty("Test_Vec2d", "Test Vector2d",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTestVec2d),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getTestVec2d),
            "Holds a test Vector2d Property", GROUPNAME));

        AddProperty(new Vec4dActorProperty("Test_Vec4d", "Test Vector4d",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTestVec4d),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getTestVec4d),
            "Holds a test Vector4d Property", GROUPNAME));

        AddProperty(new StringActorProperty("Test_String", "Test String",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTestString),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getTestString),
            "Holds a test String property (unlimited length)", GROUPNAME));

        StringActorProperty *stringProp = new StringActorProperty("Test_String2", "Test String (max 10)",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTestStringWithLength),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getTestStringWithLength),
            "Holds a test String property with a max length of 10", GROUPNAME);
        stringProp->SetMaxLength(10);
        AddProperty(stringProp);

        AddProperty(new ColorRgbaActorProperty("Test_Color", "Test Color",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTestColor),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getTestColor),
            "Holds a test Color property", GROUPNAME));

        AddProperty(new EnumActorProperty<TestEnum>("Test_Enum", "Test Enum",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTestEnum),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::getTestEnum),
            "Holds a test Enum property", GROUPNAME));

        AddProperty(new ResourceActorProperty(*this, DataType::SOUND, "Test_Sound_Resource", "Test Sound",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setSoundResourceName),
            "An example sound resource property", GROUPNAME));

        AddProperty(new ResourceActorProperty(*this, DataType::TEXTURE, "Test_Texture_Resource", "Texture",
            MakeFunctor(*this, &ExampleTestPropertyProxy::setTextureResourceName),
            "An example texture resource property", GROUPNAME));

        AddProperty(new ActorActorProperty(*this, "Test_Actor", "Test Actor",
            MakeFunctor(*this, &ExampleTestPropertyProxy::SetTestActor),
            MakeFunctorRet(*this, &ExampleTestPropertyProxy::GetTestActor), 
            "dtCore::Transformable",
            "An example linked actor property", GROUPNAME));
    }

