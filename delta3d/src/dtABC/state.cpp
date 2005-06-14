#include <dtABC/state.h>

using namespace dtABC;
using namespace dtCore;

//IMPLEMENT_MANAGEMENT_LAYER(State)

State::State( std::string name, const Type* type ) : Base(name), mType(type)
{
}

State::~State()
{
}

void State::Shutdown()
{
}

void State::PreFrame(const double deltaFrameTime)
{
}

void State::Frame(const double deltaFrameTime)
{
}

void State::PostFrame(const double deltaFrameTime)
{
}
