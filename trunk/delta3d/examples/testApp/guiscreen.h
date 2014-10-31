/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef DELTA_TEST_APP_GUI_SCREEN
#define DELTA_TEST_APP_GUI_SCREEN

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "export.h"
#include "guilistitem.h"
#include <dtCore/refptr.h>
#include <dtGUI/gui.h>
#include <osg/Referenced>
#include <osg/Vec3>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace CEGUI
{
   class Window;
}

typedef CEGUI::Window GuiNode;
typedef CEGUI::ItemListbox GuiListbox;



namespace dtExample
{
   //////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   //////////////////////////////////////////////////////////////////////////
   class TEST_APP_EXPORT GuiScreen : public osg::Referenced
   {
      public:
         typedef osg::Referenced BaseClass;

         static const std::string DEFAULT_NAME;

         GuiScreen(dtGUI::GUI& guiScene, const std::string& name, const std::string& layoutFile);

         GuiNode* GetRoot();
         const GuiNode* GetRoot() const;

         void SetName(const std::string& name);
         const std::string& GetName() const;

         const std::string& GetLayoutFileName() const;

         /**
         * Set the visibility of the screen.
         * @param visible TRUE if the screen should be made visible.
         */
         virtual void SetVisible(bool visible);
         virtual bool IsVisible() const;

         /**
          * Perform the initial setup of the object.
          * @param root Delta object that acts as the root attach point
          *        for the screen drawables. It is up to the implementation
          *        to cast the root and handle attaching elements to it.
          */
         virtual void Setup(GuiNode* parent = NULL);

         void SetParent(GuiNode* parent);

         /**
          * Method to be called by the GUI system when the screen is
          * going to be shown. Overriders of this method ought to call
          * this base method.
          */
         virtual void OnEnter();

         /**
          * Method to be called by the GUI system when the screen is
          * going to be hidden. Overriders of this method ought to call
          * this base method.
          */
         virtual void OnExit();

         virtual void Update(float timeDelta);

         /**
          * Utility method to create text
          */
         virtual GuiNode* CreateText(const std::string& name, GuiNode* parent, const std::string& text,
                                        float x, float y, float width, float height);
         /**
          * Utility method to set the text, position, and color of a text control
          * Check to see if the data changed.  The default values for color and position
          * won't do anything since they use a color and position < 0.
          */
         virtual void SetTextFormatted(GuiNode* textControl, const std::string& newText,
            osg::Vec3 color = osg::Vec3(1.0f, 1.0f, 1.0f), float x = -1.0f, float y = -1.0f);
         
         virtual void SetTextAndPosition(GuiNode* textControl, const std::string& newText,
            float x, float y);

         virtual GuiNode* GetNode(const std::string& name, bool deepSearch = true) const;

         virtual bool SetText(const std::string& controlName, const std::string& text);

         virtual bool SetVisible(const std::string& controlName, bool visible);
         virtual bool IsVisible(const std::string& controlName) const;

         /**
          * Convenience method for determining a control's type.
          */
         bool IsControlOfType(const GuiNode& control, const std::string& typeName) const;
         
         /**
          * Convenience method for accessing a listbox that may exist in the screen layout.
          */
         GuiListbox* GetListbox(const std::string& controlName) const;

         /**
          * Convenience method for adding a control to a list box that maybe part of the screen.
          */
         GuiListItem* AddListItem(const std::string& listControlName, GuiNode& itemNode) const;
         GuiListItem* AddListItem(GuiListbox& listControl, GuiNode& itemNode) const;

      protected:
         virtual ~GuiScreen();

      private:
         std::string mName;
         std::string mLayoutFile;

         // GUI Elements
         GuiNode* mRoot;

         // Keep a reference to the GUI drawable which
         // wraps calls to CEGUI. Use an observer/weak
         // pointer to prevent potential memory leaks.
         dtCore::ObserverPtr<dtGUI::GUI> mGUIScene;
   };
}

#endif
