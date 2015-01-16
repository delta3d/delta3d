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

#ifndef DELTA_OBJECT_TYPE_LIST_PANEL_H
#define DELTA_OBJECT_TYPE_LIST_PANEL_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA
#include <dtQt/export.h>
#include <dtCore/objecttype.h>
#include <dtUtil/functor.h>
// QT
#include <QtGui/QListWidgetItem>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace Ui
{
   class ObjectTypeListPanel;
}



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT ObjectTypeListPanel : public QWidget
   {
      Q_OBJECT
   public:
      typedef QWidget BaseClass;

      typedef std::vector<const dtCore::ObjectType*> ObjectTypeList;

      typedef dtUtil::Functor<bool, TYPELIST_1(const dtCore::ObjectType&)> FilterFunc;
      typedef dtUtil::Functor<QListWidgetItem*, TYPELIST_1(const dtCore::ObjectType&)> ItemCreateFunc;

      ObjectTypeListPanel(QWidget* parent = NULL);

      virtual ~ObjectTypeListPanel();

      /**
       * Sets a custom function for determining which type references should be listed.
       */
      void SetFilterFunc(FilterFunc filterFunc);

      /**
       * Sets a custom function for creating custom list widget items that represent
       * type references other than by simple text.
       */
      void SetItemCreateFunc(ItemCreateFunc createFunc);

      /**
       * Sets the type references to be listed in the UI.
       */
      void SetList(const ObjectTypeList& typeList);
      
      /**
       * Method for capturing all type references listed in the UI.
       * @param outList Collection to capture all references.
       * @return Number of references captured.
       */
      int GetList(ObjectTypeList& outList) const;
      
      /**
       * Method for capturing the selected type references.
       * @param outSelection Collection to capture the selected references.
       * @return Number of references captured.
       */
      int GetSelection(ObjectTypeList& outSelection) const;

      /**
       * Convenience method for collecting all types from the UI list.
       * @param outList Collection to capture the type references.
       * @param selectedOnly Flag to control whether only selected type are captured.
       * @return Number of references that were captured.
       */
      int GetObjectTypesFromList(ObjectTypeList& outList, bool selectedOnly = false) const;
   
      /**
       * Convenience method for finding an type reference by name over the
       * type list that was last set on this object.
       * @param name Name of the type reference to find.
       * @return Reference to an type object that matched the specified name; NULL otherwise.
       */
      const dtCore::ObjectType* FindObjectType(const std::string& name) const;

      /**
       * Method for simply determining the number of items in the list.
       * @return Number of items appearing in the list.
       */
      int GetItemCount() const;

      /**
       * Method to set whether the list will allow multiple selection or single selection.
       */
      void SetSingleSelectMode(bool singleSelect);

      /**
       * Method to determine if the list will allow multiple selection or single selection.
       */
      bool IsSingleSelectMode() const;

      /**
       * Refreshes the UI to list all available items.
       */
      virtual void UpdateUI();

   signals:
      void SignalSelectionChanged(const ObjectTypeList& typeList);
      void SignalUpdated();

   public slots:
      void OnSelectionChanged();

   protected:
      virtual void CreateConnections();

      Ui::ObjectTypeListPanel* mUI;

      FilterFunc mFilterFunc;
      ItemCreateFunc mItemCreateFunc;

      ObjectTypeList mObjectTypeList;
   };

}

#endif
