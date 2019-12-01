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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ui_propertypanel.h"
#include <dtQt/propertypanel.h>
#include <dtQt/propertycontrolfactory.h>
#include <dtUtil/log.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // HELPER FUNCTIONS
   /////////////////////////////////////////////////////////////////////////////
   dtQt::BasePropertyControl* ConvertWidgetToControl(QWidget& widget)
   {
      // TODO: Determine a proper way to check and convert.
      return (dtQt::BasePropertyControl*)&widget;
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   PropertyPanel::PropertyPanel(QWidget* parent, AbstractPropertyControlFactory* factory)
      : BaseClass(parent)
      , mUI(new Ui::PropertyPanel)
      , mControlFactory(factory)
   {
      mUI->setupUi(this);

      // Ensure that a valid factor exists.
      if ( ! mControlFactory.valid())
      {
         // Create a default property factory.
         mControlFactory = new PropertyControlFactory;
      }

      CreateConnections();

      UpdateUI();
   }

   PropertyPanel::~PropertyPanel()
   {
      delete mUI;
      mUI = NULL;
   }

   void PropertyPanel::CreateConnections()
   {
      connect(mUI->mSortMode, SIGNAL(currentIndexChanged(const QString&)),
         this, SLOT(OnSortModeSelected(const QString&)));
   }

   void PropertyPanel::UpdateUI()
   {
      // TODO:
   }

   void PropertyPanel::SetLabel(const std::string& label)
   {
      mUI->mLabel->setText(tr(label.c_str()));
   }

   const std::string PropertyPanel::GetLabel() const
   {
      return mUI->mLabel->text().toStdString();
   }

   BasePropertyControl* PropertyPanel::CreatePropertyControl(const std::string& propertyType)
   {
      BasePropertyControl* control = mControlFactory->CreateControl(propertyType);
      if (control != NULL)
      {
         control->Init();
      }
      else
      {
         LOG_WARNING("Could not create control for type: " + propertyType);
      }
      return control;
   }

   BasePropertyControl* PropertyPanel::AddProperty(const std::string& propertyType, const std::string& name)
   {
      BasePropertyControl* control = CreatePropertyControl(propertyType);

      if (control != NULL)
      {
         control->SetLabelText(name);

         mUI->mContentArea->addWidget(control);

         emit SignalPropertyAdded();
      }

      return control;
   }

   /////////////////////////////////////////////////////////////////////////////
   // HELPER STRUCT
   struct FindPropertyControlVisitor
   {
      enum FindMode {
         FIND_NAME,
         FIND_TYPE
      };

      FindPropertyControlVisitor(FindMode mode, const std::string& name, QLayout& layout)
         : mMode(mode)
         , mName(name)
         , mLayout(&layout)
      {}

      int DoFind()
      {
         int count = 0;

         int limit = mLayout->count();
         for (int i = 0; i < limit; ++i)
         {
            dtQt::BasePropertyControl* control = ConvertWidgetToControl(*mLayout->itemAt(i)->widget());
            if (control != NULL)
            {
               control->GetLabelText() == mName;
               mList.push_back(control);
            }
         }

         return count;
      }

      std::string mName;
      FindMode mMode;
      QLayout* mLayout;
      dtQt::PropertyControlList mList;
   };

   BasePropertyControl* PropertyPanel::GetProperty(const std::string& name)
   {
      BasePropertyControl* control = NULL;

      // Get the control by name.
      FindPropertyControlVisitor visitor(
         FindPropertyControlVisitor::FIND_NAME,
         name, *mUI->mContentArea);

      if ( ! visitor.mList.empty())
      {
         control = visitor.mList.front();
      }

      return control;
   }

   int PropertyPanel::GetProperties(PropertyControlList& outControls)
   {
      int count = 0;

      const QObjectList& controlList = mUI->mContentArea->children();
      if ( ! controlList.empty())
      {
         dtQt::BasePropertyControl* control = NULL;
         QObjectList::const_iterator curIter = controlList.begin();
         QObjectList::const_iterator endIter = controlList.end();
         for (; curIter != endIter; ++curIter)
         {
            control = dynamic_cast<dtQt::BasePropertyControl*>(*curIter);
            if (control != NULL)
            {
               ++count;
               outControls.push_back(control);
            }
         }
      }

      return count;
   }

   int PropertyPanel::GetPropertiesByName(const std::string& name, PropertyControlList& outControls)
   {
      int totalMatches = 0;

      FindPropertyControlVisitor visitor(
         FindPropertyControlVisitor::FIND_NAME,
         name, *mUI->mContentArea);

      if ( ! visitor.mList.empty())
      {
         totalMatches = (int)visitor.mList.size();

         outControls.insert(outControls.end(), visitor.mList.begin(), visitor.mList.end());
      }

      return totalMatches;
   }

   int PropertyPanel::GetPropertiesByType(const std::string& propertyType, PropertyControlList& outProps)
   {
      int totalMatches = 0;

      FindPropertyControlVisitor visitor(
         FindPropertyControlVisitor::FIND_TYPE,
         propertyType, *mUI->mContentArea);

      if ( ! visitor.mList.empty())
      {
         totalMatches = (int)visitor.mList.size();

         outProps.insert(outProps.end(), visitor.mList.begin(), visitor.mList.end());
      }

      return totalMatches;
   }

   bool PropertyPanel::RemoveProperty(const std::string& name)
   {
      return Internal_RemoveProperty(GetProperty(name));
   }

   int PropertyPanel::RemoveProperties(PropertyControlList& controls)
   {
      int totalMatches = 0;

      BasePropertyControl* control = NULL;
      PropertyControlList::iterator curIter = controls.begin();
      PropertyControlList::iterator endIter = controls.end();
      for ( ; curIter != endIter; ++curIter)
      {
         control = *curIter;

         if (Internal_RemoveProperty(control))
         {
            ++totalMatches;
         }
      }

      return totalMatches;
   }

   bool PropertyPanel::Internal_RemoveProperty(BasePropertyControl* control)
   {
      int numChildren = mUI->mContentArea->children().size();

      mUI->mContentArea->removeWidget(control);

      bool success = numChildren != mUI->mContentArea->children().size();

      if (success)
      {
         emit SignalPropertyRemoved();
      }

      return success;
   }

   /////////////////////////////////////////////////////////////////////////////
   // HELPER STRUCT
   struct WidgetSortFilter
   {
      enum SortMode {
         SORT_GROUP,
         SORT_NAME,
         SORT_TYPE
      };

      SortMode mMode;

      WidgetSortFilter(SortMode mode)
         : mMode(mode)
      {}

      bool operator() (BasePropertyControl* a, BasePropertyControl* b)
      {
         if (mMode == SORT_TYPE)
         {
            return a->GetDataType().GetName() < b->GetDataType().GetName();
         }
         else if (mMode == SORT_GROUP)
         {
            dtQt::PropertyType* propA = a->GetLinkedProperty();
            dtQt::PropertyType* propB = b->GetLinkedProperty();
            return propA == NULL || propB == NULL ? true
               : propA->GetGroupName() < propB->GetGroupName();
         }

         // Name Sort
         return a->GetLabel() < b->GetLabel();
      }
   };

   void SortProperties(QLayout& layout, WidgetSortFilter::SortMode sortMode)
   {
      PropertyControlList controls;

      QLayoutItem *item = NULL;
      while (layout.count())
      {
         item = layout.takeAt(0);
         dtQt::BasePropertyControl* control = ConvertWidgetToControl(*item->widget());

         if (control != NULL)
         {
            controls.push_back(control);
         }
         else
         {
            delete item->widget();
         }

         delete item;
      }

      if ( ! controls.empty())
      {
         WidgetSortFilter pred(sortMode);
         std::sort(controls.begin(), controls.end(), pred);

         PropertyControlList::iterator curIter = controls.begin();
         PropertyControlList::iterator endIter = controls.end();
         for (; curIter != endIter; ++curIter)
         {
            layout.addWidget(*curIter);
         }
      }
   }

   void PropertyPanel::SortPropertiesByName()
   {
      SortProperties(*mUI->mContentArea, WidgetSortFilter::SORT_NAME);
   }

   void PropertyPanel::SortPropertiesByType()
   {
      SortProperties(*mUI->mContentArea, WidgetSortFilter::SORT_TYPE);
   }

   void PropertyPanel::SortPropertiesByGroup()
   {
      SortProperties(*mUI->mContentArea, WidgetSortFilter::SORT_GROUP);
   }
   
   void PropertyPanel::OnSortModeSelected(const QString& value)
   {
      if (value == QString("Name"))
      {
         SortPropertiesByName();
      }
      else if (value == QString("Type"))
      {
         SortPropertiesByType();
      }
      else if (value == QString("Group"))
      {
         SortPropertiesByGroup();
      }
   }

   int PropertyPanel::Clear()
   {
      int count = 0;

      QLayoutItem *item = NULL;
      while ((item = mUI->mContentArea->takeAt(0)) != 0)
      {
         delete item->widget();
         delete item;
         ++count;
      }

      return count;
   }

}
