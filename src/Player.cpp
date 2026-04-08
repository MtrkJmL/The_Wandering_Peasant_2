#include "../include/Player.h"
#include <iostream>
#include <algorithm>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#undef min
#undef max
#endif

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"

Player::Player()
    : Player("Peasant") {}

Player::Player(const std::string& n)
    : name(n), level(1), experience(0), gold(50),
      health(100), maxHealth(100), stamina(5), maxStamina(5),
      defense(1), luck(0),
      weapon(Item::generateFlatItem(ItemType::WEAPON, ItemRarity::COMMON)),
      armor (Item::generateFlatItem(ItemType::ARMOR,  ItemRarity::COMMON)),
      experienceToNextLevel(100) {

    // Override generated names for starting gear
    weapon = Item::generateFlatItem(ItemType::WEAPON, ItemRarity::COMMON);
    armor  = Item::generateFlatItem(ItemType::ARMOR,  ItemRarity::COMMON);

    // Override weapon bonus to +2 and armor bonus to +5 (starting values)
    // We rebuild them manually since generateFlatItem picks random bonus from table
    // Common weapon = +3 by table, but design specifies +2 starting weapon
    // We create flat items directly: diceCount=0, modifier=bonus
    weapon = Item(ItemType::WEAPON, "Rusty Blade", 0, 1, 2);
    armor  = Item(ItemType::ARMOR,  "Peasant Clothes", 0, 1, 3);

    // Starting attack slots: 3 random Common skills
    for (int i = 0; i < 3; ++i) {
        attackSlots[i]     = randomAttackType(ItemRarity::COMMON);
        attackCooldowns[i] = 0;
    }

    // Starting runes: none learned yet
    runeSlots[0] = RuneType::NONE;
    runeSlots[1] = RuneType::NONE;
}

// ── Getters ──────────────────────────────────────────────────────────────────

std::string              Player::getName()       const { return name; }
int                      Player::getLevel()      const { return level; }
int                      Player::getExperience() const { return experience; }
int                      Player::getGold()       const { return gold; }
int                      Player::getHealth()     const { return health; }
int                      Player::getMaxHealth()  const { return maxHealth; }
int                      Player::getStamina()    const { return stamina; }
int                      Player::getMaxStamina() const { return maxStamina; }
int                      Player::getDefense()    const { return defense; }
int                      Player::getLuck()       const { return luck; }
const Item&              Player::getWeapon()     const { return weapon; }
      Item&              Player::getWeapon()           { return weapon; }
const Item&              Player::getArmor()      const { return armor; }
      Item&              Player::getArmor()            { return armor; }
const std::vector<Potion>& Player::getPotions()  const { return potions; }
      std::vector<Potion>& Player::getPotions()        { return potions; }
const CombatBuffs&       Player::getBuffs()      const { return buffs; }
      CombatBuffs&       Player::getBuffs()            { return buffs; }

// ── Stat mutators ────────────────────────────────────────────────────────────

void Player::addMaxHealth(int amt) {
    maxHealth += amt;
    if (maxHealth < 1) maxHealth = 1;
    if (amt > 0) health = maxHealth;
    else if (health > maxHealth) health = maxHealth;
}
void Player::addMaxStamina(int amt){ maxStamina += amt; stamina = maxStamina; }
void Player::addDefense(int amt)   { defense += amt; }
void Player::addLuck(int amt)      { luck += amt; }
void Player::addGold(int amt)      { gold += amt; }

bool Player::spendGold(int amt) {
    if (gold < amt) return false;
    gold -= amt;
    return true;
}

// ── Combat ───────────────────────────────────────────────────────────────────

void Player::takeDamage(int dmg) {
    health = std::max(0, health - dmg);
}

void Player::heal(int amt) {
    health = std::min(maxHealth, health + amt);
}

void Player::useStamina(int amt) {
    stamina = std::max(0, stamina - amt);
}

void Player::restoreStamina(int amt) {
    stamina = std::min(maxStamina, stamina + amt);
}

int Player::rollDefense() const {
    // armor.roll() returns modifier when diceCount=0 (flat bonus)
    int base = armor.roll() + defense + buffs.attackTypeDefenseThisTurn;
    if (buffs.shieldTonicTurns > 0) base += 5;
    return base;
}

bool Player::isAlive() const { return health > 0; }

