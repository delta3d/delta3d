#include "dtCore/profilemgr.h"
#include "dtCore/system.h"
#include "dtCore/notify.h"
#include "dtCore/scene.h"
#include "dtCore/prof.h"


#include <osg/MatrixTransform>
#include <osg/Projection>
#include <osg/Geode>


using namespace dtCore;

ProfileMgr::ProfileMgr(float sx, float sy):
mRenderEnabled(true),
mGraphRendered(false),
mSX(sx),
mSY(sy),
mProfMode(Prof_SORT_SELF_TIME),
mPaused(false),
mUseDefaultKeyMap(false)
{
   SetName("ProfileMgr");

   AddSender(System::GetSystem());
   

   osg::MatrixTransform *mat = new osg::MatrixTransform();
   mat->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
   mat->setMatrix(osg::Matrix::identity());

   mNode = new osg::Projection();
   dynamic_cast<osg::Projection*>(mNode.get())->setMatrix(osg::Matrix::ortho2D(0,1280,0,1024));
   dynamic_cast<osg::Projection*>(mNode.get())->addChild(mat);

   osg::StateSet* stateset = mNode->getOrCreateStateSet();
   stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
   stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
   stateset->setMode(GL_BLEND,osg::StateAttribute::ON);

   stateset->setRenderBinDetails(11,"RenderBin");

   {
      mGraph = new osg::Geode();
      mGraph->addDrawable(new osgProfileGraph());
      RenderGraph(mGraphRendered);
      mat->addChild(mGraph.get());
   }
   
   for (int i=0; i<NUM_TITLE; i++)
   {
      osg::Geode *geode = new osg::Geode();
      mTextTitle[i] = new osgText::Text();
      mTextTitle[i] ->setFont("fonts/arial.ttf");
      mTextTitle[i] ->setAlignment(osgText::Text::BASE_LINE);
      mTextTitle[i] ->setText("TITLE");

      if (i==0)
         mTextTitle[i]->setColor(osg::Vec4(0.6f, 0.4f, 0.f, 1.f));
      else
         mTextTitle[i]->setColor(osg::Vec4(0.8f, 0.1f, 0.1f, 1.f));

      geode->addDrawable(mTextTitle[i].get() );
      mat->addChild(geode);
   }

   {
      osg::Geode *geode = new osg::Geode();
      mTextHeader = new osgText::Text();
      mTextHeader ->setFont("fonts/arial.ttf");
      mTextHeader ->setAlignment(osgText::Text::BASE_LINE);
      mTextHeader ->setText("HEADER");
      mTextHeader ->setColor(osg::Vec4(0.7f, 0.7f, 0.7f, 1.f));

      geode->addDrawable(mTextHeader.get() );
      mat->addChild(geode);
   }

   for (int i=0; i<MAX_NUM_PROF_RECORDS; i++)
   {
      osg::Geode *geode = new osg::Geode();
      mTextRecord[i] = new osgText::Text();
      mTextRecord[i] ->setFont("fonts/arial.ttf");
      mTextRecord[i] ->setAlignment(osgText::Text::BASE_LINE);
      mTextRecord[i] ->setText("RECORD");
      mTextRecord[i] ->setColor(osg::Vec4(0.7f, 0.7f, 0.7f, 1.f));

      geode->addDrawable(mTextRecord[i].get() );
      mat->addChild(geode);
   }

   EnableRender(mRenderEnabled);

}

ProfileMgr::~ProfileMgr(void)
{
}

