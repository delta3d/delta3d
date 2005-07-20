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
* @author David Guthrie
*/

#include "dtDAL/enginepropertytypes.h"
#include "dtDAL/project.h"
#include "dtDAL/exception.h"
#include "dtDAL/log.h"
#include "dtDAL/map.h"
#include "dtDAL/mapxml.h"
#include "dtDAL/stringtokenizer.h"


namespace dtDAL
{

    ////////////////////////////////////////////////////////////////////////////
    bool ActorActorProperty::SetStringValue(const std::string& value)
    {
        return false;
    }

    ////////////////////////////////////////////////////////////////////////////
    const std::string ActorActorProperty::GetStringValue() const
    {
        return GetValue()->GetUniqueId().ToString();
    }


    ////////////////////////////////////////////////////////////////////////////
    void ResourceActorProperty::SetValue(ResourceDescriptor *value)
    {
        mProxy->SetResource(GetName(), value);
        if (value == NULL)
            SetPropFunctor("");
        else
        {
            try
            {
                std::string path = Project::GetInstance().GetResourcePath(*value);
                if (Log::GetInstance("EnginePropertyTypes.h").IsLevelEnabled(Log::LOG_DEBUG))
                    Log::GetInstance("EnginePropertyTypes.h").LogMessage(Log::LOG_DEBUG,
                        __FUNCTION__, __LINE__,
                        "Path to resource is: %s",
                        path.c_str());
                SetPropFunctor(path);
            }
            catch(const Exception& ex)
            {
                mProxy->SetResource(GetName(), NULL);
                SetPropFunctor("");
                Log::GetInstance("EnginePropertyTypes.h").LogMessage(Log::LOG_WARNING,
                    __FUNCTION__, __LINE__, "Resource %s not found.  Setting property %s to NULL. Error Message %s.",
                    value->GetResourceIdentifier().c_str(), GetName().c_str(), ex.What().c_str());
            }
        }
    }



    class IsSlash : public std::unary_function<char, bool> {
    public:
        bool operator()(char c) const;
    };

    inline bool IsSlash::operator()(char c) const {
        // isspace<char> returns true if c is a white-space character
        // (0x09-0x0D or 0x20)
        return c == '/';
    }

    ////////////////////////////////////////////////////////////////////////////
    ResourceDescriptor* ResourceActorProperty::GetValue() const
    {
        return mProxy->GetResource(GetName());
    }

