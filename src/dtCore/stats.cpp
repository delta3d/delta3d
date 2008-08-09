#include "prefix/dtcoreprefix-src.h"
#include <stdio.h>

#include <dtCore/stats.h>

#include <osgViewer/Renderer>
#include <osgViewer/View>

#include <osg/PolygonMode>
#include <osg/Geometry>

namespace dtCore
{

   StatsHandler::StatsHandler(osgViewer::ViewerBase &viewer ):
mStatsType(NO_STATS),
mInitialized(false),
mThreadingModel(osgViewer::ViewerBase::SingleThreaded),
mFrameRateChildNum(0),
mViewerChildNum(0),
mSceneChildNum(0),
mNumBlocks(8),
mBlockMultiplier(10000.0),
mViewer(&viewer)
{
   mCamera = new osg::Camera;
   mCamera->setRenderer(new osgViewer::Renderer(mCamera.get()));
   mCamera->setProjectionResizePolicy(osg::Camera::FIXED);
}

bool StatsHandler::SelectNextType()
{

   if (mViewer.valid() && mThreadingModelText.valid() && mViewer->getThreadingModel()!=mThreadingModel)
   {
      mThreadingModel = mViewer->getThreadingModel();
      UpdateThreadingModelText();
   }

   
   if (mViewer->getStats() == NULL) 
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
         mViewer->getStats()->collectStats("frame_rate",false);
         mViewer->getStats()->collectStats("event",false);
         mViewer->getStats()->collectStats("update",false);

         for(osgViewer::ViewerBase::Cameras::iterator itr = cameras.begin();
            itr != cameras.end();
            ++itr)
         {
            if ((*itr)->getStats()) (*itr)->getStats()->collectStats("rendering",false);
            if ((*itr)->getStats()) (*itr)->getStats()->collectStats("gpu",false);
         }

         mCamera->setNodeMask(0x0); 
         mSwitch->setAllChildrenOff();
         break;
      }
   case(FRAME_RATE):
      {
         mViewer->getStats()->collectStats("frame_rate",true);

         mCamera->setNodeMask(0xffffffff);
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

         mViewer->getStats()->collectStats("event",true);
         mViewer->getStats()->collectStats("update",true);

         for(osgViewer::ViewerBase::Cameras::iterator itr = cameras.begin();
            itr != cameras.end();
            ++itr)
         {
            if ((*itr)->getStats()) (*itr)->getStats()->collectStats("rendering",true);
            if ((*itr)->getStats()) (*itr)->getStats()->collectStats("gpu",true);
         }

         mCamera->setNodeMask(0xffffffff);
         mSwitch->setValue(mViewerChildNum, true);
         break;
      }
           
   default: break;
   }

   return true;

}

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

void StatsHandler::Reset()
{
   mInitialized = false;
   mCamera->setGraphicsContext(0);
}

void StatsHandler::SetUpHUDCamera(osgViewer::ViewerBase* viewer)
{
   osgViewer::GraphicsWindow* window = dynamic_cast<osgViewer::GraphicsWindow*>(mCamera->getGraphicsContext());
   osg::GraphicsContext* context;

   if (!window)
   {    
      osgViewer::ViewerBase::Windows windows;
      viewer->getWindows(windows);

      if (windows.empty()) return;

      window = windows.front();


      context = window;
   }

   mCamera->setGraphicsContext(context);

   mCamera->setViewport(0, 0, context->getTraits()->width, context->getTraits()->height);
   mCamera->setRenderOrder(osg::Camera::POST_RENDER, 10);

   mCamera->setProjectionMatrix(osg::Matrix::ortho2D(0,1280,0,1024));
   mCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   mCamera->setViewMatrix(osg::Matrix::identity());

   // only clear the depth buffer
   mCamera->setClearMask(0);

   mCamera->setRenderer(new osgViewer::Renderer(mCamera.get()));

   mInitialized = true;
}

struct TextDrawCallback : public virtual osg::Drawable::DrawCallback
{
   TextDrawCallback(osg::Stats* stats, const std::string& name, int frameDelta, bool averageInInverseSpace, double multiplier):
_stats(stats),
_attributeName(name),
_frameDelta(frameDelta),
_averageInInverseSpace(averageInInverseSpace),
_multiplier(multiplier),
_tickLastUpdated(0)
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

osg::ref_ptr<osg::Stats>     _stats;
std::string                 _attributeName;
int                         _frameDelta;
bool                        _averageInInverseSpace;
double                      _multiplier;
mutable char                _tmpText[128];
mutable osg::Timer_t        _tickLastUpdated;
};

