#ifndef DTQTPREFIX_H__
#define DTQTPREFIX_H__

#ifdef DT_USE_PCH
   
   #include <QtCore/QObject>

   #ifdef _MSC_VER
      #include <QtGui/QAbstractItemDelegate>
   #else //_MSC_VER
     #include <QtCore/QObject>
     #include <QtGui/QPushButton>
   #endif //_MSC_VER

#endif//DT_USE_PCH

#endif // DTQTPREFIX_H__
