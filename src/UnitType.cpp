#include "UnitType.h"
#include "SassySpecterBot.h"

UnitType::UnitType()
    : m_bot(nullptr)
    , m_type(0)
{

}

#ifdef SC2API
UnitType::UnitType(const sc2::UnitTypeID & type, SassySpecterBot & bot)
    : m_bot(&bot)
    , m_type(type)
{
    
}

sc2::UnitTypeID UnitType::getAPIUnitType() const
{
    return m_type;
}

bool UnitType::is(const sc2::UnitTypeID & type) const
{
    return m_type == type;
}

#else
UnitType::UnitType(const BWAPI::UnitType & type, SassySpecterBot & bot)
    : m_bot(&bot)
    , m_type(type)
{
    
}

BWAPI::UnitType UnitType::getAPIUnitType() const
{
    return m_type;
}

bool UnitType::is(const BWAPI::UnitType & type) const
{
    return m_type == type;
}

#endif
bool UnitType::operator < (const UnitType & rhs) const
{
    return m_type < rhs.m_type;
}

bool UnitType::operator == (const UnitType & rhs) const
{
    return m_type == rhs.m_type;
}

bool UnitType::isValid() const
{
    return m_type != 0;
}

std::string UnitType::getName() const
{
#ifdef SC2API
    return sc2::UnitTypeToName(m_type);
#else
    return m_type.getName();
#endif
}

CCRace UnitType::getRace() const
{
#ifdef SC2API
    return m_bot->Observation()->GetUnitTypeData()[m_type].race;
#else
    return m_type.getRace();
#endif
}


bool UnitType::canAttack() const {
    return m_bot->Observation()->GetUnitTypeData()[m_type].weapons.size();
}

bool UnitType::canMove() const {
    return m_bot->Observation()->GetUnitTypeData()[m_type].movement_speed != 0.0f;
}

bool UnitType::isCombatUnit() const
{
#ifdef SC2API
    if (isWorker()) { return false; }
    if (isSupplyProvider()) { return false; }
    if (isBuilding()) { return false; }
    if (isEgg() || isLarva()) { return false; }

    return true;
#else
    // check for various types of combat units
    if (m_type.canAttack() || 
        m_type == BWAPI::UnitTypes::Terran_Medic ||
        m_type == BWAPI::UnitTypes::Protoss_High_Templar ||
        m_type == BWAPI::UnitTypes::Protoss_Observer ||
        m_type == BWAPI::UnitTypes::Zerg_Overlord ||
        m_type == BWAPI::UnitTypes::Protoss_Observer)
    {
        return true;
    }

    // no workers or buildings allowed
    if (m_type.isWorker() || m_type.isBuilding())
    {
        return false;
    }

    return false;
#endif
}

bool UnitType::isSupplyProvider() const
{
    return (supplyProvided() > 0) && !isResourceDepot();
}

bool UnitType::isResourceDepot() const
{
#ifdef SC2API
    switch (m_type.ToType()) 
    {
        case sc2::UNIT_TYPEID::ZERG_HATCHERY                : return true;
        case sc2::UNIT_TYPEID::ZERG_LAIR                    : return true;
        case sc2::UNIT_TYPEID::ZERG_HIVE                    : return true;
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER         : return true;
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND        : return true;
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING  : return true;
        case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS     : return true;
        case sc2::UNIT_TYPEID::PROTOSS_NEXUS                : return true;
        default: return false;
    }
#else
    return m_type.isResourceDepot();
#endif
}

bool UnitType::isRefinery() const
{
#ifdef SC2API
    switch (m_type.ToType()) 
    {
        case sc2::UNIT_TYPEID::TERRAN_REFINERY      : return true;
        case sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR  : return true;
        case sc2::UNIT_TYPEID::ZERG_EXTRACTOR       : return true;
        default: return false;
    }
#else
    return m_type.isRefinery();
#endif
}

bool UnitType::isDetector() const
{
#ifdef SC2API
    switch (m_type.ToType())
    {
        case sc2::UNIT_TYPEID::PROTOSS_OBSERVER        : return true;
        case sc2::UNIT_TYPEID::ZERG_OVERSEER           : return true;
        case sc2::UNIT_TYPEID::TERRAN_MISSILETURRET    : return true;
        case sc2::UNIT_TYPEID::ZERG_SPORECRAWLER       : return true;
        case sc2::UNIT_TYPEID::PROTOSS_PHOTONCANNON    : return true;
        case sc2::UNIT_TYPEID::TERRAN_RAVEN            : return true;
        default: return false;
    }
#else
    return m_type.isDetector();
#endif
}

bool UnitType::isGeyser() const
{
#ifdef SC2API
    switch (m_type.ToType()) 
    {
        case sc2::UNIT_TYPEID::NEUTRAL_VESPENEGEYSER        : return true;
        case sc2::UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER : return true;
        case sc2::UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER  : return true;
        default: return false;
    }
#else
    return m_type == BWAPI::UnitTypes::Resource_Vespene_Geyser;
#endif
}

