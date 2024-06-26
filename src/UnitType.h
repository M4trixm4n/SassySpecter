#pragma once

#include "Common.h"

class SassySpecterBot;

class UnitType
{
    mutable SassySpecterBot * m_bot;
    
#ifdef SC2API
    sc2::UnitTypeID m_type;
#else
    BWAPI::UnitType m_type;
#endif

public:

    UnitType();

#ifdef SC2API
    UnitType(const sc2::UnitTypeID & type, SassySpecterBot & bot);
    sc2::UnitTypeID getAPIUnitType() const;
    bool is(const sc2::UnitTypeID & type) const;
#else
    UnitType(const BWAPI::UnitType & type, SassySpecterBot & bot);
    BWAPI::UnitType getAPIUnitType() const;
    bool is(const BWAPI::UnitType & type) const;
#endif

    bool operator < (const UnitType & rhs) const;
    bool operator == (const UnitType & rhs) const;

    std::string getName() const;
    CCRace getRace() const;
    
    bool isValid() const;
    bool isBuilding() const;
    bool isCombatUnit() const;
    bool isSupplyProvider() const;
    bool isResourceDepot() const;
    bool isRefinery() const;
    bool isDetector() const;
    bool isGeyser() const;
    bool isMineral() const;
    bool isWorker() const;
    bool isMorphedBuilding() const;
    bool isChangeling() const;
    bool isAirUnit () const;
    bool canAttack() const;
    bool canMove() const;
    bool isAddon() const;
    CCPositionType getAttackRange() const;
    int tileWidth() const;
    int tileHeight() const;
    int supplyProvided() const;
    int supplyRequired() const;
    int mineralPrice() const;
    int gasPrice() const;

    std::vector<UnitType> whatBuilds() const;

    static UnitType GetUnitTypeFromName(const std::string & name, SassySpecterBot & bot);

    // lazy functions to let me know if this type is a special type
    bool isOverlord() const;
    bool isLarva() const;
    bool isEgg() const;
    bool isQueen() const;
    bool isTank() const;

    sc2::AbilityID warpAbility() const;
    bool equivalentBuilding () const; //checks if the building is equivalent to another (and thus just copied later)
};
