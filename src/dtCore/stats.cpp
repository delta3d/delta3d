#include "prefix/dtcoreprefix.h"

#include <sstream>
#include <iomanip>
#include <stdio.h>

#include <dtCore/stats.h>
#include <dtCore/system.h>

#include <dtUtil/nodemask.h>

#include <osg/io_utils>
#include <osg/MatrixTransform>

#include <osgViewer/Renderer>
#include <osgViewer/View>

#include <osg/PolygonMode>
#include <osg/Geometry>
#include <osg/Version>

namespace dtCore
{
////////////////////////////////////////////////////////////////////////////////
StatsHandler::StatsHandler(osgViewer::ViewerBase& viewer)
   : mViewer(&viewer)
   , mStatsType(NO_STATS)
   , mInitialized(false)
   , mThreadingModel(osgViewer::ViewerBase::SingleThreaded)
   , mFrameRateChildNum(0)
   , mViewerChildNum(0)
   , mDeltaSystemChildNum(0)
   , mCameraSceneChildNum(0)
   , mViewerSceneChildNum(0)
   , mNumBlocks(8)
   , mBlockMultiplier(10000.0)
   , mStatsWidth(1280.0f)
   , mStatsHeight(1024.0f)
{
   mCamera = new osg::Camera;
   mCamera->setRenderer(new osgViewer::Renderer(mCamera.get()));
   mCamera->setProjectionResizePolicy(osg::Camera::FIXED);
}

////////////////////////////////////////////////////////////////////////////////
bool StatsHandler::SelectNextType()
{
   if (mViewer.valid() && mThreadingModelText.valid() && mViewer->getThreadingModel()!= mThreadingModel)
   {
      mThreadingModel = mViewer->getThreadingModel();
      UpdateThreadingModelText();
   }

   osg::Stats* stats;
   stats = mViewer->getViewerStats();
   if (stats == NULL)
   {
      return false;
   }

   if (!mInitialized)
   {
      SetUpHUDCamera(mViewer.get());
      SetUpScene(mViewer.get());
   }

   ++mStatsType;

   if (mStatsType==LAST) mStatsType = NO_STATS;

   osgViewer::ViewerBase::Cameras cameras;
   mViewer->getCameras(cameras);

   switch(mStatsType)
   {
   case(NO_STATS):
      {
         stats->collectStats("frame_rate",false);
         stats->collectStats("event",false);
         stats->collectStats("update",false);

         for(osgViewer::ViewerBase::Cameras::iterator itr = cameras.begin();
            itr != cameras.end();
            ++itr)
         {
            if ((*itr)->getStats()) (*itr)->getStats()->collectStats("rendering",false);
            if ((*itr)->getStats()) (*itr)->getStats()->collectStats("gpu",false);
            if ((*itr)->getStats()) (*itr)->getStats()->collectStats("scene",false);
         }

         stats->collectStats("scene",false);

         // Delta3D System stuff
         System::GetInstance().SetStats(NULL);
         stats->collectStats(System::MESSAGE_EVENT_TRAVERSAL, false);
         stats->collectStats(System::MESSAGE_POST_EVENT_TRAVERSAL, false);
         stats->collectStats(System::MESSAGE_PRE_FRAME, false);
         stats->collectStats(System::MESSAGE_CAMERA_SYNCH, false);
         stats->collectStats(System::MESSAGE_FRAME_SYNCH, false);
         stats->collectStats(System::MESSAGE_FRAME, false);
         stats->collectStats(System::MESSAGE_POST_FRAME, false);
         stats->collectStats("UpdatePlusDrawTime", false);
         stats->collectStats("FrameMinusDrawAndUpdateTime", false);
         stats->collectStats("FullDeltaFrameTime", false); // should be a constant

         // GM Stats
         stats->collectStats("GMTotalTime", false);
         stats->collectStats("GMActorsTime", false);
         stats->collectStats("GMComponentsTime", false);
         stats->collectStats("GMTotalNumActors", false);
         stats->collectStats("GMNumActorsProcessed", false);
         stats->collectStats("GMNumCompsProcessed", false);

         mCamera->setNodeMask(dtUtil::NodeMask::NOTHING);
         mSwitch->setAllChildrenOff();
         mInitialized = false;
         break;
      }
   case(FRAME_RATE):
      {
         stats->collectStats("frame_rate",true);
         mCamera->setNodeMask(dtUtil::NodeMask::EVERYTHING);
         mSwitch->setValue(mFrameRateChildNum, true);
         break;
      }
   case(VIEWER_STATS):
      {
         osgViewer::ViewerBase::Scenes scenes;
         mViewer->getScenes(scenes);
         for(osgViewer::ViewerBase::Scenes::iterator itr = scenes.begin();
            itr != scenes.end();
            ++itr)
         {
            osgViewer::Scene* scene = *itr;
            osgDB::DatabasePager* dp = scene->getDatabasePager();
            if (dp && dp->isRunning())
            {
               dp->resetStats();
            }
         }

         stats->collectStats("event",true);
         stats->collectStats("update",true);
         for(osgViewer::ViewerBase::Cameras::iterator itr = cameras.begin();
            itr != cameras.end();
            ++itr)
         {
            if ((*itr)->getStats()) (*itr)->getStats()->collectStats("rendering",true);
            if ((*itr)->getStats()) (*itr)->getStats()->collectStats("gpu",true);
         }

         mCamera->setNodeMask(dtUtil::NodeMask::EVERYTHING);
         mSwitch->setValue(mViewerChildNum, true);
         break;
      }

   case(CAMERA_SCENE_STATS):
      {
         mCamera->setNodeMask(dtUtil::NodeMask::EVERYTHING);
         mSwitch->setValue(mCameraSceneChildNum, true);

         for(osgViewer::ViewerBase::Cameras::iterator itr = cameras.begin(); itr != cameras.end(); ++itr)
         {
            if ((*itr)->getStats()) (*itr)->getStats()->collectStats("scene",true);
         }

         break;
      }
   case(VIEWER_SCENE_STATS):
      {
         mCamera->setNodeMask(dtUtil::NodeMask::EVERYTHING);
         mSwitch->setValue(mViewerSceneChildNum, true);

         stats->collectStats("scene",true);

         break;
      }
   case(DELTA_DETAILS):
      {
         // turn off camera and viewer stats to make room for delta stats
         {
            mSwitch->setValue(mCameraSceneChildNum, false);
            mSwitch->setValue(mViewerSceneChildNum, false);
            for(osgViewer::ViewerBase::Cameras::iterator itr = cameras.begin(); itr != cameras.end(); ++itr)
            {
               if ((*itr)->getStats()) (*itr)->getStats()->collectStats("scene",false);
            }
            stats->collectStats("scene",false);
         }

         System::GetInstance().SetStats(stats);
         stats->collectStats(System::MESSAGE_EVENT_TRAVERSAL, true);
         stats->collectStats(System::MESSAGE_POST_EVENT_TRAVERSAL, true);
         stats->collectStats(System::MESSAGE_PRE_FRAME, true);
         stats->collectStats(System::MESSAGE_CAMERA_SYNCH, true);
         stats->collectStats(System::MESSAGE_FRAME_SYNCH, true);
         stats->collectStats(System::MESSAGE_FRAME, true);
         stats->collectStats(System::MESSAGE_POST_FRAME, true);
         stats->collectStats("UpdatePlusDrawTime", true);
         stats->collectStats("FrameMinusDrawAndUpdateTime", true);
         stats->collectStats("FullDeltaFrameTime", true);

         // GM Stats
         stats->collectStats("GMTotalTime", true);
         stats->collectStats("GMActorsTime", true);
         stats->collectStats("GMComponentsTime", true);
         stats->collectStats("GMTotalNumActors", true);
         stats->collectStats("GMNumActorsProcessed", true);
         stats->collectStats("GMNumCompsProcessed", true);

         mCamera->setNodeMask(dtUtil::NodeMask::EVERYTHING);
         mSwitch->setValue(mDeltaSystemChildNum, true);
         break;
      }

   default: break;
   }

   return true;
}

////////////////////////////////////////////////////////////////////////////////
void StatsHandler::UpdateThreadingModelText()
{
   switch(mThreadingModel)
   {
   case(osgViewer::ViewerBase::SingleThreaded): mThreadingModelText->setText("ThreadingModel: SingleThreaded"); break;
   case(osgViewer::ViewerBase::CullDrawThreadPerContext): mThreadingModelText->setText("ThreadingModel: CullDrawThreadPerContext"); break;
   case(osgViewer::ViewerBase::DrawThreadPerContext): mThreadingModelText->setText("ThreadingModel: DrawThreadPerContext"); break;
   case(osgViewer::ViewerBase::CullThreadPerCameraDrawThreadPerContext): mThreadingModelText->setText("ThreadingModel: CullThreadPerCameraDrawThreadPerContext"); break;
   case(osgViewer::ViewerBase::AutomaticSelection): mThreadingModelText->setText("ThreadingModel: AutomaticSelection"); break;
   default:
      mThreadingModelText->setText("ThreadingModel: unknown"); break;
   }
}

////////////////////////////////////////////////////////////////////////////////
void StatsHandler::Reset()
{
   mInitialized = false;

   // TODO Fix this so that context 0 is not assumed
   mCamera->setGraphicsContext(0);
   mCamera->removeChildren( 0, mCamera->getNumChildren() );
}

////////////////////////////////////////////////////////////////////////////////
void StatsHandler::SetUpHUDCamera(osgViewer::ViewerBase* viewer)
{
   osgViewer::GraphicsWindow* window = dynamic_cast<osgViewer::GraphicsWindow*>(mCamera->getGraphicsContext());

   if (!window)
   {
      osgViewer::ViewerBase::Windows windows;
      viewer->getWindows(windows);

      if (windows.empty()) return;

      window = windows.front();
   }

   mCamera->setGraphicsContext(window);

   mCamera->setViewport(0, 0, window->getTraits()->width, window->getTraits()->height);
   mCamera->setRenderOrder(osg::Camera::POST_RENDER, 10);

   mCamera->setProjectionMatrix(osg::Matrix::ortho2D(0,mStatsWidth,0,mStatsHeight));
   mCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   mCamera->setViewMatrix(osg::Matrix::identity());


   // only clear the depth buffer
   mCamera->setClearMask(0);

   mCamera->setRenderer(new osgViewer::Renderer(mCamera.get()));

   mInitialized = true;
}

struct AveragedValueTextDrawCallback : public virtual osg::Drawable::DrawCallback
{
   AveragedValueTextDrawCallback(osg::Stats* stats, const std::string& name, int frameDelta, bool averageInInverseSpace, double multiplier)
      : _stats(stats)
      , _attributeName(name)
      , _frameDelta(frameDelta)
      , _averageInInverseSpace(averageInInverseSpace)
      , _multiplier(multiplier)
      , _tickLastUpdated(0)
   {
   }

