/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* @author Teague Coonan
*/

#ifndef __TextureBrowser_h
#define __TextureBrowser_h

#include "dtEditQt/resourceabstractbrowser.h"
#include <osg/ref_ptr>

#include "dtDAL/resourcehelper.h"

class QAction;
class QGroupBox;
class QGridLayout;
class QPushButton;
class QKeyEvent;
class QContextMenuEvent;
class QCheckBox;
class QLabel;
class QPixMap;
class QScrollArea;
class QCheckBox;
class QPixmap;
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
        TextureBrowser(dtDAL::DataType &type,QWidget *parent=0);
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
        bool eventFilter(QObject *target, QEvent *e);

    private:

        /**
        * Preview for textures
        * @return QGroupBox layout widget
        */
        QGroupBox *previewTextureGroup();

        /**
        * This defines the layout for the sound list
        * @return QGroupBox layout widget
        */
        QGroupBox *listTextureGroup();

        void doubleClickEvent();

        // Layout Objects
        QGridLayout *grid;

        /**
        * When the base class registeres a delete has occured the browser
        * will clear and refresh any preview windows that currently exist
        */
        void deleteItemEvent();

        void clearTextureWidget();

        // Required for previewing textures
        QLabel *preview;
        QPixmap *image;
        QScrollArea *scrollArea;
        QWidget *pixmapWrapper;

        QPushButton *previewIcon;

        QCheckBox *previewChk;

        // Buttons
        QPushButton *previewBtn;
    };
}

#endif
