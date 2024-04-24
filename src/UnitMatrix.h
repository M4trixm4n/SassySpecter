#pragma once

#include "UnitType.h"
#include "Timer.hpp"

struct SkirmishResult {
public:

    sc2::UnitTypeID unitA{0};
    sc2::UnitTypeID unitB{0};
    int unitAStartCount{0};
    int unitBStartCount{0};
    int unitAEndCount{0};
    int unitBEndCount{0};
    double totalBattleTime{0};
    double unitARatio{0};
    double unitBRatio{0};

    void Log () {
        using namespace std;
        cout << "========== Skirmish Result Log ==========\n";
        cout << "\t\tUnit A = " << sc2::UnitTypeToName(unitA) << endl;
        cout << "\t\tUnit B = " << sc2::UnitTypeToName(unitB) << endl;
        cout << "\t\tUnit A start count = " << unitAStartCount << endl;
        cout << "\t\tUnit B start count = " << unitBStartCount << endl;
        cout << "\t\tUnit A end count = " << unitAEndCount << endl;
        cout << "\t\tUnit B end count = " << unitBEndCount << endl;
        cout << "\t\tTotal battle time = " << totalBattleTime << endl;
        cout << "\t\tUnit A Ratio = " << unitARatio << endl;
        cout << "\t\tUnit B Ratio = " << unitBRatio << endl;
        cout << "=========================================\n\n";
    }

};

class UnitMatrix {
public:

    mutable SassySpecterBot *m_bot;                 // bot for units

    UnitType unitA{0, *m_bot};                      // first unit type
    UnitType unitB{0, *m_bot};                      // second unit type

    int currentUnitACounter{0};                     // Number of Unit As for current test
    int currentUnitBCounter{0};                     // Number of Unit Bs for current test

    Timer timer;                                    // Timer started at first attack, stopped after a unit type was eradicated

    std::vector<SkirmishResult> results;            // Contains the result for every test for current units
    std::vector<std::vector<double>> finalMatrix;   // Contains every final ratio between units, calculated with the SkiimishResults
    std::vector<sc2::UNIT_TYPEID> unitList;

    bool cycleUnits ();                             // Switches Units to a new combination (not done before)
    bool cycleCounters ();                          // Switches Counters to a new combination
    void spawnNecessaryUnits () const;              // Spawn the correct number of the correct units on each side
    void moveUnits () const;                        // Makes all spawned units gather at a point on the map to fight
    bool skirmishEnded () const;                    // Detects when a fight has ended
    void addSkirmishResults ();                     // Add a skirmish result to results
    void resetGameState ();                         // Restarts the game if needed (i.e. race change)
    bool skipUselessMatchup () const;               // Skips useless matchups (i.e. 2 passive units, 2 incompatible units, etc.)
    void addFinalRatios ();                         // Calculate final ratios between 2 units with results and adds them to finalMatrix

};