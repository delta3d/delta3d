#ifndef DIRECTOR_DIALOG_DELAY_LINE_TYPE
#define DIRECTOR_DIALOG_DELAY_LINE_TYPE

#include <export.h>
#include <dialoglinetype.h>

////////////////////////////////////////////////////////////////////////////////
class DT_DIRECTOR_DIALOG_EDITOR_EXPORT DialogDelayLineType: public DialogLineType
{
public:
   DialogDelayLineType();
   virtual ~DialogDelayLineType(void);

   /**
    * The lines type name.
    */
   virtual QString GetName() const;

   /**
    * Creates a new instance of this class.
    */
   virtual DialogLineType* Create() const;

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
   virtual void GenerateNode(DialogLineItem* line, dtDirector::Node* prevNode, const std::string& output, DirectorDialogEditorPlugin* editor);

   /**
    * Retrieves whether this line type operates on a given node.
    *
    * @param[in]  node  The node.
    *
    * @return     True if this line type operates on the given node.
    */
   virtual bool ShouldOperateOn(const dtDirector::Node* node) const;

   /**
    * Converts the operation node into the data associated with this node.
    *
    * @param[in]  line    The current line.
    * @param[in]  node    The node to operate on.
    * @param[in]  editor  The editor.
    */
   virtual void OperateOn(DialogLineItem* line, dtDirector::Node* node, DirectorDialogEditorPlugin* editor);

   /**
    * Retrieves the display name for this line.
    *
    * @param[in]  tree  The dialog tree.
    */
   virtual QString GetDisplayName(const DirectorDialogEditorPlugin* editor) const;

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

protected:

   float mDelayTime;
};

////////////////////////////////////////////////////////////////////////////////

#endif //DIRECTOR_DIALOG_DELAY_LINE_TYPE
