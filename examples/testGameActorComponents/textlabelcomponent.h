#pragma once

#include "testexport.h"
#include <dtGame/actorcomponentbase.h>
#include <dtCore/refptr.h>
#include <dtDAL/actorproperty.h>
#include <osg/Geometry>
#include <osg/PositionAttitudeTransform>
#include <osgText/Text>
#include <osg/Geode>


/**
   A game actor component that attaches a text billboard to the actor.
   The text can be made to blink on and off periodically.
*/
class TEST_GA_COMPONENTS_EXPORT TextLabelComponent : public dtGame::ActorComponent
{

public:

   // set the type of the actor component
   static const ActorComponent::ACType TYPE;

   TextLabelComponent();     

   // add and remove geometry to actor
   virtual void OnAddedToActor(dtGame::GameActor& actor);
   virtual void OnRemovedFromActor(dtGame::GameActor& actor);


   /** @return the color as a vec4 */
   osg::Vec4 GetColor() const 
   { 
      return mColor; 
   }

   /** @param color The color as 4 bytes */
   void SetColor(int color)
   {
      double a = ((color & 0xff000000) >> 24) / 256.;
      double r = ((color & 0x00ff0000) >> 16) / 256.;
      double g = ((color & 0x0000ff00) >>  8) / 256.;
      double b = ((color & 0x000000ff) >>  0) / 256.;         
      mColor.set(r, g, b, a);
      Create(); 
   }

   /** @param color The color as a vec4 */
   void SetColor(const osg::Vec4& color)
   {
      mColor = color;
      Create();
   }

   /** @return the height of the symbol above 0 */
   float GetHeight() const
   {
      return mPosition->getPosition()[2];
   }

   /** @param h the height of the symbol above 0 */
   void SetHeight(float h)
   {
      mPosition->setPosition(osg::Vec3(0, 0, h));
   }

   /** should the text label be shown? */
   bool IsEnabled() const
   {
      return mShow;
   }

   /** should the text label be shown? */
   void SetEnabled(bool show)
   {

      mShow = show;
      
      if(show)
      {
         Create();
      }
      else
      {
         Reset();
      }
   }
   
   /** set the text to be displayed */
   void SetText(const std::string& text)
   {
      mText = text;
      Create(); 
   }

   /** get the text to be displayed */
   std::string GetText() const
   {
      return mText;
   }

   /** apply a red border around text */
   void SetHighlighted(bool);

   /** set font size
       @param s size in meters
   */
   void SetFontSize(float s) 
   { 
      mFontSize = s;
      Create(); 
   }

   /** get font size in meters */
   float GetFontSize() const { return mFontSize; }

   /** is the text currently flashing? */
   bool IsFlashing() const { return mIsFlashing; }

   /** toggle flashing */
   void SetFlashing(bool b);

   /** react to game tick messages */
   virtual void OnTickLocal(const dtGame::TickMessage& tickMessage);

   /** set the interval in seconds at that the text should blink */
   void SetFlashInterval(float v) { mFlashInterval = v; }
   
   /** get the interval in seconds at that the text should blink */
   float GetFlashInterval() const { return mFlashInterval; }

   /** add actor component properties to game actor for configuring in STAGE */
   virtual void BuildPropertyMap();

protected:

    ~TextLabelComponent();

private:

   /** Clear geometry */
   void Reset();

   /** Create geometry according to properties */
   void Create();

   // text size in meters
   float mFontSize;

   // the OSG text geometry
   dtCore::RefPtr<osgText::Text> mTextGeometry;

   /// Color of the text
   osg::Vec4 mColor;

    // symbol geometry
   dtCore::RefPtr<osg::Group> mGroup;
   dtCore::RefPtr<osg::Geode> mGeode;
   dtCore::RefPtr<osg::PositionAttitudeTransform> mPosition;

   // currently flasing?
   bool mIsFlashing;

   // visible?
   bool mShow;

   // the text to display
   std::string mText;

   // flash at this interval
   float mFlashInterval;

};
