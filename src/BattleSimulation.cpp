#include "BattleSimulation.h"
#include "SassySpecterBot.h"
#include "UnitTools.h"
#include <iostream>

BattleSimulation::BattleSimulation (SassySpecterBot &bot, TechTree &tree)
    : m_bot(bot), m_techTree(tree), a1(nullptr), a2(nullptr) {

}

void BattleSimulation::onStart () {
    auto unitsMap = m_techTree.getAllUnits();
    if (!unitsMap.empty()) {
        for (std::map<UnitType, TypeData>::iterator it = unitsMap.begin(); it != unitsMap.end(); it++) {
            if (it->second.isUnit && it->first.canAttack() && it->first.canMove()) units.push_back(it->first.getAPIUnitType());
        }
    }
}

bool battleReady = false;
sc2::Point2D battleLocation;
bool battleOngoing = false;

void BattleSimulation::onFrame () {
    if (battleReady && timer.getElapsedTimeInSec() > 2) {
        timer.stop();
        timer.~Timer();
        battleReady = false;
        for (auto &unit : m_bot.Observation()->GetUnits()) {
            a1->fillBattalionStats(unit->owner, unit->health_max, unit->shield_max, unit->unit_type);
            a2->fillBattalionStats(unit->owner, unit->health_max, unit->shield_max, unit->unit_type);
        }
        if (a1->winsAgainst(*a2)) {
            std::cout << " * Victory of Army #1\n";
            expectedWinner = 1;
        } else if (a2->winsAgainst(*a1)) {
            std::cout << " * Victory of Army #2\n";
            expectedWinner = 2;
        } else {
            std::cout << " * Draw\n";
            expectedWinner = 0;
        }
        if (checkBattleEnded()) return;
        for (auto &unit : m_bot.GetUnits()) {
            unit.attackMove(battleLocation);
        }
        battleOngoing = true;
    }
    for (auto &mess : m_bot.Observation()->GetChatMessages()) {
        std::string command = mess.message.substr(0, mess.message.find(" "));
        if (command == ":revealmap") revealMap();
        else if (command == ":clean") clean(true);
        else if (command == ":setbatnum") setBattalionNumber(std::stoi(mess.message.substr(mess.message.find(" "))));
        else if (command == ":setbatmaxsize") setBattalionMaximumSize(std::stoi(mess.message.substr(mess.message.find(" "))));
        else if (command == ":setnumbattle") setNumberOfBattles(std::stoi(mess.message.substr(mess.message.find(" "))));
        else if (command == ":starttest") startTesting();
        else if (command == ":ghost") startGhost();
    }
}

void BattleSimulation::onUnitDestroyed(const sc2::Unit *unit) {
    if (!battleOngoing) return;
    if (unit->owner == a1->playerID) {
        a1->deadUnit(unit);
    } else {
        a2->deadUnit(unit);
    }
    checkBattleEnded();
}

void BattleSimulation::revealMap () {
    std::cout << "Reveal the map\n";
    m_bot.Debug()->DebugShowMap();
}

void BattleSimulation::clean (bool hardClean) {
    if (a1) delete a1;
    if (a2) delete a2;
    if (hardClean) {
        battleReady = false;
        battleOngoing = false;
        battlesDone = 0;
        accuratePredictions = 0;
        expectedWinner = 0;
    }
    std::cout << "Deleting all useless units\n";
    for (auto &unit : m_bot.GetUnits()) {
        if (unit.getUnitPtr()->unit_type == sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER || unit.getUnitPtr()->unit_type == sc2::UNIT_TYPEID::PROTOSS_NEXUS || unit.getUnitPtr()->unit_type == sc2::UNIT_TYPEID::ZERG_HATCHERY) continue;
        m_bot.Debug()->DebugKillUnit(unit.getUnitPtr());
    }
}

void BattleSimulation::setBattalionNumber (int nb) {
    std::cout << "Set the battalion number to : " << nb << std::endl;
    assert(nb > 0);
    battalionNumber = nb;
}

void BattleSimulation::setBattalionMaximumSize (int nb) {
    std::cout << "Set the battalion maximum size to : " << nb << std::endl;
    assert(nb > 0 && nb < 200);
    battalionMaximumSize = nb;
}

void BattleSimulation::setNumberOfBattles (int nb) {
    std::cout << "Set the number of battles to : " << nb << std::endl;
    assert(nb > 0 && nb < 10000);
    numberOfBattles = nb;
}


sc2::UNIT_TYPEID BattleSimulation::chooseUnit (Army done) {
    sc2::UNIT_TYPEID chosenUnit;
    do {
        int rand = mt() % units.size();
        chosenUnit = units[rand];
    } while (done.contains(chosenUnit) 
             || UnitType(chosenUnit, m_bot).isChangeling() 
             || chosenUnit == sc2::UNIT_TYPEID::ZERG_BROODLING 
             || chosenUnit == sc2::UNIT_TYPEID::PROTOSS_INTERCEPTOR
             || (done.isEmpty() ? false : done.getRace() != m_bot.Observation()->GetUnitTypeData()[static_cast<int>(chosenUnit)].race)
             );
    // changelings don't attack even though they have weapons and you order them to
    // broodlings dispawn after 6 seconds, causing problems, and they can't be made by the player
    // interceptors are only used by carriers and can't seem to be controllable without it
    return chosenUnit;
}