bool UnitType::isMineral() const
{
#ifdef SC2API
    switch (m_type.ToType()) 
    {
        case sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD         : return true;
        case sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD750      : return true;
        case sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD     : return true;
        case sc2::UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750  : return true;
        case sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD		: return true;
        case sc2::UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750	: return true;
        default: return false;
    }
#else
    return m_type.isMineralField();
#endif
}

bool UnitType::isWorker() const
{
#ifdef SC2API
    switch (m_type.ToType()) 
    {
        case sc2::UNIT_TYPEID::TERRAN_SCV           : return true;
        case sc2::UNIT_TYPEID::PROTOSS_PROBE        : return true;
        case sc2::UNIT_TYPEID::ZERG_DRONE           : return true;
        case sc2::UNIT_TYPEID::ZERG_DRONEBURROWED   : return true;
        default: return false;
    }
#else
    return m_type.isWorker();
#endif
}

bool UnitType::isChangeling() const {
    switch (m_type.ToType()) {
        case sc2::UNIT_TYPEID::ZERG_CHANGELING:
        case sc2::UNIT_TYPEID::ZERG_CHANGELINGMARINE:
        case sc2::UNIT_TYPEID::ZERG_CHANGELINGMARINESHIELD:
        case sc2::UNIT_TYPEID::ZERG_CHANGELINGZEALOT:
        case sc2::UNIT_TYPEID::ZERG_CHANGELINGZERGLING:
        case sc2::UNIT_TYPEID::ZERG_CHANGELINGZERGLINGWINGS: 
            return true;
        default: return false;
    }
}

bool UnitType::isAirUnit () const {
    switch (m_type.ToType()) {
        case sc2::UNIT_TYPEID::PROTOSS_OBSERVER:
        case sc2::UNIT_TYPEID::PROTOSS_WARPPRISM:
        case sc2::UNIT_TYPEID::PROTOSS_PHOENIX:
        case sc2::UNIT_TYPEID::PROTOSS_VOIDRAY:
        case sc2::UNIT_TYPEID::PROTOSS_CARRIER:
        case sc2::UNIT_TYPEID::PROTOSS_INTERCEPTOR:
        case sc2::UNIT_TYPEID::PROTOSS_MOTHERSHIP:
        case sc2::UNIT_TYPEID::PROTOSS_MOTHERSHIPCORE:
        case sc2::UNIT_TYPEID::PROTOSS_ORACLE:
        case sc2::UNIT_TYPEID::PROTOSS_TEMPEST:
        case sc2::UNIT_TYPEID::TERRAN_MEDIVAC:
        case sc2::UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
        case sc2::UNIT_TYPEID::TERRAN_BANSHEE:
        case sc2::UNIT_TYPEID::TERRAN_RAVEN:
        case sc2::UNIT_TYPEID::TERRAN_BATTLECRUISER:
        case sc2::UNIT_TYPEID::TERRAN_POINTDEFENSEDRONE:
        case sc2::UNIT_TYPEID::TERRAN_LIBERATOR:
        case sc2::UNIT_TYPEID::ZERG_OVERLORD:
        case sc2::UNIT_TYPEID::ZERG_OVERSEER:
        case sc2::UNIT_TYPEID::ZERG_MUTALISK:
        case sc2::UNIT_TYPEID::ZERG_CORRUPTOR:
        case sc2::UNIT_TYPEID::ZERG_BROODLORD:
        case sc2::UNIT_TYPEID::ZERG_VIPER:
            return true;
        default:
            return false;
    }
}

CCPositionType UnitType::getAttackRange() const
{
#ifdef SC2API
    auto & weapons = m_bot->Observation()->GetUnitTypeData()[m_type].weapons;
    
    if (weapons.empty())
    {
        return 0.0f;
    }

    float maxRange = 0.0f;
    for (auto & weapon : weapons)
    {
        if (weapon.range > maxRange)
        {
            maxRange = weapon.range;
        }
    }

    return maxRange;
#else
    // TODO: this is ground weapon range right now
    return m_type.groundWeapon().maxRange();
#endif
}

int UnitType::tileWidth() const
{
#ifdef SC2API
    if (isMineral()) { return 2; }
    if (isGeyser()) { return 3; }
    else { return (int)(2 * m_bot->Observation()->GetAbilityData()[m_bot->Data(*this).buildAbility].footprint_radius); }
#else
    return m_type.tileWidth();
#endif
}

int UnitType::tileHeight() const
{
#ifdef SC2API
    if (isMineral()) { return 1; }
    if (isGeyser()) { return 3; }
    else { return (int)(2 * m_bot->Observation()->GetAbilityData()[m_bot->Data(*this).buildAbility].footprint_radius); }
#else
    return m_type.tileHeight();
#endif
}

