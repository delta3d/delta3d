#include <dtABC/state.h>

using namespace dtABC;
using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(State)

State::State( std::string type ) : Base(type)
{
}

State::~State()
{
}
