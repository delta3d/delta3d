#ifdef DELTA_PCH

#ifndef DTSTAGE_PREFIX
#define DTSTAGE_PREFIX

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QAction>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
/*#include <dtEditQt/dynamicabstractcontrol.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/viewport.h>
#include <dtEditQt/resourceabstractbrowser.h>
*/
#define NO_DT_WIN_PCH_HEADER
#include <prefix/dtdalprefix-src.h>

//remove useless macros in X11 headers
#ifdef None
#undef None
#endif

#endif
#endif
