#include "dtCore/stats.h"
#include <osg/BlendFunc>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/Projection>
#include <osg/Switch>

using namespace osg;
using namespace osgUtil;
using namespace std;
using namespace dtCore;

//------------------------------------------------------------------
// Stats
//------------------------------------------------------------------
Stats::Stats(osgUtil::SceneView *sv)
{
   mPrintStats = Statistics::STAT_NONE;            // gwm change from bool was : false;
   mInitialTick = mTimer.tick();
   mFrameTick = mInitialTick;
   mFrameRate=0;                   // added by gwm to display fram Rate smoothed
   mSV = sv;
}


//------------------------------------------------------------------
// Stats::init
//------------------------------------------------------------------
void Stats::Init(osgUtil::RenderStage* stg)
{
  mStage = stg;
  UpdateFrameTick();
}


//------------------------------------------------------------------
// Stats::setTime
//------------------------------------------------------------------
void Stats::SetTime(int type)
{
  mRegTimes[type] = mTimer.tick();
}

double Stats::GetTime(int type)
{
	return mTimer.delta_m(mRegTimes[TIME_BEFORE_APP], mRegTimes[TIME_AFTER_APP]) +
		   mTimer.delta_m(mRegTimes[TIME_BEFORE_CULL], mRegTimes[TIME_AFTER_CULL]) +
	       mTimer.delta_m(mRegTimes[TIME_BEFORE_DRAW], mRegTimes[TIME_AFTER_DRAW]);
}


//------------------------------------------------------------------
// Stats::draw
//------------------------------------------------------------------
void Stats::Draw()
{
  times[2].timeApp=0.0f;
  times[2].timeCull=0.0f;
  times[2].timeDraw=0.0f;

  times[2].timeApp  += mTimer.delta_m(mRegTimes[TIME_BEFORE_APP], mRegTimes[TIME_AFTER_APP]);
  times[2].timeCull += mTimer.delta_m(mRegTimes[TIME_BEFORE_CULL], mRegTimes[TIME_AFTER_CULL]);
  times[2].timeDraw += mTimer.delta_m(mRegTimes[TIME_BEFORE_DRAW], mRegTimes[TIME_AFTER_DRAW]);

  // absolute time
  times[2].frameend=UpdateFrameTick();

  times[2].timeFrame=FrameSeconds()*1000;

  if (mPrintStats)
  {
    ShowStats();              // output selected stats at this point - timing graph.
    times[0]=times[1];        // move the times buffers down
    times[1]=times[2];
  }
} 


//------------------------------------------------------------------
// Stats::updateFrameTick
//------------------------------------------------------------------
// update time from the current frame update and the previous one.
osg::Timer_t Stats::UpdateFrameTick()
{
  mLastFrameTick = mFrameTick;
  mFrameTick = mTimer.tick();
  return mFrameTick;
}


//Select the next statistics type - will wrap around back to NONE
void Stats::SelectNextType(void)
{
   Statistics::statsType type;

   switch (mPrintStats) {
   case Statistics::STAT_NONE: type = Statistics::STAT_FRAMERATE;  	break;
   case Statistics::STAT_FRAMERATE: type = Statistics::STAT_GRAPHS; 	break;
   case Statistics::STAT_GRAPHS:  type = Statistics::STAT_PRIMS;  	break;
   case Statistics::STAT_PRIMS: type = Statistics::STAT_PRIMSPERVIEW;    	break;
   case Statistics::STAT_PRIMSPERVIEW: type = Statistics::STAT_PRIMSPERBIN;  break;
   case Statistics::STAT_PRIMSPERBIN: type = Statistics::STAT_DC;   break;
   case Statistics::STAT_DC: type = Statistics::STAT_NONE;      break;
   case Statistics::STAT_RESTART: type = Statistics::STAT_NONE; break;
   default:
      break;
   }
   
   SelectType(type);
}


