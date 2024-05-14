#pragma once

#include "TechTree.h"
#include <random>
#include <sc2api/sc2_typeenums.h>
#include <sc2api/sc2_common.h>
#include <chrono>
#include "Timer.hpp"

struct Army;
struct Battalion;

class SassySpecterBot;

class BattleSimulation {
private:
    SassySpecterBot &m_bot;
    Army *a1;
    Army *a2;
    TechTree &m_techTree;

    Timer timer;

    std::vector<sc2::UNIT_TYPEID> units{};
    std::vector<sc2::Point2D> validPositions{
        {123, 58}, {108, 58}, {89, 43}, {61, 32}, {29, 34}, {22, 54}, {40, 55}, {29, 78}, {42, 90}, {27, 110}, {44, 110}, {61, 124}, {90, 135}, {122, 135}, {128, 112}, {112, 112}, {122, 89}, {108, 77}, {95, 83}, {93, 103}, {82, 112}, {65, 93}, {86, 76}, {56, 86}, {58, 65}, {70, 55}
    };

    int battalionNumber{1};
    int battalionMaximumSize{10};
    int numberOfBattles{1};

    int battlesDone{0};
    int accuratePredictions{0};
    int expectedWinner{0};

    std::mt19937 mt{static_cast<std::mt19937::result_type>(std::chrono::steady_clock::now().time_since_epoch().count())};

    void revealMap ();
    void clean (bool hardClean);
    void setBattalionNumber (int nb);
    void setBattalionMaximumSize (int nb);
    void setNumberOfBattles (int nb);
    void singleBattle ();
    void startTesting ();
    sc2::UNIT_TYPEID chooseUnit (Army done);
    Army* generateArmy (int index);
    bool checkBattleEnded ();

public:


    void onStart ();
    void onFrame ();
    void onUnitDestroyed(const sc2::Unit *unit);

    BattleSimulation (SassySpecterBot &bot, TechTree &tree);
};