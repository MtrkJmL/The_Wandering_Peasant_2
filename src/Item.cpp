#include "../include/Item.h"
#include <random>
#include <ctime>
#include <sstream>
#include <algorithm>

// ── Statics ─────────────────────────────────────────────────────────────────

std::string Item::getRarityName(ItemRarity r) {
    switch (r) {
        case ItemRarity::COMMON:    return "Common";
        case ItemRarity::RARE:      return "Rare";
        case ItemRarity::EPIC:      return "Epic";
        case ItemRarity::LEGENDARY: return "Legendary";
    }
    return "Unknown";
}

std::string Item::getRarityColor(ItemRarity r) {
    switch (r) {
        case ItemRarity::COMMON:    return "\033[37m";  // white
        case ItemRarity::RARE:      return "\033[34m";  // blue
        case ItemRarity::EPIC:      return "\033[35m";  // magenta
        case ItemRarity::LEGENDARY: return "\033[33m";  // yellow
    }
    return "\033[0m";
}

void Item::generateStatsForRarity(ItemRarity r, int& dc, int& ds, int& mod) {
    switch (r) {
        case ItemRarity::COMMON:    dc=1; ds=6;  mod=1;  break;
        case ItemRarity::RARE:      dc=1; ds=8;  mod=3;  break;
        case ItemRarity::EPIC:      dc=2; ds=8;  mod=5;  break;
        case ItemRarity::LEGENDARY: dc=2; ds=10; mod=8;  break;
    }
}

// ── Constructors ─────────────────────────────────────────────────────────────

Item::Item()
    : name("Fist"), type(ItemType::WEAPON), rarity(ItemRarity::COMMON),
      diceCount(1), diceSides(4), modifier(0), value(0), upgradeLevel(0),
      weaponType(WeaponType::SWORD), armorType(ArmorType::CHESTPLATE) {}

Item::Item(ItemType t, ItemRarity r)
    : type(t), rarity(r), upgradeLevel(0),
      weaponType(WeaponType::SWORD), armorType(ArmorType::CHESTPLATE) {
    generateStatsForRarity(r, diceCount, diceSides, modifier);
    value = (static_cast<int>(r) + 1) * 20;

    static std::mt19937 rng(std::time(nullptr));
    if (t == ItemType::WEAPON) {
        static const char* wnames[] = {
            "Ash Blade","Cracked Sword","Peasant's Knife",
            "Bone Hatchet","Iron Spear","Dull Mace"
        };
        name = std::string(getRarityName(r)) + " " +
               wnames[rng() % 6];

        // 20% base chance for a weapon drop to carry a status effect
        // (luck bonus is applied externally via applyStatusRoll)
        {
            int roll = static_cast<int>(rng() % 100);
            if (roll < 20) {
                int pick = rng() % 3;
                statusEffect = (pick == 0) ? StatusEffect::POISON
                             : (pick == 1) ? StatusEffect::BLEED
                             :               StatusEffect::BLIND;
            }
        }
    } else {
        static const char* anames[] = {
            "Leather Vest","Tattered Cloak","Ash-stained Mail",
            "Briar Shield","Bark Armour","Soot Greaves"
        };
        name = std::string(getRarityName(r)) + " " +
               anames[rng() % 6];
    }
}

Item::Item(ItemType t, const std::string& n, int dc, int ds, int mod)
    : name(n), type(t), rarity(ItemRarity::COMMON),
      diceCount(dc), diceSides(ds), modifier(mod),
      value(dc * ds * 5), upgradeLevel(0),
      weaponType(WeaponType::SWORD), armorType(ArmorType::CHESTPLATE) {}

// ── Getters ──────────────────────────────────────────────────────────────────

std::string Item::getName()      const { return name; }
ItemType    Item::getType()      const { return type; }
ItemRarity  Item::getRarity()    const { return rarity; }
int         Item::getValue()     const { return value; }
int         Item::getSellValue() const {
    int base = diceCount * diceSides * 3 + modifier * 2 + upgradeLevel * 20;
    return std::max(5, base);
}
int         Item::getDiceCount() const { return diceCount; }
int         Item::getDiceSides() const { return diceSides; }
int         Item::getModifier()  const { return modifier; }
int         Item::getUpgradeLevel() const { return upgradeLevel; }

std::string Item::getRarityString() const { return getRarityName(rarity); }

std::string Item::getStatsString() const {
    std::ostringstream ss;
    ss << diceCount << "d" << diceSides;
    if (modifier > 0) ss << "+" << modifier;
    if (upgradeLevel > 0) ss << " [+" << upgradeLevel << "]";
    if      (statusEffect == StatusEffect::POISON) ss << " [Poison]";
    else if (statusEffect == StatusEffect::BLEED)  ss << " [Bleed]";
    else if (statusEffect == StatusEffect::BLIND)  ss << " [Blind]";
    return ss.str();
}

StatusEffect Item::getStatusEffect()     const { return statusEffect; }

std::string  Item::getStatusEffectName() const {
    switch (statusEffect) {
        case StatusEffect::POISON: return "Poison";
        case StatusEffect::BLEED:  return "Bleed";
        case StatusEffect::BLIND:  return "Blind";
        default:                   return "";
    }
}

int Item::roll() const {
    static std::mt19937 rng(std::time(nullptr));
    int total = modifier;
    std::uniform_int_distribution<int> dist(1, diceSides);
    for (int i = 0; i < diceCount; ++i) total += dist(rng);
    return std::max(1, total);
}

void Item::addDice()                      { ++diceCount; }
void Item::addModifier(int amt)           { modifier += amt; }
void Item::setStatusEffect(StatusEffect e){ statusEffect = e; }

bool Item::upgrade() {
    if (upgradeLevel >= 3) return false;
    ++upgradeLevel;
    diceSides += 2;
    value += 40;
    name += "+";
    return true;
}

Item Item::generateRandomItem(int chapterTier) {
    static std::mt19937 rng(std::time(nullptr));
    // Drop chances scale with chapter
    int roll = rng() % 100;
    ItemRarity r;
    if (chapterTier == 1) {
        r = roll < 60 ? ItemRarity::COMMON :
            roll < 90 ? ItemRarity::RARE   : ItemRarity::EPIC;
    } else if (chapterTier == 2) {
        r = roll < 40 ? ItemRarity::COMMON :
            roll < 80 ? ItemRarity::RARE   :
            roll < 95 ? ItemRarity::EPIC   : ItemRarity::LEGENDARY;
    } else {
        r = roll < 20 ? ItemRarity::COMMON :
            roll < 60 ? ItemRarity::RARE   :
            roll < 90 ? ItemRarity::EPIC   : ItemRarity::LEGENDARY;
    }
    ItemType t = (rng() % 2 == 0) ? ItemType::WEAPON : ItemType::ARMOR;
    return Item(t, r);
}