//------------------------------------------------------------------
// Stats::selectType
//------------------------------------------------------------------
void Stats::SelectType(osgUtil::Statistics::statsType type)
{

   if (!mProjection.valid()) InitTexts();

   if (mProjection->getNumParents() == 0)
   {
      osg::Group *gr = (osg::Group*)mSV->getSceneData();
      if (gr) gr->addChild(mProjection.get());
   }

   mPrintStats = type;

   // switch off stencil counting
   if (mPrintStats==Statistics::STAT_DC) 
      glDisable(GL_STENCIL_TEST);

   if (mPrintStats>=Statistics::STAT_RESTART) 
   {
      mPrintStats=Statistics::STAT_NONE;
   }

   // skip over these stats
   if ((mPrintStats==Statistics::STAT_PRIMSPERVIEW) ||
      (mPrintStats==Statistics::STAT_PRIMSPERBIN))
   {
      mPrintStats = Statistics::STAT_DC;
   }

   // count depth complexity by incrementing the stencil buffer every
   if (mPrintStats==Statistics::STAT_DC)
   {
      // time a pixel is hit
      GLint nsten=0;        // Number of stencil planes available
      glGetIntegerv(GL_STENCIL_BITS , &nsten);
      if (nsten>0)
      {
         glEnable(GL_STENCIL_TEST);
         glStencilOp(GL_INCR ,GL_INCR ,GL_INCR);
      }                     // skip this option
      else
         mPrintStats++;
   }

   EnableTextNodes(mPrintStats);
} 



