#include "TechTree.h"
#include "Util.h"
#include "SassySpecterBot.h"
#include "MetaType.h"
#include "Timer.hpp"

TechTree::TechTree(SassySpecterBot & bot)
    : m_bot(bot)
{

}

void TechTree::onStart()
{
    initUnitTypeData();
    initUpgradeData();
    outputJSON("TechTree.json");
}


#ifdef SC2API
void TechTree::initUnitTypeData()
{
    const sc2::UnitTypes types{m_bot.Observation()->GetUnitTypeData()};
    for (auto i = 1; i < 2000; i++) {
        auto tmp = static_cast<sc2::UNIT_TYPEID>(i);
        auto ut = UnitType(tmp, m_bot);
        if (ut.getName().rfind("ZERG_", 0) && ut.getName().rfind("PROTOSS_", 0) && ut.getName().rfind("TERRAN_", 0) && ut.getName().rfind("NEUTRAL_", 0)) continue;
        auto unit{types[ut.getAPIUnitType()]};
        if (ut.equivalentBuilding()) continue;
        m_unitTypeData[ut] = {
            unit.race,
            static_cast<int>(unit.mineral_cost), 
            static_cast<int>(unit.vespene_cost), 
            static_cast<int>(unit.food_required), 
            static_cast<int>(unit.build_time),
            ut.isCombatUnit(), 
            ut.isBuilding(), 
            ut.isWorker(), 
            ut.isRefinery(), 
            ut.isSupplyProvider(), 
            ut.isResourceDepot(), 
            ut.isAddon(),
            unit.ability_id, 
            ut.warpAbility(),
            ut.whatBuilds(),
            {UnitType(unit.tech_requirement, m_bot)},
            {}  //TODO
        };
        m_unitFromAbility[unit.ability_id] = unit.unit_type_id;
    }
    m_unitTypeData[UnitType(0, m_bot)] = TypeData();

    // Terran Equivalent Buildings
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSFLYING, m_bot)] =           m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKS, m_bot)];
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTERFLYING, m_bot)] =      m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, m_bot)];
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORYFLYING, m_bot)] =            m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_FACTORY, m_bot)];
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORTFLYING, m_bot)] =           m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORT, m_bot)];
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOTLOWERED, m_bot)] =       m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_SUPPLYDEPOT, m_bot)];

    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMANDFLYING, m_bot)] =     m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND, m_bot)];

    // fix the cumulative prices of morphed buildings
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_bot)].mineralCost -= getData(UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_bot)).mineralCost;
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_bot)].mineralCost -= getData(UnitType(sc2::UNIT_TYPEID::ZERG_HATCHERY, m_bot)).mineralCost;
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_PLANETARYFORTRESS, m_bot)].mineralCost -= getData(UnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, m_bot)).mineralCost;
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_ORBITALCOMMAND, m_bot)].mineralCost -= getData(UnitType(sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER, m_bot)).mineralCost;
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::ZERG_GREATERSPIRE, m_bot)].mineralCost -= getData(UnitType(sc2::UNIT_TYPEID::ZERG_SPIRE, m_bot)).mineralCost;

    //fix mistakes
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::TERRAN_PREVIEWBUNKERUPGRADED, m_bot)].isUnit = false;
    m_unitTypeData[UnitType(sc2::UNIT_TYPEID::PROTOSS_MOTHERSHIPCORE, m_bot)].isUnit = false;
}