// Timing-based attack. baseDamageRoll is pre-rolled from AttackType + weaponBonus.
int Player::timingBasedAttack(int cursorPos, int critPos, int totalPositions,
                              int baseDamageRoll, bool& isCrit) {
    isCrit = false;
    if (stamina < 1) {
        std::cout << RED << "Not enough stamina!\n" << RESET;
        return 0;
    }
    useStamina(1);

    float dist    = std::abs((float)(cursorPos - critPos));
    float maxDist = totalPositions / 2.0f;
    float mult    = 2.0f - 1.5f * std::min(1.0f, dist / maxDist);

    int base = baseDamageRoll;
    if (buffs.rageDraughtActive) {
        base = static_cast<int>(base * 1.5f);
        buffs.rageDraughtActive = false;
        std::cout << YELLOW << "Rage Draught surges through you!\n" << RESET;
    }

    int finalDmg = static_cast<int>(base * mult);

    // Always use heavy (tight) crit threshold
    if (mult >= 2.0f) {
        isCrit = true;
        std::cout << YELLOW << BOLD << "CRITICAL HIT! Perfect timing!\n" << RESET;
    } else if (mult <= 0.7f) {
        std::cout << RED << "Poor timing — weak hit!\n" << RESET;
    }

    return std::max(1, finalDmg);
}

// ── Potions ──────────────────────────────────────────────────────────────────

void Player::addPotion(const Potion& p) {
    potions.push_back(p);
}

void Player::lockPotionSlot(int idx) {
    if (idx >= 0 && idx < (int)potions.size()) {
        potions[idx].locked = true;
        buffs.lockedPotionIndex = idx;
    }
}

void Player::clearPotionLock() {
    for (auto& p : potions) p.locked = false;
    buffs.lockedPotionIndex = -1;
}

bool Player::usePotion(int idx) {
    if (idx < 0 || idx >= (int)potions.size()) return false;
    if (potions[idx].locked || potions[idx].permanentlyLocked) {
        std::cout << RED << "That potion is sealed shut — you cannot use it!\n" << RESET;
        return false;
    }
    const Potion& p = potions[idx];
    switch (p.type) {
        case PotionType::HEALING:
            heal(35);
            std::cout << GREEN << "You drink the Healing Potion. +" << 35 << " HP.\n" << RESET;
            break;
        case PotionType::STAMINA_BREW:
            restoreStamina(3);
            std::cout << CYAN << "You drink the Stamina Brew. +3 stamina.\n" << RESET;
            break;
        case PotionType::SHIELD_TONIC:
            buffs.shieldTonicTurns = 3;
            std::cout << CYAN << "You drink the Shield Tonic. +5 defense for 3 turns.\n" << RESET;
            break;
        case PotionType::RAGE_DRAUGHT:
            buffs.rageDraughtActive = true;
            std::cout << YELLOW << "You drink the Rage Draught. Next attack x1.5 damage.\n" << RESET;
            break;
        case PotionType::ANTIDOTE:
            buffs.bleeding     = false;
            buffs.poisonStacks = 0;
            buffs.blindRounds  = 0;
            std::cout << GREEN << "You drink the Antidote. All status effects cleared.\n" << RESET;
            break;
        case PotionType::PHANTOMS_DRAUGHT:
            buffs.phantomsDraughtActive = true;
            std::cout << MAGENTA << "You drink Phantom's Draught. Next attack bypasses defense.\n" << RESET;
            break;
        case PotionType::GRAVEWARDENS_OIL:
            buffs.gravewardenOilCharges = 3;
            std::cout << RED << "You coat yourself in Gravewarden Oil. Next 3 attacks apply Bleed.\n" << RESET;
            break;
        case PotionType::DEAD_MANS_COURAGE:
            buffs.bleeding     = false;
            buffs.poisonStacks = 0;
            buffs.blindRounds  = 0;
            restoreStamina(3);
            std::cout << CYAN << "You drink Dead Man's Courage. Status cleared. +3 stamina.\n" << RESET;
            break;
        case PotionType::ASH_MEMORY:
            addMaxHealth(15);
            std::cout << YELLOW << "You drink Ash Memory. +15 max HP. HP restored to maximum.\n" << RESET;
            break;
    }
    potions.erase(potions.begin() + idx);
    return true;
}

// ── Per-turn buff tick ────────────────────────────────────────────────────────

