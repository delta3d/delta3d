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
#ifndef __SkyBox__h
#define __SkyBox__h

#include <dtCore/deltadrawable.h>
#include <osg/Texture2D>
#include <osg/Transform>

namespace osg 
{
    class Geode;
}

namespace dtActors 
{

    /**
     * The skybox class encalsulates a simple environmental affect which is
     * useful in some cases to give the illusion of a vast world filled with
     * trees, mountains and other scenery of in the distance beyond the players
     * reach.  To acheive a good affect, skybox textures should be relatively large.
     */
    class SkyBox : public dtCore::DeltaDrawable 
    {
    private:

        /**
         * This class transforms the skybox such that it is rotates with the camera and
         * is always centered at the camera's location.
         */
        class SkyBoxEyePointTransform : public osg::Transform 
        {
        public:
            /**
             * Get the transformation matrix which moves from local coords to world coords.
             */
            virtual bool computeLocalToWorldMatrix(osg::Matrix &matrix,osg::NodeVisitor *nv) const;

            /**
             * Get the transformation matrix which moves from world coords to local coords.
             */
            virtual bool computeWorldToLocalMatrix(osg::Matrix &matrix,osg::NodeVisitor *nv) const;
        };
        
    public:
    
        /**
         * A skybox is a cube with six sides.  Use this enumeration to assign textures
         * to each side of the cube.
         */
        enum SkyBoxSide 
        {
            SKYBOX_FRONT = 0,
            SKYBOX_RIGHT = 1,
            SKYBOX_BACK = 2,
            SKYBOX_LEFT = 3,
            SKYBOX_TOP = 4,
            SKYBOX_BOTTOM = 5,
        };

        /**
         * Constructs the skybox.  Basically creates the geometry and texture states
         * needed to render a skybox.
         * @param name The name of the skybox drawable.
         */
        SkyBox(const std::string &name="SkyBox");
        
        /**
         * Sets the texture for a specfied side of the skybox.
         * @param side The side with which to set the texture.
         * @param fileName The file name of the image to use as the texture.
         *  If the fileName is empty, the texture for this side will be removed and
         *  no texture will be assigned.
         */
        void SetTexture(SkyBoxSide side, const std::string &fileName="");

    protected:
        virtual ~SkyBox() { }
        
        /**
         * Helper method to create the skybox geometry.
         * @return A geometry node representing the skybox.
         */
        osg::Geode* CreateSkyBoxGeom();

    private:
        ///Handles to the six textures used in rendering the skybox.
        osg::ref_ptr<osg::Texture2D> mTextureList[6];
    };
}

#endif