   /** do customized draw code.*/
   virtual void drawImplementation(osg::RenderInfo& renderInfo,const osg::Drawable* drawable) const
   {
      osgText::Text* text = (osgText::Text*)drawable;

      osg::Timer_t tick = osg::Timer::instance()->tick();
      double delta = osg::Timer::instance()->delta_m(_tickLastUpdated, tick);

      if (delta>50) // update every 50ms
      {
         _tickLastUpdated = tick;
         double value;
         if (_stats->getAveragedAttribute( _attributeName, value, _averageInInverseSpace))
         {
            sprintf(_tmpText,"%4.2f",value * _multiplier);
            text->setText(_tmpText);
         }
         else
         {
            text->setText("");
         }
      }

      text->drawImplementation(renderInfo);
   }

   osg::ref_ptr<osg::Stats>    _stats;
   std::string                 _attributeName;
   int                         _frameDelta;
   bool                        _averageInInverseSpace;
   double                      _multiplier;
   mutable char                _tmpText[128];
   mutable osg::Timer_t        _tickLastUpdated;
};

struct SimpleIntTextDrawCallback : public virtual osg::Drawable::DrawCallback
{
   SimpleIntTextDrawCallback(osg::Stats* stats, const std::string& name, bool useParentheses = false)
      : _stats(stats)
      , _attributeName(name)
      , _tickLastUpdated(0)
      , _useParentheses(useParentheses)
   {
   }

   /** do customized draw code.*/
   virtual void drawImplementation(osg::RenderInfo& renderInfo,const osg::Drawable* drawable) const
   {
      osgText::Text* text = (osgText::Text*)drawable;

      osg::Timer_t tick = osg::Timer::instance()->tick();
      double delta = osg::Timer::instance()->delta_m(_tickLastUpdated, tick);

      if (delta>250) // update less often.
      {
         _tickLastUpdated = tick;
         double value;
         if (_stats->getAttribute( _stats->getLatestFrameNumber()-1, _attributeName, value))
         {
            int intValue = (int) value;
            if (_useParentheses)
            {
               sprintf(_tmpText,"(%d)", intValue);
            }
            else
            {
               sprintf(_tmpText,"%d", intValue);
            }
            text->setText(_tmpText);
         }
         else
         {
            text->setText("");
         }
      }

      text->drawImplementation(renderInfo);
   }

   osg::ref_ptr<osg::Stats>    _stats;
   std::string                 _attributeName;
   mutable char                _tmpText[128];
   mutable osg::Timer_t        _tickLastUpdated;
   bool                        _useParentheses;
};

struct CameraSceneStatsTextDrawCallback : public virtual osg::Drawable::DrawCallback
{
   CameraSceneStatsTextDrawCallback(osg::Camera* camera, int cameraNumber)
      : _camera(camera)
      , _tickLastUpdated(0)
      , _cameraNumber(cameraNumber)
   {
   }

   /** do customized draw code.*/
   virtual void drawImplementation(osg::RenderInfo& renderInfo,const osg::Drawable* drawable) const
   {
      if (!_camera) return;

      osgText::Text* text = (osgText::Text*)drawable;

      osg::Timer_t tick = osg::Timer::instance()->tick();
      double delta = osg::Timer::instance()->delta_m(_tickLastUpdated, tick);

      if (delta > 100) // update every 100ms
      {
         _tickLastUpdated = tick;
         std::ostringstream viewStr;
         viewStr.clear();

         osg::Stats* stats = _camera->getStats();
         osgViewer::Renderer* renderer = dynamic_cast<osgViewer::Renderer*>(_camera->getRenderer());

         if (stats && renderer)
         {
            viewStr.setf(std::ios::left, std::ios::adjustfield);
            viewStr.width(14);
            // Used fixed formatting, as scientific will switch to "...e+.." notation for
            // large numbers of vertices/drawables/etc.
            viewStr.setf(std::ios::fixed);
            viewStr.precision(0);

            viewStr << std::setw(1) << "#" << _cameraNumber << std::endl;

            // Camera name
            if (!_camera->getName().empty())
               viewStr << _camera->getName();
            viewStr << std::endl;

            int frameNumber = renderInfo.getState()->getFrameStamp()->getFrameNumber();
            if (!(renderer->getGraphicsThreadDoesCull()))
            {
               --frameNumber;
            }

            #define STATS_ATTRIBUTE(str) \
               if (stats->getAttribute(frameNumber, str, value)) \
               viewStr << std::setw(8) << value << std::endl; \
               else \
               viewStr << std::setw(8) << "." << std::endl; \

            double value = 0.0;

            STATS_ATTRIBUTE("Visible number of lights");
            STATS_ATTRIBUTE("Visible number of render bins");
            STATS_ATTRIBUTE("Visible depth");
            STATS_ATTRIBUTE("Visible number of materials");
            STATS_ATTRIBUTE("Visible number of impostors");
            STATS_ATTRIBUTE("Visible number of drawables");
            STATS_ATTRIBUTE("Visible vertex count");

            STATS_ATTRIBUTE("Visible number of GL_POINTS");
            STATS_ATTRIBUTE("Visible number of GL_LINES");
            STATS_ATTRIBUTE("Visible number of GL_LINE_STRIP");
            STATS_ATTRIBUTE("Visible number of GL_LINE_LOOP");
            STATS_ATTRIBUTE("Visible number of GL_TRIANGLES");
            STATS_ATTRIBUTE("Visible number of GL_TRIANGLE_STRIP");
            STATS_ATTRIBUTE("Visible number of GL_TRIANGLE_FAN");
            STATS_ATTRIBUTE("Visible number of GL_QUADS");
            STATS_ATTRIBUTE("Visible number of GL_QUAD_STRIP");
            STATS_ATTRIBUTE("Visible number of GL_POLYGON");

            text->setText(viewStr.str());
         }
      }
      text->drawImplementation(renderInfo);
   }