void Player::tickBuffs() {
    if (buffs.shieldTonicTurns > 0) --buffs.shieldTonicTurns;

    if (buffs.bleeding) {
        takeDamage(2);
        std::cout << RED << "[BLEED] You lose 2 HP. (permanent)\n" << RESET;
    }

    if (buffs.poisonStacks > 0) {
        int dmg = buffs.poisonStacks;
        takeDamage(dmg);
        int next = buffs.poisonStacks - 1;
        std::cout << RED << "[POISON] You take " << dmg << " damage. ["
                  << buffs.poisonStacks << " -> " << next << "]\n" << RESET;
        --buffs.poisonStacks;
        if (buffs.poisonStacks == 0)
            std::cout << "The poison fades.\n";
    }

    if (buffs.blindRounds > 0) {
        std::cout << YELLOW << "[BLIND] Your vision swims. (" << buffs.blindRounds << " rounds left)\n" << RESET;
    }
}

// ── Attack type system ────────────────────────────────────────────────────────

const AttackType& Player::getAttack(int slot) const {
    return attackSlots[slot];
}

int Player::getAttackCooldown(int slot) const {
    return attackCooldowns[slot];
}

bool Player::isAttackReady(int slot) const {
    return attackCooldowns[slot] == 0;
}

void Player::setAttack(int slot, const AttackType& at) {
    attackSlots[slot]     = at;
    attackCooldowns[slot] = 0;
}

void Player::startCooldown(int slot) {
    attackCooldowns[slot] = attackSlots[slot].cooldown;
}

void Player::tickCooldowns() {
    for (int i = 0; i < 3; ++i)
        if (attackCooldowns[i] > 0) --attackCooldowns[i];
}

// ── Rune system ───────────────────────────────────────────────────────────────

void Player::learnRune(RuneType r) {
    if (r != RuneType::NONE && !hasLearnedRune(r))
        learnedRunes.push_back(r);
}

bool Player::hasLearnedRune(RuneType r) const {
    for (auto& lr : learnedRunes)
        if (lr == r) return true;
    return false;
}

bool Player::hasRune(RuneType r) const {
    return runeSlots[0] == r || runeSlots[1] == r;
}

const std::vector<RuneType>& Player::getLearnedRunes() const {
    return learnedRunes;
}

RuneType Player::getRuneSlot(int slot) const {
    return runeSlots[slot];
}

bool Player::setRuneSlot(int slot, RuneType r) {
    if (r != RuneType::NONE && !hasLearnedRune(r)) return false;
    // Prevent same rune in both slots
    if (r != RuneType::NONE && runeSlots[slot == 0 ? 1 : 0] == r) return false;
    runeSlots[slot] = r;
    return true;
}

void Player::resetRunes() {
    learnedRunes.clear();
    runeSlots[0] = RuneType::NONE;
    runeSlots[1] = RuneType::NONE;
}

// ── Player status application ─────────────────────────────────────────────────

void Player::applyBleedToPlayer() {
    buffs.bleeding = true;
}

void Player::applyPoisonToPlayer(int stacks) {
    buffs.poisonStacks = stacks;
}

void Player::applyBlindToPlayer(int rounds) {
    buffs.blindRounds = rounds;
}

// ── Equipment ────────────────────────────────────────────────────────────────

void Player::equipWeapon(const Item& w) {
    if (w.getType() != ItemType::WEAPON) return;
    weapon = w;
    std::cout << GREEN << "Equipped: " << w.getName() << " [" << w.getStatsString() << "]\n" << RESET;
}

void Player::equipArmor(const Item& a) {
    if (a.getType() != ItemType::ARMOR) return;
    armor = a;
    std::cout << GREEN << "Equipped: " << a.getName() << " [" << a.getStatsString() << "]\n" << RESET;
}

// ── Experience / levelling ────────────────────────────────────────────────────

void Player::addExperience(int xp) {
    experience += xp;
    while (experience >= experienceToNextLevel) levelUp();
}

