#include <dtABC/applicationconfigdata.h>

using namespace dtABC;

ApplicationConfigData::ApplicationConfigData() :
   WINDOW_X(0), WINDOW_Y(0),
   RESOLUTION(),
   SHOW_CURSOR(false), FULL_SCREEN(false), CHANGE_RESOLUTION(false),
   CAMERA_NAME(""), SCENE_NAME(""), WINDOW_NAME(""),
   SCENE_INSTANCE(""), WINDOW_INSTANCE("")
{
}

ApplicationConfigData::~ApplicationConfigData()
{
}