   osg::observer_ptr<osg::Camera>  _camera;
   mutable osg::Timer_t            _tickLastUpdated;
   int                             _cameraNumber;
};


struct ViewSceneStatsTextDrawCallback : public virtual osg::Drawable::DrawCallback
{
   ViewSceneStatsTextDrawCallback(osgViewer::View* view, int viewNumber):
_view(view),
   _tickLastUpdated(0),
   _viewNumber(viewNumber)
{
}

/** do customized draw code.*/
virtual void drawImplementation(osg::RenderInfo& renderInfo,const osg::Drawable* drawable) const
{
   if (!_view) return;

   osgText::Text* text = (osgText::Text*)drawable;

   osg::Timer_t tick = osg::Timer::instance()->tick();
   double delta = osg::Timer::instance()->delta_m(_tickLastUpdated, tick);

   if (delta > 200) // update every 100ms
   {
      _tickLastUpdated = tick;
      osg::Stats* stats = _view->getStats();
      if (stats)
      {
         std::ostringstream viewStr;
         viewStr.clear();
         viewStr.setf(std::ios::left, std::ios::adjustfield);
         viewStr.width(20);
         viewStr.setf(std::ios::fixed);
         viewStr.precision(0);

         viewStr << std::setw(1) << "#" << _viewNumber;

         // View name
         if (!_view->getName().empty())
            viewStr << ": " << _view->getName();
         viewStr << std::endl;

         int frameNumber = renderInfo.getState()->getFrameStamp()->getFrameNumber();
         // if (!(renderer->getGraphicsThreadDoesCull()))
         {
            --frameNumber;
         }

         #define STATS_ATTRIBUTE_PAIR(str1, str2) \
            if (stats->getAttribute(frameNumber, str1, value)) \
            viewStr << std::setw(9) << value; \
            else \
            viewStr << std::setw(9) << "."; \
            if (stats->getAttribute(frameNumber, str2, value)) \
            viewStr << std::setw(9) << value << std::endl; \
            else \
            viewStr << std::setw(9) << "." << std::endl; \

         double value = 0.0;

         // header
         viewStr << std::setw(9) << "Unique" << std::setw(9) << "Instance" << std::endl;

         STATS_ATTRIBUTE_PAIR("Number of unique StateSet","Number of instanced Stateset");
         STATS_ATTRIBUTE_PAIR("Number of unique Group","Number of instanced Group");
         STATS_ATTRIBUTE_PAIR("Number of unique Transform","Number of instanced Transform");
         STATS_ATTRIBUTE_PAIR("Number of unique LOD","Number of instanced LOD");
         STATS_ATTRIBUTE_PAIR("Number of unique Switch","Number of instanced Switch");
         STATS_ATTRIBUTE_PAIR("Number of unique Geode","Number of instanced Geode");
         STATS_ATTRIBUTE_PAIR("Number of unique Drawable","Number of instanced Drawable");
         STATS_ATTRIBUTE_PAIR("Number of unique Geometry","Number of instanced Geometry");
         STATS_ATTRIBUTE_PAIR("Number of unique Vertices","Number of instanced Vertices");
         STATS_ATTRIBUTE_PAIR("Number of unique Primitives","Number of instanced Primitives");


         text->setText(viewStr.str());
      }
      else
      {
         osg::notify(osg::WARN)<<std::endl<<"No valid view to collect scene stats from"<<std::endl;

         text->setText("");
      }
   }
   text->drawImplementation(renderInfo);
}

osg::observer_ptr<osgViewer::View>  _view;
mutable osg::Timer_t                _tickLastUpdated;
int                                 _viewNumber;
};

struct BlockDrawCallback : public virtual osg::Drawable::DrawCallback
{
   BlockDrawCallback(StatsHandler* statsHandler, float xPos, osg::Stats* viewerStats, osg::Stats* stats, const std::string& beginName, const std::string& endName, int frameDelta, int numFrames)
      : _statsHandler(statsHandler)
      , _xPos(xPos)
      , _viewerStats(viewerStats)
      , _stats(stats)
      , _beginName(beginName)
      , _endName(endName)
      , _frameDelta(frameDelta)
      , _numFrames(numFrames)
   {
   }

   /** do customized draw code.*/
   virtual void drawImplementation(osg::RenderInfo& renderInfo,const osg::Drawable* drawable) const
   {
      osg::Geometry* geom = (osg::Geometry*)drawable;
      osg::Vec3Array* vertices = (osg::Vec3Array*)geom->getVertexArray();

      int frameNumber = renderInfo.getState()->getFrameStamp()->getFrameNumber();

      int startFrame = frameNumber + _frameDelta - _numFrames + 1;
      int endFrame = frameNumber + _frameDelta;
      double referenceTime;
      if (!_viewerStats->getAttribute( startFrame, "Reference time", referenceTime))
      {
         return;
      }

      unsigned int vi = 0;
      double beginValue, endValue;
      for(int i = startFrame; i <= endFrame; ++i)
      {
         if (_stats->getAttribute( i, _beginName, beginValue) &&
            _stats->getAttribute( i, _endName, endValue) )
         {
            (*vertices)[vi++].x() = _xPos + (beginValue - referenceTime) * _statsHandler->GetBlockMultiplier();
            (*vertices)[vi++].x() = _xPos + (beginValue - referenceTime) * _statsHandler->GetBlockMultiplier();
            (*vertices)[vi++].x() = _xPos + (endValue - referenceTime) * _statsHandler->GetBlockMultiplier();
            (*vertices)[vi++].x() = _xPos + (endValue - referenceTime) * _statsHandler->GetBlockMultiplier();
         }
      }

      drawable->drawImplementation(renderInfo);
   }

   StatsHandler*               _statsHandler;
   float                       _xPos;
   osg::ref_ptr<osg::Stats>    _viewerStats;
   osg::ref_ptr<osg::Stats>    _stats;
   std::string                 _beginName;
   std::string                 _endName;
   int                         _frameDelta;
   int                         _numFrames;
};

osg::Geometry* StatsHandler::CreateBackgroundRectangle(const osg::Vec3& pos, const float width, const float height, osg::Vec4& color)
{
   osg::StateSet *ss = new osg::StateSet;

   osg::Geometry* geometry = new osg::Geometry;

   geometry->setUseDisplayList(false);
   geometry->setStateSet(ss);

   osg::Vec3Array* vertices = new osg::Vec3Array;
   geometry->setVertexArray(vertices);

   vertices->push_back(osg::Vec3(pos.x(), pos.y(), 0));
   vertices->push_back(osg::Vec3(pos.x(), pos.y()-height,0));
   vertices->push_back(osg::Vec3(pos.x()+width, pos.y()-height,0));
   vertices->push_back(osg::Vec3(pos.x()+width, pos.y(),0));

   osg::Vec4Array* colors = new osg::Vec4Array;
   colors->push_back(color);
   geometry->setColorArray(colors);
   geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

   osg::DrawElementsUInt *base =  new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS,0);
   base->push_back(0);
   base->push_back(1);
   base->push_back(2);
   base->push_back(3);

   geometry->addPrimitiveSet(base);

   return geometry;
}


struct StatsGraph : public osg::MatrixTransform
{
   StatsGraph(osg::Vec3 pos, float width, float height)
      : _pos(pos), _width(width), _height(height),
      _statsGraphGeode(new osg::Geode)
   {
      _pos -= osg::Vec3(0, height, 0.1);
      setMatrix(osg::Matrix::translate(_pos));
      addChild(_statsGraphGeode.get());
   }

   void addStatGraph(osg::Stats* viewerStats, osg::Stats* stats, const osg::Vec4& color, float max, const std::string& nameBegin, const std::string& nameEnd = "")
   {
      _statsGraphGeode->addDrawable(new Graph(_width, _height, viewerStats, stats, color, max, nameBegin, nameEnd));
   }

   osg::Vec3           _pos;
   float               _width;
   float               _height;

   osg::ref_ptr<osg::Geode> _statsGraphGeode;

protected:
   struct Graph : public osg::Geometry
   {
      Graph(float width, float height, osg::Stats* viewerStats, osg::Stats* stats,
         const osg::Vec4& color, float max, const std::string& nameBegin, const std::string& nameEnd = "")
      {
         setUseDisplayList(false);

         setVertexArray(new osg::Vec3Array);

         osg::Vec4Array* colors = new osg::Vec4Array;
         colors->push_back(color);
         setColorArray(colors);
         setColorBinding(osg::Geometry::BIND_OVERALL);

         setDrawCallback(new GraphUpdateCallback(width, height, viewerStats, stats, max, nameBegin, nameEnd));
      }
   };

   struct GraphUpdateCallback : public osg::Drawable::DrawCallback
   {
      GraphUpdateCallback(float width, float height, osg::Stats* viewerStats, osg::Stats* stats,
         float max, const std::string& nameBegin, const std::string& nameEnd = "")
         : _width((unsigned int)width), _height((unsigned int)height), _curX(0),
         _viewerStats(viewerStats), _stats(stats), _max(max), _nameBegin(nameBegin), _nameEnd(nameEnd)
      {
      }

      virtual void drawImplementation(osg::RenderInfo& renderInfo,const osg::Drawable* drawable) const
      {
         osg::Geometry* geometry = const_cast<osg::Geometry*>(drawable->asGeometry());
         if (!geometry) return;
         osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
         if (!vertices) return;

         int frameNumber = renderInfo.getState()->getFrameStamp()->getFrameNumber();

         // Get stats
         double value;
         if (_nameEnd.empty())
         {
            if (!_stats->getAveragedAttribute( _nameBegin, value, true ))
            {
               value = 0.0;
            }
         }
         else
         {
            double beginValue, endValue;
            if (_stats->getAttribute( frameNumber, _nameBegin, beginValue) &&
               _stats->getAttribute( frameNumber, _nameEnd, endValue) )
            {
               value = endValue - beginValue;
            }
            else
            {
               value = 0.0;
            }
         }

         // Add new vertex for this frame.
         value = osg::clampTo(value, 0.0, double(_max));
         vertices->push_back(osg::Vec3(float(_curX), float(_height) / _max * value, 0));

         // One vertex per pixel in X.
         if (vertices->size() > _width)
         {
            unsigned int excedent = vertices->size() - _width;
            vertices->erase(vertices->begin(), vertices->begin() + excedent);

            // Make the graph scroll when there is enough data.
            // Note: We check the frame number so that even if we have
            // many graphs, the transform is translated only once per
            // frame.
            static const float increment = -1.0;
            if (GraphUpdateCallback::_frameNumber != frameNumber)
            {
               // We know the exact layout of this part of the scene
               // graph, so this is OK...
               osg::MatrixTransform* transform =
                  geometry->getParent(0)->getParent(0)->asTransform()->asMatrixTransform();
               if (transform)
               {
                  transform->setMatrix(transform->getMatrix() * osg::Matrix::translate(osg::Vec3(increment, 0, 0)));
               }
            }
         }
         else
         {
            // Create primitive set if none exists.
            if (geometry->getNumPrimitiveSets() == 0)
               geometry->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP, 0, 0));

