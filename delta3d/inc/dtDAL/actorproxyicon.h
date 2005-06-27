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
 * @author Matthew W. Campbell
*/
#ifndef __ActorProxyIcon__h
#define __ActorProxyIcon__h

#include <map>
#include <dtUtil/enumeration.h>
#include <dtCore/export.h>
#include <dtCore/transformable.h>
#include <osg/Referenced>
#include <osg/ref_ptr>
#include <osg/Vec3>
//#include "dtDAL/enumeration.h"

namespace osg
{
    class Geometry;
    class Image;
}

namespace dtDAL
{

    class DT_EXPORT ActorProxyIcon : public osg::Referenced
    {
    public:

        //These are resources used by the editor to display billboards
        //for the actors that need them.
        static const std::string IMAGE_BILLBOARD_GENERIC;
        static const std::string IMAGE_BILLBOARD_CHARACTER;
        static const std::string IMAGE_BILLBOARD_STATICMESH;
        static const std::string IMAGE_BILLBOARD_LIGHT;
        static const std::string IMAGE_BILLBOARD_SOUND;
        static const std::string IMAGE_BILLBOARD_PARTICLESYSTEM;
        static const std::string IMAGE_BILLBOARD_MESHTERRAIN;
        static const std::string IMAGE_BILLBOARD_PLAYERSTART;
        static const std::string IMAGE_BILLBOARD_TRIGGER;

        /**
         * This enumeration enumerates the different types of billboard icons that are
         * supported by default by the level editor.
         */
        class DT_EXPORT IconType : public dtUtil::Enumeration
        {
            DECLARE_ENUM(IconType);
        public:

            static const IconType GENERIC;
            static const IconType CHARACTER;
            static const IconType STATICMESH;
            static const IconType SOUND;
            static const IconType LIGHT;
            static const IconType PARTICLESYSTEM;
            static const IconType MESHTERRAIN;
            static const IconType PLAYERSTART;
            static const IconType TRIGGER;

        protected:
            IconType(const std::string &name) : Enumeration(name)
            {
                AddInstance(this);
            }
        };

        /**
         * Constructs a new actor proxy billboard icon.  This creates the
         * necessary scene geometry and attaches the appropriate image
         * to the icon.
         * @param type Type of icon to create.  By default, IconType::GENERIC is used.
         */
        ActorProxyIcon(const IconType &type = IconType::GENERIC);

        /**
         * Sets the icon type used by this proxy icon.
         * @param type
         */
        void SetIconType(const IconType &type)
        {
            mIconType = &type;
            CreateBillBoard();
        }

        /**
         * Gets the Delta3D drawable for this proxy icon.
         * @return A Delta3D transformable which is the parent of the billboard geometry.
         */
        dtCore::Transformable* GetDrawable() { return mIconNode.get(); }

        /**
         * Gets the Delta3D drawable for this proxy icon.
         * @return A Delta3D transformable which is the parent of the billboard geometry.
         */
        const dtCore::Transformable* GetDrawable() const { return mIconNode.get(); }

    protected:
        virtual ~ActorProxyIcon();

    private:
        ActorProxyIcon &operator=(const ActorProxyIcon &rhs);
        ActorProxyIcon(const ActorProxyIcon &rhs);

        /**
         * Helper method which builds the billboard geometry and assigns a texture
         * and appropriate state to it.
         */
        void CreateBillBoard();

        /**
         * Checks the type of this billboard and loads the appropriate image.
         * @return An image mapping to the billboard type on this billboard icon.
         */
        osg::Image* GetBillBoardImage();

        /**
         * Creates a quad which is used for the billboard.
         * @param corner Upper left corner of the quad.
         * @param width Width of the quad.
         * @param height Height of the quad.
         * @return A geometry node holding the quad's geometry.
         */
        osg::Geometry* CreateGeom(const osg::Vec3 &corner, const osg::Vec3 &width,
            const osg::Vec3 &height);

        ///Type of the billboard icon.
        const IconType *mIconType;

        ///The underlying Delta3D drawable object.
        osg::ref_ptr<dtCore::Transformable> mIconNode;

        ///Path to billboard icon files
        static const std::string mPrefix;
    };
}

#endif
