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
 * William E. Johnson II
 * David Guthrie
 */

#ifndef DELTA_LIBRARY_EDITOR
#define DELTA_LIBRARY_EDITOR


#include <dtQt/baselibrarylisteditor.h>

namespace dtEditQt
{
   class LibraryEditor : public dtQt::BaseLibraryListEditor
   {
      Q_OBJECT

   public:
      /// Constructor
      LibraryEditor(QWidget* parent = NULL);

      /// Destructor
      virtual ~LibraryEditor();

   public slots:
      /// Pop up the file browser for libraries
      virtual void SpawnFileBrowser();

      /// Confirm deletion of libraries
      virtual void SpawnDeleteConfirmation();

      /// Shift the current library up 1 position
      virtual void ShiftLibraryUp();

      /// Shift the current library down 1 position
      virtual void ShiftLibraryDown();

   protected:
      // private function to obtain the library names from the map
      // and convert them to QT format
      virtual void GetLibraryNames(std::vector<QListWidgetItem*>& items) const;
   };

} // namespace dtEditQt

#endif // DELTA_LIBRARY_EDITOR
