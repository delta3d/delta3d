#ifndef DELTA_BASEPROPERTYCONTROL_H
#define DELTA_BASEPROPERTYCONTROL_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/export.h>
#include <dtCore/actorproperty.h>
#include <dtCore/datatype.h>
// Qt
#include <QtGui/QLabel.h>
#include <QtGui/QWidget.h>
// OSG
#include <osg/Referenced>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace Ui
{
   class BasePropertyControl;
}



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // TYPE DEFINITIONS
   /////////////////////////////////////////////////////////////////////////////
   typedef dtCore::ActorProperty PropertyType;
   typedef std::vector<dtCore::RefPtr<PropertyType> > PropertyArray;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT BasePropertyControl : public QWidget
   {
      Q_OBJECT
   public:
      typedef QWidget BaseClass;

      BasePropertyControl(QWidget* parent, dtCore::DataType& dataType);

      virtual ~BasePropertyControl();

      virtual void Init();

      virtual QObject* GetControlUI() = 0;

      virtual QLabel* GetLabel() const = 0;

      dtCore::DataType& GetDataType() const;

      /**
       * Update the UI values from the referenced data.
       */
      virtual void UpdateUI(const PropertyType& prop) = 0;

      /**
       * Update the referenced data with values from the UI.
       */
      virtual void UpdateData(PropertyType& propToUpdate) = 0;

      void SetLabelText(const std::string& label);
      const std::string GetLabelText() const;

      void SetToolTip(const std::string& text);
      const std::string GetToolTip() const;

      PropertyType* GetLinkedProperty(int index = 0) const;
      void AddLinkedProperty(PropertyType& prop);

      const PropertyArray& GetLinkedProperties() const;
      void AddLinkedProperties(PropertyArray& props);

      int Clear();

   signals:
      void SignalUIChanged(); // Signal to emit if user changes value.
      void SignalDataChanged(); // Signal to emit if underlying property value changed.

      void SignalUIUpdated(); // Signal to emit when UI value matches the data.
      void SignalDataUpdated(); // Signal to emit when data has been updated with values from UI.
      void SignalDataAboutToUpdate(); // Signal to emit before data is modified with values from UI.

   public slots:
      void OnUIChanged();
      void OnDataChanged();
      virtual void OnPropertyAdded();

   private:
      /**
       * Update the UI values from the referenced data.
       */
      void UpdateUI_Internal();

      /**
       * Update the referenced data with values from the UI.
       */
      void UpdateData_Internal();

   protected:
      virtual void CreateConnections();

      Ui::BasePropertyControl* mBaseUI;
      dtCore::DataType* mDataType;

      PropertyArray mProps;
   };
}

#endif