void TechTree::initUpgradeData()
{
    // 0 data for null / error return
    m_upgradeData[0] = TypeData();

    // Terran Upgrades
    m_upgradeData[sc2::UPGRADE_ID::BANSHEECLOAK] =                      { sc2::Race::Terran, 100, 100, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_BANSHEECLOAKINGFIELD, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::BANSHEESPEED] =                      { sc2::Race::Terran, 200, 200, 0, 2720, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_BANSHEEHYPERFLIGHTROTORS, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::BATTLECRUISERENABLESPECIALIZATIONS]= { sc2::Race::Terran, 150, 150, 0,  960, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_BATTLECRUISERWEAPONREFIT, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::DRILLCLAWS] =                        { sc2::Race::Terran, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_DRILLINGCLAWS, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::HIGHCAPACITYBARRELS] =               { sc2::Race::Terran, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_INFERNALPREIGNITER, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::HISECAUTOTRACKING] =                 { sc2::Race::Terran, 100, 100, 0, 1280, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_HISECAUTOTRACKING, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::LIBERATORAGRANGEUPGRADE] =           { sc2::Race::Terran, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ADVANCEDBALLISTICS, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_STARPORTTECHLAB, m_bot) }, {}, {} };
    // Removed upgrade
    //m_upgradeData[sc2::UPGRADE_ID::MAGFIELDLAUNCHERS] =                 { sc2::Race::Terran, 100, 100, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_MAGFIELDLAUNCHERS, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::MEDIVACINCREASESPEEDBOOST] =         { sc2::Race::Terran, 100, 100, 0, 1280, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_HIGHCAPACITYFUELTANKS, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::NEOSTEELFRAME] =                     { sc2::Race::Terran, 100, 100, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_NEOSTEELFRAME, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::PERSONALCLOAKING] =                  { sc2::Race::Terran, 150, 150, 0, 1920, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PERSONALCLOAKING, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_GHOSTACADEMY, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::PUNISHERGRENADES] =                  { sc2::Race::Terran,  50,  50, 0,  960, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_CONCUSSIVESHELLS, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::RAVENCORVIDREACTOR] =                { sc2::Race::Terran, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_RAVENCORVIDREACTOR, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::RAVENRECALIBRATEDEXPLOSIVES] =       { sc2::Race::Terran, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_RAVENRECALIBRATEDEXPLOSIVES, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::SHIELDWALL] =                        { sc2::Race::Terran, 100, 100, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_COMBATSHIELD, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_BARRACKSTECHLAB, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::STIMPACK] =                          { sc2::Race::Terran, 100, 100, 0, 2720, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_STIMPACK, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_SCV, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANBUILDINGARMOR] =               { sc2::Race::Terran, 150, 150, 0, 2240, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANSTRUCTUREARMORUPGRADE, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL1] =        { sc2::Race::Terran, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL2] =        { sc2::Race::Terran, 175, 175, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_bot) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_bot) }, {sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL3] =        { sc2::Race::Terran, 250, 250, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYARMORLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_bot) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_bot) }, {sc2::UPGRADE_ID::TERRANINFANTRYARMORSLEVEL2} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL1] =       { sc2::Race::Terran, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL2] =       { sc2::Race::Terran, 175, 175, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_bot) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_bot) }, {sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL3] =       { sc2::Race::Terran, 250, 250, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, m_bot) }, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_bot) }, {sc2::UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL2} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL1] =           { sc2::Race::Terran, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL2] =           { sc2::Race::Terran, 175, 175, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_bot) }, {}, {sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL3] =           { sc2::Race::Terran, 250, 250, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANSHIPWEAPONSLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_bot) }, {}, {sc2::UPGRADE_ID::TERRANSHIPWEAPONSLEVEL2} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL1] =  { sc2::Race::Terran, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL2] =  { sc2::Race::Terran, 175, 175, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_bot) }, {}, {sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL3] =  { sc2::Race::Terran, 250, 250, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEANDSHIPPLATINGLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_bot) }, {}, {sc2::UPGRADE_ID::TERRANVEHICLEANDSHIPARMORSLEVEL2} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL1] =        { sc2::Race::Terran, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL2] =        { sc2::Race::Terran, 175, 175, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_bot) }, {}, {sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL3] =        { sc2::Race::Terran, 250, 250, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TERRANVEHICLEWEAPONSLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::TERRAN_ARMORY, m_bot) }, {}, {sc2::UPGRADE_ID::TERRANVEHICLEWEAPONSLEVEL2} };

    // Protoss Upgrades
    m_upgradeData[sc2::UPGRADE_ID::ADEPTPIERCINGATTACK] =               { sc2::Race::Protoss, 100, 100, 0, 2240, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ADEPTRESONATINGGLAIVES, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::BLINKTECH] =                         { sc2::Race::Protoss, 150, 150, 0, 2720, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_BLINK, 0,                   { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::CARRIERLAUNCHSPEEDUPGRADE] =         { sc2::Race::Protoss, 150, 150, 0, 1280, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_INTERCEPTORGRAVITONCATAPULT, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::CHARGE] =                            { sc2::Race::Protoss, 100, 100, 0, 2240, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_CHARGE, 0,                  { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::DARKTEMPLARBLINKUPGRADE] =           { sc2::Race::Protoss, 100, 100, 0, 2720, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_SHADOWSTRIKE, 0,            { UnitType(sc2::UNIT_TYPEID::PROTOSS_DARKSHRINE, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::EXTENDEDTHERMALLANCE] =              { sc2::Race::Protoss, 200, 200, 0, 2240, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_EXTENDEDTHERMALLANCE, 0,    { UnitType(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSBAY, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::GRAVITICDRIVE] =                     { sc2::Race::Protoss, 100, 100, 0, 1280, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_GRAVITICDRIVE, 0,           { UnitType(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSBAY, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::OBSERVERGRAVITICBOOSTER] =           { sc2::Race::Protoss, 100, 100, 0, 1280, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_GRAVITICBOOSTER, 0,         { UnitType(sc2::UNIT_TYPEID::PROTOSS_ROBOTICSBAY, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::PHOENIXRANGEUPGRADE] =               { sc2::Race::Protoss, 150, 150, 0, 1440, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PHOENIXANIONPULSECRYSTALS, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL1] =            { sc2::Race::Protoss, 150, 150, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSAIRARMORLEVEL1, 0,   { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL2] =            { sc2::Race::Protoss, 225, 225, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSAIRARMORLEVEL2, 0,   { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_bot) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, m_bot) }, {sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL3] =            { sc2::Race::Protoss, 300, 300, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSAIRARMORLEVEL3, 0,   { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_bot) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, m_bot) }, {sc2::UPGRADE_ID::PROTOSSAIRARMORSLEVEL2} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1] =           { sc2::Race::Protoss, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL2] =           { sc2::Race::Protoss, 175, 175, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_bot) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, m_bot) }, {sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL3] =           { sc2::Race::Protoss, 250, 250, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_bot) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, m_bot) }, {sc2::UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL3} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL1] =         { sc2::Race::Protoss, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDARMORLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL2] =         { sc2::Race::Protoss, 150, 150, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDARMORLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_bot) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_bot) }, { sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL1 } };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL3] =         { sc2::Race::Protoss, 200, 200, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDARMORLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_bot) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_bot) }, { sc2::UPGRADE_ID::PROTOSSGROUNDARMORSLEVEL2 } };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1] =        { sc2::Race::Protoss, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2] =        { sc2::Race::Protoss, 150, 150, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_bot) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_bot) }, { sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1 } };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL3] =        { sc2::Race::Protoss, 200, 200, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_bot) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_bot) }, { sc2::UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2 } };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL1] =              { sc2::Race::Protoss, 150, 150, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSSHIELDSLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL2] =              { sc2::Race::Protoss, 225, 225, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSSHIELDSLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_bot) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_bot) }, { sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL1 } };
    m_upgradeData[sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL3] =              { sc2::Race::Protoss, 300, 300, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PROTOSSSHIELDSLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_FORGE, m_bot) }, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, m_bot) }, { sc2::UPGRADE_ID::PROTOSSSHIELDSLEVEL2} };
    m_upgradeData[sc2::UPGRADE_ID::PSISTORMTECH] =                      { sc2::Race::Protoss, 200, 200, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PSISTORM, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::WARPGATERESEARCH] =                  { sc2::Race::Protoss,  50,  50, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_WARPGATE, 0, { UnitType(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, m_bot) }, {}, {} };

    // Zerg Upgrades
    m_upgradeData[sc2::UPGRADE_ID::BURROW] =                            { sc2::Race::Zerg, 100, 100, 0, 1600, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_BURROW, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_HATCHERY, m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::CENTRIFICALHOOKS] =                  { sc2::Race::Zerg, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_CENTRIFUGALHOOKS, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_BANELINGNEST, m_bot) }, {}, {} }; 
    m_upgradeData[sc2::UPGRADE_ID::CHITINOUSPLATING] =                  { sc2::Race::Zerg, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_CHITINOUSPLATING, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_ULTRALISKCAVERN, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::EVOLVEGROOVEDSPINES] =               { sc2::Race::Zerg, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_GROOVEDSPINES, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_HYDRALISKDEN, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::EVOLVEMUSCULARAUGMENTS] =            { sc2::Race::Zerg, 150, 150, 0, 1600, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_MUSCULARAUGMENTS, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_HYDRALISKDEN, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::GLIALRECONSTITUTION] =               { sc2::Race::Zerg, 100, 100, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_GLIALREGENERATION, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_ROACHWARREN, m_bot) }, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_bot) }, {} };
    m_upgradeData[sc2::UPGRADE_ID::INFESTORENERGYUPGRADE] =             { sc2::Race::Zerg, 150, 150, 0, 1280, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PATHOGENGLANDS, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_INFESTATIONPIT, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::NEURALPARASITE] =                    { sc2::Race::Zerg, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_NEURALPARASITE, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_INFESTATIONPIT, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::OVERLORDSPEED] =                     { sc2::Race::Zerg, 100, 100, 0,  960, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_PNEUMATIZEDCARAPACE, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_HATCHERY, m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_bot) }, {}, {} };                       
    m_upgradeData[sc2::UPGRADE_ID::TUNNELINGCLAWS] =                    { sc2::Race::Zerg, 150, 150, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_TUNNELINGCLAWS, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_ROACHWARREN, m_bot) }, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_bot) }, {} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGFLYERARMORSLEVEL1] =             { sc2::Race::Zerg, 150, 150, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGFLYERARMORLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_SPIRE, m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_GREATERSPIRE, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGFLYERARMORSLEVEL2] =             { sc2::Race::Zerg, 225, 225, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGFLYERARMORLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_SPIRE, m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_GREATERSPIRE, m_bot) }, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_bot) }, { sc2::UPGRADE_ID::ZERGFLYERARMORSLEVEL1 } };
    m_upgradeData[sc2::UPGRADE_ID::ZERGFLYERARMORSLEVEL3] =             { sc2::Race::Zerg, 300, 300, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGFLYERARMORLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_SPIRE, m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_GREATERSPIRE, m_bot) }, { UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_bot) }, { sc2::UPGRADE_ID::ZERGFLYERARMORSLEVEL2 } };
    m_upgradeData[sc2::UPGRADE_ID::ZERGFLYERWEAPONSLEVEL1] =            { sc2::Race::Zerg, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGFLYERATTACKLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_SPIRE, m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_GREATERSPIRE, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGFLYERWEAPONSLEVEL2] =            { sc2::Race::Zerg, 175, 175, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGFLYERATTACKLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_SPIRE, m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_GREATERSPIRE, m_bot) }, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_bot) }, { sc2::UPGRADE_ID::ZERGFLYERWEAPONSLEVEL1 } };
    m_upgradeData[sc2::UPGRADE_ID::ZERGFLYERWEAPONSLEVEL3] =            { sc2::Race::Zerg, 250, 250, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGFLYERATTACKLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_SPIRE, m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_GREATERSPIRE, m_bot) }, {UnitType( sc2::UNIT_TYPEID::ZERG_HIVE, m_bot) }, { sc2::UPGRADE_ID::ZERGFLYERWEAPONSLEVEL2 } };
    m_upgradeData[sc2::UPGRADE_ID::ZERGGROUNDARMORSLEVEL1] =            { sc2::Race::Zerg, 150, 150, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGGROUNDARMORLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGGROUNDARMORSLEVEL2] =            { sc2::Race::Zerg, 225, 225, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGGROUNDARMORLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_bot) }, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_bot) }, {sc2::UPGRADE_ID::ZERGGROUNDARMORSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGGROUNDARMORSLEVEL3] =            { sc2::Race::Zerg, 300, 300, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGGROUNDARMORLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_bot) }, { UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_bot) }, {sc2::UPGRADE_ID::ZERGGROUNDARMORSLEVEL2} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGLINGATTACKSPEED] =               { sc2::Race::Zerg, 200, 200, 0, 2080, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGLINGADRENALGLANDS, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL, m_bot) }, { UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_bot) }, {} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGLINGMOVEMENTSPEED] =             { sc2::Race::Zerg, 100, 100, 0, 1760, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGLINGMETABOLICBOOST, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGMELEEWEAPONSLEVEL1] =            { sc2::Race::Zerg, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGMELEEWEAPONSLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGMELEEWEAPONSLEVEL2] =            { sc2::Race::Zerg, 150, 150, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGMELEEWEAPONSLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_bot) }, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_bot) }, {sc2::UPGRADE_ID::ZERGMELEEWEAPONSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGMELEEWEAPONSLEVEL3] =            { sc2::Race::Zerg, 200, 200, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGMELEEWEAPONSLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_bot) }, { UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_bot) }, {sc2::UPGRADE_ID::ZERGMELEEWEAPONSLEVEL2} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL1] =          { sc2::Race::Zerg, 100, 100, 0, 2560, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGMISSILEWEAPONSLEVEL1, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_bot) }, {}, {} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL2] =          { sc2::Race::Zerg, 150, 150, 0, 3040, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGMISSILEWEAPONSLEVEL2, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_bot) }, { UnitType(sc2::UNIT_TYPEID::ZERG_LAIR, m_bot), UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_bot) }, {sc2::UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL1} };
    m_upgradeData[sc2::UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL3] =          { sc2::Race::Zerg, 200, 200, 0, 3520, false, false, false, false, false, false, false, sc2::ABILITY_ID::RESEARCH_ZERGMISSILEWEAPONSLEVEL3, 0, { UnitType(sc2::UNIT_TYPEID::ZERG_EVOLUTIONCHAMBER, m_bot) }, { UnitType(sc2::UNIT_TYPEID::ZERG_HIVE, m_bot) }, {sc2::UPGRADE_ID::ZERGMISSILEWEAPONSLEVEL2} };
}
#else
void TechTree::initUpgradeData()
{
    
    m_unitTypeData[UnitType(BWAPI::UnitTypes::None, m_bot)] = TypeData();

    for (auto & type : BWAPI::UnitTypes::allUnitTypes())
    {
        TypeData typeData;

        typeData.race = type.getRace();
        typeData.mineralCost = type.mineralPrice();
        typeData.gasCost = type.gasPrice();
        typeData.supplyCost = type.supplyRequired();
        typeData.buildTime = type.buildTime();
        typeData.isUnit = true;
        typeData.isBuilding = type.isBuilding();
        typeData.isWorker = type.isWorker();
        typeData.isRefinery = type.isRefinery();
        typeData.isSupplyProvider = type.supplyProvided() > 0 && !type.isResourceDepot();
        typeData.isResourceDepot = type.isResourceDepot();
        typeData.isAddon = type.isAddon();

        std::vector<UnitType> whatBuilds;
        whatBuilds.push_back(UnitType(type.whatBuilds().first, m_bot));
        typeData.whatBuilds = whatBuilds;

        std::vector<UnitType> requiredUnits;
        for (auto & req : type.requiredUnits())
        {
            requiredUnits.push_back(UnitType(req.first, m_bot));
        }
        typeData.requiredUnits = requiredUnits;

        m_unitTypeData[UnitType(type, m_bot)] = typeData;
    }
}

