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

    // Status effects
    int bleedStacks     = 0;
    int poisonStacks    = 0;
    int blindRounds     = 0;
    // Resistance: subtract this % from the player's proc chance
    int bleedResistPct  = 0;
    int poisonResistPct = 0;
    int blindResistPct  = 0;

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

    // Status effects
    void applyBleed();
    void applyPoison(int stacks = 3);
    void applyBlind(int rounds = 2);
    void clearBleed();
    void clearPoison();
    bool isBleeding()         const;
    bool isPoisoned()         const;
    bool isBlinded()          const;
    int  getPoisonStacks()    const;
    int  getBlindRounds()     const;
    int  tickBleed();          // returns 2 dmg (permanent)
    int  tickPoison();         // returns damage, decrements stacks
    void tickBlind();

    int  getBleedResistPct()  const;
    int  getPoisonResistPct() const;
    int  getBlindResistPct()  const;
};