struct BlockDrawCallback : public virtual osg::Drawable::DrawCallback
{
   BlockDrawCallback(StatsHandler* statsHandler, float xPos, osg::Stats* viewerStats, osg::Stats* stats, const std::string& beginName, const std::string& endName, int frameDelta, int numFrames):
_statsHandler(statsHandler), 
_xPos(xPos),
_viewerStats(viewerStats),
_stats(stats),
_beginName(beginName),
_endName(endName),
_frameDelta(frameDelta),
_numFrames(numFrames) {}

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
   FrameMarkerDrawCallback(StatsHandler* statsHandler, float xPos, osg::Stats* viewerStats, int frameDelta, int numFrames):
_statsHandler(statsHandler), 
_xPos(xPos),
_viewerStats(viewerStats),
_frameDelta(frameDelta),
_numFrames(numFrames) {}

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

   PagerCallback(osgDB::DatabasePager* dp, osgText::Text* minValue, osgText::Text* maxValue, osgText::Text* averageValue, double multiplier):
_dp(dp),
_minValue(minValue),
_maxValue(maxValue),
_averageValue(averageValue),
_multiplier(multiplier)
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
   }

   traverse(node,nv);
}

osg::observer_ptr<osgDB::DatabasePager> _dp;

osg::ref_ptr<osgText::Text> _minValue;
osg::ref_ptr<osgText::Text> _maxValue;
osg::ref_ptr<osgText::Text> _averageValue;
double _multiplier;
char                _tmpText[128];
osg::Timer_t        _tickLastUpdated;
};


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

