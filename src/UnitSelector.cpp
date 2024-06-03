#include <solver.hpp>
#include "UnitSelector.h"
#include "SassySpecterBot.h"
#include "UnitTools.h"
#include "Builder.h"

UnitSelector::UnitSelector (SassySpecterBot &bot): m_bot(bot) {
    
}

void UnitSelector::onStart () {
    ally = new Army;
    allyAdditions = new Army;
    enemyArmy = new Army;
    enemyAdditions = new Army;
    updateRessources();
}

// later : generate a different random arrangement of units based on what can be created
void UnitSelector::onFrame () {
    updateRessources();
    std::vector<Building_> *free = freeUnitBuildings();
    Army *enemyCopy = enemyArmy->copyWithAdditions(enemyAdditions);
    double previousAdditionScore = 0;
    std::pair<sc2::ABILITY_ID, sc2::UNIT_TYPEID> previousAddition;
    Building_ unitThatMakesAddition;

    for (auto &build : *free) {
        for (const auto &unit : build.trainingAbilities) {
            allyAdditions->addUnit(unit.second, m_bot);
            double score = allyAdditions->armyScore(enemyCopy);
            if (score > previousAdditionScore && (!C_affordable() || !C_armyLimit() || !C_objectiveUsefulness(enemyCopy))) {
                previousAdditionScore = score;
                previousAddition = unit;
                unitThatMakesAddition = build;
            }
            allyAdditions->deadUnit(unit.second);
        }
    }
    if (previousAdditionScore != 0) {
        const sc2::Unit *u = m_bot.Observation()->GetUnit(unitThatMakesAddition.buildingPtr);
        m_bot.Actions()->UnitCommand(u, previousAddition.first);
    }
    delete free;
}

