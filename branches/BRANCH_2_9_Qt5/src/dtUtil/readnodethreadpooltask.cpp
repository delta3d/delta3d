/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2015 Caper Holdings, LLC
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
#include <dtUtil/readnodethreadpooltask.h>
#include <dtUtil/fileutils.h>
#include <osgDB/ReaderWriter>

namespace dtUtil
{
   ///////////////////////////////////////////////////////////////////////////////
   ReadNodeThreadPoolTask::ReadNodeThreadPoolTask()
   : mUseFileCaching(true)
   , mComplete(false)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ReadNodeThreadPoolTask::~ReadNodeThreadPoolTask()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ReadNodeThreadPoolTask::operator()()
   {
      if (!mFileToLoad.empty())
      {
         osg::ref_ptr<osgDB::ReaderWriter::Options> options;
         if (mLoadOptions.valid())
         {
            options = mLoadOptions;
         }
         else
         {
            options = new osgDB::Options;
         }

         if (mUseFileCaching)
         {
            options->setObjectCacheHint(osgDB::Options::CACHE_ALL);
         }
         else
         {
            options->setObjectCacheHint(osgDB::Options::CACHE_NONE);
         }

         options->setOptionString("loadMaterialsToStateSet");

         mLoadedNode = NULL;

         try
         {
            mLoadedNode = dtUtil::FileUtils::GetInstance().ReadNode(mFileToLoad, options.get());
            mComplete = true;
         }
         catch(...)
         {
            LOG_ERROR("Exception thrown trying to load data:" + mFileToLoad);
            mComplete = true;
         }
      }
      else
      {
         mComplete = true;
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Node* ReadNodeThreadPoolTask::GetLoadedNode()
   {
      return mLoadedNode;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const osg::Node* ReadNodeThreadPoolTask::GetLoadedNode() const
   {
      return mLoadedNode;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool ReadNodeThreadPoolTask::IsComplete() const
   {
      return mComplete;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ReadNodeThreadPoolTask::ResetData()
   {
      mLoadedNode = NULL;
      mComplete = false;
   }

   DT_IMPLEMENT_ACCESSOR(ReadNodeThreadPoolTask, bool, UseFileCaching);
   DT_IMPLEMENT_ACCESSOR(ReadNodeThreadPoolTask, std::string, FileToLoad);
   DT_IMPLEMENT_ACCESSOR(ReadNodeThreadPoolTask, osg::ref_ptr<osgDB::ReaderWriter::Options>, LoadOptions);

}