osg::Geometry* StatsHandler::CreateTick(const osg::Vec3& pos, float height, const osg::Vec4& colour, unsigned int numTicks)
{
   osg::Geometry* geometry = new osg::Geometry;

   geometry->setUseDisplayList(false);

   osg::Vec3Array* vertices = new osg::Vec3Array;
   geometry->setVertexArray(vertices);
   vertices->reserve(numTicks*2);

   for(unsigned int i=0; i<numTicks; ++i)
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

void StatsHandler::SetUpScene(osgViewer::ViewerBase* viewer)
{
   mSwitch = new osg::Switch;

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
         const osg::Drawable::Extensions* extensions = osg::Drawable::getExtensions(contextID, false);
         if (extensions && extensions->isTimerQuerySupported())
         {
            ++numCamrasWithTimerQuerySupport;
         }
      }
   }

   bool aquireGPUStats = numCamrasWithTimerQuerySupport==cameras.size();

   float leftPos = 10.0f;
   float startBlocks = 150.0f;
   float characterSize = 20.0f;

   osg::Vec3 pos(leftPos,1000.0f,0.0f);

   osg::Vec4 colorFR(1.0f,1.0f,1.0f,1.0f);
   osg::Vec4 colorUpdate( 0.0f,1.0f,0.0f,1.0f);
   osg::Vec4 colorUpdateAlpha( 0.0f,1.0f,0.0f,0.5f);

   osg::Vec4 colorDP( 1.0f,1.0f,0.5f,1.0f);


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

      pos.x() = frameRateLabel->getBound().xMax();

      osg::ref_ptr<osgText::Text> frameRateValue = new osgText::Text;
      geode->addDrawable( frameRateValue.get() );

      frameRateValue->setColor(colorFR);
      frameRateValue->setFont(font);
      frameRateValue->setCharacterSize(characterSize);
      frameRateValue->setPosition(pos);
      frameRateValue->setText("0.0");

      frameRateValue->setDrawCallback(new TextDrawCallback(viewer->getStats(),"Frame rate",-1, true, 1.0));

      pos.y() -= characterSize*1.5f;

   }


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

      {
         pos.x() = leftPos;

         osg::ref_ptr<osgText::Text> eventLabel = new osgText::Text;
         geode->addDrawable( eventLabel.get() );

         eventLabel->setColor(colorUpdate);
         eventLabel->setFont(font);
         eventLabel->setCharacterSize(characterSize);
         eventLabel->setPosition(pos);
         eventLabel->setText("Event: ");

         pos.x() = eventLabel->getBound().xMax();

         osg::ref_ptr<osgText::Text> eventValue = new osgText::Text;
         geode->addDrawable( eventValue.get() );

         eventValue->setColor(colorUpdate);
         eventValue->setFont(font);
         eventValue->setCharacterSize(characterSize);
         eventValue->setPosition(pos);
         eventValue->setText("0.0");

         eventValue->setDrawCallback(new TextDrawCallback(viewer->getStats(),"Event traversal time taken",-1, false, 1000.0));

         pos.x() = startBlocks;
         osg::Geometry* geometry = CreateGeometry(pos, characterSize *0.8, colorUpdateAlpha, mNumBlocks);
         geometry->setDrawCallback(new BlockDrawCallback(this, startBlocks, viewer->getStats(), viewer->getStats(), "Event traversal begin time", "Event traversal end time", -1, mNumBlocks));
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

         pos.x() = updateLabel->getBound().xMax();

         osg::ref_ptr<osgText::Text> updateValue = new osgText::Text;
         geode->addDrawable( updateValue.get() );

         updateValue->setColor(colorUpdate);
         updateValue->setFont(font);
         updateValue->setCharacterSize(characterSize);
         updateValue->setPosition(pos);
         updateValue->setText("0.0");

         updateValue->setDrawCallback(new TextDrawCallback(viewer->getStats(),"Update traversal time taken",-1, false, 1000.0));

         pos.x() = startBlocks;
         osg::Geometry* geometry = CreateGeometry(pos, characterSize *0.8, colorUpdateAlpha, mNumBlocks);
         geometry->setDrawCallback(new BlockDrawCallback(this, startBlocks, viewer->getStats(), viewer->getStats(), "Update traversal begin time", "Update traversal end time", -1, mNumBlocks));
         geode->addDrawable(geometry);

         pos.y() -= characterSize*1.5f;
      }

      pos.x() = leftPos;


      // add camera stats
      for(osgViewer::ViewerBase::Cameras::iterator citr = cameras.begin();
         citr != cameras.end();
         ++citr)
      {
         group->addChild(CreateCameraStats(font, pos, startBlocks, aquireGPUStats, characterSize, viewer->getStats(), *citr));
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
         frameMarkers->setDrawCallback(new FrameMarkerDrawCallback(this, startBlocks, viewer->getStats(), 0, mNumBlocks + 1));
         geode->addDrawable(frameMarkers);
      }


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
            pos.y() -= characterSize*1.5f;

            pos.x() = leftPos;

            osg::ref_ptr<osgText::Text> averageLabel = new osgText::Text;
            geode->addDrawable( averageLabel.get() );

            averageLabel->setColor(colorDP);
            averageLabel->setFont(font);
            averageLabel->setCharacterSize(characterSize);
            averageLabel->setPosition(pos);
            averageLabel->setText("DatabasePager time to merge new tiles - average: ");

            pos.x() = averageLabel->getBound().xMax();

            osg::ref_ptr<osgText::Text> averageValue = new osgText::Text;
            geode->addDrawable( averageValue.get() );

            averageValue->setColor(colorDP);
            averageValue->setFont(font);
            averageValue->setCharacterSize(characterSize);
            averageValue->setPosition(pos);
            averageValue->setText("1000");

            pos.x() = averageValue->getBound().xMax() + 2.0f*characterSize;


            osg::ref_ptr<osgText::Text> minLabel = new osgText::Text;
            geode->addDrawable( minLabel.get() );

            minLabel->setColor(colorDP);
            minLabel->setFont(font);
            minLabel->setCharacterSize(characterSize);
            minLabel->setPosition(pos);
            minLabel->setText("min: ");

            pos.x() = minLabel->getBound().xMax();

            osg::ref_ptr<osgText::Text> minValue = new osgText::Text;
            geode->addDrawable( minValue.get() );

            minValue->setColor(colorDP);
            minValue->setFont(font);
            minValue->setCharacterSize(characterSize);
            minValue->setPosition(pos);
            minValue->setText("1000");

            pos.x() = minValue->getBound().xMax() + 2.0f*characterSize;


            osg::ref_ptr<osgText::Text> maxLabel = new osgText::Text;
            geode->addDrawable( maxLabel.get() );

            maxLabel->setColor(colorDP);
            maxLabel->setFont(font);
            maxLabel->setCharacterSize(characterSize);
            maxLabel->setPosition(pos);
            maxLabel->setText("max: ");

            pos.x() = maxLabel->getBound().xMax();

            osg::ref_ptr<osgText::Text> maxValue = new osgText::Text;
            geode->addDrawable( maxValue.get() );

            maxValue->setColor(colorDP);
            maxValue->setFont(font);
            maxValue->setCharacterSize(characterSize);
            maxValue->setPosition(pos);
            maxValue->setText("1000");

            pos.x() = maxLabel->getBound().xMax();

            geode->setCullCallback(new PagerCallback(dp, minValue.get(), maxValue.get(), averageValue.get(), 1000.0));
         }

         pos.x() = leftPos;
      }
   }