void UnitSelector::onStep () {
    updateRessources();
    std::map<sc2::UNIT_TYPEID, int> *unitsLimit = trainableUnitsLimit();
    //Builder builder (   m_bot,
    //                    freeUnitBuildings(),
    //                    unitsLimit,
    //                    ally,
    //                    enemyArmy,
    //                    enemyAdditions,     //need to predict this
    //                    minerals,
    //                    vespene,
    //                    foodAvailable,
    //                    unitsInTraining);
    Army *a1 = new Army({
        Battalion(sc2::UNIT_TYPEID::TERRAN_BANSHEE, 4, &m_bot),
        Battalion(sc2::UNIT_TYPEID::TERRAN_GHOST, 6, &m_bot),
        Battalion(sc2::UNIT_TYPEID::TERRAN_MARINE, 10, &m_bot),
        Battalion(sc2::UNIT_TYPEID::TERRAN_REAPER, 1, &m_bot)
                        });
    Army *a2 = new Army({
        Battalion(sc2::UNIT_TYPEID::PROTOSS_ADEPT, 4, &m_bot),
        Battalion(sc2::UNIT_TYPEID::PROTOSS_IMMORTAL, 8, &m_bot),
        Battalion(sc2::UNIT_TYPEID::PROTOSS_HIGHTEMPLAR, 7, &m_bot),
        Battalion(sc2::UNIT_TYPEID::PROTOSS_MOTHERSHIP, 1, &m_bot),
        Battalion(sc2::UNIT_TYPEID::PROTOSS_TEMPEST, 3, &m_bot)
                        });
    std::vector<Building_> *b = new std::vector<Building_>();
    b->push_back({sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, 0, {{sc2::ABILITY_ID::TRAIN_SCV, sc2::UNIT_TYPEID::TERRAN_SCV}}});
    b->push_back({sc2::UNIT_TYPEID::TERRAN_BARRACKS, 0, {{sc2::ABILITY_ID::TRAIN_GHOST, sc2::UNIT_TYPEID::TERRAN_GHOST}, {sc2::ABILITY_ID::TRAIN_MARINE, sc2::UNIT_TYPEID::TERRAN_MARINE}, {sc2::ABILITY_ID::TRAIN_REAPER, sc2::UNIT_TYPEID::TERRAN_REAPER}, {sc2::ABILITY_ID::TRAIN_MARAUDER, sc2::UNIT_TYPEID::TERRAN_MARAUDER}}});
    b->push_back({sc2::UNIT_TYPEID::TERRAN_BARRACKS, 0, {{sc2::ABILITY_ID::TRAIN_GHOST, sc2::UNIT_TYPEID::TERRAN_GHOST}, {sc2::ABILITY_ID::TRAIN_MARINE, sc2::UNIT_TYPEID::TERRAN_MARINE}, {sc2::ABILITY_ID::TRAIN_REAPER, sc2::UNIT_TYPEID::TERRAN_REAPER}, {sc2::ABILITY_ID::TRAIN_MARAUDER, sc2::UNIT_TYPEID::TERRAN_MARAUDER}}});
    b->push_back({sc2::UNIT_TYPEID::TERRAN_BARRACKS, 0, {{sc2::ABILITY_ID::TRAIN_GHOST, sc2::UNIT_TYPEID::TERRAN_GHOST}, {sc2::ABILITY_ID::TRAIN_MARINE, sc2::UNIT_TYPEID::TERRAN_MARINE}, {sc2::ABILITY_ID::TRAIN_REAPER, sc2::UNIT_TYPEID::TERRAN_REAPER}, {sc2::ABILITY_ID::TRAIN_MARAUDER, sc2::UNIT_TYPEID::TERRAN_MARAUDER}}});
    b->push_back({sc2::UNIT_TYPEID::TERRAN_BARRACKS, 0, {{sc2::ABILITY_ID::TRAIN_GHOST, sc2::UNIT_TYPEID::TERRAN_GHOST}, {sc2::ABILITY_ID::TRAIN_MARINE, sc2::UNIT_TYPEID::TERRAN_MARINE}, {sc2::ABILITY_ID::TRAIN_REAPER, sc2::UNIT_TYPEID::TERRAN_REAPER}, {sc2::ABILITY_ID::TRAIN_MARAUDER, sc2::UNIT_TYPEID::TERRAN_MARAUDER}}});
    b->push_back({sc2::UNIT_TYPEID::TERRAN_BARRACKS, 0, {{sc2::ABILITY_ID::TRAIN_GHOST, sc2::UNIT_TYPEID::TERRAN_GHOST}, {sc2::ABILITY_ID::TRAIN_MARINE, sc2::UNIT_TYPEID::TERRAN_MARINE}, {sc2::ABILITY_ID::TRAIN_REAPER, sc2::UNIT_TYPEID::TERRAN_REAPER}, {sc2::ABILITY_ID::TRAIN_MARAUDER, sc2::UNIT_TYPEID::TERRAN_MARAUDER}}});
    b->push_back({sc2::UNIT_TYPEID::TERRAN_FACTORY, 0, {{sc2::ABILITY_ID::TRAIN_CYCLONE, sc2::UNIT_TYPEID::TERRAN_CYCLONE}, {sc2::ABILITY_ID::TRAIN_HELLION, sc2::UNIT_TYPEID::TERRAN_HELLION}, {sc2::ABILITY_ID::TRAIN_SIEGETANK, sc2::UNIT_TYPEID::TERRAN_SIEGETANK}, {sc2::ABILITY_ID::TRAIN_THOR, sc2::UNIT_TYPEID::TERRAN_THOR}}});
    b->push_back({sc2::UNIT_TYPEID::TERRAN_FACTORY, 0, {{sc2::ABILITY_ID::TRAIN_CYCLONE, sc2::UNIT_TYPEID::TERRAN_CYCLONE}, {sc2::ABILITY_ID::TRAIN_HELLION, sc2::UNIT_TYPEID::TERRAN_HELLION}, {sc2::ABILITY_ID::TRAIN_SIEGETANK, sc2::UNIT_TYPEID::TERRAN_SIEGETANK}, {sc2::ABILITY_ID::TRAIN_THOR, sc2::UNIT_TYPEID::TERRAN_THOR}}});
    b->push_back({sc2::UNIT_TYPEID::TERRAN_STARPORT, 0, {{sc2::ABILITY_ID::TRAIN_BANSHEE, sc2::UNIT_TYPEID::TERRAN_BANSHEE}, {sc2::ABILITY_ID::TRAIN_BATTLECRUISER, sc2::UNIT_TYPEID::TERRAN_BATTLECRUISER}, {sc2::ABILITY_ID::TRAIN_LIBERATOR, sc2::UNIT_TYPEID::TERRAN_LIBERATOR}, {sc2::ABILITY_ID::TRAIN_VIKINGFIGHTER, sc2::UNIT_TYPEID::TERRAN_VIKINGFIGHTER}}});
    b->push_back({sc2::UNIT_TYPEID::TERRAN_STARPORT, 0, {{sc2::ABILITY_ID::TRAIN_BANSHEE, sc2::UNIT_TYPEID::TERRAN_BANSHEE}, {sc2::ABILITY_ID::TRAIN_BATTLECRUISER, sc2::UNIT_TYPEID::TERRAN_BATTLECRUISER}, {sc2::ABILITY_ID::TRAIN_LIBERATOR, sc2::UNIT_TYPEID::TERRAN_LIBERATOR}, {sc2::ABILITY_ID::TRAIN_VIKINGFIGHTER, sc2::UNIT_TYPEID::TERRAN_VIKINGFIGHTER}}});
    unitsLimit = new std::map<sc2::UNIT_TYPEID, int>();
    unitsLimit->insert({sc2::UNIT_TYPEID::TERRAN_SCV, 1});
    unitsLimit->insert({sc2::UNIT_TYPEID::TERRAN_GHOST, 5});
    unitsLimit->insert({sc2::UNIT_TYPEID::TERRAN_MARINE, 5});
    unitsLimit->insert({sc2::UNIT_TYPEID::TERRAN_REAPER, 5});
    unitsLimit->insert({sc2::UNIT_TYPEID::TERRAN_MARAUDER, 5});
    unitsLimit->insert({sc2::UNIT_TYPEID::TERRAN_CYCLONE, 2});
    unitsLimit->insert({sc2::UNIT_TYPEID::TERRAN_HELLION, 2});
    unitsLimit->insert({sc2::UNIT_TYPEID::TERRAN_SIEGETANK, 2});
    unitsLimit->insert({sc2::UNIT_TYPEID::TERRAN_THOR, 2});
    unitsLimit->insert({sc2::UNIT_TYPEID::TERRAN_BANSHEE, 2});
    unitsLimit->insert({sc2::UNIT_TYPEID::TERRAN_BATTLECRUISER, 2});
    unitsLimit->insert({sc2::UNIT_TYPEID::TERRAN_LIBERATOR, 2});
    unitsLimit->insert({sc2::UNIT_TYPEID::TERRAN_VIKINGFIGHTER, 2});
    Builder builder (   m_bot,
                        freeUnitBuildings(),
                        unitsLimit,
                        a1,
                        a2,
                        enemyAdditions,     //need to predict this
                        1000,
                        150,
                        40,
                        unitsInTraining);

    Options options;
    options.parallel_runs = true;
    options.number_threads = std::max(2, options.number_threads / 2);

    ghost::Solver<Builder> solver(builder);

    vector<int> vec_solution;
    double cost;

    if (solver.fast_search(cost, vec_solution, 100ms, options)) std::cout << "Solution found\n";
    else {
        std::cout << "Error in the solver\n";
        return;
    }
    int i = 0;
    for (std::map<sc2::UNIT_TYPEID, int>::iterator it = unitsLimit->begin(); it != unitsLimit->end(); it++) {
        allyAdditions->addBattalion(Battalion(it->first, vec_solution[i++], &m_bot));
    }
    std::cout << "Additional units : \n";
    allyAdditions->Log();

}

