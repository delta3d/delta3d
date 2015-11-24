

set(SUB_PACKAGES Qt5Core Qt5Gui Qt5OpenGL Qt5WebKitWidgets Qt5Widgets)

foreach(package ${SUB_PACKAGES})
   if (FIND_Qt5_REQUIRED)
      find_package(${package} REQUIRED)
   elseif(FIND_Qt5_QUIETLY)
      find_package(${package} QUIETLY)
   else()
      find_package(${package})
   endif()
endforeach()

SET(QT_INCLUDE_DIRS ${Qt5Core_INCLUDE_DIRS};${Qt5Gui_INCLUDE_DIRS};${Qt5WebKitWidgets_INCLUDE_DIRS};${Qt5Widgets_INCLUDE_DIRS})
SET(QT_LIBRARIES ${Qt5Core_LIBRARIES};${Qt5Gui_LIBRARIES};${Qt5OpenGL_LIBRARIES};${Qt5WebKitWidgets_LIBRARIES};${Qt5Widgets_LIBRARIES})

SET(QT5_FOUND ${Qt5Core_FOUND})