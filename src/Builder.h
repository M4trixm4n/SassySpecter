#pragma once

#include <vector>
#include <memory>

#include "model_builder.hpp"
#include "UnitSelector.h"

using namespace std;
using namespace ghost;


class Builder: public ModelBuilder {

    SassySpecterBot &m_bot;
	std::vector<UnitSelector::Building_> *_allyBuildings;
	std::vector<UnitSelector::Building_> *_freeBuildings;
    Army *_ally;                             // ally army
    Army *_enemyArmy;                        // enemy units we've seen
    Army *_enemyAdditions;                   // what we think the enemy army contains, excluding what we've already seen
    int _minerals;                           // total actual minerals count
    int _vespene;                            // total actual gas count
    int _foodAvailable;                      // total actual food (=supply/...) count
    int _unitsInTraining;

public:

	Builder (SassySpecterBot &bot,
             std::vector<UnitSelector::Building_> *allyBuildings,
             std::vector<UnitSelector::Building_> *freeBuildings,
             Army *ally,
             Army *enemyArmy,
             Army *enemyAdditions,
             int minerals,
             int vespene,
             int foodAvailable,
             int unitsInTraining);

	void declare_variables() override;
	void declare_constraints() override;
	void declare_objective() override;
};