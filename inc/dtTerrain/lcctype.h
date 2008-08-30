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
* Teague Coonan
*/
#ifndef _LCCTYPE_H
#define _LCCTYPE_H

#include <map>
#include <string>

#include <osg/Node>

#include <dtCore/refptr.h>
#include <dtTerrain/terrain_export.h>

namespace dtTerrain
{
   
   class DT_TERRAIN_EXPORT LCCType
   {
      public:
      
         /**
          * Describes an LCC's geometric model info
          */
         struct LCCModel
         {
            LCCModel() : name(""), scale(1.0f), sceneNode(NULL) { }
            
            std::string name;
            float scale;
            dtCore::RefPtr<osg::Node> sceneNode;
         };

         /**
          * Constructor
          */
         LCCType(unsigned int index, const std::string &name);
   
         /**
          * Destructor
          */
         ~LCCType() { }
   
         /**
          * This returns the index of the lcc type
          * @return index of the lcctype as defined lcc configuration data
          */
         int GetIndex() const { return mIndex; }
   
         /**
          * Sets the RGB color value of this type.  The color value maps the the 
          * color defined by the land classification cover image map.
          */
         void SetRGB(unsigned char red, unsigned char green, unsigned char blue)
         {
            mRGB[0] = red;
            mRGB[1] = green;
            mRGB[2] = blue;
         }
         
         /**
          * Gets the RGB color identifier for this type.
          */
         void GetRGB(unsigned char &red, unsigned char &green, unsigned char &blue)
         {
            red = mRGB[0];
            green = mRGB[1];
            blue = mRGB[2];
         }
         
         /**
          * Gets the LCC name of this type.
          * @return the name of the LCC type, such as snow, water, evergreen, snow.
         */
         const std::string &GetLCCName() const { return mLCCName; }
   
         /**
         * @param minimum slope
         * @param maximum slope
         * @param slope sharpness
         */
         void SetSlope(float min, float max, float sharpness)
         {
            mMinSlope = min;
            mMaxSlope = max;
            mSlopeSharpness = sharpness;
         }
   
         /**
          * @return maximum slope
          */
         float GetMaxSlope() const { return mMaxSlope; }
   
         /**
          * @return minimum slope
          */
         float GetMinSlope() const { return mMinSlope; }
   
         /**
          * @param minimum elevation
          * @param maximum elevation
          * @param elevation sharpness
          */
         void SetElevation(float min, float max, float sharpness)
         {
            mMinElevation = min;
            mMaxElevation = max;
            mElevationSharpness = sharpness;
         }
   
         /**
         * @return maximum elevation
         */
         float GetMaxElevation() const { return mMaxElevation; }
   
         /**
          * @return minimum elevation
          */
         float GetMinElevation() const { return mMinElevation; }
         
         /**
          * @param minimum elevation
          * @param maximum elevation
          * @param relative elevation sharpness
          */
         void SetRelativeElevation(float min, float max, float sharpness)
         {
            mMinRelativeElevation = min;
            mMaxRelativeElevation = max;
            mRelativeElevationSharpness = sharpness;
         }
   
         /**
          * @param aspect
          */
         void SetAspect(int aspect) { mAspect = aspect; }
   
         /**
          * @return aspect
          */
         float GetAspect() const { return mAspect; }  
   
         /**
          * This adds a model to the list of available models for this vegetation type.
          * When placing vegetation of this LCC type, a model is randomly chosen from
          * the model list.
          * @param name File name of the model.
          * @param scale Uniform scaling factor for this model.
          * @note The model is loaded when vegetation is placed.
          */
         void AddModel(const std::string &name, float scale = 1.0);
         
         /**
          * Removes the LCC model from this LCC type's list of models.
          * @param The name of the model.  If this name is not found in the
          *    list of models, this method does nothing.
          */
         void RemoveModel(const std::string &name);
   
         /**
          * Retrives the model at the given index.
          * @param index An index into the list of models.
          * @return If index is valid, a pointer to the model is returned, 
          *    else this method will return NULL.
          */
         LCCModel *GetModel(unsigned int index);
         
         /**
          * Looks up an LCC model on this type by name.
          * @param name The name of the LCC model to retrieve.
          * @return A valid model or NULL if the model was not found.
          */
         LCCModel *GetModel(const std::string &name);
           
         /**
          * Gets the number of models registered under this LCC type.
          * @return The number of models.
          */  
         unsigned int GetNumberOfModels() const { return mModels.size(); }       
         
         /**
          * Removes all the LCC models from this LCC type.
          */
         void ClearModels() { mModels.clear(); }     

      private:
      
         std::string mLCCName;
         unsigned char mRGB[3];
         std::map<std::string,LCCModel> mModels;
         int mAspect;
         
         //The LCC type index.
         unsigned int mIndex;
   
         //Vegetation slope information.
         float mMinSlope, mMaxSlope, mSlopeSharpness;
   
         //Elevation information for this type.
         float mMinElevation, mMaxElevation, mElevationSharpness;
         
         //Relative elevation information for this type.
         float mMinRelativeElevation, mMaxRelativeElevation;
         float mRelativeElevationSharpness;        
   };
}
#endif
