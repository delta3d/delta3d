/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2015, Caper Holdings, LLC
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
 */

#include <dtVoxel/voxelcell.h>
#include <osgVolume/MultipassTechnique>
#include <osg/Texture2D>
#include <iostream>

namespace dtVoxel
{

   VoxelCell::VoxelCell()
   {
   }

   VoxelCell::~VoxelCell()
   {
   }

   void VoxelCell::Init(osg::Matrix& transform, const osg::Vec3& cellSize, const osg::Vec3i& texture_resolution)
   {
       static int count = 0;

       std::cout << "Creating Voxel Cell " << count++ << std::endl;

       mVolumeTile = new osgVolume::VolumeTile;
       mImage = new osgVolume::ImageLayer();

       AllocateImage(texture_resolution[0], texture_resolution[1], texture_resolution[2]);

       mVolumeTile->setLayer(mImage.get());
       mVolumeTile->setVolumeTechnique(new osgVolume::MultipassTechnique());

       dtCore::RefPtr<osgVolume::Locator> locator = new osgVolume::Locator();
       mImage->setLocator(locator.get());
       mVolumeTile->setLocator(locator.get());

       osg::ref_ptr<osgVolume::AlphaFuncProperty> ap = new
           osgVolume::AlphaFuncProperty(0.1f);
       osg::ref_ptr<osgVolume::TransparencyProperty> tp = new
           osgVolume::TransparencyProperty(0.0f);

       osg::ref_ptr<osgVolume::SampleRatioProperty> sr =
           new osgVolume::SampleRatioProperty(1.0);

       osg::ref_ptr<osgVolume::ExteriorTransparencyFactorProperty> etfp = new
           osgVolume::ExteriorTransparencyFactorProperty(0.99f);

       osg::ref_ptr<osgVolume::CompositeProperty> cp = new
           osgVolume::CompositeProperty;

       cp->addProperty(ap.get());
       cp->addProperty(sr.get());
       cp->addProperty(tp.get());
       cp->addProperty(etfp.get());

       mImage->addProperty(cp.get());
       //mImage->setDefaultValue(osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f));

       osg::Vec3 pos = transform.getTrans();

       locator->setTransformAsExtents(
           pos.x() + (-cellSize.x() / 2.0), pos.y() + (-cellSize.y() / 2.0),
           pos.x() + (cellSize.x() / 2.0), pos.y() + (cellSize.y() / 2.0),
           pos.z() + (-cellSize.z() / 2.0), pos.z() + (cellSize.z() / 2.0));


       //create hull
       //osg::Group* hullRoot = new osg::Group();
       //osg::Box* box = new osg::Box(pos, cellSize.x(), cellSize.y(), cellSize.z());
       //osg::ShapeDrawable* sd = new osg::ShapeDrawable(box);
       //osg::Geode* sdg = new osg::Geode();
       //sdg->addDrawable(sd);
       //hullRoot->addChild(sdg);
       //mVolumeTile->addChild(hullRoot);

   }

   void VoxelCell::AllocateImage(int width, int height, int slices)
   {
       dtCore::RefPtr<osg::Image> image = new osg::Image;

       GLenum pixelFormat = GL_RGBA;  // GL_ALPHA, GL_LUMINANCE, GL_RGB or GL_RGBA
       GLenum dataType = GL_UNSIGNED_BYTE;
       int components = osg::Image::computeNumComponents(pixelFormat);


       // Make sure 0's are written in the image's data segment
       int imageSize = width * height * slices * components;
       unsigned char* dataPtr = new unsigned char[imageSize], *ptr;
       unsigned char charToFill = 0;
       memset(dataPtr, charToFill, imageSize);

       //this one we will increment as we set
       ptr = dataPtr;

       for (int i = 0; i < slices; ++i)
       {
           for (int j = 0; j < height; ++j)
           {
               for (int k = 0; k < width; ++k)
               {
                   *(ptr++) = (unsigned char)255;// voxel;
                   *(ptr++) = (unsigned char)255;// voxel;
                   *(ptr++) = (unsigned char)255;// voxel;
                   *(ptr++) = (unsigned char)255;// voxel;
               }
           }
       }

       image->setImage(width, height, slices, pixelFormat, pixelFormat, dataType,
           dataPtr, osg::Image::USE_NEW_DELETE);

       mImage = new osgVolume::ImageLayer(image.get());
   }

   osgVolume::ImageLayer* VoxelCell::GetImageLayer()
   {
       return mImage.get();
   }

   const osgVolume::ImageLayer* VoxelCell::GetImageLayer() const
   {
       return mImage.get();
   }

   osgVolume::VolumeTile* VoxelCell::GetVolumeTile()
   {
       return mVolumeTile.get();
   }

   const osgVolume::VolumeTile* VoxelCell::GetVolumeTile() const
   {
       return mVolumeTile.get();
   }

   osg::Vec3 VoxelCell::GetOffset() const
   {
       osg::Vec3 result;
       osgVolume::Locator* locator = mVolumeTile->getLocator();
       if (locator != NULL)
       {
           result = locator->getTransform().getTrans();
       }

       return result;
   }


} /* namespace dtVoxel */
