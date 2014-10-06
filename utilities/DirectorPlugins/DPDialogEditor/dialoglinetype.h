#ifndef DIRECTOR_DIALOG_LINE_TYPE
#define DIRECTOR_DIALOG_LINE_TYPE

#include <export.h>
#include <dialogeditor.h>

#include <osg/Referenced>

#include <dtCore/uniqueid.h>

#include <QtCore/QString>

#include <QtGui/QLayout>
#include <QtGui/QWidget>

namespace dtDirector
{
   class Node;
}

class QGridLayout;
class DialogChoiceItem;
class DialogLineItem;
class DialogTreeWidget;
class DirectorDialogEditorPlugin;
class QLineEdit;
class QTreeWidgetItem;

/**
 * Base class for a dialog line type.  This class should be inherited
 * to provide your own types of dialog lines.
 */
class DT_DIRECTOR_DIALOG_EDITOR_EXPORT DialogLineType: public QWidget, public osg::Referenced
{
   Q_OBJECT
public:
   DialogLineType();
   virtual ~DialogLineType(void);

   /**
    * The lines type name.
    */
   virtual QString GetName() const = 0;

   /**
    * The lines display color.
    */
   virtual QColor GetColor() const;

   /**
    * Creates a new instance of this class.
    */
   virtual DialogLineType* Create() const = 0;

   /**
    * Initializes this line type.
    *
    * @param[in]  line    The current line.
    * @param[in]  editor  The editor.
    */
   virtual void Init(DialogLineItem* line, DirectorDialogEditorPlugin* editor);

   /**
    * Generates the Director script node(s) for this dialog line.
    * Any node(s) created must continue the chain using the provided
    * output link.  This method should then handle the generation of
    * reference connections (See GenerateNodeForReferences method)
    * and continue to generate nodes for all child lines of the current
    * (See GenerateNodeForChild method).
    *
    * @param[in]  line      The current line item.
    * @param[in]  prevNode  The previous node item.
    * @param[in]  output    The output of the previous item.
    * @param[in]  editor    The editor.
    */
   virtual void GenerateNode(DialogLineItem* line, dtDirector::Node* prevNode, const std::string& output, DirectorDialogEditorPlugin* editor) = 0;

   /**
    * Retrieves whether this line type operates on a given node.
    *
    * @param[in]  node  The node.
    *
    * @return     True if this line type operates on the given node.
    */
   virtual bool ShouldOperateOn(const dtDirector::Node* node) const = 0;

   /**
    * Converts the given node into the data associated with this line.
    *
    * @param[in]  line    The current line.
    * @param[in]  node    The node to operate on.
    * @param[in]  editor  The editor.
    */
   virtual void OperateOn(DialogLineItem* line, dtDirector::Node* node, DirectorDialogEditorPlugin* editor) = 0;

   /**
    * Retrieves whether this line can contain sub-lines.
    */
   virtual bool CanHaveSubLine() const;

   /**
    * Retrieves whether this line contains choices.
    */
   virtual bool IsChoice() const;

   /**
    * If this line is a choice line, this retrieves the maximum number
    * of choices allowed.  -1 means unlimited.
    */
   virtual int GetChoiceLimit() const;

   /**
    * Retrieves the display name for this line.
    *
    * @param[in]  tree  The dialog tree.
    */
   virtual QString GetDisplayName(const DirectorDialogEditorPlugin* editor) const = 0;

   /**
    * Create an editor widget for editing this line within the tree widget.
    *
    * @param[in]  parent  The parent for the new editor widget.
    * @param[in]  tree    The editor tree.
    *
    * @return             A newly created editor widget.
    */
   virtual QWidget* CreateInlineEditor(QWidget* parent, DirectorDialogEditorPlugin* editor) const;

   /**
    * Refreshes the inline editor widget with the current status of the lines data.
    *
    * @param[in]  editor  The editor widget.
    */
   virtual void RefreshInlineEditor(QWidget* editor) const;