void ProfileMgr::UpdateText(void)
{
#ifdef Prof_ENABLED
   Prof(ProfileMgr_UpdateText);

   Prof_Report *pob;
   pob = Prof_create_report();

   float sy = mSY;
   const float line_spacing = mTextTitle[0]->getFontHeight()*1.5f;
   float height = 1024.f;
   int max_records = 0;

   for (int i=0; i<NUM_TITLE; ++i)
   {
      if (pob->title[i])
      {
         mTextTitle[i]->setPosition(osg::Vec3(mSX+2.f, sy, 0.f));
         mTextTitle[i]->setText(pob->title[i]);
         
         sy -= 1.5f*line_spacing;
         height -= abs(line_spacing)*1.5f;
      }
      else
      {
         mTextTitle[i]->setText("");
      }
   }

   max_records = height / abs(line_spacing);
   int o = 0;
   int n = pob->num_record;

   if (n>max_records) n = max_records;

   if (pob->hilight >= o+n) o = pob->hilight - n + 1;

   char tmpBuf[256];
   std::string buf;

   if (pob->header[0])
   {
      mTextHeader->setPosition(osg::Vec3(mSX, sy, 0.f));
      sprintf(tmpBuf, "%-30.30s", pob->header[0]);
      buf += tmpBuf;
   }

   for (int hIdx=1; hIdx<NUM_HEADER; ++hIdx)
   {
      if (pob->header[hIdx])
      {
         sprintf(tmpBuf, "%8.8s", pob->header[hIdx]);
         buf += tmpBuf;
      }
   }

   mTextHeader->setText(buf);
   buf.clear();

   sy -= line_spacing;

   for (int i=0; i<n; i++)
   {
      assert(i<MAX_NUM_PROF_RECORDS);

      Prof_Report_Record *r = &pob->record[i+o];
      
      for (int j=0; j<r->indent; ++j)  buf += ' ';
                
      if (r->prefix) buf += r->prefix;

      if (r->number)
      {
         sprintf(tmpBuf,"%-24.24s (%d)",r->name, r->number );
         buf += tmpBuf;
         if (r->number < 10) buf += ' ';
      }
      else
      {
         sprintf(tmpBuf,"%-29.29s", r->name );
         buf += tmpBuf;
      }

      if (!r->prefix) buf += ' ';

      osg::Vec4 textColor, glowColor;
      if (GetColors(r->heat, &textColor, &glowColor))
      {
         //render the glow text here
      }

      if (r->prefix == '!')
         mTextRecord[i]->setColor(osg::Vec4(1.f, 0.5f, 0.5f, 1.f));
      else
         mTextRecord[i]->setColor(textColor);

      mTextRecord[i]->setPosition(osg::Vec3(mSX, sy, 0.f));

      for (int j=0; j<NUM_VALUES; ++j)
      {
         if (r->value_flag & (1 << j))
         {
            sprintf(tmpBuf," %7.3lf", r->values[j] );
            buf += tmpBuf;  
         }
         else  buf += "        ";
      }

      if (i+o == pob->hilight)
         mTextRecord[i]->setDrawMode(osgText::Text::BOUNDINGBOX|osgText::Text::TEXT);
      else
         mTextRecord[i]->setDrawMode(osgText::Text::TEXT);

      mTextRecord[i]->setText(buf);

      buf.clear();

      sy -= line_spacing;
   }

   //clear unused record texts
   for (int i=n; i<MAX_NUM_PROF_RECORDS; i++)
   {
      mTextRecord[i]->setText("");
   }

   Prof_free_report(pob);

#endif
}

void ProfileMgr::IncReportMode(void)
{
   switch(mProfMode) 
   {
   case Prof_SORT_SELF_TIME         : mProfMode = Prof_SORT_HIERARCHICAL_TIME;	break;
   case Prof_SORT_HIERARCHICAL_TIME : mProfMode = Prof_CALL_GRAPH;	            break;
   case Prof_CALL_GRAPH             : mProfMode = Prof_CALL_GRAPH_DESCENDENTS;	break;
   case Prof_CALL_GRAPH_DESCENDENTS : mProfMode = Prof_SORT_SELF_TIME;        	break;
   default                          : mProfMode = Prof_SORT_SELF_TIME;           break;
   }

   Notify(DEBUG_INFO, "ProfileMgr report mode:%d", mProfMode);
   Prof_set_report_mode(mProfMode);
}

