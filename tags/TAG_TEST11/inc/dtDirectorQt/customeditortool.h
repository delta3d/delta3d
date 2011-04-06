/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
 */

#ifndef DIRECTORQT_CUSTOM_EDITOR_TOOL
#define DIRECTORQT_CUSTOM_EDITOR_TOOL

#include <dtDirectorQt/export.h>

#include <dtDirectorQt/directoreditor.h>

namespace dtDirector
{
   /**
    * @class CustomEditorTool
    *
    * @brief Base class for custom editing tools for special Director sub-Graphs.
    */
   class DT_DIRECTOR_QT_EXPORT CustomEditorTool
   {
   public:
      /**
       * Constructs a new help box.
       *
       * @param[in]  name  The name of the tool.
       */
      CustomEditorTool(const std::string& name);

      /**
       * Destructor
       */
      virtual ~CustomEditorTool();

      /**
       * Retrieves the name of the tool.
       */
      const std::string& GetName() const {return mToolName;}

      /**
       * Sets the main Director Editor.
       *
       * @param[in]  editor  The editor.
       */
      void SetEditor(DirectorEditor* editor);

      /**
       * Retrieves the main Director Editor.
       *
       * @return  The editor.
       */
      DirectorEditor* GetEditor() const {return mEditor;}

      /**
       * Retrieves the currently openned Director Graph.
       *
       * @return  The Director Graph.
       */
      DirectorGraph* GetGraph() const {return mGraph;}

      /**
       * Event handler when this tool should be opened for the given graph.
       * @Note:  This method should be overloaded to perform any initial
       * operations when this tool is activated.
       *
       * @param[in]  graph  The graph to open the tool for.
       */
      virtual void Open(DirectorGraph* graph);

      /**
       * Event handler to close the tool.
       * @Note:  This method should be overloaded to perform any shut down
       * operations when this tool is deactivated.
       */
      virtual void Close();

      /**
       * Retrieves whether the editor is currently open.
       *
       * @return  True if the editor is currently open.
       */
      bool IsOpen() const {return mIsOpen;}

   protected:

   private:

      bool              mIsOpen;

      std::string       mToolName;
      DirectorEditor*   mEditor;

      DirectorGraph*    mGraph;
   };

} // namespace dtDirector

#endif // DIRECTORQT_CUSTOM_EDITOR_TOOL
