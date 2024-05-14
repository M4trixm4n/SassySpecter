#include "UnitTools.h"
#include "UnitType.h"

Battalion::Battalion (sc2::UNIT_TYPEID type, int count, SassySpecterBot *bot)
    : unitType(type)
    , count(count)
    , m_bot(bot)
{}

double Battalion::battalionDPS () const {
    auto unit = m_bot->Observation()->GetUnitTypeData()[static_cast<int>(unitType)];
    auto weapon = unit.weapons[0];
    double dps = weapon.damage_ * weapon.attacks * (1 / weapon.speed); // if damage applied to another battalion, check for bonus damage and if type is compatible (eg. ground weapon can't attack flying units)
    return dps;
}

void Battalion::spawnBattalion (sc2::Point2D pos, int playerID) const {
    m_bot->Debug()->DebugCreateUnit(unitType, pos, playerID, count);
}

void Battalion::Log () const {
    std::cout << "Battalion containing " << count << " " << sc2::UnitTypeToName(unitType) << std::endl;
}

Army::Army () {
    
}

Army::Army(std::vector<Battalion> battalions)
    : battalions(battalions)
{}

double Army::armyDPS () const {
    double dps{0};
    for (auto & bat : battalions) {
        dps += bat.battalionDPS();
    }
    return dps;
}

double Army::armyDPS (std::vector<float> coef) const { // 3rd
    double dps{0};
    for (int i = 0; i < battalions.size(); i++) {
        dps += (battalions[i].battalionDPS() * coef[i]);
    }
    return dps;
}

bool Army::winsAgainst (Army &enemy) const {
    return winsAgainst(&enemy);
}

std::vector<float> Army::getDPSCoef (const Army *enemy) const {
    std::vector<float> coef;
    for (auto &bat : battalions) {
        coef.push_back(1);
    }
    for (int i = 0; i < battalions.size(); i++) {
        coef[i] *= getCoefModifier(battalions[i], enemy);
    }
    return coef;
}

/* 
 * To implement : 
 * floating coefficient for a battalion if it cannot attack every enemy, depending on the value of the enemy (6)
 * 1st parameter : if the whole army can't fight against against any of the enemies, lose
 * 2nd parameter : if a battalion cannot fight any of the enemy troops, their dps isn't taken into account
 * 3rd parameter : dps is calculated using the coefficient of each troop (only 2nd param modifies the coef for now)
 * 4th parameter : a battalion's coef is reduced when it cannot target every enemy unit, depending on the number of units (for now);
 * 5th parameter : bonuses of a battalion against an army are taken into account individually, then used to inflate dps number
 * 
 */
bool Army::winsAgainst (const Army *enemy) const {
    if (!canFight(enemy)) return false; //1st
    return enemy->totalHealth()/armyDPS(getDPSCoef(enemy)) < totalHealth()/enemy->armyDPS(enemy->getDPSCoef(this));
}

void Army::spawnArmy (sc2::Point2D pos, int pID) {
    playerID = pID;
    for (auto &bat : battalions) {
        bat.spawnBattalion(pos, pID);
    }
}

void Army::Log () const {
    std::cout << "Army containing " << battalions.size() << " Battalion(s) :\n";
    for (auto &bat : battalions) {
        std::cout << "\t* ";
        bat.Log();
    }
}

bool Army::contains (sc2::UNIT_TYPEID unit) const {
    for (auto &bat : battalions) {
        if (bat.unitType == unit) return true;
    }
    return false;
}

void Army::addBattalion (Battalion bat) {
    if (contains(bat.unitType)) {
        std::cout << "An army cannot contain 2 Battalions with the same unit\n";
        return;
    }
    battalions.push_back(bat);
}

void Battalion::decrementUnit () {
    count--;
}

