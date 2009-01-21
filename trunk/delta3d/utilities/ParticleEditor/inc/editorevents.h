#ifndef DELTA_EDITOR_EVENTS
#define DELTA_EDITOR_EVENTS

#include <dtCore/refptr.h>
#include <osg/Referenced>

/**
* @class EditorEvents
* @brief This class is a single repository for any events that are sent around the User
* Interface.  These are implemented with QT's signal/slot behavior.  Although this class
* is not technically required, it acts as a single point of implementation for signals.
* This class is a singleton
*/
class EditorEvents : public osg::Referenced
{
public:
   static EditorEvents &GetInstance();

private:
   EditorEvents();
   EditorEvents &operator=(const EditorEvents &rhs) { return *this; }
   EditorEvents(const EditorEvents &rhs) { }
   virtual ~EditorEvents();

   static dtCore::RefPtr<EditorEvents> instance;
};
#endif /*DELTA_EDITOR_EVENTS*/