///Performs collision detection and updates physics
void ProfileMgr::OnMessage(MessageData *data)
{
   if (IS_A(data->sender, System*))
   {
      if (data->message == "frame")
      {
         if (mPaused)  Prof_update(PROF_discard);
         else          Prof_update(PROF_accumulate);
      }

      if (mRenderEnabled)
      {
         UpdateText();
      }
   }
}

int ProfileMgr::GetColors( float factor,
                    osg::Vec4 *textColor,
                    osg::Vec4 *glowColor)
{
   const float GLOW_RANGE = 0.5f;
   const float GLOW_ALPHA_MAX = 0.5f;
   osg::Vec4 hot(1.f, 1.f, 0.9f, 1.f);
   osg::Vec4 cold(0.15f, 0.9f, 0.15f, 1.f);

   osg::Vec4 glowCold(0.5f, 0.5f, 0.f, 1.f);
   osg::Vec4 glowHot(1.f, 1.f, 0.f, 1.f);

   if (factor < 0) factor = 0;
   if (factor > 1) factor = 1;

   *textColor = cold + (hot - cold) *factor;

   // Figure out whether to start up the glow as well.
   float glow_alpha = (factor - GLOW_RANGE) / (1 - GLOW_RANGE);
   if (glow_alpha < 0) {
      glowColor->_v[3] = 0.f;
      return 0;
   }

   *glowColor = glowCold + (glowHot-glowCold) * factor;
   glowColor->_v[3] = glow_alpha * GLOW_ALPHA_MAX;
   return 1;
}

void ProfileMgr::EnableRender(const bool enable)
{
   mRenderEnabled = enable;
   
   if (mRenderEnabled)
   {
      mNode->setNodeMask(0xffffffff);
      Notify(DEBUG_INFO, "ProfileMgr is rendering");
   }
   else 
   {
      mNode->setNodeMask(0x0);
      Notify(DEBUG_INFO, "ProfileMgr not rendering");
   }
}

void ProfileMgr::RenderGraph(const bool enable)
{
   mGraphRendered = enable;

   if (mGraphRendered)
      mGraph->setNodeMask(0xffffffff);
   else
      mGraph->setNodeMask(0x0);
}

/**
* Called when a key is pressed.
*
* @param keyboard the source of the event
* @param key the key pressed
* @param character the corresponding character
*/
void ProfileMgr::KeyPressed(Keyboard* keyboard, 
                        Producer::KeyboardKey key,
                        Producer::KeyCharacter character)
{
   switch(key) 
   {
   case Producer::Key_F10:  EnableRender( !IsRendering() ); 	break;
   case Producer::Key_F11:  IncReportMode();                   break;
   case Producer::Key_Down: MoveCursorDown();                  break;
   case Producer::Key_Up:   MoveCursorUp();                    break;
   case Producer::Key_Return: Select();                        break;
   case Producer::Key_G:    RenderGraph( !IsGraphRendered() ); break;
   case Producer::Key_P:   Pause( !IsPaused() );               break;
   default: break;
   }
}

/** If true, this will subscribe the ProfileMgr to the first instance
 *  of the Keyboard.  When a key is pressed, it will be compared to the
 *  internal keyboard mappings.
 *  If false, the ProfileMgr will be unsubscribed from the first instance
 *  of the Keyboard.
 *  ProfileMgr will not use its internal keyboard mapping by default.
 */
void ProfileMgr::UseDefaultKeyboardMapping(const bool enable)
{
   if (enable == mUseDefaultKeyMap) return;

   if (enable && !mUseDefaultKeyMap)
   {
      Keyboard::GetInstance(0)->AddKeyboardListener(this);
   }
   else if (!enable && mUseDefaultKeyMap)
   {
      Keyboard::GetInstance(0)->RemoveKeyboardListener(this);
   }
}