            // Update primitive set.
            osg::DrawArrays* drawArrays = dynamic_cast<osg::DrawArrays*>(geometry->getPrimitiveSet(0));
            if (!drawArrays) return;
            drawArrays->setFirst(0);
            drawArrays->setCount(vertices->size());
         }

         _curX++;
         GraphUpdateCallback::_frameNumber = frameNumber;

         geometry->dirtyBound();

         drawable->drawImplementation(renderInfo);
      }

      const unsigned int      _width;
      const unsigned int      _height;
      mutable unsigned int    _curX;
      osg::Stats*             _viewerStats;
      osg::Stats*             _stats;
      const float             _max;
      const std::string       _nameBegin;
      const std::string       _nameEnd;
      static int              _frameNumber;
   };
};

int StatsGraph::GraphUpdateCallback::_frameNumber = 0;


osg::Geometry* StatsHandler::CreateGeometry(const osg::Vec3& pos, float height, const osg::Vec4& colour, unsigned int numBlocks)
{
   osg::Geometry* geometry = new osg::Geometry;

   geometry->setUseDisplayList(false);

   osg::Vec3Array* vertices = new osg::Vec3Array;
   geometry->setVertexArray(vertices);
   vertices->reserve(numBlocks*4);

   for(unsigned int i=0; i<numBlocks; ++i)
   {
      vertices->push_back(pos+osg::Vec3(i*20, height, 0.0));
      vertices->push_back(pos+osg::Vec3(i*20, 0.0, 0.0));
      vertices->push_back(pos+osg::Vec3(i*20+10.0, 0.0, 0.0));
      vertices->push_back(pos+osg::Vec3(i*20+10.0, height, 0.0));
   }

   osg::Vec4Array* colours = new osg::Vec4Array;
   colours->push_back(colour);
   geometry->setColorArray(colours);
   geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

   geometry->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, numBlocks*4));

   return geometry;
}


struct FrameMarkerDrawCallback : public virtual osg::Drawable::DrawCallback
{
   FrameMarkerDrawCallback(StatsHandler* statsHandler, float xPos, osg::Stats* viewerStats, int frameDelta, int numFrames)
      : _statsHandler(statsHandler)
      , _xPos(xPos)
      , _viewerStats(viewerStats)
      , _frameDelta(frameDelta)
      , _numFrames(numFrames)
   {
   }

   /** do customized draw code.*/
   virtual void drawImplementation(osg::RenderInfo& renderInfo,const osg::Drawable* drawable) const
   {
      osg::Geometry* geom = (osg::Geometry*)drawable;
      osg::Vec3Array* vertices = (osg::Vec3Array*)geom->getVertexArray();

      int frameNumber = renderInfo.getState()->getFrameStamp()->getFrameNumber();

      int startFrame = frameNumber + _frameDelta - _numFrames + 1;
      int endFrame = frameNumber + _frameDelta;
      double referenceTime;
      if (!_viewerStats->getAttribute( startFrame, "Reference time", referenceTime))
      {
         return;
      }

      unsigned int vi = 0;
      double currentReferenceTime;
      for(int i = startFrame; i <= endFrame; ++i)
      {
         if (_viewerStats->getAttribute( i, "Reference time", currentReferenceTime))
         {
            (*vertices)[vi++].x() = _xPos + (currentReferenceTime - referenceTime) * _statsHandler->GetBlockMultiplier();
            (*vertices)[vi++].x() = _xPos + (currentReferenceTime - referenceTime) * _statsHandler->GetBlockMultiplier();
         }
      }

      drawable->drawImplementation(renderInfo);
   }

   StatsHandler*               _statsHandler;
   float                       _xPos;
   osg::ref_ptr<osg::Stats>    _viewerStats;
   std::string                 _endName;
   int                         _frameDelta;
   int                         _numFrames;
};

struct PagerCallback : public virtual osg::NodeCallback
{
   PagerCallback( osgDB::DatabasePager* dp,
      osgText::Text* minValue,
      osgText::Text* maxValue,
      osgText::Text* averageValue,
      osgText::Text* filerequestlist,
      osgText::Text* compilelist,
      double multiplier)
      : _dp(dp)
      , _minValue(minValue)
      , _maxValue(maxValue)
      , _averageValue(averageValue)
      , _filerequestlist(filerequestlist)
      , _compilelist(compilelist)
      , _multiplier(multiplier)
   {
   }

   virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
   {
      if (_dp.valid())
      {
         double value = _dp->getAverageTimeToMergeTiles();
         if (value>= 0.0 && value <= 1000)
         {
            sprintf(_tmpText,"%4.0f",value * _multiplier);
            _averageValue->setText(_tmpText);
         }
         else
         {
            _averageValue->setText("");
         }

         value = _dp->getMinimumTimeToMergeTile();
         if (value>= 0.0 && value <= 1000)
         {
            sprintf(_tmpText,"%4.0f",value * _multiplier);
            _minValue->setText(_tmpText);
         }
         else
         {
            _minValue->setText("");
         }

         value = _dp->getMaximumTimeToMergeTile();
         if (value>= 0.0 && value <= 1000)
         {
            sprintf(_tmpText,"%4.0f",value * _multiplier);
            _maxValue->setText(_tmpText);
         }
         else
         {
            _maxValue->setText("");
         }

         sprintf(_tmpText,"%4d", _dp->getFileRequestListSize());
         _filerequestlist->setText(_tmpText);

         sprintf(_tmpText,"%4d", _dp->getDataToCompileListSize());
         _compilelist->setText(_tmpText);
      }

      traverse(node,nv);
   }

   osg::observer_ptr<osgDB::DatabasePager> _dp;

   osg::ref_ptr<osgText::Text> _minValue;
   osg::ref_ptr<osgText::Text> _maxValue;
   osg::ref_ptr<osgText::Text> _averageValue;
   osg::ref_ptr<osgText::Text> _filerequestlist;
   osg::ref_ptr<osgText::Text> _compilelist;
   double _multiplier;
   char                _tmpText[128];
   osg::Timer_t        _tickLastUpdated;
};

////////////////////////////////////////////////////////////////////////////////
osg::Geometry* StatsHandler::CreateFrameMarkers(const osg::Vec3& pos, float height, const osg::Vec4& colour, unsigned int numBlocks)
{
   osg::Geometry* geometry = new osg::Geometry;

   geometry->setUseDisplayList(false);

   osg::Vec3Array* vertices = new osg::Vec3Array;
   geometry->setVertexArray(vertices);
   vertices->reserve(numBlocks*2);

   for(unsigned int i=0; i<numBlocks; ++i)
   {
      vertices->push_back(pos+osg::Vec3(double(i)*mBlockMultiplier*0.01, height, 0.0));
      vertices->push_back(pos+osg::Vec3(double(i)*mBlockMultiplier*0.01, 0.0, 0.0));
   }

   osg::Vec4Array* colours = new osg::Vec4Array;
   colours->push_back(colour);
   geometry->setColorArray(colours);
   geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

   geometry->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, numBlocks*2));

   return geometry;
}

////////////////////////////////////////////////////////////////////////////////
osg::Geometry* StatsHandler::CreateTick(const osg::Vec3& pos, float height, const osg::Vec4& colour, unsigned int numTicks)
{
   osg::Geometry* geometry = new osg::Geometry;

   geometry->setUseDisplayList(false);

   osg::Vec3Array* vertices = new osg::Vec3Array;
   geometry->setVertexArray(vertices);
   vertices->reserve(numTicks*2);

   for (unsigned int i = 0; i < numTicks; ++i)
   {
      float tickHeight = (i%10) ? height : height * 2.0;
      vertices->push_back(pos+osg::Vec3(double(i)*mBlockMultiplier*0.001, tickHeight , 0.0));
      vertices->push_back(pos+osg::Vec3(double(i)*mBlockMultiplier*0.001, 0.0, 0.0));
   }

   osg::Vec4Array* colours = new osg::Vec4Array;
   colours->push_back(colour);
   geometry->setColorArray(colours);
   geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

   geometry->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, numTicks*2));

   return geometry;
}