void UnitSelector::onUnitDestroyed (const sc2::Unit *unit) {
    bool isBuilding = m_bot.Data(UnitType(unit->unit_type, m_bot)).isBuilding;
    if (unit->alliance == 1 || unit->alliance == 2) {
        if (isBuilding) removeBuilding(unit);
        else ally->deadUnit(unit);
    } else {
        if (isBuilding) return;
        for (int i = 0; i < enemyPtrs.size(); i++) {
            if (enemyPtrs[i] == unit->tag) {
                enemyPtrs.erase(enemyPtrs.begin() + i);
                enemyArmy->deadUnit(unit);
                break;
            }
        }
    }
}

void UnitSelector::onUnitCreated (const sc2::Unit *unit) {
    bool isBuilding = m_bot.Data(UnitType(unit->unit_type, m_bot)).isBuilding;
    if (unit->alliance == 1 || unit->alliance == 2) {
        if (isBuilding) addBuilding(unit);
        else ally->addUnit(unit, m_bot);
    } else {
        if (isBuilding) return;
        enemyArmy->addUnit(unit, m_bot);
        enemyPtrs.push_back(unit->tag);
    }
}

void UnitSelector::onUnitEnterVision (const sc2::Unit *unit) {
    for (auto &ptr : enemyPtrs) {
        if (ptr == unit->tag) return;
    }
    enemyArmy->addUnit(unit, m_bot);
    enemyPtrs.push_back(unit->tag);
}