bool UnitType::isAddon() const
{
#ifdef SC2API
    switch (m_type.ToType()) {
        case sc2::UNIT_TYPEID::TERRAN_BARRACKSREACTOR:
        case sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB:
        case sc2::UNIT_TYPEID::TERRAN_FACTORYREACTOR:
        case sc2::UNIT_TYPEID::TERRAN_FACTORYTECHLAB:
        case sc2::UNIT_TYPEID::TERRAN_STARPORTREACTOR:
        case sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB: return true;
        default: return false;
    }
#else
    return m_type.isAddon();
#endif
}

bool UnitType::isBuilding() const
{
#ifdef SC2API
    for (auto &attr : m_bot->Observation()->GetUnitTypeData()[m_type].attributes) {
        if (attr == sc2::Attribute::Structure) return true;
    }
    return false;
#else
    return m_type.isBuilding();
#endif
}

int UnitType::supplyProvided() const
{
#ifdef SC2API
    return (int)m_bot->Observation()->GetUnitTypeData()[m_type].food_provided;
#else
    return m_type.supplyProvided();
#endif
}

int UnitType::supplyRequired() const
{
#ifdef SC2API
    return (int)m_bot->Observation()->GetUnitTypeData()[m_type].food_required;
#else
    return m_type.supplyRequired();
#endif
}

int UnitType::mineralPrice() const
{
#ifdef SC2API
    return (int)m_bot->Observation()->GetUnitTypeData()[m_type].mineral_cost;
#else
    return m_type.mineralPrice();
#endif
}

int UnitType::gasPrice() const
{
#ifdef SC2API
    return (int)m_bot->Observation()->GetUnitTypeData()[m_type].vespene_cost;
#else
    return m_type.gasPrice();
#endif
}

UnitType UnitType::GetUnitTypeFromName(const std::string & name, SassySpecterBot & bot)
{
#ifdef SC2API
    for (const sc2::UnitTypeData & data : bot.Observation()->GetUnitTypeData())
    {
        if (name == data.name)
        {
            return UnitType(data.unit_type_id, bot);
        }
    }
#else

#endif

    return UnitType();
}

bool UnitType::isOverlord() const
{
#ifdef SC2API
    return m_type == sc2::UNIT_TYPEID::ZERG_OVERLORD;
#else
    return m_type == BWAPI::UnitTypes::Zerg_Overlord;
#endif
}

bool UnitType::isLarva() const
{
#ifdef SC2API
    return m_type == sc2::UNIT_TYPEID::ZERG_LARVA;
#else
    return m_type == BWAPI::UnitTypes::Zerg_Larva;
#endif
}

bool UnitType::isEgg() const
{
#ifdef SC2API
    return m_type == sc2::UNIT_TYPEID::ZERG_EGG;
#else
    return m_type == BWAPI::UnitTypes::Zerg_Egg;
#endif
}

bool UnitType::isQueen() const
{
#ifdef SC2API
    return m_type == sc2::UNIT_TYPEID::ZERG_QUEEN;
#else
    return m_type == BWAPI::UnitTypes::Zerg_Queen;
#endif
}

bool UnitType::isTank() const
{
#ifdef SC2API
    return m_type == sc2::UNIT_TYPEID::TERRAN_SIEGETANK || m_type == sc2::UNIT_TYPEID::TERRAN_SIEGETANKSIEGED;
#else
    return m_type == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode || m_type == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode;
#endif
}

bool UnitType::isMorphedBuilding() const
{
#ifdef SC2API
    
    switch (m_type.ToType())
    {
        case sc2::UNIT_TYPEID::ZERG_LAIR:                   { return true;  }
        case sc2::UNIT_TYPEID::ZERG_HIVE:                   { return true;  }
        case sc2::UNIT_TYPEID::ZERG_GREATERSPIRE:           { return true;  }
        case sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:    { return true;  }
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND:       { return true;  }
        default:                                            { return false; }                                                            
    }
#else
    return  m_type == BWAPI::UnitTypes::Zerg_Sunken_Colony ||
            m_type == BWAPI::UnitTypes::Zerg_Spore_Colony ||
            m_type == BWAPI::UnitTypes::Zerg_Lair ||
            m_type == BWAPI::UnitTypes::Zerg_Hive ||
            m_type == BWAPI::UnitTypes::Zerg_Greater_Spire;
#endif
}

sc2::AbilityID UnitType::warpAbility () const {
    switch (m_type.ToType()) {
        case sc2::UNIT_TYPEID::PROTOSS_ADEPT: return sc2::ABILITY_ID::TRAINWARP_ADEPT;
        case sc2::UNIT_TYPEID::PROTOSS_DARKTEMPLAR: return sc2::ABILITY_ID::TRAINWARP_DARKTEMPLAR;
        case sc2::UNIT_TYPEID::PROTOSS_HIGHTEMPLAR: return sc2::ABILITY_ID::TRAINWARP_HIGHTEMPLAR;
        case sc2::UNIT_TYPEID::PROTOSS_SENTRY: return sc2::ABILITY_ID::TRAINWARP_SENTRY;
        case sc2::UNIT_TYPEID::PROTOSS_STALKER: return sc2::ABILITY_ID::TRAINWARP_STALKER;
        case sc2::UNIT_TYPEID::PROTOSS_ZEALOT: return sc2::ABILITY_ID::TRAINWARP_ZEALOT;
        default: return 0;
    }
}

