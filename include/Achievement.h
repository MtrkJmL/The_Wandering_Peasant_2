#pragma once
#include <string>

enum class AchievementID {
    // Progression
    ROAD_TO_ASH = 0,
    INTO_THE_FOG,
    GOSPEL_OF_ASH,
    TRUTH_WAS_ALWAYS_THERE,   // secret
    // Challenge runs
    NO_POTIONS,
    PEASANT_CLOTHES,
    RUNELESS,
    // Skill / discovery
    FIRST_RUNE,
    RUNE_SCHOLAR,
    WORDSMITH,
    BARGAINER,
    FULLY_FORGED,
    MASTER_PEASANT,

    COUNT_  // sentinel — keep last
};

static const int ACH_COUNT = static_cast<int>(AchievementID::COUNT_);

struct AchievementData {
    const char* key;
    const char* name;
    const char* description;
    bool        secret;   // if true and locked: show "???" in the achievements screen
};

const AchievementData& achievementData(AchievementID id);

struct Achievements {
    bool unlocked[ACH_COUNT] = {};
    int  consecutiveRuns     = 0; // completed runs in a row (for Master Peasant)

    // Returns true if newly unlocked
    bool unlock(AchievementID id);
    bool isUnlocked(AchievementID id) const;
    int  unlockedCount() const;

    // Call on successful run completion (Ch3 beaten).
    // Returns true if Master Peasant was newly unlocked this call.
    bool onRunCompleted();

    // Call on death — resets consecutive-run streak
    void onRunFailed();

    void load(const std::string& path);
    void save(const std::string& path) const;
};
