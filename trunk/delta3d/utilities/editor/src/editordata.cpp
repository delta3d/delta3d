/*
* Delta3D Open Source Game and Simulation Engine 
* Simulation, Training, and Game Editor (STAGE)
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
* @author Curtiss Murphy
*/

#include "dtEditQt/global.h"
#include "dtEditQt/mainwindow.h"
#include "dtEditQt/editordata.h"
#include "dtDAL/map.h"
#include "dtDAL/log.h"

namespace dtEditQt
{

    //Singleton global variable for this class.
    osg::ref_ptr<EditorData> EditorData::instance(NULL);
    ///////////////////////////////////////////////////////////////////////////////
    EditorData::EditorData()
    {
        LOG_INFO("Initializing Editor Data.");

        this->mainWindow = NULL;
        this->myMap = NULL;
        this->gridSize = 16;
        numRecentProjects = 0;
        loadLastMap = loadLastProject = rigidCamera = true;
        selectionColor = Qt::red;
    }

    ///////////////////////////////////////////////////////////////////////////////
    EditorData::EditorData(const EditorData &rhs)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    EditorData &EditorData::operator=(EditorData &rhs)
    {
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////
    EditorData::~EditorData()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////
    EditorData &EditorData::getInstance()
    {
        if (EditorData::instance.get() == NULL)
            EditorData::instance = new EditorData();
        return *(EditorData::instance.get());
    }
    //////////////////////////////////////////////////////////////////////////////
    void EditorData::setMainWindow(MainWindow *window)
    {
        mainWindow = window;
    }
    //////////////////////////////////////////////////////////////////////////////
    void EditorData::setCurrentMap(dtDAL::Map *map)
    {
        myMap = map;
    }
    //////////////////////////////////////////////////////////////////////////////
    osg::ref_ptr<dtDAL::Map> EditorData::getCurrentMap()
    {
        return myMap;
    }
    //////////////////////////////////////////////////////////////////////////////
    void EditorData::setCurrentSoundResource(dtDAL::ResourceDescriptor newResource)
    {
        this->soundResource = newResource;
    }
    //////////////////////////////////////////////////////////////////////////////
    void EditorData::setCurrentMeshResource(dtDAL::ResourceDescriptor newResource)
    {
        this->meshResource = newResource;
    }
    //////////////////////////////////////////////////////////////////////////////
    void EditorData::setCurrentTextureResource(dtDAL::ResourceDescriptor newResource)
    {
        this->textureResource = newResource;
    }
    //////////////////////////////////////////////////////////////////////////////
    void EditorData::setCurrentParticleResource(dtDAL::ResourceDescriptor newResource)
    {
        this->particleResource = newResource;
    }
    //////////////////////////////////////////////////////////////////////////////
    void EditorData::setCurrentCharacterResource(dtDAL::ResourceDescriptor newResource)
    {
        this->characterResource = newResource;
    }
    //////////////////////////////////////////////////////////////////////////////
    void EditorData::setCurrentTerrainResource(dtDAL::ResourceDescriptor newResource)
    {
        this->terrainResource = newResource;
    }
}

