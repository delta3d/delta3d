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
* @author Teague Coonan
*/

#ifndef _LCCTYPE_H
#define _LCCTYPE_H

#include <osg/Vec3>
#include "dtTerrain/terrain_export.h"
#include "dtTerrain/imageutils.h"


namespace dtTerrain
{
   class DT_TERRAIN_EXPORT LCCType
   {
   public:

      /**
      * Constructor
      */
      LCCType(unsigned int index, std::string name);

      /**
      * Destructor
      */
      virtual ~LCCType();

      /**
      * Describes an LCC's geometric model info
      */
      struct LCCModel
      {
         std::string name;
         float scale;
         dtCore::RefPtr<osg::Group> vegetationObject;
      };

      /**
      * This returns the index of the lcc type
      * @return index of the lcctype as defined lcc configuration data
      */
      int GetIndex()
      {
         return this->index;
      }

      /**
      * @param red color value between 0 and 255
      * @param green color value between 0 and 255
      * @param blue color value between 0 and 255
      */
      void SetRGB(unsigned int r,unsigned int g, unsigned int b)
      {
         this->rgb[0] = r;
         this->rgb[1] = g;
         this->rgb[2] = b;
      }
      
      /**
      * @return pointer to the rgb array
      */
      int *GetRGB()
      {
         return &this->rgb[3];
      }
      
      /**
      * This returns the current name of the lcc type
      * @return the name of the LCC type, such as snow, water, evergreen, snow.
      */
      std::string GetLCCName()
      {
         return this->LCCName;
      }

      /**
      * @param minimum slope
      * @param maximum slope
      * @param slope sharpness
      */
      void SetSlope(float min, float max, float sharpness)
      {
         this->slopeMin = min;
         this->slopeMax = max;
         this->slopeSharpness = sharpness;
      }

      /**
      * @return maximum slope
      */
      float GetMaxSlope()
      {
         return this->slopeMax;
      }

      /**
      * @return minimum slope
      */
      float GetMinSlope()
      {
         return this->slopeMin;
      }

      /**
      * @param minimum elevation
      * @param maximum elevation
      * @param elevation sharpness
      */
      void SetElevation(float min, float max, float sharpness)
      {
         this->elevationMin = min;
         this->elevationMax = max;
         this->elevationSharpness = sharpness;
      }

      /**
      * @return maximum elevation
      */
      float GetMaxElevation()
      {
         return this->elevationMax;
      }

      /**
      * @return minimum elevation
      */
      float GetMinElevation()
      {
         return this->elevationMin;
      }
      
      /**
      * @param minimum elevation
      * @param maximum elevation
      * @param relative elevation sharpness
      */
      void SetRelativeElevation(float min, float max, float sharpness)
      {
         this->relativeElevationMin = min;
         this->relativeElevationMax = max;
         this->relativeElevationSharpness = sharpness;
      }

      /**
      * @param aspect
      */
      void SetAspect(int aspect)
      {
         this->aspect = aspect;
      }

      /**
      * @return aspect
      */
      float GetAspect()
      {
         return this->aspect;
      }  

      /**
      * This adds a model to the list of available models for this vegetation type
      * @param name of the model
      * @param scale of the model
      * @param osg group node of the object
      */
      void AddModel(std::string name, float scale = 1.0);

      /**
      * Retrieves the models filename at index 'x'
      * @param index of the model
      * @return the name of a model at 'index'
      */
      std::string GetModelName(unsigned int index = 0)
      {
         return this->models.at(index).name;
      }

      int GetModelNum()
      {
         return this->models.size();
      }
      /**
      * Grabs the osg group node at index 'x'
      * @param index of the model
      * @return osg group node assigned to the model

      */ 
      dtCore::RefPtr<osg::Group> GetVegetationObject(unsigned int index = 0)
      {
         return this->models.at(index).vegetationObject;
      }
      
      /**
      * Sets the vegetation object group node at index 'x'
      * @param index of the model
      * @param osg group node of the vegetation object
      */
      void SetVegetationObject(dtCore::RefPtr<osg::Group> vegetationObject, unsigned int index = 0)
      {
         this->models.at(index).vegetationObject = vegetationObject;
      }

      /**
      * Gets the scale of the model at index 'x'
      * @param index of the model
      * @return scale of the model 
      */
      int GetModelScale(unsigned int index = 0)
      {
         return (int)this->models.at(index).scale;
      }

      std::string LCCName;
      int rgb[3];

   private:
      
      // Basic LCC Type information
      unsigned int index;
      unsigned int mModelNum;

      // Vegetation slope information
      float slopeMin;
      float slopeMax;
      float slopeSharpness;

      // elevation information
      float elevationMin;
      float elevationMax;
      float elevationSharpness;

      // relative elevation information
      float relativeElevationMin;
      float relativeElevationMax;
      float relativeElevationSharpness;

      int aspect;

      // Vectors of all available models assigned to this vegetation type
      std::vector<LCCModel> models;
   };
}
#endif
