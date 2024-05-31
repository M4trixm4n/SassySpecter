#include "ObjectiveFunction.h"

double BestAdditions::required_cost (const vector<Variable*> &variables) const {
    Army *additions = new Army();
    for (auto &var : variables) {
        additions->addBattalion(Battalion(static_cast<sc2::UNIT_TYPEID>(stoi(var->get_name())), var->get_value(), &m_bot));
    }
    Army *allyCopy = _ally->copyWithAdditions(additions);
    double allyScore = allyCopy->armyScore(_enemy);
    double enemyScore = _enemy->armyScore(allyCopy);
    return allyScore - enemyScore;
}

BestAdditions::BestAdditions(const vector<Variable> &variables, 
                             Army *ally, 
                             Army *enemy,
                             SassySpecterBot &bot) :
    Maximize(variables, "Best additions"),
    _ally(ally),
    _enemy(enemy),
    m_bot(bot) {}
