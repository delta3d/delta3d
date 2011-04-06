#include "buttonconnector.h"

#include <dtUtil/log.h>

ButtonConnector::ButtonConnector(Type t, QWidget* parent /*= 0*/, Qt::WindowFlags f /*=0*/)
   : mUI()
   , QWidget(parent, f)   
{
   //apply layout made with QtDesigner (see ui file)
   mUI.setupUi(this);

   SetType(t);
}

ButtonConnector::~ButtonConnector()
{
}

void ButtonConnector::SetType(Type t)
{
   mType = t;

   switch(t)
   {
      /*  Doing this a better way now
      case ButtonConnector::TOP_BOTTOM:
      { 
         SetLineWidths(2, 2, 0, 0);
            
         //This is a little weird, but is done this way because I need a
         //way of having unbroken top to bottom lines.  I also want to 
         //use the same ButtonConnector::SetType API so things don't get too 
         //confusing for a user of ButtonConnectors, so things get slightly
         //hairy here:  
         //(the other cases are simple calls to SetLineWidths, though)        
         TopBottomConnectorNoGap* tbcng = new TopBottomConnectorNoGap();         
         lo->addWidget(tbcng, GetRow(), GetCol());           
         
      break;
      }
      */

      case ButtonConnector::TOP_BOTTOM_RIGHT:
      {
         SetLineWidths(2, 2, 0, 2);         
      break;
      }

      case ButtonConnector::BOTTOM_LEFT:
      {
         SetLineWidths(0, 2, 2, 0);         
      break;
      }

      case ButtonConnector::BOTTOM_LEFT_RIGHT:
      {
         SetLineWidths(0, 2, 2, 2);         
      break;
      }

      case ButtonConnector::LEFT_RIGHT:
      {
         SetLineWidths(0, 0, 2, 2);         
      break;
      }

      default:
      {
         dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR,
            __FUNCTION__, __LINE__, "Unrecognized connector type.");

         mType = ButtonConnector::UNSPECIFIED;
      break;
      }
   }//switch
}

void ButtonConnector::SetLineWidths(int top, int bottom, int left, int right)
{
   SetLineWidth(mUI.topConnector, top);
   SetLineWidth(mUI.bottomConnector, bottom);
   
   SetLineWidth(mUI.leftConnector, left);
   SetLineWidth(mUI.leftMidConnector, left);

   SetLineWidth(mUI.rightConnector, right);
   SetLineWidth(mUI.rightMidConnector, right);   
}

void ButtonConnector::SetLineWidth(QFrame* line, int width)
{
   if (line == NULL)
      return;

   line->setLineWidth(width);
   if (width <= 0)
   {
      line->setFrameShadow(QFrame::Sunken);
   }
   else
   {
      line->setFrameShadow(QFrame::Plain);
   }
}