std::vector<UnitType> UnitType::whatBuilds() const {
    using namespace sc2;
    switch (m_type.ToType()) {
        case UNIT_TYPEID::PROTOSS_ASSIMILATOR:
        case UNIT_TYPEID::PROTOSS_CYBERNETICSCORE:
        case UNIT_TYPEID::PROTOSS_DARKSHRINE:
        case UNIT_TYPEID::PROTOSS_FLEETBEACON:
        case UNIT_TYPEID::PROTOSS_FORGE:
        case UNIT_TYPEID::PROTOSS_GATEWAY:
        case UNIT_TYPEID::PROTOSS_NEXUS:
        case UNIT_TYPEID::PROTOSS_PHOTONCANNON:
        case UNIT_TYPEID::PROTOSS_PYLON:
        case UNIT_TYPEID::PROTOSS_ROBOTICSBAY:
        case UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY:
        case UNIT_TYPEID::PROTOSS_STARGATE:
        case UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE:
        case UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL:
            return {UnitType(UNIT_TYPEID::PROTOSS_PROBE, *m_bot)};
        
        case UNIT_TYPEID::PROTOSS_WARPGATE:
        case UNIT_TYPEID::PROTOSS_SHIELDBATTERY:
            return {UnitType(UNIT_TYPEID::PROTOSS_GATEWAY, *m_bot)};
        
        case UNIT_TYPEID::PROTOSS_ADEPT:
        case UNIT_TYPEID::PROTOSS_DARKTEMPLAR:
        case UNIT_TYPEID::PROTOSS_HIGHTEMPLAR:
        case UNIT_TYPEID::PROTOSS_SENTRY:
        case UNIT_TYPEID::PROTOSS_STALKER:
        case UNIT_TYPEID::PROTOSS_ZEALOT:
            return {UnitType(UNIT_TYPEID::PROTOSS_GATEWAY, *m_bot), UnitType(UNIT_TYPEID::PROTOSS_WARPGATE, *m_bot)};
        
        case UNIT_TYPEID::PROTOSS_CARRIER:
        case UNIT_TYPEID::PROTOSS_ORACLE:
        case UNIT_TYPEID::PROTOSS_PHOENIX:
        case UNIT_TYPEID::PROTOSS_TEMPEST:
        case UNIT_TYPEID::PROTOSS_VOIDRAY:
            return {UnitType(UNIT_TYPEID::PROTOSS_STARGATE, *m_bot)};
        
        case UNIT_TYPEID::PROTOSS_COLOSSUS:
        case UNIT_TYPEID::PROTOSS_DISRUPTOR:
        case UNIT_TYPEID::PROTOSS_IMMORTAL:
        case UNIT_TYPEID::PROTOSS_OBSERVER:
        case UNIT_TYPEID::PROTOSS_WARPPRISM:
            return {UnitType(UNIT_TYPEID::PROTOSS_ROBOTICSFACILITY, *m_bot)};

        case UNIT_TYPEID::PROTOSS_INTERCEPTOR:
            return {UnitType(UNIT_TYPEID::PROTOSS_CARRIER, *m_bot)};

        case UNIT_TYPEID::PROTOSS_MOTHERSHIP:
        case UNIT_TYPEID::PROTOSS_MOTHERSHIPCORE:
        case UNIT_TYPEID::PROTOSS_PROBE:
            return {UnitType(UNIT_TYPEID::PROTOSS_NEXUS, *m_bot)};

        case UNIT_TYPEID::PROTOSS_ORACLESTASISTRAP:
            return {UnitType(UNIT_TYPEID::PROTOSS_ORACLE, *m_bot)};

        case UNIT_TYPEID::PROTOSS_ARCHON: //needs verification
            return {UnitType(UNIT_TYPEID::PROTOSS_HIGHTEMPLAR, *m_bot), UnitType(UNIT_TYPEID::PROTOSS_DARKTEMPLAR, *m_bot)};

        case UNIT_TYPEID::TERRAN_ARMORY:
        case UNIT_TYPEID::TERRAN_BARRACKS:
        case UNIT_TYPEID::TERRAN_BUNKER:
        case UNIT_TYPEID::TERRAN_COMMANDCENTER:
        case UNIT_TYPEID::TERRAN_ENGINEERINGBAY:
        case UNIT_TYPEID::TERRAN_FACTORY:
        case UNIT_TYPEID::TERRAN_FUSIONCORE:
        case UNIT_TYPEID::TERRAN_GHOSTACADEMY:
        case UNIT_TYPEID::TERRAN_MISSILETURRET:
        case UNIT_TYPEID::TERRAN_REFINERY:
        case UNIT_TYPEID::TERRAN_SENSORTOWER:
        case UNIT_TYPEID::TERRAN_STARPORT:
        case UNIT_TYPEID::TERRAN_SUPPLYDEPOT:
            return {UnitType(UNIT_TYPEID::TERRAN_SCV, *m_bot)};
        
        case UNIT_TYPEID::TERRAN_ORBITALCOMMAND:
        case UNIT_TYPEID::TERRAN_PLANETARYFORTRESS:
        case UNIT_TYPEID::TERRAN_SCV:
            return {UnitType(UNIT_TYPEID::TERRAN_COMMANDCENTER, *m_bot)};

        case UNIT_TYPEID::TERRAN_BARRACKSREACTOR:
        case UNIT_TYPEID::TERRAN_BARRACKSTECHLAB:
        case UNIT_TYPEID::TERRAN_GHOST:
        case UNIT_TYPEID::TERRAN_MARAUDER:
        case UNIT_TYPEID::TERRAN_MARINE:
        case UNIT_TYPEID::TERRAN_REAPER:
            return {UnitType(UNIT_TYPEID::TERRAN_BARRACKS, *m_bot)};

        case UNIT_TYPEID::TERRAN_FACTORYREACTOR:
        case UNIT_TYPEID::TERRAN_FACTORYTECHLAB:
        case UNIT_TYPEID::TERRAN_CYCLONE:
        case UNIT_TYPEID::TERRAN_HELLION:
        case UNIT_TYPEID::TERRAN_SIEGETANK:
        case UNIT_TYPEID::TERRAN_THOR:
        case UNIT_TYPEID::TERRAN_WIDOWMINE:
            return {UnitType(UNIT_TYPEID::TERRAN_FACTORY, *m_bot)};

        case UNIT_TYPEID::TERRAN_STARPORTREACTOR:
        case UNIT_TYPEID::TERRAN_STARPORTTECHLAB:
        case UNIT_TYPEID::TERRAN_BANSHEE:
        case UNIT_TYPEID::TERRAN_BATTLECRUISER:
        case UNIT_TYPEID::TERRAN_LIBERATOR:
        case UNIT_TYPEID::TERRAN_MEDIVAC:
        case UNIT_TYPEID::TERRAN_RAVEN:
        case UNIT_TYPEID::TERRAN_VIKINGFIGHTER:
            return {UnitType(UNIT_TYPEID::TERRAN_STARPORT, *m_bot)};

        case UNIT_TYPEID::TERRAN_AUTOTURRET:
        case UNIT_TYPEID::TERRAN_POINTDEFENSEDRONE:
            return {UnitType(UNIT_TYPEID::TERRAN_RAVEN, *m_bot)};

        case UNIT_TYPEID::TERRAN_HELLIONTANK:
            return {UnitType(UNIT_TYPEID::TERRAN_HELLION, *m_bot)};

        case UNIT_TYPEID::TERRAN_MULE:
            return {UnitType(UNIT_TYPEID::TERRAN_ORBITALCOMMAND, *m_bot)};

        case UNIT_TYPEID::TERRAN_NUKE:
            return {UnitType(UNIT_TYPEID::TERRAN_GHOSTACADEMY, *m_bot)};

        case UNIT_TYPEID::TERRAN_TECHLAB:
            return {UnitType(UNIT_TYPEID::TERRAN_STARPORTTECHLAB, *m_bot), UnitType(UNIT_TYPEID::TERRAN_FACTORYTECHLAB, *m_bot), UnitType(UNIT_TYPEID::TERRAN_BARRACKSTECHLAB, *m_bot)};

        case UNIT_TYPEID::TERRAN_REACTOR:
            return {UnitType(UNIT_TYPEID::TERRAN_STARPORTREACTOR, *m_bot), UnitType(UNIT_TYPEID::TERRAN_FACTORYREACTOR, *m_bot), UnitType(UNIT_TYPEID::TERRAN_BARRACKSREACTOR, *m_bot)};

        case UNIT_TYPEID::ZERG_BANELINGNEST:
        case UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER:
        case UNIT_TYPEID::ZERG_EXTRACTOR:
        case UNIT_TYPEID::ZERG_HATCHERY:
        case UNIT_TYPEID::ZERG_HYDRALISKDEN:
        case UNIT_TYPEID::ZERG_INFESTATIONPIT:
        case UNIT_TYPEID::ZERG_LURKERDENMP:
        case UNIT_TYPEID::ZERG_NYDUSCANAL:
        case UNIT_TYPEID::ZERG_NYDUSNETWORK:
        case UNIT_TYPEID::ZERG_ROACHWARREN:
        case UNIT_TYPEID::ZERG_SPAWNINGPOOL:
        case UNIT_TYPEID::ZERG_SPINECRAWLER:
        case UNIT_TYPEID::ZERG_SPIRE:
        case UNIT_TYPEID::ZERG_SPORECRAWLER:
        case UNIT_TYPEID::ZERG_ULTRALISKCAVERN:
            return {UnitType(UNIT_TYPEID::ZERG_DRONE, *m_bot)};

        case UNIT_TYPEID::ZERG_CREEPTUMOR:
            return {UnitType(UNIT_TYPEID::ZERG_CREEPTUMORQUEEN, *m_bot)};

        case UNIT_TYPEID::ZERG_CREEPTUMORQUEEN:
            return {UnitType(UNIT_TYPEID::ZERG_QUEEN, *m_bot)};

        case UNIT_TYPEID::ZERG_GREATERSPIRE:
            return {UnitType(UNIT_TYPEID::ZERG_SPIRE, *m_bot)};

        case UNIT_TYPEID::ZERG_HIVE:
            return {UnitType(UNIT_TYPEID::ZERG_LAIR, *m_bot)};

        case UNIT_TYPEID::ZERG_LAIR:
            return {UnitType(UNIT_TYPEID::ZERG_HATCHERY, *m_bot)};

        case UNIT_TYPEID::ZERG_BANELING:
        case UNIT_TYPEID::ZERG_CORRUPTOR:
        case UNIT_TYPEID::ZERG_DRONE:
        case UNIT_TYPEID::ZERG_HYDRALISK:
        case UNIT_TYPEID::ZERG_INFESTOR:
        case UNIT_TYPEID::ZERG_MUTALISK:
        case UNIT_TYPEID::ZERG_OVERLORD:
        case UNIT_TYPEID::ZERG_ROACH:
        case UNIT_TYPEID::ZERG_SWARMHOSTMP:
        case UNIT_TYPEID::ZERG_ULTRALISK:
        case UNIT_TYPEID::ZERG_VIPER:
        case UNIT_TYPEID::ZERG_ZERGLING:
            return {UnitType(UNIT_TYPEID::ZERG_LARVA, *m_bot)};

        case UNIT_TYPEID::ZERG_BROODLING:
        case UNIT_TYPEID::ZERG_CHANGELINGMARINE:
        case UNIT_TYPEID::ZERG_CHANGELINGMARINESHIELD:
        case UNIT_TYPEID::ZERG_CHANGELINGZEALOT:
        case UNIT_TYPEID::ZERG_CHANGELINGZERGLING:
        case UNIT_TYPEID::ZERG_CHANGELINGZERGLINGWINGS:
        case UNIT_TYPEID::ZERG_EGG:
        case UNIT_TYPEID::ZERG_LARVA:
        case UNIT_TYPEID::ZERG_BANELINGCOCOON: //ignore cocoons/eggs
        case UNIT_TYPEID::ZERG_BROODLORDCOCOON:
        case UNIT_TYPEID::ZERG_OVERLORDCOCOON:
        case UNIT_TYPEID::ZERG_INFESTEDTERRANSEGG:
        case UNIT_TYPEID::ZERG_LURKERMPEGG:
        case UNIT_TYPEID::ZERG_RAVAGERCOCOON:
        case UNIT_TYPEID::ZERG_TRANSPORTOVERLORDCOCOON:
        case UNIT_TYPEID::ZERG_BANELINGBURROWED: //ignore burrowed
        case UNIT_TYPEID::ZERG_DRONEBURROWED:
        case UNIT_TYPEID::ZERG_HYDRALISKBURROWED:
        case UNIT_TYPEID::ZERG_ROACHBURROWED:
        case UNIT_TYPEID::ZERG_ZERGLINGBURROWED:
        case UNIT_TYPEID::ZERG_QUEENBURROWED:
        case UNIT_TYPEID::ZERG_INFESTORBURROWED:
        case UNIT_TYPEID::ZERG_CREEPTUMORBURROWED:
        case UNIT_TYPEID::ZERG_SWARMHOSTBURROWEDMP:
        case UNIT_TYPEID::TERRAN_WIDOWMINEBURROWED:
        case UNIT_TYPEID::ZERG_LURKERMPBURROWED:
        case UNIT_TYPEID::ZERG_ULTRALISKBURROWED:
        case UNIT_TYPEID::ZERG_SPINECRAWLERUPROOTED: // ignore uprooted
        case UNIT_TYPEID::ZERG_SPORECRAWLERUPROOTED:
        case UNIT_TYPEID::NEUTRAL_FORCEFIELD: // no idea what it is
        case UNIT_TYPEID::NEUTRAL_KARAKFEMALE:
        case UNIT_TYPEID::NEUTRAL_UTILITYBOT:
        case UNIT_TYPEID::ZERG_PARASITICBOMBDUMMY:
        case UNIT_TYPEID::TERRAN_KD8CHARGE:
        case UNIT_TYPEID::TERRAN_PREVIEWBUNKERUPGRADED:
        case UNIT_TYPEID::TERRAN_SIEGETANKSIEGED:       //abilities to morph aren't stored in the unit's information, can't actually get this unit with the current implementation
        case UNIT_TYPEID::TERRAN_VIKINGASSAULT:         
        case UNIT_TYPEID::PROTOSS_WARPPRISMPHASING:
        case UNIT_TYPEID::TERRAN_THORAP:
        case UNIT_TYPEID::ZERG_LOCUSTMPFLYING:
        case UNIT_TYPEID::PROTOSS_DISRUPTORPHASED:
        case UNIT_TYPEID::TERRAN_LIBERATORAG:
        case UNIT_TYPEID::PROTOSS_ADEPTPHASESHIFT:
        case UNIT_TYPEID::PROTOSS_OBSERVERSIEGEMODE:
        case UNIT_TYPEID::ZERG_OVERSEERSIEGEMODE:
        case UNIT_TYPEID::TERRAN_REFINERYRICH:
        case UNIT_TYPEID::PROTOSS_ASSIMILATORRICH:
        case UNIT_TYPEID::ZERG_EXTRACTORRICH:
        case UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD: // uncraftable / already on the map
        case UNIT_TYPEID::NEUTRAL_RICHMINERALFIELD750:
        case UNIT_TYPEID::NEUTRAL_XELNAGATOWER:
        case UNIT_TYPEID::NEUTRAL_SCANTIPEDE:
        case UNIT_TYPEID::NEUTRAL_MINERALFIELD:
        case UNIT_TYPEID::NEUTRAL_VESPENEGEYSER:
        case UNIT_TYPEID::NEUTRAL_SPACEPLATFORMGEYSER:
        case UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEDEBRIS6X6:
        case UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEROCK6X6:
        case UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEDEBRISRAMPDIAGONALHUGEULBR:
        case UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEDEBRISRAMPDIAGONALHUGEBLUR:
        case UNIT_TYPEID::NEUTRAL_UNBUILDABLEBRICKSDESTRUCTIBLE:
        case UNIT_TYPEID::NEUTRAL_UNBUILDABLEPLATESDESTRUCTIBLE:
        case UNIT_TYPEID::NEUTRAL_MINERALFIELD750:
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERDEBRIS:
        case UNIT_TYPEID::NEUTRAL_DEBRISRAMPLEFT:
        case UNIT_TYPEID::NEUTRAL_DEBRISRAMPRIGHT:
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLEROCKTOWERDEBRIS:
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERPUSHUNITRAMPLEFT:
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERPUSHUNITRAMPRIGHT:
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLEROCKTOWERPUSHUNIT:
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERPUSHUNIT:
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLEROCKTOWERDIAGONAL:
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERDIAGONAL:
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERRAMPLEFT:
        case UNIT_TYPEID::NEUTRAL_COLLAPSIBLETERRANTOWERRAMPRIGHT:
        case UNIT_TYPEID::NEUTRAL_PROTOSSVESPENEGEYSER:
        case UNIT_TYPEID::NEUTRAL_DESTRUCTIBLEROCKEX1DIAGONALHUGEBLUR:
        case UNIT_TYPEID::NEUTRAL_LABMINERALFIELD:
        case UNIT_TYPEID::NEUTRAL_LABMINERALFIELD750:
        case UNIT_TYPEID::PROTOSS_PYLONOVERCHARGED:
        case UNIT_TYPEID::NEUTRAL_RICHVESPENEGEYSER:
        case UNIT_TYPEID::NEUTRAL_DESTRUCTIBLECITYDEBRIS6X6:
        case UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD:
        case UNIT_TYPEID::NEUTRAL_PURIFIERRICHMINERALFIELD750:
        case UNIT_TYPEID::NEUTRAL_PURIFIERVESPENEGEYSER:
        case UNIT_TYPEID::NEUTRAL_SHAKURASVESPENEGEYSER:
        case UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD:
        case UNIT_TYPEID::NEUTRAL_PURIFIERMINERALFIELD750:
        case UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD:
        case UNIT_TYPEID::NEUTRAL_BATTLESTATIONMINERALFIELD750:
        case UNIT_TYPEID::NEUTRAL_MINERALFIELD450:
            return {UnitType()};

        case UNIT_TYPEID::ZERG_BROODLORD:
            return {UnitType(UNIT_TYPEID::ZERG_CORRUPTOR, *m_bot)};

        case UNIT_TYPEID::ZERG_CHANGELING:
            return {UnitType(UNIT_TYPEID::ZERG_OVERSEER, *m_bot)};

        case UNIT_TYPEID::ZERG_INFESTORTERRAN:
            return {UnitType(UNIT_TYPEID::ZERG_INFESTOR, *m_bot)};

        case UNIT_TYPEID::ZERG_LURKERMP:
            return {UnitType(UNIT_TYPEID::ZERG_HYDRALISK, *m_bot)};

        case UNIT_TYPEID::ZERG_OVERSEER:
        case UNIT_TYPEID::ZERG_OVERLORDTRANSPORT: //upgrade needed
            return {UnitType(UNIT_TYPEID::ZERG_OVERLORD, *m_bot)};

        case UNIT_TYPEID::ZERG_QUEEN:
            return {UnitType(sc2::UNIT_TYPEID::ZERG_HATCHERY, *m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, *m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, *m_bot)};

        case UNIT_TYPEID::ZERG_RAVAGER:
            return {UnitType(UNIT_TYPEID::ZERG_ROACH, *m_bot)};

        case UNIT_TYPEID::ZERG_LOCUSTMP:
            return {UnitType(UNIT_TYPEID::ZERG_SWARMHOSTMP, *m_bot)};

        default:
            std::cout << "Missing whatBuilds case for this unit type : " << m_type.to_string() << std::endl;
            return {};
    }
}

