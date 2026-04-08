#pragma once
#include <string>

enum class ItemType { WEAPON, ARMOR };

enum class ItemRarity { COMMON, RARE, EPIC, LEGENDARY };

enum class StatusEffect { NONE, POISON, BLEED, BLIND };

enum class WeaponType { SWORD, AXE, MACE, DAGGER, SPEAR };
enum class ArmorType  { HELMET, CHESTPLATE, GAUNTLETS, GREAVES, SHIELD };

class Item {
private:
    std::string  name;
    ItemType     type;
    ItemRarity   rarity;
    int diceCount;
    int diceSides;
    int modifier;
    int value;
    WeaponType   weaponType;
    ArmorType    armorType;
    int upgradeLevel; // 0-3, each level +2 diceSides
    StatusEffect statusEffect = StatusEffect::NONE;

    static std::string getRarityName(ItemRarity r);
    static void generateStatsForRarity(ItemRarity r, int& dc, int& ds, int& mod);

public:
    Item();
    Item(ItemType type, ItemRarity rarity);
    Item(ItemType type, const std::string& name,
         int diceCount, int diceSides, int modifier);

    std::string getName()     const;
    ItemType    getType()     const;
    ItemRarity  getRarity()   const;
    int         getValue()     const;
    int         getSellValue() const;  // derived from stats: diceCount*diceSides*3 + modifier*2 + upgradeLevel*20
    int         getDiceCount()const;
    int         getDiceSides()const;
    int         getModifier() const;
    int         getUpgradeLevel() const;
    std::string  getStatsString()       const;
    std::string  getRarityString()      const;
    StatusEffect getStatusEffect()      const;
    std::string  getStatusEffectName()  const;

    int  roll()       const;
    void addDice();
    void addModifier(int amount);
    bool upgrade();   // returns false if already max level
    void setStatusEffect(StatusEffect e);

    static Item generateRandomItem(int chapterTier = 1);
    // Elite-drop flat-bonus equipment: diceCount=0, modifier=flatBonus
    static Item generateFlatItem(ItemType type, ItemRarity rarity);
    static std::string getRarityColor(ItemRarity r);
};
