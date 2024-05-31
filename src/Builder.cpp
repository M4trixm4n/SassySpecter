#include "Builder.h"
#include "Constraints.h"
#include "UnitType.h"
#include "ObjectiveFunction.h"

Builder::Builder (SassySpecterBot &bot, 
                  std::vector<UnitSelector::Building_> *allyBuildings,
                  std::vector<UnitSelector::Building_> *freeBuildings,
                  Army *ally,
                  Army *enemyArmy,
                  Army *enemyAdditions,
                  int minerals,
                  int vespene,
                  int foodAvailable,
                  int unitsInTraining):
    m_bot(bot),
    _allyBuildings(allyBuildings),
    _freeBuildings(freeBuildings),
    _ally(ally),
    _enemyArmy(enemyArmy),
    _enemyAdditions(enemyAdditions),
    _minerals(minerals),
    _vespene(vespene),
    _foodAvailable(foodAvailable),
    _unitsInTraining(unitsInTraining)
{}

void Builder::declare_variables() {
    for (auto &unit : UnitType::trainableUnits()) {
        variables.emplace_back(0, 5, std::to_string(static_cast<int>(unit)));
    }
}

void Builder::declare_constraints() {
    constraints.push_back(make_shared<UnitLimit>(variables, _ally, _unitsInTraining));
    constraints.push_back(make_shared<SameRace>(variables));
    constraints.push_back(make_shared<FreeBuildings>(variables, _freeBuildings));
    constraints.push_back(make_shared<RessourceCost>(variables, _minerals, _vespene, _foodAvailable, m_bot));
}

void Builder::declare_objective() {
    objective = make_shared<BestAdditions>(variables, _ally, _enemyArmy, m_bot);
}