void UnitSelector::addBuilding (const sc2::Unit *unit) {
    allyBuildings.push_back({unit->unit_type, unit->tag});
}

void UnitSelector::removeBuilding (const sc2::Unit *unit) {
    for (int i = 0; i < allyBuildings.size(); i++) {
        if (unit->tag == allyBuildings[i].buildingPtr) {
            allyBuildings.erase(allyBuildings.begin() + i);
            break;
        }
    }
}

void UnitSelector::updateRessources () {
    minerals = m_bot.Observation()->GetMinerals();
    vespene = m_bot.Observation()->GetVespene();
    foodAvailable = m_bot.Observation()->GetFoodCap() - m_bot.Observation()->GetFoodUsed();
}

std::vector<UnitSelector::Building_> * UnitSelector::freeUnitBuildings () {
    unitsInTraining = 0;
    std::vector<Building_> *free = new std::vector<Building_>;
    for (auto &build : allyBuildings) {
        if (m_bot.GetUnit(build.buildingPtr).isTraining()) {
            unitsInTraining += 1;
            continue;
        }
        if (m_bot.GetUnit(build.buildingPtr).isBeingConstructed()) continue;
        sc2::AvailableAbilities abilities = m_bot.Query()->GetAbilitiesForUnit(m_bot.Observation()->GetUnit(build.buildingPtr));
        for (auto &ability : abilities.abilities) {
            if (isTrainingAbility(ability.ability_id)) {
                build.trainingAbilities[ability.ability_id] = m_bot.getUnitFromAbility(ability.ability_id.ToType());
            }
        }
        if (!build.trainingAbilities.empty()) free->push_back(build);
    }
    return free;
}

bool UnitSelector::C_armyLimit () {
    return ally->size() <= 200;
}

bool UnitSelector::C_objectiveUsefulness (Army *enemyCopy) {
    //compare current planned additions with previously studied additions aswell as the enemy army
    Army *allyCopy = ally->copyWithAdditions(allyAdditions);
    double allyScore = allyCopy->armyScore(enemyCopy);
    return allyScore > enemyCopy->armyScore(allyCopy) && allyScore > ally->armyScore(enemyCopy);
}

bool UnitSelector::C_affordable () {
    return ally->mineralCost() <= minerals && ally->vespeneCost() <= vespene && ally->foodCost() <= foodAvailable;
}

std::map<sc2::UNIT_TYPEID, int> *UnitSelector::trainableUnitsLimit () {
    std::map<sc2::UNIT_TYPEID, int> *list = new std::map<sc2::UNIT_TYPEID, int>();
    for (auto &build : *freeUnitBuildings()) {
        for (auto &ability : build.trainingAbilities) {
            if (list->find(ability.second) == list->end()) (*list)[ability.second] = 1;
            else (*list)[ability.second]++;
        }
    }
    return list;
}