////////////////////////////////////////////////////////////////////////////////
void StatsHandler::SetUpScene(osgViewer::ViewerBase* viewer)
{
   mSwitch = new osg::Switch;

   mCamera->removeChildren(0, mCamera->getNumChildren());

   mCamera->addChild(mSwitch.get());

   osg::StateSet* stateset = mSwitch->getOrCreateStateSet();
   stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
   stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
   stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
   stateset->setAttribute(new osg::PolygonMode(), osg::StateAttribute::PROTECTED);

   std::string font("fonts/arial.ttf");

   // collect all the relevant camers
   osgViewer::ViewerBase::Cameras validCameras;
   viewer->getCameras(validCameras);

   osgViewer::ViewerBase::Cameras cameras;
   for(osgViewer::ViewerBase::Cameras::iterator itr = validCameras.begin();
      itr != validCameras.end();
      ++itr)
   {
      if ((*itr)->getStats())
      {
         cameras.push_back(*itr);
      }
   }

   // check for querry time support
   unsigned int numCamrasWithTimerQuerySupport = 0;
   for(osgViewer::ViewerBase::Cameras::iterator citr = cameras.begin();
      citr != cameras.end();
      ++citr)
   {
      if ((*citr)->getGraphicsContext())
      {
         unsigned int contextID = (*citr)->getGraphicsContext()->getState()->getContextID();

         const osg::GLExtensions* extensions = osg::GLExtensions::Get(contextID, true);
         if (extensions && extensions->isTimerQuerySupported)
         {
            ++numCamrasWithTimerQuerySupport;
         }
      }
   }

   bool aquireGPUStats = numCamrasWithTimerQuerySupport==cameras.size();

   float leftPos = 10.0f;
   float startBlocks = 150.0f;
   float characterSize = 20.0f;

   osg::Vec3 pos(leftPos, mStatsHeight-24.0f,0.0f);

   osg::Vec4 colorFR(1.0f,1.0f,1.0f,1.0f);
   osg::Vec4 colorFRAlpha(1.0f,1.0f,1.0f,0.5f);
   osg::Vec4 colorUpdate( 0.0f,1.0f,0.0f,1.0f);
   osg::Vec4 colorUpdateAlpha( 0.0f,1.0f,0.0f,0.5f);
   osg::Vec4 colorEvent(0.0f, 1.0f, 0.5f, 1.0f);
   osg::Vec4 colorEventAlpha(0.0f, 1.0f, 0.5f, 0.5f);
   osg::Vec4 colorCull( 0.0f,1.0f,1.0f,1.0f);
   osg::Vec4 colorCullAlpha( 0.0f,1.0f,1.0f,0.5f);
   osg::Vec4 colorDraw( 1.0f,1.0f,0.0f,1.0f);
   osg::Vec4 colorDrawAlpha( 1.0f,1.0f,0.0f,0.5f);
   osg::Vec4 colorGPU( 1.0f,0.5f,0.0f,1.0f);
   osg::Vec4 colorGPUAlpha( 1.0f,0.5f,0.0f,0.5f);

   osg::Vec4 colorDP( 1.0f,1.0f,0.5f,1.0f);

   osg::Stats* stats;
   stats = mViewer->getViewerStats();

   // frame rate stats
   {
      osg::Geode* geode = new osg::Geode();
      mFrameRateChildNum = mSwitch->getNumChildren();
      mSwitch->addChild(geode, false);

      osg::ref_ptr<osgText::Text> frameRateLabel = new osgText::Text;
      geode->addDrawable( frameRateLabel.get() );

      frameRateLabel->setColor(colorFR);
      frameRateLabel->setFont(font);
      frameRateLabel->setCharacterSize(characterSize);
      frameRateLabel->setPosition(pos);
      frameRateLabel->setText("Frame Rate: ");

      pos.x() = frameRateLabel->getBoundingBox().xMax();

      osg::ref_ptr<osgText::Text> frameRateValue = new osgText::Text;
      geode->addDrawable( frameRateValue.get() );

      frameRateValue->setColor(colorFR);
      frameRateValue->setFont(font);
      frameRateValue->setCharacterSize(characterSize);
      frameRateValue->setPosition(pos);
      frameRateValue->setText("0.0");
      frameRateValue->setDrawCallback(new AveragedValueTextDrawCallback(stats,"Frame rate",-1, true, 1.0));

      pos.y() -= characterSize * 1.5f;
   }

   osg::Vec4 backgroundColor(0.0, 0.0, 0.0f, 0.3);
   osg::Vec4 staticTextColor(1.0, 1.0, 0.0f, 1.0);
   osg::Vec4 dynamicTextColor(1.0, 1.0, 1.0f, 1.0);
   float backgroundMargin = 5;
   float backgroundSpacing = 3;


   // viewer stats
   {
      osg::Group* group = new osg::Group;
      mViewerChildNum = mSwitch->getNumChildren();
      mSwitch->addChild(group, false);

      osg::Geode* geode = new osg::Geode();
      group->addChild(geode);

      {
         pos.x() = leftPos;

         mThreadingModelText = new osgText::Text;
         geode->addDrawable( mThreadingModelText.get() );

         mThreadingModelText->setColor(colorFR);
         mThreadingModelText->setFont(font);
         mThreadingModelText->setCharacterSize(characterSize);
         mThreadingModelText->setPosition(pos);

         UpdateThreadingModelText();

         pos.y() -= characterSize*1.5f;
      }

      float topOfViewerStats = pos.y() + characterSize;

      geode->addDrawable(CreateBackgroundRectangle(
         pos + osg::Vec3(-backgroundMargin, characterSize + backgroundMargin, 0),
         mStatsWidth - 2 * backgroundMargin,
         (3 + 4.5 * cameras.size()) * characterSize + 2 * backgroundMargin,
         backgroundColor) );

      {
         pos.x() = leftPos;

         osg::ref_ptr<osgText::Text> eventLabel = new osgText::Text;
         geode->addDrawable(eventLabel.get());

         eventLabel->setColor(colorUpdate);
         eventLabel->setFont(font);
         eventLabel->setCharacterSize(characterSize);
         eventLabel->setPosition(pos);
         eventLabel->setText("Event: ");

         pos.x() = eventLabel->getBoundingBox().xMax();

         osg::ref_ptr<osgText::Text> eventValue = new osgText::Text;
         geode->addDrawable(eventValue.get());

         eventValue->setColor(colorUpdate);
         eventValue->setFont(font);
         eventValue->setCharacterSize(characterSize);
         eventValue->setPosition(pos);
         eventValue->setText("0.0");

         eventValue->setDrawCallback(new AveragedValueTextDrawCallback(stats,"Event traversal time taken",-1, false, 1000.0));
         pos.x() = startBlocks;
         osg::Geometry* geometry = CreateGeometry(pos, characterSize *0.8, colorUpdateAlpha, mNumBlocks);
         geometry->setDrawCallback(new BlockDrawCallback(this, startBlocks, stats, stats, "Event traversal begin time", "Event traversal end time", -1, mNumBlocks));
         geode->addDrawable(geometry);

         pos.y() -= characterSize*1.5f;
      }

      {
         pos.x() = leftPos;

         osg::ref_ptr<osgText::Text> updateLabel = new osgText::Text;
         geode->addDrawable( updateLabel.get() );

         updateLabel->setColor(colorUpdate);
         updateLabel->setFont(font);
         updateLabel->setCharacterSize(characterSize);
         updateLabel->setPosition(pos);
         updateLabel->setText("Update: ");

         pos.x() = updateLabel->getBoundingBox().xMax();

         osg::ref_ptr<osgText::Text> updateValue = new osgText::Text;
         geode->addDrawable(updateValue.get());

         updateValue->setColor(colorUpdate);
         updateValue->setFont(font);
         updateValue->setCharacterSize(characterSize);
         updateValue->setPosition(pos);
         updateValue->setText("0.0");

         updateValue->setDrawCallback(new AveragedValueTextDrawCallback(stats,"Update traversal time taken",-1, false, 1000.0));

         pos.x() = startBlocks;
         osg::Geometry* geometry = CreateGeometry(pos, characterSize *0.8, colorUpdateAlpha, mNumBlocks);
         geometry->setDrawCallback(new BlockDrawCallback(this, startBlocks, stats, stats, "Update traversal begin time", "Update traversal end time", -1, mNumBlocks));
         geode->addDrawable(geometry);

         pos.y() -= characterSize*1.5f;
      }

      pos.x() = leftPos;

      // add camera stats
      for(osgViewer::ViewerBase::Cameras::iterator citr = cameras.begin();
         citr != cameras.end();
         ++citr)
      {
         group->addChild(CreateCameraTimeStats(font, pos, startBlocks, aquireGPUStats, characterSize, stats, *citr));
      }

      // add frame ticks
      {
         osg::Geode* geode = new osg::Geode;
         group->addChild(geode);

         osg::Vec4 colourTicks(1.0f,1.0f,1.0f, 0.5f);

         pos.x() = startBlocks;
         pos.y() += characterSize;
         float height = topOfViewerStats - pos.y();

         osg::Geometry* ticks = CreateTick(pos, 5.0f, colourTicks, 100);
         geode->addDrawable(ticks);

         osg::Geometry* frameMarkers = CreateFrameMarkers(pos, height, colourTicks, mNumBlocks + 1);
         frameMarkers->setDrawCallback(new FrameMarkerDrawCallback(this, startBlocks, stats, 0, mNumBlocks + 1));
         geode->addDrawable(frameMarkers);

         pos.x() = leftPos;
      }

      // Stats line graph
      {
         pos.y() -= (backgroundSpacing + 2 * backgroundMargin);
         float width = mStatsWidth - 4 * backgroundMargin;
         float height = 5 * characterSize;

         // Create a stats graph and add any stats we want to track with it.
         StatsGraph* statsGraph = new StatsGraph(pos, width, height);
         group->addChild(statsGraph);

         statsGraph->addStatGraph(viewer->getViewerStats(), viewer->getViewerStats(), colorFR, 100, "Frame rate");
         statsGraph->addStatGraph(viewer->getViewerStats(), viewer->getViewerStats(), colorEvent, 0.016, "Event traversal time taken");
         statsGraph->addStatGraph(viewer->getViewerStats(), viewer->getViewerStats(), colorUpdate, 0.016, "Update traversal time taken");

         for(osgViewer::ViewerBase::Cameras::iterator citr = cameras.begin();
            citr != cameras.end();
            ++citr)
         {
            statsGraph->addStatGraph(viewer->getViewerStats(), (*citr)->getStats(), colorCull, 0.016, "Cull traversal time taken");
            statsGraph->addStatGraph(viewer->getViewerStats(), (*citr)->getStats(), colorDraw, 0.016, "Draw traversal time taken");
            statsGraph->addStatGraph(viewer->getViewerStats(), (*citr)->getStats(), colorGPU, 0.016, "GPU draw time taken");
         }

         geode->addDrawable(CreateBackgroundRectangle( pos + osg::Vec3(-backgroundMargin, backgroundMargin, 0),
            width + 2 * backgroundMargin,
            height + 2 * backgroundMargin,
            backgroundColor) );

         pos.x() = leftPos;
         pos.y() -= height + 2 * backgroundMargin;
      }

      // Databasepager stats
      osgViewer::ViewerBase::Scenes scenes;
      viewer->getScenes(scenes);
      for(osgViewer::ViewerBase::Scenes::iterator itr = scenes.begin();
         itr != scenes.end();
         ++itr)
      {
         osgViewer::Scene* scene = *itr;
         osgDB::DatabasePager* dp = scene->getDatabasePager();
         if (dp && dp->isRunning())
         {
            pos.y() -= (characterSize + backgroundSpacing);

            geode->addDrawable(CreateBackgroundRectangle(    pos + osg::Vec3(-backgroundMargin, characterSize + backgroundMargin, 0),
               mStatsWidth - 2 * backgroundMargin,
               characterSize + 2 * backgroundMargin,
               backgroundColor));

            osg::ref_ptr<osgText::Text> averageLabel = new osgText::Text;
            geode->addDrawable( averageLabel.get() );

            averageLabel->setColor(colorDP);
            averageLabel->setFont(font);
            averageLabel->setCharacterSize(characterSize);
            averageLabel->setPosition(pos);
            averageLabel->setText("DatabasePager time to merge new tiles - average: ");

            pos.x() = averageLabel->getBoundingBox().xMax();

            osg::ref_ptr<osgText::Text> averageValue = new osgText::Text;
            geode->addDrawable( averageValue.get() );

            averageValue->setColor(colorDP);
            averageValue->setFont(font);
            averageValue->setCharacterSize(characterSize);
            averageValue->setPosition(pos);
            averageValue->setText("1000");

            pos.x() = averageValue->getBoundingBox().xMax() + 2.0f*characterSize;

            osg::ref_ptr<osgText::Text> minLabel = new osgText::Text;
            geode->addDrawable( minLabel.get() );

            minLabel->setColor(colorDP);
            minLabel->setFont(font);
            minLabel->setCharacterSize(characterSize);
            minLabel->setPosition(pos);
            minLabel->setText("min: ");

            pos.x() = minLabel->getBoundingBox().xMax();

            osg::ref_ptr<osgText::Text> minValue = new osgText::Text;
            geode->addDrawable(minValue.get());

            minValue->setColor(colorDP);
            minValue->setFont(font);
            minValue->setCharacterSize(characterSize);
            minValue->setPosition(pos);
            minValue->setText("1000");

            pos.x() = minValue->getBoundingBox().xMax() + 2.0f*characterSize;


            osg::ref_ptr<osgText::Text> maxLabel = new osgText::Text;
            geode->addDrawable(maxLabel.get());

            maxLabel->setColor(colorDP);
            maxLabel->setFont(font);
            maxLabel->setCharacterSize(characterSize);
            maxLabel->setPosition(pos);
            maxLabel->setText("max: ");

            pos.x() = maxLabel->getBoundingBox().xMax();

            osg::ref_ptr<osgText::Text> maxValue = new osgText::Text;
            geode->addDrawable(maxValue.get());

            maxValue->setColor(colorDP);
            maxValue->setFont(font);
            maxValue->setCharacterSize(characterSize);
            maxValue->setPosition(pos);
            maxValue->setText("1000");

            pos.x() = maxValue->getBoundingBox().xMax();

            osg::ref_ptr<osgText::Text> requestsLabel = new osgText::Text;
            geode->addDrawable( requestsLabel.get() );

            requestsLabel->setColor(colorDP);
            requestsLabel->setFont(font);
            requestsLabel->setCharacterSize(characterSize);
            requestsLabel->setPosition(pos);
            requestsLabel->setText("requests: ");

            pos.x() = requestsLabel->getBoundingBox().xMax();

            osg::ref_ptr<osgText::Text> requestList = new osgText::Text;
            geode->addDrawable( requestList.get() );

            requestList->setColor(colorDP);
            requestList->setFont(font);
            requestList->setCharacterSize(characterSize);
            requestList->setPosition(pos);
            requestList->setText("0");

            pos.x() = requestList->getBoundingBox().xMax() + 2.0f*characterSize;;

            osg::ref_ptr<osgText::Text> compileLabel = new osgText::Text;
            geode->addDrawable( compileLabel.get() );

            compileLabel->setColor(colorDP);
            compileLabel->setFont(font);
            compileLabel->setCharacterSize(characterSize);
            compileLabel->setPosition(pos);
            compileLabel->setText("tocompile: ");

            pos.x() = compileLabel->getBoundingBox().xMax();

            osg::ref_ptr<osgText::Text> compileList = new osgText::Text;
            geode->addDrawable( compileList.get() );

            compileList->setColor(colorDP);
            compileList->setFont(font);
            compileList->setCharacterSize(characterSize);
            compileList->setPosition(pos);
            compileList->setText("0");

            pos.x() = maxLabel->getBoundingBox().xMax();

            geode->setCullCallback(new PagerCallback(dp, minValue.get(), maxValue.get(), averageValue.get(), requestList.get(), compileList.get(), 1000.0));
         }

         pos.x() = leftPos;
      }
   }


   // Camera scene stats
   {
      pos.y() -= (characterSize + backgroundSpacing + 2 * backgroundMargin);

      osg::Group* group = new osg::Group;
      mCameraSceneChildNum = mSwitch->getNumChildren();
      mSwitch->addChild(group, false);

      osg::Geode* geode = new osg::Geode();
      geode->setCullingActive(false);
      group->addChild(geode);
      geode->addDrawable(CreateBackgroundRectangle(pos + osg::Vec3(-backgroundMargin, characterSize + backgroundMargin, 0),
         7 * characterSize + 2 * backgroundMargin,
         19 * characterSize + 2 * backgroundMargin,
         backgroundColor));

      // Camera scene & primitive stats static text
      osg::ref_ptr<osgText::Text> camStaticText = new osgText::Text;
      geode->addDrawable( camStaticText.get() );
      camStaticText->setColor(staticTextColor);
      camStaticText->setFont(font);
      camStaticText->setCharacterSize(characterSize);
      camStaticText->setPosition(pos);

      std::ostringstream viewStr;
      viewStr.clear();
      viewStr.setf(std::ios::left, std::ios::adjustfield);
      viewStr.width(14);
      viewStr << "Camera" << std::endl;
      viewStr << "" << std::endl; // placeholder for Camera name
      viewStr << "Lights" << std::endl;
      viewStr << "Bins" << std::endl;
      viewStr << "Depth" << std::endl;
      viewStr << "Matrices" << std::endl;
      viewStr << "Imposters" << std::endl;
      viewStr << "Drawables" << std::endl;
      viewStr << "Vertices" << std::endl;
      viewStr << "Points" << std::endl;
      viewStr << "Lines" << std::endl;
      viewStr << "Line strips" << std::endl;
      viewStr << "Line loops" << std::endl;
      viewStr << "Triangles" << std::endl;
      viewStr << "Tri. strips" << std::endl;
      viewStr << "Tri. fans" << std::endl;
      viewStr << "Quads" << std::endl;
      viewStr << "Quad strips" << std::endl;
      viewStr << "Polygons" << std::endl;
      viewStr.setf(std::ios::right,std::ios::adjustfield);
      camStaticText->setText(viewStr.str());

      // Move camera block to the right
      pos.x() += 7 * characterSize + 2 * backgroundMargin + backgroundSpacing;

      // Add camera scene stats, one block per camera
      int cameraCounter = 0;
      for(osgViewer::ViewerBase::Cameras::iterator citr = cameras.begin(); citr != cameras.end(); ++citr)
      {
         geode->addDrawable(CreateBackgroundRectangle(pos + osg::Vec3(-backgroundMargin, characterSize + backgroundMargin, 0),
            5 * characterSize + 2 * backgroundMargin,
            19 * characterSize + 2 * backgroundMargin,
            backgroundColor));

         // Camera scene stats
         osg::ref_ptr<osgText::Text> camStatsText = new osgText::Text;
         geode->addDrawable( camStatsText.get() );

         camStatsText->setColor(dynamicTextColor);
         camStatsText->setFont(font);
         camStatsText->setCharacterSize(characterSize);
         camStatsText->setPosition(pos);
         camStatsText->setText("");
         camStatsText->setDrawCallback(new CameraSceneStatsTextDrawCallback(*citr, cameraCounter));

         // Move camera block to the right
         pos.x() +=  5 * characterSize + 2 * backgroundMargin + backgroundSpacing;
         cameraCounter++;
      }
   }

   // Viewer scene stats
   {
      osg::Group* group = new osg::Group;
      mViewerSceneChildNum = mSwitch->getNumChildren();
      mSwitch->addChild(group, false);

      osg::Geode* geode = new osg::Geode();
      geode->setCullingActive(false);
      group->addChild(geode);

      geode->addDrawable(CreateBackgroundRectangle(pos + osg::Vec3(-backgroundMargin, characterSize + backgroundMargin, 0),
         6 * characterSize + 2 * backgroundMargin,
         12 * characterSize + 2 * backgroundMargin,
         backgroundColor));

      // View scene stats static text
      osg::ref_ptr<osgText::Text> camStaticText = new osgText::Text;
      geode->addDrawable( camStaticText.get() );
      camStaticText->setColor(staticTextColor);
      camStaticText->setFont(font);
      camStaticText->setCharacterSize(characterSize);
      camStaticText->setPosition(pos);

      std::ostringstream viewStr;
      viewStr.clear();
      viewStr.setf(std::ios::left, std::ios::adjustfield);
      viewStr.width(14);
      viewStr << "View" << std::endl;
      viewStr << " " << std::endl;
      viewStr << "Stateset" << std::endl;
      viewStr << "Group" << std::endl;
      viewStr << "Transform" << std::endl;
      viewStr << "LOD" << std::endl;
      viewStr << "Switch" << std::endl;
      viewStr << "Geode" << std::endl;
      viewStr << "Drawable" << std::endl;
      viewStr << "Geometry" << std::endl;
      viewStr << "Vertices" << std::endl;
      viewStr << "Primitives" << std::endl;
      viewStr.setf(std::ios::right, std::ios::adjustfield);
      camStaticText->setText(viewStr.str());

      // Move viewer block to the right
      pos.x() += 6 * characterSize + 2 * backgroundMargin + backgroundSpacing;

      std::vector<osgViewer::View*> views;
      viewer->getViews(views);

      std::vector<osgViewer::View*>::iterator it;
      int viewCounter = 0;
      for (it = views.begin(); it != views.end(); ++it)
      {
         geode->addDrawable(CreateBackgroundRectangle(pos + osg::Vec3(-backgroundMargin, characterSize + backgroundMargin, 0),
            10 * characterSize + 2 * backgroundMargin,
            12 * characterSize + 2 * backgroundMargin,
            backgroundColor));

         // Text for scene statistics
         osgText::Text* text = new  osgText::Text;
         geode->addDrawable( text );

         text->setColor(dynamicTextColor);
         text->setFont(font);
         text->setCharacterSize(characterSize);
         text->setPosition(pos);
         text->setDrawCallback(new ViewSceneStatsTextDrawCallback(*it, viewCounter));

         pos.x() += 10 * characterSize + 2 * backgroundMargin + backgroundSpacing;
         viewCounter++;
      }
   }

   // Delta Details stats
   {
      osg::Group* group = new osg::Group;
      mDeltaSystemChildNum = mSwitch->getNumChildren();
      mSwitch->addChild(group, false);

      osg::Geode* geode = new osg::Geode();
      group->addChild(geode);

      pos.y() -= characterSize*2.0f;
      osg::Vec4 colorDelta(1.0f,1.0f,0.4f,1.0f);
      osg::Vec4 colorTotal(1.0f,0.7f,0.1f,1.0f);
      osgText::Text* text;

      // MESSAGE_EVENT_TRAVERSAL
      pos.x() = leftPos;
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "Input Events: "); // label
      pos.x() = text->getBoundingBox().xMax();
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "0.0"); // Value
      text->setDrawCallback(new AveragedValueTextDrawCallback(stats,System::MESSAGE_EVENT_TRAVERSAL,-1, true, 1.0));
      pos.y() -= characterSize * 1.5f;

      // MESSAGE_POST_EVENT_TRAVERSAL
      pos.x() = leftPos;
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "Post Event: "); // label
      pos.x() = text->getBoundingBox().xMax();
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "0.0"); // Value
      text->setDrawCallback(new AveragedValueTextDrawCallback(stats,System::MESSAGE_POST_EVENT_TRAVERSAL,-1, true, 1.0));
      pos.y() -= characterSize * 1.5f;

      // MESSAGE_PRE_FRAME
      pos.x() = leftPos;
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "Pre Frame: "); // label
      pos.x() = text->getBoundingBox().xMax();
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "12.34  "); // Value
      text->setDrawCallback(new AveragedValueTextDrawCallback(stats,System::MESSAGE_PRE_FRAME,-1, true, 1.0));
      pos.x() = text->getBoundingBox().xMax();

      // The GameManager part of PreFrame - GM [Total: 12.34  Comps: 12.34 (99)  Actors: 12.34 (100/999)]
      // GM Total -- GM [Total: 12.34
      //pos.x() += 60.0f;
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "GM [Total: "); // label
      pos.x() = text->getBoundingBox().xMax();
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "12.34  "); // Value
      text->setDrawCallback(new AveragedValueTextDrawCallback(stats,"GMTotalTime",-1, true, 1.0));
      pos.x() = text->getBoundingBox().xMax(); /* 65.0f */ // "12.34" is about the right size
      // GM Components -- Comps: 12.34 (99)
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "Comps: "); // label
      pos.x() = text->getBoundingBox().xMax();
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "12.34 "); // Value
      text->setDrawCallback(new AveragedValueTextDrawCallback(stats,"GMComponentsTime",-1, true, 1.0));
      pos.x() = text->getBoundingBox().xMax(); // "12.34" is about the right size
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "(99)  "); // Value
      text->setDrawCallback(new SimpleIntTextDrawCallback(stats, "GMNumCompsProcessed", true));
      // GM Actors -- Actors: 12.34 (100/999) ]
      pos.x() = text->getBoundingBox().xMax(); /* 50.0f */ // "(99)" is about the right size
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "Actors: "); // label
      pos.x() = text->getBoundingBox().xMax();
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "12.34 "); // Value
      text->setDrawCallback(new AveragedValueTextDrawCallback(stats,"GMActorsTime",-1, true, 1.0));
      pos.x() = text->getBoundingBox().xMax(); // "12.34" is about the right size
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "("); // label
      pos.x() = text->getBoundingBox().xMax();
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "999 "); // Value
      text->setDrawCallback(new SimpleIntTextDrawCallback(stats, "GMNumActorsProcessed", false));
      pos.x() = text->getBoundingBox().xMax(); // "999" is about the right size
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "/"); // label
      pos.x() = text->getBoundingBox().xMax();
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "999 "); // Value
      text->setDrawCallback(new SimpleIntTextDrawCallback(stats, "GMTotalNumActors", false));
      pos.x() = text->getBoundingBox().xMax(); // "999" is about the right size
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, ") ]"); // label
      pos.y() -= characterSize * 1.5f;

      // MESSAGE_CAMERA_SYNCH
      pos.x() = leftPos;
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "Camera Sync: "); // label
      pos.x() = text->getBoundingBox().xMax();
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "0.0"); // Value
      text->setDrawCallback(new AveragedValueTextDrawCallback(stats,System::MESSAGE_CAMERA_SYNCH,-1, true, 1.0));
      pos.y() -= characterSize * 1.5f;

      // MESSAGE_FRAME_SYNCH
      pos.x() = leftPos;
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "Frame Sync: "); // label
      pos.x() = text->getBoundingBox().xMax();
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "0.0"); // Value
      text->setDrawCallback(new AveragedValueTextDrawCallback(stats,System::MESSAGE_FRAME_SYNCH,-1, true, 1.0));
      pos.y() -= characterSize * 1.5f;

      // MESSAGE_FRAME  --  Frame: 12.34  RenderTime: 12.34  Diff: 12.34
      pos.x() = leftPos;
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "Frame: "); // label
      pos.x() = text->getBoundingBox().xMax();
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "12.34 "); // Value
      text->setDrawCallback(new AveragedValueTextDrawCallback(stats,System::MESSAGE_FRAME,-1, true, 1.0));
      pos.x() = 15.0f + text->getBoundingBox().xMax(); // "12.34" is about the right size
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "UpdatePlusDraw: "); // label
      pos.x() = text->getBoundingBox().xMax();
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "12.34 "); // Value
      text->setDrawCallback(new AveragedValueTextDrawCallback(stats,"UpdatePlusDrawTime",-1, true, 1.0));
      pos.x() = 10.0f + text->getBoundingBox().xMax(); // "12.34" is about the right size
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "Diff: "); // label
      pos.x() = text->getBoundingBox().xMax();
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "12.34 "); // Value
      text->setDrawCallback(new AveragedValueTextDrawCallback(stats,"FrameMinusDrawAndUpdateTime",-1, true, 1.0));
      pos.y() -= characterSize * 1.5f;


      // MESSAGE_POST_FRAME
      pos.x() = leftPos;
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "Post Frame: "); // label
      pos.x() = text->getBoundingBox().xMax();
      text = CreateTextControl(geode, colorDelta, font, characterSize, pos, "0.0"); // Value
      text->setDrawCallback(new AveragedValueTextDrawCallback(stats,System::MESSAGE_POST_FRAME,-1, true, 1.0));
      pos.y() -= characterSize * 1.5f;

      // TOTAL
      pos.x() = leftPos;
      text = CreateTextControl(geode, colorTotal, font, characterSize, pos, "Total: "); // label
      pos.x() = text->getBoundingBox().xMax();
      text = CreateTextControl(geode, colorTotal, font, characterSize, pos, "0.0"); // Value
      text->setDrawCallback(new AveragedValueTextDrawCallback(stats,"FullDeltaFrameTime",-1, true, 1.0));
      pos.y() -= characterSize * 1.5f;

   }
}


