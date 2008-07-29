/*
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2008 MOVES Institute 
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free 
* Software Foundation; either version 2.1 of the License, or (at your option) 
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
* details.
*
* You should have received a copy of the GNU Lesser General Public License 
* along with this library; if not, write to the Free Software Foundation, Inc., 
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
*
*/

#ifndef DELTA3DTHREAD_H__
#define DELTA3DTHREAD_H__

#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <dtCore/refptr.h>

///////////////////////////////////////////////////////////////////////////////

class ObjectWorkspace;
class ObjectViewer;
class QApplication;

///////////////////////////////////////////////////////////////////////////////

class Delta3DThread : public QThread
{
   Q_OBJECT
public:
	Delta3DThread(QApplication* parent=0);
	~Delta3DThread();

   void SetObjectWorkspace(ObjectWorkspace* win) { mWin = win;}
   virtual void run();
	
   dtCore::RefPtr<ObjectViewer> mViewer;

private:

   ObjectWorkspace* mWin;
   std::string mStartupFile;
};

////////////////////////////////////////////////////////////////////////////////

#endif // DELTA3DTHREAD_H__
