/*
* Delta3D Open Source Game and Simulation Engine Level Editor
* Copyright (C) 2005, BMH Associates, Inc.
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2 of the License, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* @author Teague Coonan
*/

#include <QHeaderView>

#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QString>
#include <QDir>

#include <QGroupBox>

#include <QPushButton>
#include <QCheckBox>

#include <QAction>
#include <QContextMenuEvent>
#include <QIcon>

#include <QPixmap>
#include <QScrollArea>
#include <QLabel>
#include <QIcon>
#include "dtEditQt/resourcetreewidget.h"
#include "dtEditQt/resourceimportdialog.h"
#include "dtDAL/project.h"
#include "dtEditQt/editordata.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/texturebrowser.h"
#include "dtEditQt/uiresources.h"

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    TextureBrowser::TextureBrowser(dtDAL::DataType &type,QWidget *parent)
        : ResourceAbstractBrowser(&type,parent)
    {

        // This sets our resource icon that is visible on leaf nodes
        resourceIcon = new QIcon();
        resourceIcon->addPixmap(QPixmap(UIResources::ICON_TEXTURE_RESOURCE.c_str()));
        ResourceAbstractBrowser::resourceIcon = *resourceIcon;

        QSplitter *splitter = new QSplitter(Qt::Vertical,this);

        splitter->addWidget(previewTextureGroup());
        splitter->addWidget(listTextureGroup());

        // setup the grid layouts
        grid = new QGridLayout(this);
        grid->addWidget(splitter,0,0);
        grid->addWidget(standardButtons(QString("Resource Tools")),1,0,Qt::AlignCenter);

        //stop the buttons from stretching
        grid->setRowStretch(1,0);
    }
    ///////////////////////////////////////////////////////////////////////////////
    TextureBrowser::~TextureBrowser(){}
    ///////////////////////////////////////////////////////////////////////////////
    QGroupBox *TextureBrowser::listTextureGroup()
    {
        QGroupBox *groupBox = new QGroupBox(tr("Textures"));
        QGridLayout *grid = new QGridLayout(groupBox);
        QHBoxLayout *hbox = new QHBoxLayout();

        // Checkbox for auto preview
        previewChk = new QCheckBox(tr("Auto Preview"),groupBox);
        connect(previewChk,SIGNAL(stateChanged(int)),this,SLOT(checkBoxSelected()));
        previewChk->setChecked(false);

        // Preview button for a selected mesh
        previewBtn = new QPushButton("Preview",groupBox);
        connect(previewBtn, SIGNAL(clicked()), this, SLOT(previewTexture()));
        previewBtn->setDisabled(true);

        hbox->addWidget(previewChk,0,Qt::AlignLeft);
        hbox->addWidget(previewBtn,0,Qt::AlignRight);
        grid->addLayout(hbox,0,0);
        grid->addWidget(tree,1,0);

        return groupBox;
    }
    ///////////////////////////////////////////////////////////////////////////////
    QGroupBox *TextureBrowser::previewTextureGroup()
    {
        QGroupBox *group = new QGroupBox(tr("Preview"));
        QHBoxLayout *hbox = new QHBoxLayout(group);

        image = new QPixmap();
        scrollArea = new QScrollArea(group);

        pixmapWrapper = new QWidget(scrollArea);
        
        preview = new QLabel(pixmapWrapper);
        preview->setPixmap(*image);
        preview->setShown(true);
        
        scrollArea->setWidget(pixmapWrapper);

        hbox->addWidget(scrollArea);

        return group;
    }
    ///////////////////////////////////////////////////////////////////////////////
    // Keyboard Event filter
    ///////////////////////////////////////////////////////////////////////////////
    bool TextureBrowser::eventFilter(QObject *obj, QEvent *e)
    {
        if (obj == tree) 
        {
            //For some reason, KeyPress is getting defined by something...
            //Without this undef, it will not compile under Linux..
            //It would be great if someone could figure out exactly what's
            //going on.
#undef KeyPress
            if (e->type() == QEvent::KeyPress) 
            {
                QKeyEvent *keyEvent = (QKeyEvent *)e;
                switch(keyEvent->key())
                {
                case Qt::Key_Return :
                    if(selection->isResource())
                    {
                        previewTexture();
                    }
                    break;
                case Qt::Key_Enter:
                    if(selection->isResource())
                    {
                        previewTexture();
                    }
                    break;
                default:
                    return tree->eventFilter(obj,e);
                }
            }
            else
            {
                // pass the event on to the parent class
                return tree->eventFilter(obj, e);
            }
        }
        return false;
    }
    ///////////////////////////////////////////////////////////////////////////////
    // Slots
    ///////////////////////////////////////////////////////////////////////////////
    void TextureBrowser::previewTexture()
    {
        ResourceTreeWidget *selection = currentSelection();
        bool validFile = false;

        if(selection != NULL)
        {
            QString file;
            QString context;

            dtDAL::Project &project = dtDAL::Project::GetInstance();

            // Find the currently selected tree item
            dtDAL::ResourceDescriptor resource = EditorData::getInstance().getCurrentTextureResource();
           
            try 
            {
                file = QString(project.GetResourcePath(resource).c_str());
                validFile = true;
            } 
            catch (dtDAL::Exception &e)
            {
                validFile = false;
            }

            if(file != NULL) 
            {
                context = QString(project.GetContext().c_str());
                // The following is performed to comply with linux and windows file systems
                file = context+"\\"+file;
                file.replace("\\","/");
                file.replace("//","/");

                scrollArea->setShown(true);
                //Load the new file.
                delete preview;
                delete image;
                image = new QPixmap();
                image->load(file);
                preview = new QLabel(pixmapWrapper);
                preview->setPixmap(*image);
                preview->setShown(true);

                pixmapWrapper->setMinimumSize(preview->sizeHint());
                pixmapWrapper->setMaximumSize(preview->sizeHint());
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void TextureBrowser::selectionChanged()
    {
        // This is the abstract base classes original functionality
        ResourceAbstractBrowser::selectionChanged();

        if(selection != NULL)
        {
            if(selection->isResource())
            {
                // auto preview
                if(previewChk->isChecked())
                {
                    previewTexture();
                }
                //context sensitive menu items
                previewBtn->setDisabled(false);
            }
            else
            {
                previewBtn->setDisabled(true);
                clearTextureWidget();
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void TextureBrowser::checkBoxSelected()
    {
        if(previewChk->isChecked())
        {
            if(selection->isResource())
            {
                // preview current item
                selectionChanged();
                previewTexture();
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void TextureBrowser::doubleClickEvent()
    {
          if(selection->isResource())
          {
              previewTexture();
          }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void TextureBrowser::deleteItemEvent()
    {
        if(selection->isResource())
        {
            clearTextureWidget();
            previewBtn->setDisabled(true);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void TextureBrowser::clearTextureWidget()
    {
        // When any item is selected, clear the texture
        delete preview;
        delete image;
        image = new QPixmap();
        preview = new QLabel(pixmapWrapper);
        preview->setPixmap(*image);
    }
}
