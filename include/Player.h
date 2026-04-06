#pragma once
#include <string>
#include <vector>
#include "Item.h"
#include "Potion.h"
#include "Stance.h"

struct CombatBuffs {
    int  shieldTonicTurns      = 0;     // +5 defense for N turns
    bool rageDraughtActive     = false; // next attack x1.5
    int  lockedPotionIndex     = -1;    // Soul Binder effect (-1 = none)
    // Status effects applied to the player by enemies
    bool bleeding              = false; // Ash Hound — 2 dmg/turn, permanent
    int  poisonStacks          = 0;     // Plague Shaman — X dmg/turn, decays by 1
    int  blindRounds           = 0;     // Ember Witch — player misses 30%, 3 rounds
    // Town of Dead Ballads potion effects
    bool phantomsDraughtActive = false; // next attack ignores enemy defense
    int  gravewardenOilCharges = 0;     // next N attacks apply Bleed guaranteed
};

class Player {
private:
    std::string name;
    int         level;
    int         experience;
    int         gold;
    int         health;
    int         maxHealth;
    int         stamina;
    int         maxStamina;
    int         defense;
    int         luck;
    Item        weapon;
    Item        armor;
    std::vector<Potion> potions; // usable in combat, free action
    int         experienceToNextLevel;
    CombatBuffs buffs;

    // Stance system
    std::vector<StanceType> learnedStances; // pool built over the run
    StanceType              activeStance;
    bool                    stanceActive;   // false only on first init before picking

public:
    Player();
    Player(const std::string& playerName);

    // Getters
    std::string              getName()      const;
    int                      getLevel()     const;
    int                      getExperience()const;
    int                      getGold()      const;
    int                      getHealth()    const;
    int                      getMaxHealth() const;
    int                      getStamina()   const;
    int                      getMaxStamina()const;
    int                      getDefense()   const;
    int                      getLuck()      const;
    const Item&              getWeapon()    const;
          Item&              getWeapon();
    const Item&              getArmor()     const;
          Item&              getArmor();
    const std::vector<Potion>& getPotions() const;
          std::vector<Potion>& getPotions();
    const CombatBuffs&       getBuffs()     const;
          CombatBuffs&       getBuffs();

    // Stat mutators
    void addMaxHealth(int amount);
    void addMaxStamina(int amount);
    void addDefense(int amount);
    void addLuck(int amount);
    void addGold(int amount);
    bool spendGold(int amount);

    // Combat
    void takeDamage(int dmg);
    void heal(int amount);
    void useStamina(int amount);
    void restoreStamina(int amount);
    int  rollDefense() const;
    bool isAlive()     const;

    // Timing-based attack (randomised crit position)
    // critPos is generated outside and passed in so UI can display it
    int  timingBasedAttack(int cursorPos, int critPos, int totalPositions,
                           bool isHeavy, bool& isCrit);

    // Potions (free action — no turn cost)
    bool usePotion(int index); // returns false if slot locked or empty
    void addPotion(const Potion& p);
    void lockPotionSlot(int index);
    void clearPotionLock();

    // Per-turn buff tick (call at start of player turn)
    void tickBuffs();

    // Stance system
    void                           learnStance(StanceType s);
    bool                           hasLearnedStance(StanceType s) const;
    const std::vector<StanceType>& getLearnedStances() const;
    StanceType                     getActiveStance()   const;
    bool                           hasStanceActive()   const;
    bool                           switchStance(StanceType s); // true if in pool
    void                           resetStances();             // call on death/new run

    // Player status effect application (by enemies)
    void applyBleedToPlayer();
    void applyPoisonToPlayer(int stacks = 5);
    void applyBlindToPlayer(int rounds = 3);

    // Equipment
    void equipWeapon(const Item& w);
    void equipArmor(const Item& a);

    // Experience / levelling
    void addExperience(int xp);
    void levelUp();

    // Status display
    void showStatus()    const;
    void showInventory() const;
};
