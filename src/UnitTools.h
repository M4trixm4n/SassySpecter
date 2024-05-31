#pragma once

#include "SassySpecterBot.h"

struct Battalion { //part of an army, contains every unit of a single type
private:

    SassySpecterBot *m_bot;
    int count;

public:

    sc2::UNIT_TYPEID unitType;
    float health;
    float shield;

    Battalion(sc2::UNIT_TYPEID type, int count, SassySpecterBot *bot);

    void Log () const;
    double battalionDPS () const;
    void spawnBattalion (sc2::Point2D pos, int playerID) const;
    void decrementUnit ();
    void incrementUnit ();
    void incrementUnit (int inc);
    bool isEmpty () const;
    bool canTarget (const Battalion &bat) const;
    float bonusCoef (Battalion enemy) const;
    float totalHealth () const;
    sc2::Race getRace () const;
    int getCount () const;
    int mineralCost () const;
    int vespeneCost () const;
    int foodCost () const;
};

struct Army {
private:
    std::vector<Battalion> battalions; //1 battalion per unit type

    double armyDPS () const;
    double armyDPS (std::vector<float> coef) const;
    std::vector<float> getDPSCoef (const Army *enemy) const;
    float getCoefModifier (Battalion bat, const Army *enemy) const;

public:

    int playerID{0};

    Army();
    Army(std::vector<Battalion> battalions);

    void Log () const;
    void spawnArmy (sc2::Point2D pos, int pID);
    void addBattalion (Battalion bat);
    bool contains (sc2::UNIT_TYPEID unit) const;
    void deadUnit (const sc2::Unit *unit);
    void deadUnit (const sc2::UNIT_TYPEID unit);
    void addUnit (const sc2::Unit *unit, SassySpecterBot &bot);
    void addUnit (const sc2::UNIT_TYPEID unit, SassySpecterBot &bot);
    bool isEmpty () const;
    bool winsAgainst (Army &enemy) const;
    bool winsAgainst (const Army *enemy) const;
    bool canFight (const Army *enemy) const;
    sc2::Race getRace () const;
    void fillBattalionStats (int pID, float health, float shield, sc2::UNIT_TYPEID unit);
    float totalHealth () const;
    int size () const;
    int mineralCost () const;
    int vespeneCost () const;
    int foodCost () const;
    double armyScore (const Army *enemy) const;
    Army *copyWithAdditions (Army *additions) const;
    Army *copy () const;
};