////////////////////////////////////////////////////////////////////////////////
osgText::Text* StatsHandler::CreateTextControl(osg::Geode *geode, osg::Vec4& colorFR,
   const std::string& font, float characterSize,
   osg::Vec3& pos, const std::string &initialText)
{
   osg::ref_ptr<osgText::Text> newText = new osgText::Text;

   if (geode != NULL)
   {
      geode->addDrawable(newText.get());
   }
   newText->setColor(colorFR);
   newText->setFont(font);
   newText->setCharacterSize(characterSize);
   newText->setPosition(pos);
   newText->setText(initialText);

   return newText.get();
}

////////////////////////////////////////////////////////////////////////////////
osg::Node* StatsHandler::CreateCameraTimeStats(const std::string& font, osg::Vec3& pos, float startBlocks, bool aquireGPUStats, float characterSize, osg::Stats* viewerStats, osg::Camera* camera)
{
   osg::Stats* stats = camera->getStats();
   if (!stats) return 0;

   osg::Group* group = new osg::Group;

   osg::Geode* geode = new osg::Geode();
   group->addChild(geode);

   float leftPos = pos.x();

   osg::Vec4 colorCull( 0.0f,1.0f,1.0f,1.0f);
   osg::Vec4 colorCullAlpha( 0.0f,1.0f,1.0f,0.5f);
   osg::Vec4 colorDraw( 1.0f,1.0f,0.0f,1.0f);
   osg::Vec4 colorDrawAlpha( 1.0f,1.0f,0.0f,0.5f);
   osg::Vec4 colorGPU( 1.0f,0.5f,0.0f,1.0f);
   osg::Vec4 colorGPUAlpha( 1.0f,0.5f,0.0f,0.5f);

   {
      pos.x() = leftPos;

      osg::ref_ptr<osgText::Text> cullLabel = new osgText::Text;
      geode->addDrawable( cullLabel.get() );

      cullLabel->setColor(colorCull);
      cullLabel->setFont(font);
      cullLabel->setCharacterSize(characterSize);
      cullLabel->setPosition(pos);
      cullLabel->setText("Cull: ");

      pos.x() = cullLabel->getBoundingBox().xMax();

      osg::ref_ptr<osgText::Text> cullValue = new osgText::Text;
      geode->addDrawable( cullValue.get() );

      cullValue->setColor(colorCull);
      cullValue->setFont(font);
      cullValue->setCharacterSize(characterSize);
      cullValue->setPosition(pos);
      cullValue->setText("0.0");

      cullValue->setDrawCallback(new AveragedValueTextDrawCallback(stats,"Cull traversal time taken",-1, false, 1000.0));

      pos.x() = startBlocks;
      osg::Geometry* geometry = CreateGeometry(pos, characterSize *0.8, colorCullAlpha, mNumBlocks);
      geometry->setDrawCallback(new BlockDrawCallback(this, startBlocks, viewerStats, stats, "Cull traversal begin time", "Cull traversal end time", -1, mNumBlocks));
      geode->addDrawable(geometry);

      pos.y() -= characterSize*1.5f;
   }

   {
      pos.x() = leftPos;

      osg::ref_ptr<osgText::Text> drawLabel = new osgText::Text;
      geode->addDrawable( drawLabel.get() );

      drawLabel->setColor(colorDraw);
      drawLabel->setFont(font);
      drawLabel->setCharacterSize(characterSize);
      drawLabel->setPosition(pos);
      drawLabel->setText("Draw: ");

      pos.x() = drawLabel->getBoundingBox().xMax();

      osg::ref_ptr<osgText::Text> drawValue = new osgText::Text;
      geode->addDrawable( drawValue.get() );

      drawValue->setColor(colorDraw);
      drawValue->setFont(font);
      drawValue->setCharacterSize(characterSize);
      drawValue->setPosition(pos);
      drawValue->setText("0.0");

      drawValue->setDrawCallback(new AveragedValueTextDrawCallback(stats,"Draw traversal time taken",-1, false, 1000.0));


      pos.x() = startBlocks;
      osg::Geometry* geometry = CreateGeometry(pos, characterSize *0.8, colorDrawAlpha, mNumBlocks);
      geometry->setDrawCallback(new BlockDrawCallback(this, startBlocks, viewerStats, stats, "Draw traversal begin time", "Draw traversal end time", -1, mNumBlocks));
      geode->addDrawable(geometry);

      pos.y() -= characterSize*1.5f;
   }

   if (aquireGPUStats)
   {
      pos.x() = leftPos;

      osg::ref_ptr<osgText::Text> gpuLabel = new osgText::Text;
      geode->addDrawable( gpuLabel.get() );

      gpuLabel->setColor(colorGPU);
      gpuLabel->setFont(font);
      gpuLabel->setCharacterSize(characterSize);
      gpuLabel->setPosition(pos);
      gpuLabel->setText("GPU: ");

      pos.x() = gpuLabel->getBoundingBox().xMax();

      osg::ref_ptr<osgText::Text> gpuValue = new osgText::Text;
      geode->addDrawable( gpuValue.get() );

      gpuValue->setColor(colorGPU);
      gpuValue->setFont(font);
      gpuValue->setCharacterSize(characterSize);
      gpuValue->setPosition(pos);
      gpuValue->setText("0.0");

      gpuValue->setDrawCallback(new AveragedValueTextDrawCallback(stats,"GPU draw time taken",-1, false, 1000.0));

      pos.x() = startBlocks;
      osg::Geometry* geometry = CreateGeometry(pos, characterSize *0.8, colorGPUAlpha, mNumBlocks);
      geometry->setDrawCallback(new BlockDrawCallback(this, startBlocks, viewerStats, stats, "GPU draw begin time", "GPU draw end time", -1, mNumBlocks));
      geode->addDrawable(geometry);

      pos.y() -= characterSize*1.5f;
   }

   pos.x() = leftPos;

   return group;
}

