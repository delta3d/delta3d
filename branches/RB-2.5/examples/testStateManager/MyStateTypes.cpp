#include "MyStateTypes.h"

// State stuff
IMPLEMENT_ENUM(MyStateType);
MyStateType::MyStateType(const std::string& name) : dtABC::State::Type(name) { AddInstance(this); }
MyStateType::~MyStateType() {}

const MyStateType MyStateType::SHELL("SHELL");
const MyStateType MyStateType::OPTIONS("OPTIONS");
const MyStateType MyStateType::GAME("GAME");

Options::Options(const std::string& name) : dtABC::State(&MyStateType::OPTIONS,name) {}
Shell::Shell(const std::string& name) : dtABC::State(&MyStateType::SHELL,name) {}
Game::Game(const std::string& name) : dtABC::State(&MyStateType::GAME,name) {}
