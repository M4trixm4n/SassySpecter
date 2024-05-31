#pragma once

#include "objective.hpp"
#include "variable.hpp"
#include "UnitTools.h"

using namespace std;
using namespace ghost;

class BestAdditions: public Maximize {
    Army *_ally;
    Army *_enemy;
    SassySpecterBot &m_bot;

    double required_cost (const vector<Variable*> &variables) const override;

public:
    BestAdditions (const vector<Variable> &variables,
                   Army *ally,
                   Army *enemy,
                   SassySpecterBot &bot);
};