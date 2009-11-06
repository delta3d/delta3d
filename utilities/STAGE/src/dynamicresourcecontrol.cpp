/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Curtiss Murphy
 */
#include <prefix/dtstageprefix-src.h>

#include <dtEditQt/dynamicresourcecontrol.h>
#include <dtEditQt/editordata.h>
#include <dtDAL/datatype.h>
#include <dtDAL/enginepropertytypes.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicResourceControl::DynamicResourceControl()
   : dtQt::DynamicResourceControlBase()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicResourceControl::~DynamicResourceControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   dtDAL::ResourceDescriptor DynamicResourceControl::getCurrentResource()
   {
      dtDAL::ResourceActorProperty& resProperty = GetProperty();
      if (resProperty.GetDataType() == dtDAL::DataType::SOUND)
      {
         return EditorData::GetInstance().getCurrentSoundResource();
      }
      else if (resProperty.GetDataType() == dtDAL::DataType::STATIC_MESH)
      {
         return EditorData::GetInstance().getCurrentMeshResource();
      }
      else if (resProperty.GetDataType() == dtDAL::DataType::TEXTURE)
      {
         return EditorData::GetInstance().getCurrentTextureResource();
      }
      else if (resProperty.GetDataType() == dtDAL::DataType::TERRAIN)
      {
         return EditorData::GetInstance().getCurrentTerrainResource();
      }
      else if (resProperty.GetDataType() == dtDAL::DataType::PARTICLE_SYSTEM)
      {
         return EditorData::GetInstance().getCurrentParticleResource();
      }
      else if (resProperty.GetDataType() == dtDAL::DataType::SKELETAL_MESH)
      {
         return EditorData::GetInstance().getCurrentSkeletalModelResource();
      }
      else if (resProperty.GetDataType() == dtDAL::DataType::PREFAB)
      {
         return EditorData::GetInstance().getCurrentPrefabResource();
      }
      else
      {
         LOG_ERROR("Error setting current resource because DataType [" +
            resProperty.GetDataType().GetName() +
            "] is not supported for property [" + resProperty.GetName() + "].");
         // return something so we don't crash
         return dtDAL::ResourceDescriptor();
      }
   }
} // namespace dtEditQt