   /**
    * Refreshes the data of the line with the contents of the current inline editor.
    *
    * @param[in]  editor  The editor widget.
    */
   virtual void RefreshInlineData(QWidget* editor);

   /**
    * Create an editor widget for editing this line within the property editor panel.
    *
    * @param[in]  tree  The editor tree.
    *
    * @return     A newly created layout.
    */
   virtual QLayout* CreatePropertyEditor(DialogTreeWidget* tree);

   /**
    * Closes the editor widget used for editing.
    *
    * @param[in]  tree  The editor tree.
    */
   virtual void ClosePropertyEditor(DialogTreeWidget* tree);

   /**
    * Create an editor widget for editing the properties of a choice child
    * owned by this line.
    *
    * @param[in]  tree    The editor tree.
    * @param[in]  choice  The child choice being edited.
    * @param[in]  index   The index of the choice being edited.
    *
    * @return     A newly created layout.
    */
   virtual QLayout* CreatePropertyEditorForChild(DialogTreeWidget* tree, DialogChoiceItem* choice, int index);

   /**
    * Closes the editor widget used for editing a choice child.
    *
    * @param[in]  tree    The editor tree.
    * @param[in]  choice  The child choice being edited.
    * @param[in]  index   The index of the choice being edited.
    */
   virtual void ClosePropertyEditorForChild(DialogTreeWidget* tree, DialogChoiceItem* choice, int index);

   /**
    * Event handler for when a child choice item has been removed from
    * this line.
    * Note: This should be overloaded to provide custom implementation
    * for this event.
    *
    * @param[in]  choice  The choice being removed.
    * @param[in]  index   The index of the item before it was removed.
    */
   virtual void OnChildChoiceRemoved(DialogChoiceItem* choice, int index);

   /**
    * Event handler for when a child choice item has been added to
    * this line.
    * Note: This should be overloaded to provide custom implementation
    * for this event.
    *
    * @param[in]  choice  The choice being added.
    * @param[in]  index   The index position of this child.
    */
   virtual void OnChildChoiceAdded(DialogChoiceItem* choice, int index);

   /**
    * Event handler for when a child choice has changed index position.
    * Note: This should be overloaded to provide custom implementation
    * for this event.
    *
    * @param[in]  choice    The choice being moved.
    * @param[in]  oldIndex  The old index position of the choice.
    * @param[in]  newIndex  The new index position of the choice.
    */
   virtual void OnChildChoiceMoved(DialogChoiceItem* choice, int oldIndex, int newIndex);

   /**
    * Event handler for when a speaker has been removed from the speaker listing.
    * Note: This should be overloaded to provide custom implementation
    * for this event.
    *
    * @param[in]  speaker  The speaker that was removed.
    */
   virtual void OnSpeakerRemoved(const QString& speaker);

   /**
    * Event handler for when a speaker has been renamed.
    * Note: This should be overloaded to provide custom implementation
    * for this event.
    *
    * @param[in]  oldName  The speakers old name.
    * @param[in]  newName  The speakers new name.
    */
   virtual void OnSpeakerRenamed(const QString& oldName, const QString& newName);

protected:

   friend class DirectorDialogEditorPlugin;

   /**
    * Handles the generation of the pre and during event output nodes.
    * This is called automatically before the execution of the
    * GenerateNode method. Once this method is executed, the previous
    * node and output parameters will be modified to reflect the new
    * previous node and output values.
    *
    * @param[in]  prevNode  The previous node item.
    * @param[in]  output    The output of the previous item.
    * @param[in]  editor    The editor.
    */
   void GeneratePreEventNode(dtDirector::Node*& prevNode, std::string& output, DirectorDialogEditorPlugin* editor);

   /**
    * Handles the generation of the post event output node.
    * This is called automatically during the execution of the
    * GenerateNodeForChild method. Once this method is executed, the previous
    * node and output parameters will be modified to reflect the new
    * previous node and output values.
    *
    * @param[in]  prevNode  The previous node item.
    * @param[in]  output    The output of the previous item.
    * @param[in]  editor    The editor.
    */
   void GeneratePostEventNode(dtDirector::Node*& prevNode, std::string& output, DirectorDialogEditorPlugin* editor);

