#pragma once

#include <sc2api/sc2_unit.h>

class SassySpecterBot;
class Army;

class UnitSelector {

private:
    SassySpecterBot &m_bot;

    Army *ally;                             // ally army
    Army *allyAdditions;                    // what we wish to add to our army
    Army *enemyArmy;                        // enemy units we've seen
    std::vector<sc2::Tag> enemyPtrs{};      // every seen enemy unit's pointer
    Army *enemyAdditions;                   // what we think the enemy army contains, excluding what we've already seen
    int minerals;                           // total actual minerals count
    int vespene;                            // total actual gas count
    int foodAvailable;                      // total actual food (=supply/...) count
    int unitsInTraining;                     // units that are currently being trained

public:

    struct Building_ {
        sc2::UNIT_TYPEID type;
        sc2::Tag buildingPtr;
        std::map<sc2::AbilityID, sc2::UNIT_TYPEID> trainingAbilities;
    };

    // Constraints
    // true means we're ok, false means the constraint isn't fulfilled
    bool C_armyLimit ();
    bool C_objectiveUsefulness (Army *enemyCopy);
    bool C_affordable ();


    // Others
    void onStart ();
    void onFrame ();            // legacy code, call onStep instead
    void onStep ();             // Do not call this every frame
    void onUnitDestroyed (const sc2::Unit *unit);
    void onUnitCreated (const sc2::Unit *unit);
    void onUnitEnterVision (const sc2::Unit *unit);
    
    void addBuilding (const sc2::Unit *unit);
    void removeBuilding (const sc2::Unit *unit);
    void updateRessources ();
    std::vector<Building_> * freeUnitBuildings ();

    UnitSelector (SassySpecterBot &bot);

private:
    std::vector<Building_> allyBuildings{};

};

static bool isTrainingAbility(sc2::AbilityID ability);