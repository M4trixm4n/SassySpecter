#pragma once

#include <vector>

#include "constraint.hpp"
#include "variable.hpp"
#include "UnitSelector.h"
#include "SassySpecterBot.h"

using namespace std;
using namespace ghost;

/*
* We cannot go over 200 total units
*/
class UnitLimit: public Constraint {

    Army *ally;
    int unitsInTraining{};

    double required_error (const vector<Variable*> &variables) const override;

public:
    UnitLimit (const std::vector<Variable> &variables, 
               Army *ally,
               int unitInTraining) : 
        Constraint(variables), 
        ally(ally),
        unitsInTraining(unitsInTraining) {}

};

/*
* We must have enough ressources (minerals, gas, and food)
*/
class RessourceCost: public Constraint {

    int minerals{}, vespene{}, foodAvailable{};
    SassySpecterBot &bot;

    double required_error(const vector<Variable*> &variables) const override;

public:
    RessourceCost (const std::vector<Variable> &variables,
                   int minerals,
                   int vespene,
                   int foodAvailable,
                   SassySpecterBot &bot) : 
        Constraint(variables),
        minerals(minerals),
        vespene(vespene),
        foodAvailable(foodAvailable),
        bot(bot) {}
};

/*
* According buildings must be free to train chosen units instantly
*/
class FreeBuildings: public Constraint {

    vector<UnitSelector::Building_> *freeBuildings;

    double required_error(const vector<Variable*> &variables) const override;

public:
    FreeBuildings (const std::vector<Variable> &variables, vector<UnitSelector::Building_> *freeBuildings)
        : Constraint(variables),
          freeBuildings(freeBuildings) {}
};

/*
* All chosen units must be from the same race
*/
class SameRace: public Constraint {

    double required_error(const vector<Variable*> &variables) const override;

public:
    SameRace (const std::vector<Variable> &variables) : Constraint(variables) {}
};