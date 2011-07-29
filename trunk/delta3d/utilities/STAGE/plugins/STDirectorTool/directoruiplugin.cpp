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
 * Jeff P. Houde
 */

#define QT_STATICPLUGIN

#include "directoruiplugin.h"
#include "director.h"

#include <dtCore/uniqueid.h>

#include <dtCore/actortype.h>
#include <dtCore/map.h>
#include <dtCore/project.h>

#include <dtEditQt/editordata.h>

#include <QtCore/QString>

#include <QtGui/QWidget>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   /////////////    Director UI Plugin    ////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////

   DirectorUIPlugin::DirectorUIPlugin()
      : mName("DirectorEditor")
   {}

   ///////////////////////////////////////////////////////////////////////////////
   QWidget* DirectorUIPlugin::CreateWidget(QWidget* parent)
   {
      mDirector = new dtDirector::Director();
      if (mDirector)
      {
         mDirector->Init(NULL, dtEditQt::EditorData::GetInstance().getCurrentMap());

         mEditor = new DirectorToolEditor();
         if (mEditor)
         {
            mEditor->SetDirector(mDirector);
            mEditor->show();
            if (!GetResource().IsEmpty())
            {
               mEditor->LoadScript(dtCore::Project::GetInstance().GetResourcePath(GetResource()));
            }

            connect(mEditor, SIGNAL(ScriptChanged(const std::string&)), this, SIGNAL(ScriptChanged(const std::string&)));

            return mEditor;
         }
      }

      return NULL;
   }

}
//This export line had to be done this way because the first name is used in a function name in the macro, but I have to
//put the call outside a namespace, so the second parameter has to have the namespace of the actual class.  God help us.
Q_EXPORT_PLUGIN2(DirectorUIPlugin, dtEditQt::DirectorUIPlugin);
//The plugin is all static, so just export and import it all at the same time.
Q_IMPORT_PLUGIN(DirectorUIPlugin);