void TechTree::initUnitTypeData()
{

}
#endif

const TypeData & TechTree::getData(const UnitType & type) const
{
    if (m_unitTypeData.find(type) == m_unitTypeData.end())
    {
        std::cout << "WARNING: Unit type not found: " << type.getName() << "\n";
        return m_unitTypeData.begin()->second;
    }

    return m_unitTypeData.at(type);
}

const TypeData & TechTree::getData(const CCUpgrade & type)  const
{
    if (m_upgradeData.find(type) == m_upgradeData.end())
    {
        //std::cout << "WARNING: Upgrade not found: " << sc2::UpgradeIDToName(type) << "\n";
        return m_unitTypeData.begin()->second;
    }

    return m_upgradeData.at(type);
}

const TypeData & TechTree::getData(const MetaType & type) const
{
    if (type.getMetaType() == MetaTypes::Unit)
    {
        return getData(type.getUnitType());
    }
    else if (type.getMetaType() == MetaTypes::Upgrade)
    {
        return getData(type.getUpgrade());
    }
    
    BOT_ASSERT(false, "Can't getData this type: %s", type.getName().c_str());

    return m_unitTypeData.begin()->second;
}

const sc2::UNIT_TYPEID TechTree::getUnit (sc2::ABILITY_ID id) const {
    return m_unitFromAbility.at(id);
}