void Player::levelUp() {
    ++level;
    experience -= experienceToNextLevel;
    experienceToNextLevel = static_cast<int>(experienceToNextLevel * 1.6f);

    std::cout << "\n" << YELLOW << BOLD
              << "  ╔══════════════════════════════════════════════════════════╗\n"
              << "  ║                    ★  LEVEL UP  ★                      ║\n"
              << "  ║                     Now  Level  " << level;
    if (level < 10) std::cout << " ";
    std::cout << "                     ║\n"
              << "  ╚══════════════════════════════════════════════════════════╝\n"
              << RESET << "\n";

    std::cout << BOLD << YELLOW << "  [ 1 ]" << RESET << "  +20 max HP\n";
    std::cout << BOLD << YELLOW << "  [ 2 ]" << RESET << "  +1 max stamina\n";
    std::cout << BOLD << YELLOW << "  [ 3 ]" << RESET << "  +2 defense\n";
    std::cout << BOLD << YELLOW << "  [ 4 ]" << RESET << "  +3 luck\n";
    std::cout << "\n  Choose: ";

    int choice = 0;
    while (choice < 1 || choice > 4) {
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            choice = 0;
        }
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "\n";
    switch (choice) {
        case 1: addMaxHealth(20);  std::cout << GREEN  << "  ✓ Max HP +" << 20 << " → " << maxHealth  << "\n" << RESET; break;
        case 2: addMaxStamina(1);  std::cout << CYAN   << "  ✓ Max stamina +1 → " << maxStamina        << "\n" << RESET; break;
        case 3: addDefense(2);     std::cout << CYAN   << "  ✓ Defense +2 → " << defense               << "\n" << RESET; break;
        case 4: addLuck(3);        std::cout << YELLOW << "  ✓ Luck +3 → " << luck                     << "\n" << RESET; break;
    }

    health  = maxHealth;
    stamina = maxStamina;
    std::cout << GREEN << "  Your wounds close. HP and stamina fully restored.\n" << RESET;
}

// ── Display ──────────────────────────────────────────────────────────────────

void Player::showStatus() const {
    std::cout << "\n─── " << BOLD << name << RESET << " (Lv." << level << ") ──────────────────\n";
    std::cout << "  HP:      " << GREEN << health << "/" << maxHealth << RESET << "\n";
    std::cout << "  Stamina: " << CYAN  << stamina << "/" << maxStamina << RESET << "\n";
    std::cout << "  Gold:    " << YELLOW << gold << RESET << "\n";
    std::cout << "  Defense: " << defense << "\n";
    std::cout << "  Weapon:  " << weapon.getName() << " [" << weapon.getStatsString() << "]\n";
    std::cout << "  Armour:  " << armor.getName()  << " [" << armor.getStatsString()  << "]\n";

    // Active runes
    std::cout << "  Runes: ";
    bool any = false;
    for (int i = 0; i < 2; ++i) {
        if (runeSlots[i] != RuneType::NONE) {
            if (any) std::cout << " | ";
            std::cout << CYAN << runeName(runeSlots[i]) << RESET;
            any = true;
        }
    }
    if (!any) std::cout << DIM << "none" << RESET;
    std::cout << "\n";

    // Attack slots
    std::cout << "  Skills:\n";
    for (int i = 0; i < 3; ++i) {
        std::cout << "    [" << (i+1) << "] " << BOLD << attackSlots[i].name << RESET
                  << " (" << attackSlots[i].getStatsString() << ")";
        if (attackCooldowns[i] > 0)
            std::cout << RED << "  [cd:" << attackCooldowns[i] << "]" << RESET;
        else
            std::cout << GREEN << "  [ready]" << RESET;
        std::cout << "\n";
    }

    if (buffs.bleeding)
        std::cout << RED << "  STATUS:  BLEEDING (2 dmg/turn, permanent)\n" << RESET;
    if (buffs.poisonStacks > 0)
        std::cout << RED << "  STATUS:  POISONED (" << buffs.poisonStacks << " stacks)\n" << RESET;
    if (buffs.blindRounds > 0)
        std::cout << YELLOW << "  STATUS:  BLINDED (" << buffs.blindRounds << " rounds)\n" << RESET;
}

void Player::showInventory() const {
    std::cout << "\n─── Potions ─────────────────────────────\n";
    if (potions.empty()) {
        std::cout << "  (none)\n";
        return;
    }
    for (int i = 0; i < (int)potions.size(); ++i) {
        std::cout << "  [" << (i+1) << "] ";
        if (potions[i].locked) std::cout << RED << "[LOCKED] " << RESET;
        std::cout << potions[i].name << " — " << potions[i].description << "\n";
    }
}