//------------------------------------------------------------------
// Stats::showStats
//------------------------------------------------------------------
void Stats::ShowStats()
{                             // collect stats for viewport
  static int maxbins=1;       // count number of bins
  static GLfloat tmax=222;

  if (mPrintStats >= osgUtil::Statistics::STAT_RESTART) return;

  int x,y,width,height;
  mSV->getViewport(x,y, width, height);

  glViewport(0,0,width,height);

  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  // set tmax using hysteresis to prevent flip-flopping between two values of tmax
  // tmax is the scale for the mStage timing graph
  if (times[2].timeFrame>360.0f && tmax<1600) tmax=1600;
  else if (times[2].timeFrame<300.0f && tmax>800) tmax=800;
  else if (times[2].timeFrame>180.0f && tmax<800) tmax=800;
  else if (times[2].timeFrame<150.0f && tmax>400) tmax=400;
  else if (times[2].timeFrame>90.0f && tmax<400) tmax=400;
  else if (times[2].timeFrame<75.0f && tmax>200) tmax=200;
  else if (times[2].timeFrame>45.0f && tmax<200) tmax=200;
  else if (times[2].timeFrame<36.0f && tmax>100) tmax=100;
  glOrtho(-0.1f*tmax, tmax*1.1f,0,height,0,500);
  glDepthRange(0,1);

  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  glDisable(GL_STENCIL_TEST); // dont want to count pixels set by performance graphs

  if (mPrintStats>0)          // output the text frame rate
  {
    char clin[72];            // buffer to print

    if (mFrameRate>10.0f)
    {
      float smoothRatio = 0.3;// should be >0 and <= 1.0,
                              // lower the value greater smoothing.
                              // smooth out variations in frame rate
      mFrameRate=(1.0f-smoothRatio)*mFrameRate+smoothRatio*FrameRate();
    }
    else
    {
      mFrameRate=FrameRate();     // frame rate so slow no need to smooth in frame rate
    }
    sprintf(clin,"%.1f Hz.", mFrameRate);
    mFrameRateCounterText->setText(clin);
  }
                              // more stats - graphs this time
  if (mPrintStats>=Statistics::STAT_GRAPHS  && 
      mPrintStats!=Statistics::STAT_PRIMSPERVIEW  && 
      mPrintStats!=Statistics::STAT_PRIMSPERBIN)
  {

    int sampleIndex = 2;
    float timeApp=times[sampleIndex].timeApp;
    float timeCull=times[sampleIndex].timeCull;
    float timeDraw=times[sampleIndex].timeDraw;
    float timeFrame=times[sampleIndex].timeFrame;

    osg::Vec4 app_color(0.0f,1.0f,0.0f,1.0f);
    osg::Vec4 cull_color(1.0f,0.0f,1.0f,1.0f);
    osg::Vec4 draw_color(0.0f,1.0f,1.0f,1.0f);
    osg::Vec4 swap_color(1.0f,0.5f,0.5f,1.0f);

    char clin[72];            // buffer to print
    sprintf(clin,"App %.2f ms.", timeApp);
    mUpdateTimeText->setText(clin);

    sprintf(clin,"Cull %.2f ms.", timeCull);
    mCullTimeText->setText(clin);

    sprintf(clin,"Draw %.2f ms.", timeDraw);
    mDrawTimeText->setText(clin);

    sprintf(clin,"Frame %.2f ms.", timeFrame);
    mFrameRateTimeText->setText(clin);

    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP );
    // yellow frame/grid in front of stats, units of .01 sec(?)
    glColor3f(.6f,.6f,0.0f);
    glVertex2f(0.0,0.97f*height);
    glVertex2f(tmax,0.97f*height);
    glVertex2f(tmax,0.88f*height);
    glVertex2f(0.0,0.88f*height);
    glEnd();
    glBegin(GL_LINES);
    // time marks
    int i;
    for (i=10; i<tmax; i+=10)
    {
                              // red mark every 0.1 sec
      if ((i%100) == 0) glColor3f(.6f,.0f,0.0f);
                              // yellow every 0.01 sec
      else glColor3f(.6f,.6f,0.0f);
      glVertex2f(i,0.97f*height);
      glVertex2f(i,0.88f*height);
    }
    // plot time for app, cull, draw, frame...
    float tstart=0;
    for (i=0; i<3; i++)
    {
      glColor4fv((GLfloat * )&app_color);
      glVertex2f(tstart,0.95f*height);
      glVertex2f(tstart+times[i].timeApp,0.95f*height);
      glColor4fv((GLfloat * )&cull_color);
      glVertex2f(tstart+times[i].timeApp,0.93f*height);
      glVertex2f(tstart+times[i].timeApp+times[i].timeCull, 0.93f*height);
      glColor4fv((GLfloat * )&draw_color);
      glVertex2f(tstart+times[i].timeApp+times[i].timeCull, 0.91f*height);
      glVertex2f(tstart+times[i].timeApp+times[i].timeCull+times[i].timeDraw, 0.91f*height);
      glColor4fv((GLfloat * )&swap_color);
      glVertex2f(tstart+times[i].timeApp+times[i].timeCull+times[i].timeDraw, 0.90f*height);
      glVertex2f(tstart+times[i].timeFrame, 0.90f*height);
      tstart+=times[i].timeFrame;
    }
    glEnd();
    glLineWidth(1.0f);
  }
                              // yet more stats - add triangles, number of strips...
  if (mPrintStats==Statistics::STAT_PRIMS)
  {
    /* 
     * Use the new renderStage.  Required mods to RenderBin.cpp, and RenderStage.cpp (add getPrims)
     * also needed to define a new class called Statistic (see osgUtil/Statistic).
     * RO, July 2001.
     */
    Statistics primStats;
    mStage->getPrims(&primStats);
                            // full print out required
    primStats.setType(Statistics::STAT_PRIMS);
    WritePrims((int)(0.86f*1024),primStats);
    maxbins=(primStats.getBins()>maxbins)?primStats.getBins():maxbins;
 
  }
                              // more stats - add triangles, number of strips... as seen per bin
  if (mPrintStats==Statistics::STAT_PRIMSPERBIN)
  {
    /* 
     * Use the new renderStage.  Required mods to RenderBin.cpp, and RenderStage.cpp (add getPrims)
     * also needed to define a new class called Statistic (see osgUtil/Statistic).
     * RO, July 2001.
     */
                              // array of bin stats
    Statistics *primStats=new Statistics[maxbins];
    mStage->getPrims(primStats, maxbins);

    int nbinsUsed=(primStats[0].getBins()<maxbins)?primStats[0].getBins():maxbins;
    int ntop=0;               // offset
    for (int i=0; i<nbinsUsed; i++)
    {
                              // cuts out vertices & triangles to save space on screen
      primStats[i].setType(Statistics::STAT_PRIMSPERBIN);
      ntop+=WritePrims((int)(0.86f*1024-ntop),primStats[i]);
    }
    maxbins=(primStats[0].getBins()>maxbins)?primStats[0].getBins():maxbins;
    delete [] primStats;   // free up
  }
                              // yet more stats - read the depth complexity
  if (mPrintStats==Statistics::STAT_DC)
  {
    int wid=width, ht=height;      // temporary local screen size - must change during this section
    if (wid>0 && ht>0)
    {
      const int blsize=16;
                              // buffer to print dc
      char *clin=new char[wid/blsize+2];
      char ctext[128];        // buffer to print details
      float mdc=0;
      GLubyte *buffer=new GLubyte[wid*ht];
      if (buffer)
      {
                              // no extra bytes at ends of rows- easier to analyse
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glColor3f(.9f,.9f,0.0f);
        glReadPixels(0,0,wid,ht, GL_STENCIL_INDEX ,GL_UNSIGNED_BYTE, buffer);
                              // break up screen into blsize*blsize pixel blocks
        for (int j=0; j<ht; j+=blsize)
        {
          char *clpt=clin;    // moves across the clin to display lines of text
                              // horizontal pixel blocks
          for (int i=0; i<wid; i+=blsize)
          {
            int dc=0;
            int nav=0;        // number of pixels averaged for DC calc
            for (int jj=j; jj<j+blsize; jj++)
            {
              for (int ii=i; ii<i+blsize; ii++)
              {
                if (jj<ht && ii<wid && jj>=0 && ii>=0)
                {
                  dc+=buffer[ii+ (ht-jj-1)*wid];
                  nav++;
                }
              }
            }
            mdc+=dc;
                              // fine detail in dc=[0,1]; 0.1 increment in display, space for empty areas
            if (dc<nav) *clpt= ' '+(10*dc)/nav;
                              // show 1-9 for DC=1-9; then ascii to 127
            else if (dc<80*nav) *clpt= '0'+dc/nav;
            else *clpt= '+';  // too large a DC - use + to show over limit
            clpt++;
          }
          *clpt='\0';
                              // display average DC over the blsize box
/**/      //displaytext(0,(int)(0.84f*vh-(j*12)/blsize),clin);
        }
        sprintf(ctext, "Pixels hit %.1f Mean DC %.2f: %4d by %4d pixels.", mdc, mdc/(wid*ht), wid, ht);
/**/    //displaytext(0,(int)(0.86f*vh),ctext);
        osg::Vec3 pos(0.f, 0.86f*1024, 0.f);
        mDcText->setPosition(pos);
        mDcText->setText(ctext);

                              // re-enable stencil buffer counting
        glEnable(GL_STENCIL_TEST);
        delete [] buffer;
      }
      delete [] clin;
    }
  }

  glMatrixMode( GL_MODELVIEW );
  glPopMatrix();

  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
}