////////////////////////////////////////////////////////////////////////////////
void StatsHandler::PrintOutStats( osgViewer::ViewerBase * viewer )
{
   if (viewer->getViewerStats())
   {
      osg::notify(osg::NOTICE)<<std::endl<<"Stats report:"<<std::endl;
      typedef std::vector<osg::Stats*> StatsList;
      StatsList statsList;
      statsList.push_back(viewer->getViewerStats());

      osgViewer::ViewerBase::Contexts contexts;
      viewer->getContexts(contexts);
      for(osgViewer::ViewerBase::Contexts::iterator gcitr = contexts.begin();
         gcitr != contexts.end();
         ++gcitr)
      {
         osg::GraphicsContext::Cameras& cameras = (*gcitr)->getCameras();
         for(osg::GraphicsContext::Cameras::iterator itr = cameras.begin();
            itr != cameras.end();
            ++itr)
         {
            if ((*itr)->getStats()) { statsList.push_back((*itr)->getStats()); }
         }
      }

      for(size_t i = viewer->getViewerStats()->getEarliestFrameNumber(); i <= viewer->getViewerStats()->getLatestFrameNumber() - 1; ++i)
      {
         for(StatsList::iterator itr = statsList.begin();
            itr != statsList.end();
            ++itr)
         {
            if (itr==statsList.begin()) (*itr)->report(osg::notify(osg::NOTICE), i);
            else (*itr)->report(osg::notify(osg::NOTICE), i, "    ");
         }
         osg::notify(osg::NOTICE)<<std::endl;
      }

   }
}

////////////////////////////////////////////////////////////////////////////////
}
