/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2008 MOVES Institute 
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

/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield 
*
* This library is open source and may be redistributed and/or modified under  
* the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
* (at your option) any later version.  The full license is in LICENSE file
* included with this distribution, and on the openscenegraph.org website.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
* OpenSceneGraph Public License for more details.
*/

#ifndef DELTA_STATS
#define DELTA_STATS 

#include <osg/Camera>
#include <osgViewer/ViewerBase>
#include <osgText/Text>
#include <osg/Switch>

namespace osg
{
   class Geometry;
}

namespace osgGA
{
   class GUIEventAdapter;
   class GUIActionAdapter;
}


namespace dtCore
{

   /** Used by dtABC::Application to render application statistics.  Originally
    *  derived from the OpenSceneGraph StatsHandler class and adapted for use
    *  with Delta3D.
    */
   class DT_CORE_EXPORT StatsHandler 
   {
   public: 

      StatsHandler( osgViewer::ViewerBase &viewer );

      enum StatsType
      {
         NO_STATS = 0,
         FRAME_RATE = 1,
         VIEWER_STATS = 2,
         LAST = 3
      };


      void Reset();

      bool SelectNextType();

      double GetBlockMultiplier() const { return mBlockMultiplier; }

   private:
      osg::Geometry* CreateGeometry(const osg::Vec3& pos, float height, const osg::Vec4& colour, unsigned int numBlocks);

      osg::Geometry* CreateFrameMarkers(const osg::Vec3& pos, float height, const osg::Vec4& colour, unsigned int numBlocks);

      osg::Geometry* CreateTick(const osg::Vec3& pos, float height, const osg::Vec4& colour, unsigned int numTicks);

      osg::Node* CreateCameraStats(const std::string& font, osg::Vec3& pos, float startBlocks, bool aquireGPUStats, float characterSize, osg::Stats* viewerStats, osg::Camera* camera);

      void SetUpScene(osgViewer::ViewerBase* viewer);

      void UpdateThreadingModelText();
      void SetUpHUDCamera(osgViewer::ViewerBase* viewer);
      void PrintOutStats( osgViewer::ViewerBase * viewer );

      osg::ref_ptr<osgViewer::ViewerBase> mViewer;

      int                                 mStatsType;

      bool                                mInitialized;
      osg::ref_ptr<osg::Camera>           mCamera;

      osg::ref_ptr<osg::Switch>           mSwitch;

      osgViewer::ViewerBase::ThreadingModel mThreadingModel;
      osg::ref_ptr<osgText::Text>         mThreadingModelText;

      unsigned int                        mFrameRateChildNum;
      unsigned int                        mViewerChildNum;
      unsigned int                        mSceneChildNum;
      unsigned int                        mNumBlocks;
      double                              mBlockMultiplier;

   };
}


#endif // DELTA_STATS