int Stats::WritePrims( const int ypos, osgUtil::Statistics& stats)
{
    char clin[128]; // buffer to print
    char ctmp[32];
    char intro[32]; // start of first line
    int npix=10; // offset from ypos
    static char *prtypes[]=
    {
        "  Point", // GL_POINTS               0x0000
        "  Lines", // GL_LINES                0x0001
        " LnLoop", // GL_LINE_LOOP            0x0002
        "  LnStr", // GL_LINE_SRIP            0x0002
        "   Tris", // GL_TRIANGLES            0x0004 
        " TriStr", // GL_TRIANGLE_STRIP       0x0005
        " TriFan", // GL_TRIANGLE_FAN         0x0006
        "  Quads", // GL_QUADS                0x0007
        " QudStr", // GL_QUAD_STRIP           0x0008
        "   Poly"  // GL_POLYGON              0x0009
    };         
            
    glColor3f(.9f,.9f,0.0f);

    if (stats.depth==0) sprintf(intro,"General Stats: ");
    else sprintf(intro,"==> Bin %2d", stats._binNo);
    sprintf(clin,"%s  %d Drawables   %d Lights   %d Bins   %d Impostors", 
        intro ,stats.numDrawables, stats.nlights, stats.nbins, stats.nimpostor);

    osg::Vec3 pos(0, ypos-npix, 0.0f);
    mPrimTotalsText->setPosition(pos);
    mPrimTotalsText->setText(clin);

    npix+=34;
    
    strcpy(clin,"              Total");
    
    unsigned int totalNumPrimives = 0;
    unsigned int totalNumIndices = 0;
    unsigned int totalNumTriangles = 0;
    osgUtil::Statistics::PrimtiveValueMap::iterator pItr;
    for(pItr=stats._primitiveCount.begin(); pItr!=stats._primitiveCount.end(); ++pItr)
    {
        totalNumPrimives += (pItr->second.first);
        totalNumIndices += (pItr->second.second);
		switch(pItr->first)
		{
		case 4: //Tris
			totalNumTriangles += pItr->second.second / 3; break;
		case 5: //TriStr
		case 6: //TriFan
		case 8: //QudStr
		case 9: //Poly
			totalNumTriangles += pItr->second.second - 2; break;
		case 7: //Quads
			totalNumTriangles += pItr->second.second / 4; break;
		}
        strcat(clin, prtypes[pItr->first]);
    }

    mPrimTypesText->setText(clin);
    pos.set(0, ypos-npix,0);
    mPrimTypesText->setPosition(pos);

    npix+=30;
    
    strcpy(clin,"Primitives: ");
    sprintf(ctmp,"%7d", totalNumPrimives);
    strcat(clin, ctmp);
    for(pItr=stats._primitiveCount.begin(); pItr!=stats._primitiveCount.end(); ++pItr)
    {
        sprintf(ctmp,"%7d", pItr->second.first);
        strcat(clin, ctmp);
    }

    pos.set(0, ypos-npix, 0);
    mPrimText->setPosition(pos);
    mPrimText->setText(clin);

    npix+=30;

    strcpy(clin,"Vertices:   ");
    sprintf(ctmp,"%7d", totalNumIndices);
    strcat(clin, ctmp);
    for(pItr=stats._primitiveCount.begin(); pItr!=stats._primitiveCount.end(); ++pItr)
    {
        sprintf(ctmp,"%7d", pItr->second.second);
        strcat(clin, ctmp);
    }

    pos.set(0.f, ypos-npix, 0.f);
    mVerticesText->setPosition(pos);
    mVerticesText->setText(clin);

    npix+=30;

    strcpy(clin,"Triangles:  ");
    sprintf(ctmp,"%7d", totalNumTriangles);
    strcat(clin, ctmp);
    unsigned int triangles = 0;
    for(pItr=stats._primitiveCount.begin(); pItr!=stats._primitiveCount.end(); ++pItr)
    {
		switch(pItr->first)
		{
		case 4: //Tris
			triangles = pItr->second.second / 3; break;
		case 5: //TriStr
		case 6: //TriFan
		case 8: //QudStr
		case 9: //Poly
			triangles = pItr->second.second - 2; break;
		case 7: //Quads
			triangles = pItr->second.second / 4; break;
		default:
			triangles = 0; break;
		}
        sprintf(ctmp,"%7d", triangles);
        strcat(clin, ctmp);
    }

    pos.set(0.f, ypos-npix, 0.f);
    mTrianglesText->setPosition(pos);
    mTrianglesText->setText(clin);

    npix+=30;

    return npix+20;
}


