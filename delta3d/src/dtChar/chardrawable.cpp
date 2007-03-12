

#include "dtChar/chardrawable.h"
#include <osg/MatrixTransform>

using namespace dtChar;


CharDrawable::CharDrawable():
mModel(new dtChar::Model() )
{
   GetMatrixNode()->addChild(mModel.get());
}

CharDrawable::~CharDrawable()
{
}
