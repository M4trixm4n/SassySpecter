#include "Constraints.h"
#include "UnitTools.h"

using namespace ghost;
using namespace std;

double UnitLimit::required_error (const vector<Variable*> &variables) const {
    return max(0., ally->size() + unitsInTraining - 200.);
}

double RessourceCost::required_error (const vector<Variable*> &variables) const {
    int neededM{}, neededV{}, neededF{};
    for (auto &var : variables) {
        if (!var->get_value()) continue;
        const TypeData unitData = bot.Data(UnitType(stoi(var->get_name()), bot));
        neededM += unitData.mineralCost;
        neededV += unitData.gasCost;
        neededF += unitData.supplyCost; //might need to check if it gives supply aswell, and not just consuming it
    }
    int error{};
    if (minerals < neededM) error -= minerals - neededM;
    if (vespene < neededV) error -= vespene - neededV;
    if (foodAvailable < neededF) error -= ((foodAvailable - neededF) * 10);
    return error;

}

double FreeBuildings::required_error (const vector<Variable*> &variables) const {
    int toFind, error{};
    vector<UnitSelector::Building_> free = *freeBuildings;
    for (auto &var : variables) {
        toFind = var->get_value();
        if (!toFind) continue;
        for (int i = 0; i < free.size(); i++) {
            for (auto &ability : free[i].trainingAbilities) {
                if (ability.second == static_cast<sc2::UNIT_TYPEID>(stoi(var->get_name()))) {
                    toFind--;
                    free.erase(free.begin() + i); //if building will be used, can't be used for more than 1 unit
                    break;
                }
            }
        }
        if (toFind > 0) error += toFind;
    }
    return error;
}