    ////////////////////////////////////////////////////////////////////////////
    bool ResourceActorProperty::SetStringValue(const std::string& value)
    {
        bool result = true;
        if (value.empty() || value == "NULL")
        {
            SetValue(NULL);
        }
        else
        {

            std::vector<std::string> tokens;
            StringTokenizer<IsSlash> stok;

            stok.tokenize(tokens, value);

            if (tokens.size() == 2)
            {
                std::string displayName(tokens[0]);
                std::string identifier(tokens[1]);

                trim(identifier);
                trim(displayName);

                ResourceDescriptor descriptor(displayName, identifier);
                SetValue(&descriptor);
            }
            else
                result = false;
        }

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////
    const std::string ResourceActorProperty::GetStringValue() const
    {
        ResourceDescriptor* r = GetValue();
        if (r == NULL)
            return "";
        else
            return r->GetDisplayName() + "/" + r->GetResourceIdentifier();
    }

    ////////////////////////////////////////////////////////////////////////////
    bool IntActorProperty::SetStringValue(const std::string& value)
    {
        std::istringstream stream;
        stream.str(value);
        int i;
        stream >> i;
        SetValue(i);
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////
    const std::string IntActorProperty::GetStringValue() const
    {
        std::ostringstream stream;
        stream << GetValue();
        return stream.str();
    }

    ////////////////////////////////////////////////////////////////////////////
    bool BooleanActorProperty::SetStringValue(const std::string& value)
    {
        std::istringstream stream;
        stream.str(value);
        int i;
        stream >> i;
        SetValue(i);
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////
    const std::string BooleanActorProperty::GetStringValue() const
    {
         const char* result = GetValue() ? "true" : "false";
         return result;
    }

    ////////////////////////////////////////////////////////////////////////////
    bool FloatActorProperty::SetStringValue(const std::string& value)
    {
        std::istringstream stream;
        stream.str(value);
        float i;
        stream >> i;
        SetValue(i);
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////
    const std::string FloatActorProperty::GetStringValue() const
    {
        std::ostringstream stream;
        stream << GetValue();
        return stream.str();
    }

    ////////////////////////////////////////////////////////////////////////////
    bool DoubleActorProperty::SetStringValue(const std::string& value)
    {
        std::istringstream stream;
        stream.str(value);
        double i;
        stream >> i;
        SetValue(i);
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////
    const std::string DoubleActorProperty::GetStringValue() const
    {
        std::ostringstream stream;
        stream << GetValue();
        return stream.str();
    }

    ////////////////////////////////////////////////////////////////////////////
    bool LongActorProperty::SetStringValue(const std::string& value)
    {
        std::istringstream stream;
        stream.str(value);
        long i;
        stream >> i;
        SetValue(i);
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////
    const std::string LongActorProperty::GetStringValue() const
    {
        std::ostringstream stream;
        stream << GetValue();
        return stream.str();
    }

    ////////////////////////////////////////////////////////////////////////////
    bool Vec2ActorProperty::SetStringValue(const std::string& value)
    {
        osg::Vec2 setValue;

        bool result = true;
        if (value.empty() || value == "NULL")
        {
            setValue.set(0.0, 0.0);
            SetValue(setValue);
        }
        else if (sscanf(value.c_str(), "%f %f", &setValue.x(), &setValue.y()) == 2
            || sscanf(value.c_str(), "%f, %f", &setValue.x(), &setValue.y()) == 2)
        {
            SetValue(setValue);
        }
        else
        {
            result = false;
        }

        return result;

    }

    ////////////////////////////////////////////////////////////////////////////
    const std::string Vec2ActorProperty::GetStringValue() const
    {
        std::ostringstream stream;
        osg::Vec2 vec2 = GetValue();
        stream << vec2[0] << " " << vec2[1];
        return stream.str();
    }

    ////////////////////////////////////////////////////////////////////////////
    bool Vec3ActorProperty::SetStringValue(const std::string& value)
    {
        osg::Vec3 setValue;

        bool result = true;
        if (value.empty() || value == "NULL")
        {
            setValue.set(0.0, 0.0, 0.0);
            SetValue(setValue);
        }
        else if (sscanf(value.c_str(), "%f %f %f", &setValue.x(), &setValue.y(), &setValue.z()) == 3
            || sscanf(value.c_str(), "%f, %f, %f", &setValue.x(), &setValue.y(), &setValue.z()) == 3)
        {
            SetValue(setValue);
        }
        else
        {
            result = false;
        }

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////
    const std::string Vec3ActorProperty::GetStringValue() const
    {
        std::ostringstream stream;
        osg::Vec3 vec3 = GetValue();
        stream << vec3[0] << " " << vec3[1] << " " << vec3[2];
        return stream.str();
    }

    ////////////////////////////////////////////////////////////////////////////
    bool Vec4ActorProperty::SetStringValue(const std::string& value)
    {
        osg::Vec4 setValue;

        bool result = true;
        if (value.empty() || value == "NULL")
        {
            setValue.set(0.0, 0.0, 0.0, 0.0);
            SetValue(setValue);
        }
        else if (sscanf(value.c_str(), "%f %f %f %f", &setValue.x(), &setValue.y(), &setValue.z(), &setValue.w()) == 4
            || sscanf(value.c_str(), "%f, %f, %f, %f", &setValue.x(), &setValue.y(), &setValue.z(), &setValue.w()) == 4)
        {
            SetValue(setValue);
        }
        else
        {
            result = false;
        }

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////
    const std::string Vec4ActorProperty::GetStringValue() const
    {
        std::ostringstream stream;
        osg::Vec4 vec4 = GetValue();
        stream << vec4[0] << " " << vec4[1] << " " << vec4[2] << " " << vec4[3];
        return stream.str();
    }

    ////////////////////////////////////////////////////////////////////////////
    bool ColorRgbaActorProperty::SetStringValue(const std::string& value)
    {
        osg::Vec4 setValue;

        bool result = true;
        if (value.empty() || value == "NULL")
        {
            setValue.set(0.0, 0.0, 0.0, 0.0);
            SetValue(setValue);
        }
        else if (sscanf(value.c_str(), "%f %f %f %f", &setValue.x(), &setValue.y(), &setValue.z(), &setValue.w()) == 4
            || sscanf(value.c_str(), "%f, %f, %f, %f", &setValue.x(), &setValue.y(), &setValue.z(), &setValue.w()) == 4)
        {
            SetValue(setValue);
        }
        else
        {
            result = false;
        }

        return result;
    }

    ////////////////////////////////////////////////////////////////////////////
    const std::string ColorRgbaActorProperty::GetStringValue() const
    {
        std::ostringstream stream;
        osg::Vec4 vec4 = GetValue();
        stream << vec4[0] << " " << vec4[1] << " " << vec4[2] << " " << vec4[3];
        return stream.str();
    }
}
