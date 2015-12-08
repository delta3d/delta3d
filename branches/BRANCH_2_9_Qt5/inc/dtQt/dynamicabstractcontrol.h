/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - dynamicabstractcontrol (.h & .cpp) - Using 'The MIT License'
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
 * Curtiss Murphy
 */
#ifndef DELTA_DYNAMICABSTRACTCONTROL
#define DELTA_DYNAMICABSTRACTCONTROL

#include <dtQt/export.h>
#include <QtCore/QObject>
#include <dtCore/actorproxy.h>
#include <dtCore/actorproperty.h>
#include <dtQt/typedefs.h>
#include <QtWidgets/QAbstractItemDelegate>

#include <dtCore/refptr.h>
#include <dtUtil/objectfactory.h>

class QWidget;
class QGridLayout;
class QStyleOptionViewItem;
class QModelIndex;
class QLineEdit;
class QPainter;
class QSize;
class QColor;

namespace dtCore
{
   class PropertyContainer;
   class ActorProperty;
}

namespace dtQt
{

   // The number of digits to the right of a decimal point for floats/doubles
   const unsigned int NUM_DECIMAL_DIGITS_FLOAT  = 8;
   const unsigned int NUM_DECIMAL_DIGITS_DOUBLE = 15;

   class PropertyEditorModel;
   class PropertyEditorTreeView;

   class DynamicControlFactory;
   class SubQToolButton;

   /**
   * @class DynamicAbstractControl
   * @brief This is the base class for all the dynamic controls that are used in the property editor
   */
   class DT_QT_EXPORT DynamicAbstractControl : public QObject
   {
      Q_OBJECT
   public:
      struct LinkedPropertyData
      {
         dtCore::RefPtr<dtCore::PropertyContainer> propCon;
         dtCore::ActorProperty*                    property;
      };

      /**
      * Constructor
      */
      DynamicAbstractControl();

      /**
      * Destructor
      */
      virtual ~DynamicAbstractControl();

      ///If a control needs to create sub-controls, here's where it can get the factory.
      DynamicControlFactory* GetDynamicControlFactory();

      ///If a control needs to create sub-controls, here's where it can get the factory.
      void SetDynamicControlFactory(DynamicControlFactory* factory);

      /**
      * Initialize the object.  We don't do this in the constructor because it's created via the
      * ObjectFactory which primarily works with parameterless constructors at the moment.
      * @note - The base class does not store the property.  Each subclass is responsible for doing
      * that.
      */
      virtual void InitializeData(DynamicAbstractControl* newParent, PropertyEditorModel* model,
               dtCore::PropertyContainer* pc, dtCore::ActorProperty* property);

      /**
      * This method is called by the delegate between the model and the view when data should
      * be pushed to the editor from the model.  I'm not exactly sure when that happens, but
      * I assume that's at construction time of the editor.  The default does nothing.
      *
      * @NOTE - In all likelihood, this method won't need the widget that's passed in,
      * since it's tracked by the individual subclass.
      */
      virtual void updateEditorFromModel(QWidget* widget);

      /**
      * This method is called by the delegate between the model and the view when data should
      * be pushed into the model from the editor.  This is called when the control thinks
      * it's done editing.  Perhaps the user pressed a key, or tabbed away, or something
      * like that.  The default does nothing and returns false.
      *
      * @return Returns true if the model was successfully changed.  ie, the data was different
      * and there were no errors.
      *
      * @note It would be nice if the caller of this method would generate the event message.
      * However, since it can be called from a variety of places, we have to generate the
      * property changed event from inside here.  So, don't forget :)
      */
      virtual bool updateModelFromEditor(QWidget* widget);

      /**
      * Create the editor widget to be used when editing this control.  It can be complex
      * or simple.   The default method does nothing.
      */
      virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
               const QModelIndex& index);