bool UnitType::equivalentBuilding () const {
    switch (m_type.ToType()) {
        case sc2::UNIT_TYPEID::TERRAN_BARRACKSFLYING:
        case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING:
        case sc2::UNIT_TYPEID::TERRAN_FACTORYFLYING:
        case sc2::UNIT_TYPEID::TERRAN_STARPORTFLYING:
        case sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED:
        case sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING:
            return true;
        default:
            return false;
    }
}

std::vector<sc2::UNIT_TYPEID> UnitType::trainableUnits () {
    using namespace sc2;
    std::vector<sc2::UNIT_TYPEID> list{
        UNIT_TYPEID::PROTOSS_ADEPT,
        UNIT_TYPEID::PROTOSS_ARCHON,
        UNIT_TYPEID::PROTOSS_CARRIER,
        UNIT_TYPEID::PROTOSS_COLOSSUS,
        UNIT_TYPEID::PROTOSS_DARKTEMPLAR,
        UNIT_TYPEID::PROTOSS_DISRUPTOR,
        UNIT_TYPEID::PROTOSS_HIGHTEMPLAR,
        UNIT_TYPEID::PROTOSS_IMMORTAL,
        UNIT_TYPEID::PROTOSS_MOTHERSHIP,
        UNIT_TYPEID::PROTOSS_OBSERVER,
        UNIT_TYPEID::PROTOSS_ORACLE,
        UNIT_TYPEID::PROTOSS_PHOENIX,
        UNIT_TYPEID::PROTOSS_PROBE,
        UNIT_TYPEID::PROTOSS_SENTRY,
        UNIT_TYPEID::PROTOSS_STALKER,
        UNIT_TYPEID::PROTOSS_TEMPEST,
        UNIT_TYPEID::PROTOSS_VOIDRAY,
        UNIT_TYPEID::PROTOSS_ZEALOT,
        UNIT_TYPEID::TERRAN_BANSHEE,
        UNIT_TYPEID::TERRAN_BATTLECRUISER,
        UNIT_TYPEID::TERRAN_CYCLONE,
        UNIT_TYPEID::TERRAN_GHOST,
        UNIT_TYPEID::TERRAN_HELLION,
        UNIT_TYPEID::TERRAN_HELLIONTANK,
        UNIT_TYPEID::TERRAN_LIBERATOR,
        UNIT_TYPEID::TERRAN_MARAUDER,
        UNIT_TYPEID::TERRAN_MARINE,
        UNIT_TYPEID::TERRAN_MEDIVAC,
        UNIT_TYPEID::TERRAN_MULE,
        UNIT_TYPEID::TERRAN_NUKE,
        UNIT_TYPEID::TERRAN_RAVEN,
        UNIT_TYPEID::TERRAN_REAPER,
        UNIT_TYPEID::TERRAN_SCV,
        UNIT_TYPEID::TERRAN_SIEGETANK,
        UNIT_TYPEID::TERRAN_THOR,
        UNIT_TYPEID::TERRAN_THORAP,
        UNIT_TYPEID::TERRAN_VIKINGFIGHTER,
        UNIT_TYPEID::TERRAN_WIDOWMINE,
        UNIT_TYPEID::ZERG_BANELING,
        UNIT_TYPEID::ZERG_BROODLING,
        UNIT_TYPEID::ZERG_BROODLORD,
        UNIT_TYPEID::ZERG_CHANGELING,
        UNIT_TYPEID::ZERG_CORRUPTOR,
        UNIT_TYPEID::ZERG_DRONE,
        UNIT_TYPEID::ZERG_HYDRALISK,
        UNIT_TYPEID::ZERG_INFESTOR,
        UNIT_TYPEID::ZERG_INFESTORTERRAN,
        UNIT_TYPEID::ZERG_LOCUSTMP,
        UNIT_TYPEID::ZERG_LURKERMP,
        UNIT_TYPEID::ZERG_MUTALISK,
        UNIT_TYPEID::ZERG_OVERLORD,
        UNIT_TYPEID::ZERG_OVERLORDTRANSPORT,
        UNIT_TYPEID::ZERG_OVERSEER,
        UNIT_TYPEID::ZERG_QUEEN,
        UNIT_TYPEID::ZERG_RAVAGER,
        UNIT_TYPEID::ZERG_ROACH,
        UNIT_TYPEID::ZERG_SWARMHOSTMP,
        UNIT_TYPEID::ZERG_ULTRALISK,
        UNIT_TYPEID::ZERG_VIPER,
        UNIT_TYPEID::ZERG_ZERGLING
    };
    return list;
}