static bool isTrainingAbility(sc2::AbilityID ability) {
    switch (ability.ToType()) {
        case sc2::ABILITY_ID::TRAINWARP_ADEPT:
        case sc2::ABILITY_ID::TRAINWARP_DARKTEMPLAR:
        case sc2::ABILITY_ID::TRAINWARP_HIGHTEMPLAR:
        case sc2::ABILITY_ID::TRAINWARP_SENTRY:
        case sc2::ABILITY_ID::TRAINWARP_STALKER:
        case sc2::ABILITY_ID::TRAINWARP_ZEALOT:
        case sc2::ABILITY_ID::TRAIN_ADEPT:
        case sc2::ABILITY_ID::TRAIN_ARCHON:
        case sc2::ABILITY_ID::TRAIN_BANSHEE:
        case sc2::ABILITY_ID::TRAIN_BATTLECRUISER:
        case sc2::ABILITY_ID::TRAIN_CARRIER:
        case sc2::ABILITY_ID::TRAIN_COLOSSUS:
        case sc2::ABILITY_ID::TRAIN_CORRUPTOR:
        case sc2::ABILITY_ID::TRAIN_CYCLONE:
        case sc2::ABILITY_ID::TRAIN_DARKTEMPLAR:
        case sc2::ABILITY_ID::TRAIN_DISRUPTOR:
        case sc2::ABILITY_ID::TRAIN_DRONE:
        case sc2::ABILITY_ID::TRAIN_GHOST:
        case sc2::ABILITY_ID::TRAIN_HELLBAT:
        case sc2::ABILITY_ID::TRAIN_HELLION:
        case sc2::ABILITY_ID::TRAIN_HIGHTEMPLAR:
        case sc2::ABILITY_ID::TRAIN_HYDRALISK:
        case sc2::ABILITY_ID::TRAIN_IMMORTAL:
        case sc2::ABILITY_ID::TRAIN_INFESTEDABOMINATION:
        case sc2::ABILITY_ID::TRAIN_INFESTEDCIVILIAN:
        case sc2::ABILITY_ID::TRAIN_INFESTEDTERRANCAMPAIGN:
        case sc2::ABILITY_ID::TRAIN_INFESTOR:
        case sc2::ABILITY_ID::TRAIN_LIBERATOR:
        case sc2::ABILITY_ID::TRAIN_MARAUDER:
        case sc2::ABILITY_ID::TRAIN_MARINE:
        case sc2::ABILITY_ID::TRAIN_MEDIVAC:
        case sc2::ABILITY_ID::TRAIN_MOTHERSHIP:
        case sc2::ABILITY_ID::TRAIN_MOTHERSHIPCORE:
        case sc2::ABILITY_ID::TRAIN_MUTALISK:
        case sc2::ABILITY_ID::TRAIN_OBSERVER:
        case sc2::ABILITY_ID::TRAIN_ORACLE:
        case sc2::ABILITY_ID::TRAIN_OVERLORD:
        case sc2::ABILITY_ID::TRAIN_PHOENIX:
        case sc2::ABILITY_ID::TRAIN_PROBE:
        case sc2::ABILITY_ID::TRAIN_QUEEN:
        case sc2::ABILITY_ID::TRAIN_RAVEN:
        case sc2::ABILITY_ID::TRAIN_REAPER:
        case sc2::ABILITY_ID::TRAIN_ROACH:
        case sc2::ABILITY_ID::TRAIN_SCV:
        case sc2::ABILITY_ID::TRAIN_SENTRY:
        case sc2::ABILITY_ID::TRAIN_SIEGETANK:
        case sc2::ABILITY_ID::TRAIN_STALKER:
        case sc2::ABILITY_ID::TRAIN_SWARMHOST:
        case sc2::ABILITY_ID::TRAIN_TEMPEST:
        case sc2::ABILITY_ID::TRAIN_THOR:
        case sc2::ABILITY_ID::TRAIN_ULTRALISK:
        case sc2::ABILITY_ID::TRAIN_VIKINGFIGHTER:
        case sc2::ABILITY_ID::TRAIN_VIPER:
        case sc2::ABILITY_ID::TRAIN_VOIDRAY:
        case sc2::ABILITY_ID::TRAIN_WARPINMOHANDAR:
        case sc2::ABILITY_ID::TRAIN_WARPINSCOUT:
        case sc2::ABILITY_ID::TRAIN_WARPINSELENDIS:
        case sc2::ABILITY_ID::TRAIN_WARPINURUN:
        case sc2::ABILITY_ID::TRAIN_WARPINZERATUL:
        case sc2::ABILITY_ID::TRAIN_WARPPRISM:
        case sc2::ABILITY_ID::TRAIN_WIDOWMINE:
        case sc2::ABILITY_ID::TRAIN_ZEALOT:
        case sc2::ABILITY_ID::TRAIN_ZERGLING:
            return true;
        default:
            return false;
    }
}