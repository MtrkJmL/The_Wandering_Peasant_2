#pragma once
#include <string>
#include <vector>
#include "Item.h"

enum class BossType {
    DARKBLOOD_LIEUTENANT, // Ch1
    WITCH_OF_GHAAL,       // Ch2
    DARKBLOOD_LORD,       // Ch3
    SLAVE_KING_AMAX       // True final (Banner required)
};

struct BossPhase {
    int  healthThreshold; // switch to this phase when HP <= threshold
    int  attackBonus;
    std::string phaseMessage;
};

class Boss {
private:
    BossType    type;
    std::string name;
    int         health;
    int         maxHealth;
    int         phase;         // 0 = first phase
    std::vector<BossPhase> phases;
    Item        weapon;
    Item        armor;
    int         goldReward;
    int         experienceReward;

public:
    Boss(BossType type, int chapter);

    std::string getName()    const;
    BossType    getType()    const;
    int         getHealth()  const;
    int         getMaxHealth() const;
    int         getCurrentPhase() const;
    int         getGoldReward() const;
    int         getExperienceReward() const;

    void takeDamage(int dmg);
    bool isAlive() const;
    bool checkPhaseTransition(); // returns true if phase changed
    int  rollAttack() const;
    int  rollDefense() const;

    std::string getIntroText()  const;
    std::string getDefeatText() const;
    std::string getPhaseText()  const;
};
