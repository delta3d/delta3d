#pragma once

#include "UI/cui_ui.h"
#include "UI/CUI_ListItem.h"
#include "UI/CUI_ListBox.h"
#include "tinystr.h"
#include "tinyxml.h"
#include <stack>

class UIParser
{
public:
   UIParser( CUI_UI *ui):
      mUI(ui)
      {
      }

      ~UIParser() {}
      void LoadGUIFile(std::string filename);

private:
   CUI_UI *mUI;
   typedef struct ELEMDATA
   {
      ELEMDATA():elem(NULL),data(NULL){}
      TiXmlElement *elem;
      void *data;
   };

   //! type - STL stack of TiXml Elements
   typedef std::stack<ELEMDATA*> ELEMSTACK;

   //! stack of frame hierarchy
   ELEMSTACK m_elementStack;

   void LoadControlFrame( ELEMDATA *elem, CUI_Frame *frame = NULL );
   void LoadControlDraggableFrame( ELEMDATA *elem );
    void LoadControlDraggableViewFrame( ELEMDATA *elem );
    void LoadControlTextLabel( ELEMDATA *elem, CUI_Widget *frame = NULL );
    void LoadControlButton( ELEMDATA *elem );
    void LoadControlResizeButton( ELEMDATA *elem );
    void LoadControlProgressBar( ELEMDATA *elem, CUI_ProgressBar *frame = NULL );
    void LoadControlSliderBar( ELEMDATA *elem );
    void LoadControlTextBox( ELEMDATA *elem, CUI_TextBox *frame = NULL );
    void LoadControlEditTextBox( ELEMDATA *elem );
    void LoadControlListBox( ELEMDATA *elem, CUI_ListBox *frame = NULL );
    void LoadControlComboBox( ELEMDATA *elem );
    void LoadControlMenu( ELEMDATA *elem );
    void LoadControlRadioGroup( ELEMDATA *elem );
    void LoadControlScrollFrame( ELEMDATA *elem );
    void LoadControlScrollTextBox( ELEMDATA *elem );
   void LoadResourceFBMFont( ELEMDATA *elem );
   void LoadResourceCoordSys( ELEMDATA *elem);
   void LoadResourcePolyBorder(ELEMDATA *elem);
   void LoadDataState(ELEMDATA *elem);
   void LoadDataRadioItem(ELEMDATA *elem);
   void LoadDataMenuItem(ELEMDATA *elem);
   void LoadDataListItem(ELEMDATA *elem,CUI_ListItem *item=NULL);
   void LoadDataRect(ELEMDATA *obj);
   void LoadDataPoint2d(ELEMDATA *elem);
   void ParseElement(TiXmlElement *elem);
   void EndElement();
};