void Army::deadUnit (const sc2::Unit *unit) {
    for (int i = 0; i < battalions.size(); i++) {
        if (battalions[i].unitType == unit->unit_type) {
            battalions[i].decrementUnit();
            if (battalions[i].isEmpty()) {
                battalions.erase(battalions.begin() + i);
                i--;
            }
        }
    }
}

bool Battalion::isEmpty () const {
    return !count;
}

bool Army::isEmpty () const {
    for (auto &bat : battalions) {
        if (!bat.isEmpty()) return false;
    }
    return true;
}

bool Army::canFight (const Army *enemy) const {
    if (isEmpty()) return false;
    if (enemy->isEmpty()) return true;
    std::vector<bool> targetableEnemyBattalions{};
    for (auto &bat : enemy->battalions) {
        targetableEnemyBattalions.push_back(false);
    }
    for (auto &bat : battalions) {
        for (int i = 0; i < enemy->battalions.size(); i++) {
            if (targetableEnemyBattalions[i]) continue;
            if (bat.canTarget(enemy->battalions[i])) targetableEnemyBattalions[i] = true;
        }
    }
    for (auto &b : targetableEnemyBattalions) {
        if (!b) return false;
    }
    return true;
}

bool Battalion::canTarget (const Battalion &bat) const{
    const sc2::UnitTypes types{m_bot->Observation()->GetUnitTypeData()};
    bool flyingEnemy = UnitType(bat.unitType, *m_bot).isAirUnit();
    for (auto &weapon : types[static_cast<sc2::UnitTypeID>(unitType)].weapons) {
        if (weapon.type == sc2::Weapon::TargetType::Any) return true;
        if (weapon.type == sc2::Weapon::TargetType::Air && flyingEnemy) return true;
        if (weapon.type == sc2::Weapon::TargetType::Ground && !flyingEnemy) return true;
    }
    return false;
}

float Army::getCoefModifier (Battalion bat, const Army *enemy) const { //4th
    std::vector<float> enemyCoef;
    for (int i = 0; i < enemy->battalions.size(); i++) {
        enemyCoef.push_back(1);
        if (!bat.canTarget(enemy->battalions[i])) enemyCoef[i] = 0;
        else enemyCoef[i] *= bat.bonusCoef(enemy->battalions[i]);
    }
    float result = 0;
    for (int i = 0; i < enemy->battalions.size(); i++) {
        result += enemyCoef[i];
    }
    return result / static_cast<float>(enemy->battalions.size());
}

float Battalion::bonusCoef (Battalion enemy) const { // 5th
    float coef = 1;
    auto weapons = m_bot->Observation()->GetUnitTypeData()[static_cast<int>(unitType)].weapons;
    auto enemyAttributes = m_bot->Observation()->GetUnitTypeData()[static_cast<int>(enemy.unitType)].attributes;
    for (int i = 0; i < enemyAttributes.size(); i++) {
        for (int j = 0; j < weapons.size(); j++) {
            for (int k = 0; k < weapons[j].damage_bonus.size(); k++) {
                if (enemyAttributes[i] == weapons[j].damage_bonus[k].attribute) {
                    coef += (weapons[j].damage_bonus[k].bonus / weapons[j].damage_);
                }
            }
        }
    }
    return coef;
}

float Battalion::totalHealth () const {
    return (health + shield) * count;
}

float Army::totalHealth () const {
    float result = 0;
    for (auto &bat : battalions) {
        result += bat.totalHealth();
    }
    return result;
}

sc2::Race Army::getRace () const {
    if (battalions.empty()) return sc2::Race::Random;
    return battalions[0].getRace();
}

sc2::Race Battalion::getRace () const {
    return m_bot->Observation()->GetUnitTypeData()[static_cast<int>(unitType)].race;
}

void Army::fillBattalionStats (int pID, float health, float shield, sc2::UNIT_TYPEID unit) {
    if (pID != playerID) return;
    if (!contains(unit)) return;
    for (auto &bat : battalions) {
        if (unit == bat.unitType) {
            bat.health = health;
            bat.shield = shield;
        }
    }
}