void Stats::InitTexts()
{
   float characterSize = 20.0f;

   osg::Vec4 colorFR(1.0f,1.0f,1.0f,1.0f);
   osg::Vec4 colorUpdate( 0.0f,1.0f,0.0f,1.0f);
   osg::Vec4 colorCull(1.0f,0.0f,1.0f,1.0f);
   osg::Vec4 colorDraw( 0.0f,1.0f, 1.0f,1.0f);
   osg::Vec4 swapColor(1.0f,0.5f,0.5f,1.0f);
   osg::Vec4 colorPrimTotal(.9f,.9f,0.0f, 1.f);

   float leftPos = 0.0f;

   osg::Vec3 pos(leftPos,1000.f,0.0f);

   mSwitch = new osg::Switch();

   osg::StateSet* stateset = mSwitch->getOrCreateStateSet();
   stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
   stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
   stateset->setRenderBinDetails(11,"RenderBin");

   {//#0
      osg::Geode* geode = new osg::Geode();
      mFrameRateCounterText = new osgText::Text;
      mFrameRateCounterText->setFont("fonts/arial.ttf");
      mFrameRateCounterText->setColor(colorFR);
      mFrameRateCounterText->setPosition(pos);
      mFrameRateCounterText->setAlignment(osgText::Text::BASE_LINE);
      mFrameRateCounterText->setText("0123456789.");
      geode->addDrawable( mFrameRateCounterText.get() );
      mSwitch->addChild(geode, false);
   }

   {//#1
      osg::Geode* geode = new osg::Geode();
      pos.set(leftPos+190.f, 1000.f, 0.f);
      mUpdateTimeText = new osgText::Text;
      mUpdateTimeText->setFont("fonts/arial.ttf");
      mUpdateTimeText->setColor(colorUpdate);
      mUpdateTimeText->setFontResolution((unsigned int)characterSize,(unsigned int)characterSize);
      mUpdateTimeText->setPosition(pos);
      mUpdateTimeText->setAlignment(osgText::Text::BASE_LINE);
      mUpdateTimeText->setText("0123456789.");
      geode->addDrawable( mUpdateTimeText.get() );
      mSwitch->addChild(geode, false);
   }

   {//#2
      osg::Geode* geode = new osg::Geode();
      pos.set(leftPos+450.f, 1000.f, 0.f);
      mCullTimeText = new osgText::Text;
      mCullTimeText->setFont("fonts/arial.ttf");
      mCullTimeText->setColor(colorCull);
      mCullTimeText->setFontResolution((unsigned int)characterSize,(unsigned int)characterSize);
      mCullTimeText->setPosition(pos);
      mCullTimeText->setAlignment(osgText::Text::BASE_LINE);
      mCullTimeText->setText("0123456789.");
      geode->addDrawable( mCullTimeText.get() );
      mSwitch->addChild(geode, false);
   }

   {//#3
      osg::Geode* geode = new osg::Geode();
      pos.set(leftPos+700.f, 1000.f, 0.f);
      mDrawTimeText = new osgText::Text;
      mDrawTimeText->setFont("fonts/arial.ttf");
      mDrawTimeText->setColor(colorDraw);
      mDrawTimeText->setFontResolution((unsigned int)characterSize,(unsigned int)characterSize);
      mDrawTimeText->setPosition(pos);
      mDrawTimeText->setAlignment(osgText::Text::BASE_LINE);
      mDrawTimeText->setText("1000.00");
      geode->addDrawable( mDrawTimeText.get() );
      mSwitch->addChild(geode, false);
   }

   {//#4
      osg::Geode* geode = new osg::Geode();
      pos.set(leftPos+960.f, 1000.f, 0.f);
      mFrameRateTimeText = new osgText::Text;
      mFrameRateTimeText->setFont("fonts/arial.ttf");
      mFrameRateTimeText->setColor(swapColor);
      mFrameRateTimeText->setFontResolution((unsigned int)characterSize,(unsigned int)characterSize);
      mFrameRateTimeText->setPosition(pos);
      mFrameRateTimeText->setAlignment(osgText::Text::BASE_LINE);
      mFrameRateTimeText->setText("0123456789.");
      geode->addDrawable( mFrameRateTimeText.get() );
      mSwitch->addChild(geode, false);
   }

   { //#5
      osg::Geode* geode = new osg::Geode();
      pos.set(leftPos, 880.f, 0.f);
      mPrimTotalsText = new osgText::Text;
      mPrimTotalsText->setFont("fonts/COURBD.TTF");
      mPrimTotalsText->setColor(colorPrimTotal);
      mPrimTotalsText->setFontResolution((unsigned int)characterSize,(unsigned int)characterSize);
      mPrimTotalsText->setPosition(pos);
      mPrimTotalsText->setAlignment(osgText::Text::BASE_LINE);
      mPrimTotalsText->setText("0123456789.");
      geode->addDrawable( mPrimTotalsText.get() );

      pos.set(leftPos, 880.f, 0.f);
      mPrimTypesText = new osgText::Text;
      mPrimTypesText->setFont("fonts/COURBD.TTF");
      mPrimTypesText->setColor(colorPrimTotal);
      mPrimTypesText->setFontResolution((unsigned int)characterSize,(unsigned int)characterSize);
      mPrimTypesText->setPosition(pos);
      mPrimTypesText->setAlignment(osgText::Text::BASE_LINE);
      mPrimTypesText->setText("0123456789.");
      geode->addDrawable( mPrimTypesText.get() );

      pos.set(leftPos, 880.f, 0.f);
      mPrimText = new osgText::Text;
      mPrimText->setFont("fonts/COURBD.TTF");
      mPrimText->setColor(colorPrimTotal);
      mPrimText->setFontResolution((unsigned int)characterSize,(unsigned int)characterSize);
      mPrimText->setPosition(pos);
      mPrimText->setAlignment(osgText::Text::BASE_LINE);
      mPrimText->setText("0123456789.");
      geode->addDrawable( mPrimText.get() );

      mVerticesText = new osgText::Text;
      mVerticesText->setFont("fonts/COURBD.TTF");
      mVerticesText->setColor(colorPrimTotal);
      mVerticesText->setFontResolution((unsigned int)characterSize,(unsigned int)characterSize);
      mVerticesText->setPosition(pos);
      mVerticesText->setAlignment(osgText::Text::BASE_LINE);
      mVerticesText->setText("0123456789.");
      geode->addDrawable( mVerticesText.get() );

      mTrianglesText = new osgText::Text;
      mTrianglesText->setFont("fonts/COURBD.TTF");
      mTrianglesText->setColor(colorPrimTotal);
      mTrianglesText->setFontResolution((unsigned int)characterSize,(unsigned int)characterSize);
      mTrianglesText->setPosition(pos);
      mTrianglesText->setAlignment(osgText::Text::BASE_LINE);
      mTrianglesText->setText("0123456789.");
      geode->addDrawable( mTrianglesText.get() );

      mSwitch->addChild(geode, false);
   }

   {//#6
      osg::Geode* geode = new osg::Geode();
      pos.set(leftPos+960.f, 1000.f, 0.f);
      mDcText = new osgText::Text;
      mDcText->setFont("fonts/arial.ttf");
      mDcText->setColor(colorPrimTotal);
      mDcText->setFontResolution((unsigned int)characterSize,(unsigned int)characterSize);
      mDcText->setPosition(pos);
      mDcText->setAlignment(osgText::Text::BASE_LINE);
      mDcText->setText("0123456789.");
      geode->addDrawable( mDcText.get() );
      mSwitch->addChild(geode, false);
   }


   osg::MatrixTransform* modelview_abs = new osg::MatrixTransform;
   modelview_abs->setReferenceFrame(osg::Transform::RELATIVE_TO_ABSOLUTE);
   modelview_abs->setMatrix(osg::Matrix::identity());
   modelview_abs->addChild(mSwitch.get());

   mProjection = new osg::Projection;
   mProjection->setMatrix(osg::Matrix::ortho2D(0,1280,0,1024));
   mProjection->addChild(modelview_abs);
}

