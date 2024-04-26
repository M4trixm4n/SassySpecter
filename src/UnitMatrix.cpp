#include "UnitMatrix.h"

UnitMatrix::UnitMatrix (std::vector<UnitType> unitList, SassySpecterBot *m_bot)
    : m_bot(m_bot)
    , unitList(unitList)
{

}

bool UnitMatrix::cycleUnits () {
    bool next = false;
    for (auto & unit : unitList) { //cycle unitB, cycle unitA if unitB is the last
        if (next) {
            next = false;
            unitB = unit;
            if (skipUselessMatchup()) return cycleUnits();
            return true;
        }
        if (unitB == unit) next = true;
    }
    next = false;
    for (auto & unit : unitList) { //cycle unitA, end if unitA is the last
        if (next) {
            unitA = unit;
            if (skipUselessMatchup()) return cycleUnits();
            return true;
        }
        if (unitA == unit) next = true;
    }
    return false; //every unit combination explored, end
}

bool UnitMatrix::cycleCounters () {
    if (currentUnitBCounter == 10) { //cycle B, A+1
        currentUnitBCounter = 1;
        currentUnitACounter += 1;
    }
    if (currentUnitACounter == 11) { //cycle A, UnitB+1
        currentUnitACounter = 1;
        return cycleUnits();
    }
    return true;
}

void UnitMatrix::spawnNecessaryUnits () const {

}

void UnitMatrix::moveUnits () const {

}

bool UnitMatrix::skirmishEnded () const {

}

void UnitMatrix::addSkirmishResults () {

}

void UnitMatrix::resetGameState () {

}

bool UnitMatrix::skipUselessMatchup () const {
    //need more cases, no weapons, flying unit untargetable by the other etc
    if (unitA == unitB) return true; //same unit, same strength
    return false;
}

void UnitMatrix::addFinalRatios () {

}