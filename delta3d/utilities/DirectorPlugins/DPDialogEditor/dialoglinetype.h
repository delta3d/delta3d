#ifndef DIRECTOR_DIALOG_LINE_TYPE
#define DIRECTOR_DIALOG_LINE_TYPE

#include <export.h>
#include <dialogeditor.h>

#include <osg/Referenced>

#include <dtCore/uniqueid.h>

#include <QtCore/QString>


namespace dtDirector
{
   class Node;
}

class QWidget;
class DialogTreeWidget;

/**
 * Base class for a dialog line type.  This class should be inherited
 * to provide your own types of dialog lines.
 */
class DT_DIRECTOR_DIALOG_EDITOR_EXPORT DialogLineType: public osg::Referenced
{
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
    * Generates the Director script node(s) for the given child line.
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
    * Operates on a given child of the current node.
    *
    * @param[in]  item    the parent item.
    * @param[in]  node    The node to operate on.
    * @param[in]  output  The output of the node to find children.
    * @param[in]  editor  The editor.
    */
   void OperateOnChild(QTreeWidgetItem* item, dtDirector::Node* node, const std::string& output, DirectorDialogEditorPlugin* editor);

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
   QTreeWidgetItem* CreateChoice(DialogLineItem* line, const std::string& choiceName, DirectorDialogEditorPlugin* editor, bool moveable = true, bool nameable = true);

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
    * @return           A newly created editor widget.
    */
   virtual QWidget* CreatePropertyEditor(DialogTreeWidget* tree) const;

protected:
};

////////////////////////////////////////////////////////////////////////////////

#endif //DIRECTOR_DIALOG_LINE_TYPE