      /**
       *	Adds a linked property to the control.
       *
       * @param[in]  property  The linked property to add.
       */
      void AddLinkedProperty(dtCore::RefPtr<dtCore::PropertyContainer>& propCon, dtCore::ActorProperty* property);

      /**
       *	Gets the linked properties from this control.
       */
      std::vector<LinkedPropertyData>& GetLinkedProperties();
      const std::vector<LinkedPropertyData>& GetLinkedProperties() const;

      /**
       *	Sets the value of all linked properties to the current value
       * of the base property.
       */
      void CopyBaseValueToLinkedProperties(bool resetAlwaysSave = false);

      /**
      * Returns the parent of this control.  NULL if there is no parent.  All controls have
      * a parent except for those at the root.
      */
      DynamicAbstractControl* getParent();

      /**
      * This retuns the index of the child so that the model can construct a QModelIndex.
      * Note that not all dynamic controls can have children.  However, there are several
      * that can (such as Groups and complex controls like the Vec3).  Therefore, we
      * put the behavior on the base.  If a control doesn't support children, just returns 0.
      * Default is 0.
      */
      virtual int getChildIndex(DynamicAbstractControl* child);

      /**
      * This is the reverse of getChildIndex.  It gets the child from the index.  Note again that
      * not all controls can have children.  If you don't have children, then return NULL.
      * Default is NULL
      */
      virtual DynamicAbstractControl* getChild(int index);

      /**
      * This tells you how many children you have.  Note again that not all controls have children.
      * Default is 0.
      */
      virtual int getChildCount();

      /**
      * This will be called by the child to notify their parent when they are being edited.
      * This function should be overwritten if the parent needs to know when any of its
      * children are about to be edited.
      */
      virtual void OnChildPreUpdate(DynamicAbstractControl* child);

      /**
      * Essentially returns a displayable string.  Each control needs to handle this specially, since
      * some controls have properties and some don't.  Ie, a group control doesn't have a real
      * property under it, nor does the X and Y values of Vec3. Default is "".
      */
      virtual const QString getDisplayName();

      /**
      * Returns a helpful description.  Usually a tooltip string.  Each control needs to handle
      * this, since some controls have properties and some don't.  Ie, a group control doesn't
      * have a real property under it, nor does the X and Y values of Vec3. Default is "".
      */
      virtual const QString getDescription();

      /**
      * Returns a string representation of the value.  This is typically only useful for
      * controls that are not editable.  Default is "".
      */
      virtual const QString getValueAsString();

      /**
       * Converts of a float to as string in a consistent manner for the property editor.
       */
      static QString RealToString( qreal num, int digits );

      /**
      * Returns true if this control is typically editable.  For instance, a float control is editable,
      * but a group isn't.  Note that the subelements of a vector are editor whereas the vector control
      * itself isn't editable.  If you return true, then you need to be able to delegate a control
      * to edit the data.  Default is false.
      */
      virtual bool isEditable();

      /**
      * Returns true if this control knows how to do custom painting for a specific column.  This is
      * necessary for some controls that have non string elements like a button or a color swatch.
      * If this method returns true, you MUST override the paint and sizeHint methods or you won't
      * see anything for your row. This method is based on the column, so you will only get
      * paint and sizehint calls for a column that returns true.  Default is false.
      * @note Be careful using this on editable columns as you may get yourself in trouble with
      * nested and competing paints.
      * @return True if your control does custom paint and sizeHint() for this column.
      */
      virtual bool isControlDoesCustomPainting(int column);

      /**
       *	Returns true if the base property as well as all linked properties match.
       */
      virtual bool doPropertiesMatch();

      /**
      * This method allows you to do a custom paint on a specific column for a control.
      * It works with isControlDoesCustomPainting and sizeHint().
      * For instance, the color control might put up a specific picker button or maybe a little
      * color swatch of the selected color.  The only way to do that would be in here.
      */
      virtual void paintColumn(int column, QPainter* painter, const QStyleOptionViewItem& opt);

