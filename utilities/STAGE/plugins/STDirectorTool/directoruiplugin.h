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

#ifndef DELTA_DIRECTOR_UI_PLUGIN
#define DELTA_DIRECTOR_UI_PLUGIN

#include "export.h"
#include <QtCore/QObject>
#include <dtEditQt/resourceuiplugin.h>

#include <dtDirector/director.h>

class DirectorToolEditor;

namespace dtEditQt
{
   /**
    * @class DirectorUIPlugin
    * @brief Plugin implementation for a group ui to edit tasks.
    */
   class DirectorUIPlugin : public dtEditQt::ResourceUIPlugin
   {
      Q_OBJECT;
      Q_INTERFACES(dtEditQt::ResourceUIPlugin);

   public:

      DirectorUIPlugin();
      virtual ~DirectorUIPlugin() {}
 
      /**
       * @return the name of the ui plugin.
       */
      virtual const std::string& GetName() const { return mName; }

      ///@return a new instance of the Director Editor.
      virtual QWidget* CreateWidget(QWidget* parent);

   signals:

      /**
       * Signaled when the director file being edited has changed.
       */
      void ScriptChanged(const std::string& script);

   private:
      std::string mName;

      dtCore::RefPtr<dtDirector::Director> mDirector;
      DirectorToolEditor* mEditor;
   };

} // namespace dtEditQt

#endif // DELTA_DIRECTOR_UI_PLUGIN