   /**
    * Generates the Director script node(s) for the given child line.
    * This also generates the post event output nodes for the given
    * output link.
    *
    * @param[in]  childLine  The child line.
    * @param[in]  prevNode   The previous node item.
    * @param[in]  output     The output of the previous item.
    * @param[in]  editor     The editor.
    */
   void GenerateNodeForChild(DialogLineItem* childLine, dtDirector::Node* prevNode, const std::string& output, DirectorDialogEditorPlugin* editor);

   /**
    * Generates a remote event link to this line if a reference line requires it.
    * This should be called directly after the first node is generated during the
    * GenerateNode method.
    *
    * @param[in]  line    The current line.
    * @param[in]  node    The first node for this line.
    * @param[in]  input   The input to attach the event.
    * @param[in]  editor  The editor.
    */
   void GenerateNodeForReferences(DialogLineItem* line, dtDirector::Node* node, const std::string& input, DirectorDialogEditorPlugin* editor);
   bool recurseFindRef(const dtCore::UniqueId& id, QTreeWidgetItem* item) const;

   /**
    * Operates on a given child of the current node.
    *
    * @param[in]  item    the parent item.
    * @param[in]  node    The node to operate on.
    * @param[in]  output  The output of the node to find children.
    * @param[in]  editor  The editor.
    */
   void OperateOnChild(QTreeWidgetItem* item, dtDirector::Node* node, const std::string& output, DirectorDialogEditorPlugin* editor);

   /**
    * Operates on a given node and determines if it is an event.
    *
    * @param[in]   node       The node to operate on.
    * @param[out]  eventName  The name of the event.
    *
    * @return     True if the given node is a event.
    */
   static bool OperateOnPreEvent(dtDirector::Node* node, QString& eventName);
   static bool OperateOnDuringEvent(dtDirector::Node* node, QString& eventName);
   static bool OperateOnPostEvent(dtDirector::Node* node, QString& eventName);

   /**
    * Creates a new choice item for this line.
    *
    * @param[in]  line        The current line.
    * @param[in]  choiceName  The name of the choice.
    * @param[in]  editor      The editor.
    * @param[in]  moveable    True if this choice is moveable by the user.
    * @param[in]  nameable    True if this choice is nameable by the user.
    *
    * @return     The newly created choice item.
    */
   QTreeWidgetItem* CreateChoice(DialogLineItem* line, const QString& choiceName, DirectorDialogEditorPlugin* editor, bool moveable = true, bool nameable = true);

   /**
    * Checks whether the operated node is being referenced and stores
    * relevant data in the reference mapping.
    *
    * @param[in]  line    The current line.
    * @param[in]  node    The operated node.
    * @param[in]  input   The input link of the operated node.
    * @param[in]  editor  The editor.
    */
   void CheckForReferencing(DialogLineItem* line, dtDirector::Node* node, const std::string& input, DirectorDialogEditorPlugin* editor);

public slots:

   /**
    * Event handlers for when the event check boxes are toggled.
    */
   void OnPreEventCheckBoxChanged(int state);
   void OnDuringEventCheckBoxChanged(int state);
   void OnPostEventCheckBoxChanged(int state);

   /**
    * Event handlers for when the event text edits are changed.
    */
   void OnPreEventTextEdited(const QString& text);
   void OnDuringEventTextEdited(const QString& text);
   void OnPostEventTextEdited(const QString& text);

protected:

   QLayout*     mWrapper;

   QLineEdit*   mPreEventEdit;
   QLineEdit*   mDuringEventEdit;
   QLineEdit*   mPostEventEdit;

   bool         mHasPreEvent;
   bool         mHasDuringEvent;
   bool         mHasPostEvent;

   QString      mPreEventName;
   QString      mDuringEventName;
   QString      mPostEventName;
};

////////////////////////////////////////////////////////////////////////////////

#endif //DIRECTOR_DIALOG_LINE_TYPE