      /**
      * This method works with isControlDoesCustomPainting() and paintcolumn.  You should
      * return the size hint for your column.  Default does nothing.
      * @return the QSize sizehint for this column in the tree.
      */
      virtual QSize sizeHint(int column, const QStyleOptionViewItem& opt);

      /**
      * Gets the editor model from the control.  It should be rare that you ever need this.
      */
      PropertyEditorModel* GetModel()
      { return mModel; }

      /**
      * This was a temp hack used to set a parent widget for creating sub widgets.
      * This shouldn't be needed and can be deleted once the final tree is settled on
      */
      void SetTreeView(PropertyEditorTreeView* newPropertyTree);

      /**
      * Called when the abstract control is constructed to determine whether or not we
      * need a persistent editor.  Persistent editors are created after the control is
      * created and closed when the control is removed from the tree. This is used
      * for some controls that need to ALWAYS show a button or edit box in order to
      * prevent user confusion.  Default is false;
      *
      * @note This doesn't work.  Well, the method works, but there is currently no way
      * to make an editor persistent as I'd like.  By persistent, I mean, actually make
      * the control show up permanently, so that the user doesn't have to click anything
      * to make it show up.
      */
      virtual bool NeedsPersistentEditor();

      /**
      * A simple utility method to set the background color for all modes for a widget.
      * This is typically used when we have wrapper widgets with children in the edit
      * portion of a dynamic control to make the control blend in better...
      */
      void SetBackgroundColor(QWidget* widget, QColor color);

      /**
      * Allows the abstract control to work with the delegate event filter.  If you
      * have any nested controls that are created as part of the editor, you need to
      * use one of the sub controls and make sure you handle NULL'ing your pointer and
      * then override this method to install an event filter on your non-null editor objects.
      * This is called by the PropertyEditorDelegate after the editor is created.
      * The default calls QObject::installEventFilter()
      */
      virtual void InstallEventFilterOnControl(QObject* filterObj);

      /**
      * This will notify the parent that the child is about to be updated.
      */
      virtual void NotifyParentOfPreUpdate();

      /**
       * Is the value this Control is displaying the default for the ActorProperty?
       *
       * @return true if the ActorProperty is set to it's default value, false otherwise
       */
      virtual bool IsPropertyDefault();

      /**
       * Sets the array index for this property control if it
       * belongs to an array property.
       */
      void SetArrayIndex(int index) {mArrayIndex = index;}

      /**
       * Retrieves the array index for this property control.
       * Returns -1 if this property does not belong to an array.
       */
      int GetArrayIndex () const {return mArrayIndex;}

      /**
      * When a property changes, we have to update our editor.  It is likely that
      * many properties will change with no effect, but if the user is using undo/redo
      * or is moving an actor in the viewport, then it is possible that they will also
      * be sitting on the editor for one of the affected values. This gives us a chance
      * to reflect the change in our editor.
      * @note The default implementation does nothing.
      */
      virtual void actorPropertyChanged(dtCore::PropertyContainer& propCon, dtCore::ActorProperty& property);

      bool IsInitialized() const { return mInitialized; }

   signals:
      void SignalShiftUpClicked(int itemIndex);
      void SignalShiftDownClicked(int itemIndex);
      void SignalCopyClicked(int itemIndex);
      void SignalDeleteClicked(int itemIndex);

      void PropertyAboutToChange(dtCore::PropertyContainer&, dtCore::ActorProperty& prop,
               std::string oldValue, std::string newValue);

      void PropertyChanged(dtCore::PropertyContainer&, dtCore::ActorProperty& prop);

   public slots:

      /// Event handler when the property has changed.
      void OnPropertyChanged(dtCore::PropertyContainer& propCon, dtCore::ActorProperty& prop);