Army* BattleSimulation::generateArmy (int index) {
    Army *army = new Army{};
    for (int i = 0; i < battalionNumber; i++) {
        sc2::UNIT_TYPEID chosenUnit;
        do chosenUnit = chooseUnit(*army); while (index == 2 && chosenUnit == sc2::UNIT_TYPEID::PROTOSS_DARKTEMPLAR);
        // Dark templars are camouflaged while in the enemy team, and cannot be controlled because they can't be seen even with a full map reveal. Use only in ally armies
        army->addBattalion({chosenUnit, static_cast<int>(mt() % battalionMaximumSize + 1), &m_bot});
    }
    return army;
}

void BattleSimulation::singleBattle () {
    int p1 = m_bot.Observation()->GetGameInfo().player_info.at(0).player_id;
    int p2 = m_bot.Observation()->GetGameInfo().player_info.at(1).player_id;
    std::cout << ">> Generating 2 armies for battle #" << battlesDone+1 << std::endl;
    std::cout << ">> Army #1 :\n";
    a1 = generateArmy(1);
    a1->Log();
    std::cout << ">> Army #2 :\n";
    a2 = generateArmy(2);
    a2->Log();
    //std::cout << ">> Choosing 2 spawn positions for the armies" << std::endl;
    sc2::Point2D pos1 = validPositions[mt() % validPositions.size()];
    //std::cout << " * Position #1 : " << pos1.x << " - " << pos1.y << std::endl;
    sc2::Point2D pos2;
    do pos2 = validPositions[mt() % validPositions.size()]; while (pos1 == pos2);
    //std::cout << " * Position #2 : " << pos2.x << " - " << pos2.y << std::endl;
    //std::cout << ">> Choosing battle location" << std::endl;

    do {
        battleLocation = validPositions[mt() % validPositions.size()];
    } while (battleLocation == pos2 || battleLocation == pos1);
    //std::cout << " * Location : " << battleLocation.x << " - " << battleLocation.y << std::endl;
    std::cout << ">> Starting battle #" << battlesDone + 1 << std::endl;
    std::cout << "Expected outcome :\n";
    a1->spawnArmy(pos1, p1);
    a2->spawnArmy(pos2, p2);
    timer = Timer();
    timer.start();
    battleReady = true;
}

void BattleSimulation::startTesting () {
    m_bot.Debug()->DebugEnemyControl();
    std::cout << ">> Started testing\n";
    std::cout << ">> Scheduled " << numberOfBattles << " random battles\n";
    std::cout << ">> Current settings : \n";
    std::cout << " * Number of Battalions per army : " << battalionNumber << std::endl;
    std::cout << " * Maximum Battalion size : " << battalionMaximumSize << std::endl << std::endl;
    singleBattle();
}

bool BattleSimulation::checkBattleEnded () {
    if (!a1->canFight(a2)) {
        std::cout << "The actual Army #1 cannot attack the Army#2, stopping further simulation\n";
        if (expectedWinner == 2) accuratePredictions++;
        goto battleEnded;
    } else if (!a2->canFight(a1)) {
        std::cout << "The actual Army #2 cannot attack the Army#1, stopping further simulation\n";
        if (expectedWinner == 1) accuratePredictions++;
        goto battleEnded;
    } else if (!a1->canFight(a2) && !a2->canFight(a1)) {
        std::cout << "Both armies can't attack each other, stopping the match.\n";
        if (expectedWinner == 0) accuratePredictions++;
        goto battleEnded;
    } else if (a1->isEmpty()) {
        std::cout << "Match over, Army #2 wins\n";
        if (expectedWinner == 2) accuratePredictions++;
        goto battleEnded;
    } else if (a2->isEmpty()) {
        std::cout << "Match over, Army #1 wins\n";
        if (expectedWinner == 1) accuratePredictions++;
        goto battleEnded;
    } 
    return false;
battleEnded:
    battlesDone++;
    battleOngoing = false;
    if (battlesDone < numberOfBattles) {
        std::cout << "Starting next battle immediately\n";
        clean(false);
        singleBattle();
    } else {
        std::cout << "Number of battles done : " << battlesDone << std::endl;
        std::cout << "Accurate predictions : " << accuratePredictions << std::endl;
        std::cout << "Accurate predictions % : " << (static_cast<double>(accuratePredictions) / static_cast<double>(battlesDone)) * 100 << "%\n";
        clean(true);
    }
    return true;
}

void BattleSimulation::startGhost () {
    m_bot.unitSelectorOnStep();
}
