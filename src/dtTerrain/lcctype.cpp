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
#include <dtTerrain/lcctype.h>

namespace dtTerrain
{
   
   //////////////////////////////////////////////////////////////////////////
   LCCType::LCCType(unsigned int index, const std::string &name)
   {
      mLCCName = name;
      mIndex = index;
      mRGB[0] = mRGB[1] = mRGB[2] = 0;
   }

   //////////////////////////////////////////////////////////////////////////
   void LCCType::AddModel(const std::string &name, float scale)
   {
      // create a new model
      LCCModel newModel;
      
      newModel.name = name;
      newModel.scale = scale;
      
      // add it to our list of models for this vegetation type
      mModels.insert(std::make_pair(newModel.name,newModel));
   }
   
   //////////////////////////////////////////////////////////////////////////
   void LCCType::RemoveModel(const std::string &name)
   {
      std::map<std::string,LCCModel>::iterator itor = mModels.find(name);
      if (itor != mModels.end())
         mModels.erase(itor);
   }
      
   //////////////////////////////////////////////////////////////////////////
   LCCType::LCCModel *LCCType::GetModel(const std::string &name)
   {
      std::map<std::string,LCCModel>::iterator itor = mModels.find(name);
      if (itor != mModels.end())
         return &itor->second;
      else
         return NULL;
   }
   
   //////////////////////////////////////////////////////////////////////////
   LCCType::LCCModel *LCCType::GetModel(unsigned int index)
   {
      if (index >= mModels.size())
         return NULL;
         
      std::map<std::string,LCCModel>::iterator itor = mModels.begin();
      unsigned int i = 0;
      while (i < index)
      {
         ++itor;
         ++i;
      }
      
      return &itor->second;
   }  
      
}
