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

#ifndef DELTA_PROPERTY_PANEL_H
#define DELTA_PROPERTY_PANEL_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/export.h>
#include <dtQt/basepropertycontrolfactory.h>
#include <dtCore/refptr.h>
#include <QtGui/qwidget.h>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace Ui
{
   class PropertyPanel;
}

namespace dtQt
{
   class BasePropertyControl;



   /////////////////////////////////////////////////////////////////////////////
   // TYPE DEFINTIONS
   /////////////////////////////////////////////////////////////////////////////
   typedef std::vector<dtQt::BasePropertyControl*> PropertyControlList;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT PropertyPanel : public QWidget
   {
      Q_OBJECT
   public:
      typedef QWidget BaseClass;

      PropertyPanel(QWidget* parent = NULL, AbstractPropertyControlFactory* factory = NULL);

      virtual ~PropertyPanel();

      virtual void UpdateUI();

      void SetLabel(const std::string& label);
      const std::string GetLabel() const;

      virtual BasePropertyControl* CreatePropertyControl(const std::string& propertyType);

      int GetProperties(PropertyControlList& outProps);

      int GetPropertiesByName(const std::string& name, PropertyControlList& outProps);

      int GetPropertiesByType(const std::string& propertyType, PropertyControlList& outControls);

      BasePropertyControl* GetProperty(const std::string& name);

      BasePropertyControl* AddProperty(const std::string& propertyType, const std::string& name);

      bool RemoveProperty(const std::string& name);
      int RemoveProperties(PropertyControlList& controls);

      void SortPropertiesByName();
      void SortPropertiesByType();
      void SortPropertiesByGroup();

      virtual int Clear();

   signals:
      void SignalPropertyChanged();
      void SignalPropertyAdded();
      void SignalPropertyRemoved();

   public slots:
      void OnSortModeSelected(const QString& value);

   protected:
      virtual void CreateConnections();

      bool Internal_RemoveProperty(BasePropertyControl* control);

      Ui::PropertyPanel* mUI;

      dtCore::RefPtr<AbstractPropertyControlFactory> mControlFactory;
   };

}

#endif
