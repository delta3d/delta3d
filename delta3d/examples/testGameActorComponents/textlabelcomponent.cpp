#include "textlabelcomponent.h"

#include <dtDAL/booleanactorproperty.h>
#include <dtDAL/floatactorproperty.h>
#include <dtDAL/stringactorproperty.h>

#include <dtGame/basemessages.h>
#include <dtGame/gameactor.h>

#include <osg/Geode>

const dtGame::ActorComponent::ACType TextLabelComponent::TYPE("TextLabelComponent");

////////////////////////////////////////////////////////////////////////////////
TextLabelComponent::TextLabelComponent()
   : ActorComponent(TYPE)
   , mFontSize(0.5f)
   , mTextGeometry(new osgText::Text())
   , mColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f))
   , mIsFlashing(false)
   , mShow(true)
   , mText("")
   , mFlashInterval(1)
{
   mGroup = new osg::Group();
   mGeode = new osg::Geode();
   mPosition = new osg::PositionAttitudeTransform();

   mGroup->addChild(mPosition.get());
   mPosition->addChild(mGeode.get());
}

////////////////////////////////////////////////////////////////////////////////
TextLabelComponent::~TextLabelComponent()
{
   Reset();
}

////////////////////////////////////////////////////////////////////////////////
void TextLabelComponent::OnAddedToActor(dtGame::GameActor& actor)
{
   // add text node geometry to actor
   actor.GetOSGNode()->asGroup()->addChild(mGroup.get());
}

////////////////////////////////////////////////////////////////////////////////
void TextLabelComponent::OnRemovedFromActor(dtGame::GameActor& actor)
{
   // remove text node geometry from actor
   actor.GetOSGNode()->asGroup()->removeChild(mGroup.get());
}

////////////////////////////////////////////////////////////////////////////////
void TextLabelComponent::Reset()
{
   // remove all drawables from geode
   mGeode->removeDrawables(0, mGeode->getNumDrawables());
}

////////////////////////////////////////////////////////////////////////////////
void TextLabelComponent::Create()
{
   if (!mShow)
   {
      return;
   }

   Reset();

   mTextGeometry->setText(mText);
   mTextGeometry->setColor(mColor);
   mTextGeometry->setAlignment(osgText::TextBase::CENTER_CENTER);
   mTextGeometry->setAxisAlignment(osgText::TextBase::SCREEN);
   //mTextGeometry->setFont("Fonts/FreeSans.ttf");
   mTextGeometry->setBackdropType(osgText::Text::NONE);
   mTextGeometry->setCharacterSize(mFontSize);
   // highlight color
   mTextGeometry->setBackdropColor(osg::Vec4(1.0f, 0.2f, 0.0f, 1.0f));

   mGeode->addDrawable(mTextGeometry.get());

   osg::StateSet* ss = mGeode->getOrCreateStateSet();
   ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
}

////////////////////////////////////////////////////////////////////////////////
void TextLabelComponent::SetHighlighted(bool h)
{
   mTextGeometry->setBackdropType(h ? osgText::Text::OUTLINE : osgText::Text::NONE);
}

////////////////////////////////////////////////////////////////////////////////
void TextLabelComponent::SetFlashing(bool b)
{
   mIsFlashing = b;
   if (b)
   {
      // Get per frame updates to the OnTickLocal function
      RegisterForTicks();
   }
   else
   {
      UnregisterForTicks();
      SetHighlighted(false);
   }
}

////////////////////////////////////////////////////////////////////////////////
void TextLabelComponent::OnTickLocal(const dtGame::TickMessage& tickMessage)
{
   double time = tickMessage.GetSimulationTime();

   if (fmod(time, (double)mFlashInterval * 2) < (double)mFlashInterval)
   {
      SetHighlighted(true);
   }
   else
   {
      SetHighlighted(false);
   }
}

////////////////////////////////////////////////////////////////////////////////
void TextLabelComponent::BuildPropertyMap()
{
   static const dtUtil::RefString GROUPNAME = "Text Label";

   AddProperty(new dtDAL::StringActorProperty("Text", "Text",
      dtDAL::StringActorProperty::SetFuncType(this, &TextLabelComponent::SetText),
      dtDAL::StringActorProperty::GetFuncType(this, &TextLabelComponent::GetText),
      "",
      GROUPNAME));

   AddProperty(new dtDAL::FloatActorProperty("Text Height", "Text Height",
      dtDAL::FloatActorProperty::SetFuncType(this, &TextLabelComponent::SetHeight),
      dtDAL::FloatActorProperty::GetFuncType(this, &TextLabelComponent::GetHeight),
      "height of text above actor",
      GROUPNAME));

   AddProperty(new dtDAL::FloatActorProperty("Flash Interval", "Flash Interval",
      dtDAL::FloatActorProperty::SetFuncType(this, &TextLabelComponent::SetFlashInterval),
      dtDAL::FloatActorProperty::GetFuncType(this, &TextLabelComponent::GetFlashInterval),
      "interval for flashing text during alarm",
      GROUPNAME));

   AddProperty(new dtDAL::BooleanActorProperty("Is Flashing", "Is Flashing",
      dtDAL::BooleanActorProperty::SetFuncType(this, &TextLabelComponent::SetFlashing),
      dtDAL::BooleanActorProperty::GetFuncType(this, &TextLabelComponent::IsFlashing),
      "interval for flashing text during alarm",
      GROUPNAME));

   AddProperty(new dtDAL::BooleanActorProperty("Visible", "Visible",
      dtDAL::BooleanActorProperty::SetFuncType(this, &TextLabelComponent::SetEnabled),
      dtDAL::BooleanActorProperty::GetFuncType(this, &TextLabelComponent::IsEnabled),
      "",
      GROUPNAME));
}