const std::map<UnitType, TypeData> & TechTree::getAllUnits () const {
    return m_unitTypeData;
}

void TechTree::outputJSON(const std::string & filename) const
{
#ifdef SC2APIXXX
    std::ofstream out(filename);
    out << "{\n";
    std::string q = "\"";
    std::string qcs = "\", ";

    std::vector<std::pair<int, UnitTypeData>> allData;
    for (auto & kv : m_unitTypeData) { allData.push_back({kv.first, kv.second}); }
    for (auto & kv : m_upgradeData) { allData.push_back({kv.first, kv.second}); }

    for (auto & kv : allData)
    {
        std::string name( kv.second.isUnit ? m_bot.Observation()->GetUnitTypeData()[kv.first].name : m_bot.Observation()->GetUpgradeData()[kv.first].name);
        if (name.find("Flying") != std::string::npos) { continue; }
        if (name.find("Lowered") != std::string::npos) { continue; }

        auto & data = kv.second;
               
        out << "    [";
        out << q << name << qcs;
        out << q << Util::GetStringFromRace(data.race) << qcs;
        out << data.mineralCost << ", " << data.gasCost << ", " << data.supplyCost << ", " << data.buildTime << ", ";
        out << data.isUnit << ", " << data.isBuilding << ", " << data.isWorker<< ", " << data.isSupplyDepot << ", " << data.isTownHall << ", " << data.isAddon << ", ";

        out << "[";
        for (size_t i(0); i < data.whatBuilds.size(); ++i) 
        { 
            out << q << m_bot.Observation()->GetUnitTypeData()[data.whatBuilds[i].getAPIUnitType()].name << q;
            if (i < data.whatBuilds.size() - 1) { out << ", "; }
        }
        out << "], ";

        out << "[";
        for (size_t i(0); i < data.requiredUnits.size(); ++i) 
        { 
            out << q << m_bot.Observation()->GetUnitTypeData()[data.requiredUnits[i].getAPIUnitType()].name << q;
            if (i < data.requiredUnits.size() - 1) { out << ", "; }
        }
        out << "], ";

        out << "[";
        for (size_t i(0); i < data.requiredUpgrades.size(); ++i) 
        { 
            out << q << m_bot.Observation()->GetUpgradeData()[data.requiredUpgrades[i]].name << q;
            if (i < data.requiredUpgrades.size() - 1) { out << ", "; }
        }
        out << "] ";

        out << "],\n";
    }

    out << "}";
    out.close();
#else

#endif
}