/** Based on the statsType, turn on/off the correct text nodes */
void Stats::EnableTextNodes(int statsType)
{
   switch(statsType)
   {
   case osgUtil::Statistics::STAT_NONE:
      mSwitch->setAllChildrenOff();
      break;

   case osgUtil::Statistics::STAT_FRAMERATE:
      mSwitch->setSingleChildOn(0);
      break;

   case osgUtil::Statistics::STAT_GRAPHS:
      mSwitch->setValue(0, true); //fr  //
      mSwitch->setValue(1, true); //update //
      mSwitch->setValue(2, true); //cull //
      mSwitch->setValue(3, true); //draw //
      mSwitch->setValue(4, true); //frametime //
      mSwitch->setValue(5, false);  //
      mSwitch->setValue(6, false);  //
      break;
   case osgUtil::Statistics::STAT_PRIMS:
      mSwitch->setValue(0, true); //fr  // 
      mSwitch->setValue(1, true); //update  //
      mSwitch->setValue(2, true); //cull  //
      mSwitch->setValue(3, true); //draw //
      mSwitch->setValue(4, true); //frametime  //
      mSwitch->setValue(5, true); //prims total  //
      mSwitch->setValue(6, false);  //
      break;

   case osgUtil::Statistics::STAT_PRIMSPERVIEW:
      mSwitch->setValue(0, true); //fr  //
      mSwitch->setValue(1, false); //update  //
      mSwitch->setValue(2, false); //cull  //
      mSwitch->setValue(3, false); //draw  //
      mSwitch->setValue(4, false); //frametime  //
      mSwitch->setValue(5, true); //prims total
      mSwitch->setValue(6, false);  //
      break;
   case osgUtil::Statistics::STAT_PRIMSPERBIN:
      mSwitch->setValue(0, true); //fr  //
      mSwitch->setValue(1, false); //update  //
      mSwitch->setValue(2, false); //cull //
      mSwitch->setValue(3, false); //draw //
      mSwitch->setValue(4, false); //frametime //
      mSwitch->setValue(5, true); //prims total  //
      mSwitch->setValue(6, false);  //
      break;
   case osgUtil::Statistics::STAT_DC:
      mSwitch->setValue(0, true); //fr  //
      mSwitch->setValue(1, true); //update //
      mSwitch->setValue(2, true); //cull //
      mSwitch->setValue(3, true); //draw //
      mSwitch->setValue(4, true); //frametime //
      mSwitch->setValue(5, false);  //
      mSwitch->setValue(6, true);  //
      break;
   case osgUtil::Statistics::STAT_RESTART:
      mSwitch->setAllChildrenOff();
      break;

   default:
      mSwitch->setAllChildrenOff();
      break;
   }
}
