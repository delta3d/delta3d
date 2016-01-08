/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2016 Caper Holdings, LLC
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
 */
#include <dtVoxel/readovdbthreadpooltask.h>
#include <dtUtil/fileutils.h>
#include <osgDB/ReaderWriter>

namespace dtVoxel
{
   ///////////////////////////////////////////////////////////////////////////////
   ReadOVDBThreadPoolTask::ReadOVDBThreadPoolTask()
   : mComplete(false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ReadOVDBThreadPoolTask::~ReadOVDBThreadPoolTask()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ReadOVDBThreadPoolTask::operator()()
   {
      if (!mFileToLoad.empty())
      {
         LOGN_DEBUG("voxelactor.cpp", "Loading Grid");
         try
         {
            openvdb::io::File file(mFileToLoad);
            file.open();
            mGrids = file.getGrids();
            file.close();

            LOGN_DEBUG("voxelactor.cpp", "Done Loading Grid");
            mComplete = true;
         }
         catch (const openvdb::IoError& ioe)
         {
            LOG_ERROR("Error Loading Grid");
            mComplete = true;
         }
      }
      else
      {
         mComplete = true;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   openvdb::GridPtrVecPtr ReadOVDBThreadPoolTask::GetLoadedGrids()
   {
      return mGrids;
   }


   ///////////////////////////////////////////////////////////////////////////////
   bool ReadOVDBThreadPoolTask::IsComplete() const
   {
      return mComplete;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ReadOVDBThreadPoolTask::ResetData()
   {
      mGrids = nullptr;
      mComplete = false;
   }

   DT_IMPLEMENT_ACCESSOR(ReadOVDBThreadPoolTask, std::string, FileToLoad);

}
