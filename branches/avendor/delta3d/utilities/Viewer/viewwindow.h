#pragma once
// header files
#include <vector>

#include <Fl_DT_Window.h>

#include "Viewer.h"



// forward references
class ViewState;
class UserInterface;



// FLTK window to be shown
class ViewWindow  :  public   Fl_DT_Window<Viewer>
{
      typedef  Fl_DT_Window<Viewer>   MyParent;

      DECLARE_MANAGEMENT_LAYER(ViewWindow)

   private:
      typedef  std::vector<ViewState*> FILE_LIST;

      static   const char*             DEF_PATH;

   public:
                                       ViewWindow();
                                       ViewWindow( int w, int h, const char* l = 0L );
                                       ViewWindow( int x, int y, int w, int h, const char* l = 0L );
      virtual                          ~ViewWindow();

      virtual  void                    show( void );
      virtual  void                    OnMessage( dtCore::Base::MessageData* data );

               const char*             GetPath( void );
               void                    SetPath( const char* path );
               void                    Container( UserInterface* container );
               bool                    LoadFile( const char* file );
               bool                    SaveFileAs( const char* filename );
               void                    SelectFile( unsigned int indx );
               void                    SetDisplay( unsigned int state, bool value = true );
               void                    SetMotion( unsigned int state );
               void                    SetJoystick( unsigned int state, bool value = true );
               void                    CommandLine( int argc, char** argv );

   private:
      inline   void                    ctor( void );
      inline   void                    KeyboardEventHandler( const dtABC::KeyboardEvent& ev );

   private:
               std::string             mPath;
               FILE_LIST               mFileList;
               ViewState*              mCurFile;
               UserInterface*          mContainer;
               bool                    mFileLoaded; ///<magic
};
