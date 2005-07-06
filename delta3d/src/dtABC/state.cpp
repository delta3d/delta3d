#include <dtABC/state.h>

using namespace dtABC;
using namespace dtCore;

State::State( const Type* type, std::string name ) : Base(name), mType(type)
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
