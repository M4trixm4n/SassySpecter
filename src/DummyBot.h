#pragma once
#include "SassySpecterBot.h"

#ifdef SC2API
class DummyBot : public sc2::Agent 
#else
class DummyBot
#endif
{

public:
    DummyBot();

#ifdef SC2API
    void OnGameStart() override;
    void OnStep() override;
#else
    void OnGameStart();
    void OnStep();
#endif

};