#if 0
   // scene stats
   {
      pos.x() = leftPos;

      osg::Geode* geode = new osg::Geode();

      {
         osgText::Text* text = new  osgText::Text;
         geode->addDrawable( text );

         text->setFont(font);
         text->setCharacterSize(characterSize);
         text->setPosition(pos);
         text->setText("Scene Stats to do...");

         pos.y() -= characterSize*1.5f;
      }    

      _sceneChildNum = _switch->getNumChildren();
      _switch->addChild(geode, false);
   }
#endif    
}

osg::Node* StatsHandler::CreateCameraStats(const std::string& font, osg::Vec3& pos, float startBlocks, bool aquireGPUStats, float characterSize, osg::Stats* viewerStats, osg::Camera* camera)
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

      pos.x() = cullLabel->getBound().xMax();

      osg::ref_ptr<osgText::Text> cullValue = new osgText::Text;
      geode->addDrawable( cullValue.get() );

      cullValue->setColor(colorCull);
      cullValue->setFont(font);
      cullValue->setCharacterSize(characterSize);
      cullValue->setPosition(pos);
      cullValue->setText("0.0");

      cullValue->setDrawCallback(new TextDrawCallback(stats,"Cull traversal time taken",-1, false, 1000.0));

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

      pos.x() = drawLabel->getBound().xMax();

      osg::ref_ptr<osgText::Text> drawValue = new osgText::Text;
      geode->addDrawable( drawValue.get() );

      drawValue->setColor(colorDraw);
      drawValue->setFont(font);
      drawValue->setCharacterSize(characterSize);
      drawValue->setPosition(pos);
      drawValue->setText("0.0");

      drawValue->setDrawCallback(new TextDrawCallback(stats,"Draw traversal time taken",-1, false, 1000.0));


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

      pos.x() = gpuLabel->getBound().xMax();

      osg::ref_ptr<osgText::Text> gpuValue = new osgText::Text;
      geode->addDrawable( gpuValue.get() );

      gpuValue->setColor(colorGPU);
      gpuValue->setFont(font);
      gpuValue->setCharacterSize(characterSize);
      gpuValue->setPosition(pos);
      gpuValue->setText("0.0");

      gpuValue->setDrawCallback(new TextDrawCallback(stats,"GPU draw time taken",-1, false, 1000.0));

      pos.x() = startBlocks;
      osg::Geometry* geometry = CreateGeometry(pos, characterSize *0.8, colorGPUAlpha, mNumBlocks);
      geometry->setDrawCallback(new BlockDrawCallback(this, startBlocks, viewerStats, stats, "GPU draw begin time", "GPU draw end time", -1, mNumBlocks));
      geode->addDrawable(geometry);

      pos.y() -= characterSize*1.5f;
   }


   pos.x() = leftPos;

   return group;
}



void StatsHandler::PrintOutStats( osgViewer::ViewerBase * viewer )
{
   if (viewer->getStats())
   {
      osg::notify(osg::NOTICE)<<std::endl<<"Stats report:"<<std::endl;
      typedef std::vector<osg::Stats*> StatsList;
      StatsList statsList;
      statsList.push_back(viewer->getStats());

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
            if ((*itr)->getStats())
            {
               statsList.push_back((*itr)->getStats());
            }
         }
      }

      for(int i = viewer->getStats()->getEarliestFrameNumber(); i<= viewer->getStats()->getLatestFrameNumber()-1; ++i)
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


}
