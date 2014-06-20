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
* Matthew W. Campbell
*/
#include "dtTerrain/heightfield.h"

#include <iostream>
#include <sstream>
#include <cstring>
#include <climits>

#include <osgDB/WriteFile>

namespace dtTerrain
{
   //////////////////////////////////////////////////////////////////////////
   HeightField::HeightField()
   {
      mNumColumns = mNumRows = 0;
   }
   
   //////////////////////////////////////////////////////////////////////////
   HeightField::HeightField(unsigned int numCols, unsigned int numRows)
   {
      Allocate(numCols,numRows);
   }
   
   //////////////////////////////////////////////////////////////////////////
   HeightField::~HeightField()
   {
      
   }
   
   //////////////////////////////////////////////////////////////////////////
   void HeightField::Allocate(unsigned int numCols, unsigned int numRows)
   {
      if (numCols == 0 || numRows == 0) 
         throw dtTerrain::HeightFieldOutOfBoundsException("Cannot allocate heightfield. "
            "Width and height must be greater than zero.", __FILE__, __LINE__);
      
      if (mNumColumns != numCols || mNumRows != numRows)
      {   
         mNumColumns = numCols;
         mNumRows = numRows;
         mData.resize(mNumColumns*mNumRows);
      }
   }
   
   //////////////////////////////////////////////////////////////////////////
   short HeightField::GetHeight(unsigned int c, unsigned int r) const
   {
      if (mData.capacity() == 0)
         throw dtTerrain::HeightFieldInvalidException(
         "Height field data is null.", __FILE__, __LINE__);
         
      if (c >= mNumColumns)
         c = mNumColumns-1;
      if (r >= mNumRows)
         r = mNumRows-1;
         
      /*if (c >= mNumColumns || r >= mNumRows)
      {
         std::ostringstream errorString;
         errorString << "Cannot retrieve height value. The given c,r (" << c << ","
            << r << ")  is greater that the heightfield dimensions (" << mNumColumns << ","
            << mNumRows << ").";
            
         throw dtTerrain::HeightFieldOutOfBoundsException(errorString.str());
      }*/
      
      return mData[c+(r*mNumColumns)];      
   }
   
   //////////////////////////////////////////////////////////////////////////
   void HeightField::SetHeight(unsigned int c, unsigned int r, short newHeight)
   {
      if (mData.capacity() == 0)
         throw dtTerrain::HeightFieldInvalidException(
         "Height field data is null.", __FILE__, __LINE__);
         
      if (c >= mNumColumns || r >= mNumRows)
      {
         std::ostringstream errorString;
         errorString << "Cannot set height value. The given c,r (" << c << ","
            << r << ")  is greater that the heightfield dimensions (" << mNumColumns << ","
            << mNumRows << ").";
            
         throw dtTerrain::HeightFieldOutOfBoundsException(errorString.str(), __FILE__, __LINE__);
      }
      
      mData[c+(r*mNumColumns)] = newHeight;
   }

   //////////////////////////////////////////////////////////////////////////   
   osg::Image *HeightField::ConvertToImage() const
   {
      if (mData.capacity() == 0)
      {
         LOG_ERROR("Cannot convert heightfield to an image.  The heightfield "
            "has NULL data.");
         return NULL;
      }
      
      osg::Image *newImage = new osg::Image();
      newImage->allocateImage(mNumColumns,mNumRows,1,GL_LUMINANCE,GL_UNSIGNED_SHORT);
      short *data = (short *)newImage->data();
      for (unsigned int i=0; i<mNumRows; i++)
         for (unsigned int j=0; j<mNumColumns; j++)
            *(data++) = mData[j+(i*mNumColumns)] + osg::absolute(SHRT_MIN);
   
      return newImage;
   }
   
   //////////////////////////////////////////////////////////////////////////   
   void HeightField::ConvertFromImage(const osg::Image &image)
   {
      Allocate(image.s(),image.t());
      //TODO IMPLEMENT THIS!     
   }
   
   //////////////////////////////////////////////////////////////////////////   
   void HeightField::ConvertFromRaw(unsigned int numColumns, unsigned int numRows,
            short *heightData)
   {
      if (heightData == NULL || numColumns == 0 || numRows == 0)
         return;
         
      Allocate(numColumns,numRows);
      memcpy(&mData[0],heightData,sizeof(short)*numColumns*numRows);
   }
  
   //////////////////////////////////////////////////////////////////////////
   float HeightField::GetInterpolatedHeight(float x, float y) const
   {
      int fx = (int)floorf(x), cx = (int)ceilf(x);
      int fy = (int)floorf(y), cy = (int)ceilf(y);

      float v1 = GetHeight(fx,fy);
      float v2 = GetHeight(cx,fy);
      float v3 = GetHeight(fx,cy);
      float v4 = GetHeight(cx,cy);
      float v12 = v1 + (v2-v1)*(x-fx);
      float v34 = v3 + (v4-v3)*(x-fx);

      return v12 + (v34-v12)*(y-fy);
   }

   ////////////////////////////////////////////////////////////////////////////////
   HeightFieldOutOfBoundsException::HeightFieldOutOfBoundsException(const std::string& message, const std::string& filename, unsigned int linenum)
      :dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   HeightFieldInvalidException::HeightFieldInvalidException(const std::string& message, const std::string& filename, unsigned int linenum)
      :dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   HeightFieldInvalidImageException::HeightFieldInvalidImageException(const std::string& message, const std::string& filename, unsigned int linenum)
      :dtUtil::Exception(message, filename, linenum)
   {
   }
}
