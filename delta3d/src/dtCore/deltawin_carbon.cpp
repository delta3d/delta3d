// window.cpp: implementation of the DeltaWin class.
//
//////////////////////////////////////////////////////////////////////

#ifdef __APPLE__

#include <cassert>

#include <Carbon/Carbon.h>
#include <dtCore/deltawin.h>
#include <dtUtil/log.h>


namespace dtCore
{
   void DeltaWin::KillGLWindow()
   {
   }
   
//
// Lion replacement for CGDisplayBitsPerPixel(CGDirectDisplayID displayId)
//
size_t displayBitsPerPixel( CGDirectDisplayID displayId )
{
#if MAC_OS_X_VERSION_MIN_REQUIRED < 1060
    return CGDisplayBitsPerPixel(displayId);
#else
    CGDisplayModeRef mode = CGDisplayCopyDisplayMode(displayId);
    if (!mode)
    {
        OSG_WARN << "CGDisplayCopyDisplayMode returned NULL" << std::endl;
        return 0;
    }
 
    CFStringRef pixEnc = CGDisplayModeCopyPixelEncoding(mode);
    if (!pixEnc)
    {
        OSG_WARN << "CGDisplayModeCopyPixelEncoding returned NULL" << std::endl;
        CGDisplayModeRelease(mode);
        return 0;
    }

    size_t depth = 0;
    if (CFStringCompare(pixEnc, CFSTR(IO32BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
    {
        depth = 32;
    }
    else if (CFStringCompare(pixEnc, CFSTR(IO16BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
    {
        depth = 16;
    }
    else if (CFStringCompare(pixEnc, CFSTR(IO8BitIndexedPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
    {
        depth = 8;
    }
    else
    {
        OSG_WARN << "Unable to match pixel encoding '" << CFStringGetCStringPtr(pixEnc, kCFStringEncodingUTF8) << "'" << std::endl;
    }

    CGDisplayModeRelease(mode);
    CFRelease(pixEnc);

    return depth;
#endif
}
   
   DeltaWin::ResolutionVec DeltaWin::GetResolutions()
   {
     DeltaWin::ResolutionVec rv;
     return rv;
   }
   
   // get a double value from a dictionary
   static double getDictDouble (CFDictionaryRef refDict, CFStringRef key)
   {
      double double_value;
      CFNumberRef number_value = (CFNumberRef) CFDictionaryGetValue(refDict, key);
      if (!number_value) // if can't get a number for the dictionary
         return -1;  // fail
      if (!CFNumberGetValue(number_value, kCFNumberDoubleType, &double_value)) // or if cant convert it
         return -1; // fail
      return double_value; // otherwise return the long value
   }
   
   bool DeltaWin::ChangeScreenResolution( int width, int height, int colorDepth, int refreshRate ) 
   {
      return false;
   }
   
   DeltaWin::Resolution DeltaWin::GetCurrentResolution()
   {
      int w,h;
   
      static CGDirectDisplayID *displays = NULL;
      static CGDisplayCount numDisplays = 0;
      
      DeltaWin::Resolution r = { 0,0,0,0 };
   
      if( CGGetActiveDisplayList( 0, NULL, &numDisplays ) != CGDisplayNoErr )
      {
         LOG_ERROR("CGGetActiveDisplayList() falied");
         return r;
      }
      
      if( numDisplays == 0 )
      {
         LOG_ERROR("openDisplay: No Displays!");
         return r;
      }
      
      displays = new CGDirectDisplayID[numDisplays];
      if( CGGetActiveDisplayList( numDisplays, displays, &numDisplays ) != CGDisplayNoErr )
      {
         LOG_ERROR("CGGetActiveDisplayList() falied\n");
         delete[] displays;
         return r;
      }
      
      CGDirectDisplayID* dpy = &displays[0];
   
      w = CGDisplayPixelsWide(*dpy);
      h = CGDisplayPixelsHigh(*dpy);
         
      
      // add next line and on following line replace hard coded depth and refresh rate
      int refresh =  (int)getDictDouble (CGDisplayCurrentMode (*dpy), kCGDisplayRefreshRate);
      int depth = (int)displayBitsPerPixel(*dpy);
   
      delete[] displays;
   
      DeltaWin::Resolution result = { w,h, depth, refresh };
      return result;
   }

   //////////////////////////////////////////////////////////////////////////
   void DeltaWin::HideWindowsConsole(bool hide)
   {
   }
}
#endif // __APPLE__

