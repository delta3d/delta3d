#ifndef DELTA_EDTIOR_ACTIONS
#define DELTA_EDTIOR_ACTIONS

#include <dtCore/refptr.h>
#include <osg/Referenced>

/**
* This class holds all the UI QActions.  It has a list of the actions that are
* used for menus, dialogs, popups, and shortcuts.  Each action uses the signal/slot
* behavior from QT.
* @note The EditorActions class follows the Singleton pattern.
*/
class EditorActions : public osg::Referenced
{
public:
   /**
   * Gets the singleton instance of the EditorActions.
   * @return Returns the singleton instance.
   */
   static EditorActions &GetInstance();

protected:
   /**
   * Protected destructor, but does nothing.  Protected because it's a singleton
   */
   virtual ~EditorActions();

private:
   /**
   * Constructs the EditorActions class.   This is private because it's a singleton
   */
   EditorActions();
   EditorActions &operator=(const EditorActions &rhs);
   EditorActions(const EditorActions &rhs);

   ///Singleton instance of this class.
   static dtCore::RefPtr<EditorActions> instance;
};

#endif /*DELTA_EDTIOR_ACTIONS*/
