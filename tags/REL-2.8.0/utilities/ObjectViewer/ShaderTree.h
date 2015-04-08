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
*/

#ifndef SHADER_TREE
#define SHADER_TREE

#include <QtGui/QTreeWidget>

///////////////////////////////////////////////////////////////////////////////

/**
 * @class ShaderTree
 * @brief This class contains the context menu support for the shader tree.
 */
class ShaderTree : public QTreeWidget
{
public:
   ShaderTree(QWidget* parent = NULL);
   ~ShaderTree();

   void SetShaderSourceEnabled(bool vertexEnabled, 
                               bool geometryEnabled,
                               bool fragmentEnabled);

private:

   void CreateContextActions();
   void CreateContextMenus();

   void contextMenuEvent(QContextMenuEvent* contextEvent);

   // Context Menu
   QAction* mEditShaderDef;
   QAction* mRemoveShaderDef;

   QAction* mOpenVertexSource;
   QAction* mOpenGeometrySource;
   QAction* mOpenFragmentSource;

   QMenu*   mDefinitionContext;
   QMenu*   mProgramContext;
};

#endif // SHADER_TREE