      /**
       * Called when we should take the data out of the controls and put it into the
       * actor.  This is typically trapped to a lost focus or return pressed or similar
       * user event behavior.  This can also be called by the parent control at the
       * moment we change selection.
       * @return Returns true if any data was actually changed and successfully set on the control
       * @note - This is purely virtual
       */
      virtual bool updateData(QWidget* widget);

      /**
       * Called when the user presses the enter key on the editing widget.
       */
      virtual void enterPressed();

     /**
      * This method is called by one of the Sub Widgets from DynamicSubWidgets.h from the
      * destructor of the passed in widget.  The intent is to work around the way editors
      * work in QT trees.  Since the editors are created only when the user should be editing the
      * field and then destroyed as soon as they lose focus, it is important that any Dynamic
      * controls not be holding on to bogus pointers.  This method gives you a chance to set
      * a pointer back to NULL or whatever.
      */
      virtual void handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint = QAbstractItemDelegate::NoHint);

      /**
       * Signal when the Reset button has been clicked.
       */
      virtual void onResetClicked();

      /**
       * Signal when the Shift Up button has been clicked.
       */
      void onShiftUpClicked();

      /**
       * Signal when the Shift Down button has been clicked.
       */
      void onShiftDownClicked();

      /**
       * Signal when the Copy button has been clicked.
       */
      void onCopyClicked();

      /**
       * Signal when the Delete button has been clicked.
       */
      void onDeleteClicked();

   protected slots:
      /// This is so child controls can, if desired can be connected to this which will emit the related signal.
      virtual void PropertyAboutToChangePassThrough(dtCore::PropertyContainer&, dtCore::ActorProperty& prop,
               std::string oldValue, std::string newValue);

      /// This is so child controls can, if desired can be connected to this which will emit the related signal.
      virtual void PropertyChangedPassThrough(dtCore::PropertyContainer&, dtCore::ActorProperty& prop);

   protected:

      /**
       * Updates the enabled status of the control buttons after an update.
       */
      virtual void UpdateButtonStates();

      // indicates whether the object has been initialized
   private:
      bool mInitialized;
   protected:

      int  mArrayIndex;

      dtCore::RefPtr<dtCore::PropertyContainer> mPropContainer;
      dtCore::ObserverPtr<dtCore::ActorProperty> mBaseProperty;

      // Linked properties are a list of all other properties that are
      // determined to "match" the base property by name, type, and category.
      // These properties belong to any multiply selected property containers
      // to be displayed within the property editor.
      std::vector<LinkedPropertyData> mLinkedProperties;

      // The parent control of this control.  All controls have a parent except root level
      // controls which are likely to be groups
      DynamicAbstractControl *mParent;
      PropertyEditorModel *mModel;

      PropertyEditorTreeView *mPropertyTree;

      // Reset to default button.
      SubQToolButton* mDefaultResetButton;
      SubQToolButton* mShiftUpButton;
      SubQToolButton* mShiftDownButton;
      SubQToolButton* mCopyButton;
      SubQToolButton* mDeleteButton;

      // Grid layout for editor widget.
      QGridLayout* mGridLayout;

      QWidget* mWrapper;

   private:
      dtCore::RefPtr<DynamicControlFactory> mControlFactory;
   };

   ////////////////////////////
   class DynamicControlFactory: public osg::Referenced
   {
   public:
      DynamicControlFactory();

      DynamicAbstractControl* CreateDynamicControl(const dtCore::ActorProperty& prop);

      template <typename DynControlType>
      void RegisterControlForDataType(dtCore::DataType& dataType)
      {
         mControlFactory->RemoveType(&dataType);
         mControlFactory->RegisterType<DynControlType>(&dataType);
      }

   private:
      ~DynamicControlFactory();
      // the dynamic control factory can create objects for each type.
      dtCore::RefPtr< dtUtil::ObjectFactory<dtCore::DataType*, DynamicAbstractControl> > mControlFactory;
   };
} // namespace dtEditQt

#endif // DELTA_DYNAMICABSTRACTCONTROL
