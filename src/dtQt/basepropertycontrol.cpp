
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ui_basepropertycontrol.h"
#include <dtQt/basepropertycontrol.h>
#include <dtUtil/log.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   BasePropertyControl::BasePropertyControl(QWidget* parent, dtCore::DataType& dataType)
      : BaseClass(parent)
      , mBaseUI(new Ui::BasePropertyControl)
      , mDataType(&dataType)
   {
      mBaseUI->setupUi(this);
   }

   void BasePropertyControl::Init()
   {
      QObject* controls = GetControlUI();
      if (controls == NULL)
      {
         LOG_ERROR("No widgets were created for property control.");
      }

      QLayout* layout = dynamic_cast<QLayout*>(controls);
      QWidget* widget = dynamic_cast<QWidget*>(controls);

      if (widget != NULL)
      {
         mBaseUI->mContentArea->layout()->addWidget(widget);
      }
      else if (layout != NULL)
      {
         QLayout* contentLayout = mBaseUI->mContentArea->layout();

         // Replace the content area layout if it is not the same
         // as the one specified by the property widget.
         if (contentLayout != layout)
         {
            if (contentLayout != NULL)
            {
               delete contentLayout;
               contentLayout = NULL;
            }

            mBaseUI->mContentArea->setLayout(layout);
         }
      }

      // Create the widget connections after updating the UI
      // so that initial value setting on widgets does not
      // trigger unwanted updates.
      CreateConnections();
   }

   void BasePropertyControl::OnPropertyAdded()
   {
      emit SignalDataChanged();
   }

   BasePropertyControl::~BasePropertyControl()
   {
      delete mBaseUI;
      mBaseUI = NULL;
   }

   void BasePropertyControl::CreateConnections()
   {
      connect(this, SIGNAL(SignalUIChanged()),
         this, SLOT(OnUIChanged()));
      connect(this, SIGNAL(SignalDataChanged()),
         this, SLOT(OnDataChanged()));
   }

   dtCore::DataType& BasePropertyControl::GetDataType() const
   {
      return *mDataType;
   }

   void BasePropertyControl::UpdateUI_Internal()
   {
      if ( ! mProps.empty())
      {
         PropertyType* prop = mProps.front();

         // Update the tooltip.
         if (GetToolTip().empty())
         {
            const std::string& desc = prop->GetDescription();
            SetToolTip(desc);
         }

         // Set property data values to UI.
         UpdateUI(*prop);

         mBaseUI->mContentArea->setEnabled( ! prop->IsReadOnly());

         emit SignalUIUpdated();
      }
   }

   void BasePropertyControl::UpdateData_Internal()
   {
      if ( ! mProps.empty())
      {
         emit SignalDataAboutToUpdate();

         PropertyType* curProp = NULL;
         PropertyArray::iterator curIter = mProps.begin();
         PropertyArray::iterator endIter = mProps.end();

         for ( ; curIter != endIter; ++curIter)
         {
            curProp = curIter->get();

            // Set UI values to property data.
            UpdateData(*curProp);
         }

         emit SignalDataUpdated();
      }
   }

   void BasePropertyControl::SetLabelText(const std::string& label)
   {
      GetLabel()->setText(tr(label.c_str()));
   }

   const std::string BasePropertyControl::GetLabelText() const
   {
      return GetLabel()->text().toStdString();
   }

   void BasePropertyControl::SetToolTip(const std::string& text)
   {
      GetLabel()->setToolTip(tr(text.c_str()));
   }

   const std::string BasePropertyControl::GetToolTip() const
   {
      return GetLabel()->toolTip().toStdString();
   }

   PropertyType* BasePropertyControl::GetLinkedProperty(int index) const
   {
      PropertyType* prop = NULL;

      if (index < (int)mProps.size())
      {
         prop = mProps[index].get();
      }

      return prop;
   }

   void BasePropertyControl::AddLinkedProperty(PropertyType& prop)
   {
      mProps.push_back(&prop);

      OnPropertyAdded();
   }

   const PropertyArray& BasePropertyControl::GetLinkedProperties() const
   {
      return mProps;
   }

   void BasePropertyControl::AddLinkedProperties(PropertyArray& props)
   {
      if ( ! props.empty())
      {
         mProps.insert(mProps.end(), props.begin(), props.end());

         OnPropertyAdded();
      }
   }

   int BasePropertyControl::Clear()
   {
      int count = (int)mProps.size();
      mProps.clear();
      return count;
   }

   void BasePropertyControl::OnUIChanged()
   {
      UpdateData_Internal();
   }

   void BasePropertyControl::OnDataChanged()
   {
      UpdateUI_Internal();
   }

}
