/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Teague Coonan
 */

#ifndef DELTA_TEXTURE_BROWSER
#define DELTA_TEXTURE_BROWSER

#include <dtEditQt/resourceabstractbrowser.h>

#include <dtCore/resourcehelper.h>

class QAction;
class QGroupBox;
class QGridLayout;
class QPushButton;
class QKeyEvent;
class QContextMenuEvent;
class QCheckBox;
class QLabel;
class QScrollArea;
class QLabel;

namespace dtEditQt
{

   class ResourceTreeWidget;
   class ResourceImportDialog;
   class ResourceTree;

   /**
    * @class TextureBrowser
    * @brief Lists textures that can be selected
    */
   class TextureBrowser : public ResourceAbstractBrowser
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      TextureBrowser(dtCore::DataType& type, QWidget* parent = 0);

      /**
       * Destructor
       */
      virtual ~TextureBrowser();

      /**
       * Derived from our abstract base class. When a tree selection changes
       * this method will be called. This will handle our context sensitive buttons
       * so we know when to preview a texture
       */
      void selectionChanged();

   private slots:

      /**
       * Slot - preview the selected texture
       */
      void previewTexture();

      /**
       * Slot - Handle the event when the checkbox is selected, preview selected item.
       */
      void checkBoxSelected();

   protected:
      /**
       * Added an event filter to capture keyboard events sent to the tree widget
       * so we can trap the enter key and play sounds. All other events are
       * passed on to the parent. This has been overridden from the base abstract
       * class to provide browser specific functionality.
       * @param Event
       * @return bool if the event was not captured for the appropriate widget
       */
      bool eventFilter(QObject* target, QEvent* e);

   private:
      /**
       * Preview for textures
       * @return QGroupBox layout widget
       */
      QGroupBox* previewTextureGroup();

      /**
       * This defines the layout for the sound list
       * @return QGroupBox layout widget
       */
      QGroupBox* listTextureGroup();

      void doubleClickEvent();

      // Layout Objects
      QGridLayout* mGrid;

      /**
       * When the base class registers a delete has occured the browser
       * will clear and refresh any preview windows that currently exist
       */
      void deleteItemEvent();

      void clearTextureWidget();

      // Required for previewing textures
      QLabel*      mPreview;
      QScrollArea* mScrollArea;
      QWidget*     mPixmapWrapper;

      QPushButton* mPreviewIcon;

      QCheckBox*   mPreviewChk;

      // Buttons
      QPushButton* mPreviewBtn;
   };

} // namespace dtEditQt

#endif // DELTA_TEXTURE_BROWSER
