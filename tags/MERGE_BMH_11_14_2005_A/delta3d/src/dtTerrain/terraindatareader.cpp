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
#include <dtCore/scene.h>

#include <sstream>
#include <climits>
#include <iomanip>

#include "dtTerrain/terraindatarenderer.h"
#include "dtTerrain/terraindatareader.h"

namespace dtTerrain
{
   //////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(TerrainDataReaderException);  
   TerrainDataReaderException 
      TerrainDataReaderException::DATA_RESOURCE_NOT_FOUND("DATA_RESOURCE_NOT_FOUND");
   TerrainDataReaderException
      TerrainDataReaderException::COULD_NOT_READ_DATA("COULD_NOT_READ_DATA");
   TerrainDataReaderException 
      TerrainDataReaderException::READER_PLUGIN_NOT_FOUND("READER_PLUGIN_NOT_FOUND");
      
   //////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(HeightFieldResizePolicy);
   const HeightFieldResizePolicy HeightFieldResizePolicy::NONE("NONE");         
   const HeightFieldResizePolicy 
      HeightFieldResizePolicy::NEAREST_POWER_OF_TWO("NEAREST_POWER_OF_TWO");
   const HeightFieldResizePolicy 
      HeightFieldResizePolicy::NEAREST_POWER_OF_TWO_PLUS_ONE("NEAREST_POWER_OF_TWO_PLUS_ONE");
   
   //////////////////////////////////////////////////////////////////////////
   HeightField *TerrainDataReader::ConvertHeightField(osg::HeightField *hf)
   {
      dtCore::RefPtr<osg::HeightField> newHF = ScaleOSGHeightField(hf);      
      HeightField *result = new HeightField();
    
      result->Allocate(newHF->getNumColumns(),newHF->getNumRows());
      for (unsigned int i=0; i<newHF->getNumRows(); i++)
      {      
         for (unsigned int j=0; j<newHF->getNumColumns(); j++)
         {
            float value = osg::clampTo(newHF->getHeight(j,i),(float)SHRT_MIN,
               (float)SHRT_MAX);
            result->SetHeight(j,i,(short)value);
         }
      }
      
      return result;
   }   
   
   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::HeightField> TerrainDataReader::ScaleOSGHeightField(
      osg::HeightField *hf)
   {
      unsigned int width = hf->getNumColumns();
      unsigned int height = hf->getNumRows();
      unsigned int dims;
      int correctSize;

      //We need to make sure that the dimensions of the heightmap correspond
      //to the current resize policy.
      if (*mResizePolicy == HeightFieldResizePolicy::NONE)
         return hf;
         
      dims = osg::maximum(width,height);
      if (*mResizePolicy == HeightFieldResizePolicy::NEAREST_POWER_OF_TWO)
         correctSize = osg::Image::computeNearestPowerOfTwo(dims);
      else if (*mResizePolicy == HeightFieldResizePolicy::NEAREST_POWER_OF_TWO_PLUS_ONE)
         correctSize = osg::Image::computeNearestPowerOfTwo(dims) + 1;
      
      //Make sure the width and height are equal and that the heightfield
      //meets the current resize policy.
      if (width == (unsigned)correctSize && width == height)
         return hf;
      
      //Log some good information.
      std::ostringstream ss;
      ss << "Resizing heightfield data from: (" << width << "," << height <<
         ") to: (" << correctSize << "," << correctSize << ").";
      LOG_DEBUG(ss.str());
      
      osg::HeightField *newHF = new osg::HeightField();
      double x,xStep;
      double yStep;
      double y = 0.0;

      newHF->allocate(correctSize,correctSize);
      xStep = (width-1.0) / (correctSize-1.0);
      yStep = (height-1.0) / (correctSize-1.0);
      for (int i=0; i<correctSize; i++)
      {
         x = 0.0;
         for (int j=0; j<correctSize; j++)
         {
            float newValue = GetInterpolatedHeight(hf,x,y);
            newHF->setHeight(j,i,newValue);
            x += xStep;
         }
         
         y += yStep;
      }     
      
      return newHF;     
   }
   
   //////////////////////////////////////////////////////////////////////////   
   float TerrainDataReader::GetInterpolatedHeight(const osg::HeightField *hf, 
      float x, float y)
   {
      int fx = (int)floor(x), fy = (int)floor(y);
      int cx = (int)ceil(x), cy = (int)ceil(y);
      
      double v1 = hf->getHeight(fx,fy);
      double v2 = hf->getHeight(cx,fy);
      double v3 = hf->getHeight(fx,cy);
      double v4 = hf->getHeight(cx,cy);
      
      double v12 = v1 + (v2-v1)*(x-fx);
      double v34 = v3 + (v4-v3)*(x-fx);
      
      return v12 + (v34-v12)*(y-fy);
   }
}
