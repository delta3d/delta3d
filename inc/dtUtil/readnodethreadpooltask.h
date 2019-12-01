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

#ifndef DELTA_READ_NODE_THREAD_POOL_TASK_
#define DELTA_READ_NODE_THREAD_POOL_TASK_

#include <dtUtil/export.h>
#include <dtUtil/getsetmacros.h>
#include <dtUtil/threadpool.h>
#include <osg/Node>
#include <osg/ref_ptr>
#include <osgDB/Options>

namespace dtUtil
{
   class DT_UTIL_EXPORT ReadNodeThreadPoolTask : public dtUtil::ThreadPoolTask
   {
   public:
      ReadNodeThreadPoolTask();

      void operator()() override;

      osg::Node* GetLoadedNode();
      const osg::Node* GetLoadedNode() const;

      /// Check to see if the loading is complete.  If it returns true, call WaitUntilComplete() to make sure.
      bool IsComplete() const;

      virtual void ResetData();

      DT_DECLARE_ACCESSOR(bool, UseFileCaching);
      DT_DECLARE_ACCESSOR(std::string, FileToLoad);
      DT_DECLARE_ACCESSOR(osg::ref_ptr<osgDB::Options>, LoadOptions);

   protected:
      virtual ~ReadNodeThreadPoolTask();
   private:
      osg::ref_ptr<osg::Node> mLoadedNode;
      volatile bool mComplete;
   };
}

#endif
