#include "MyEventTypes.h"

// Event stuff
IMPLEMENT_ENUM(MyEventType);
MyEventType::MyEventType(const std::string& name) : dtABC::Event::Type(name) { AddInstance(this); }
MyEventType::~MyEventType() {}

const MyEventType MyEventType::ALT("ALT");
const MyEventType MyEventType::START("START");

Alt::Alt() : dtABC::Event(&MyEventType::ALT) {}
Start::Start() : dtABC::Event(&MyEventType::START) {}
