#pragma once
#include <string>
#include "RunState.h"

// The Banner side-quest chain.
// Each chapter has one quest event node that advances the chain.
struct BannerQuest {
    static std::string ch1EventTitle();
    static std::string ch1EventText();
    static bool ch1TriggerCondition(const RunState& rs);
    static void ch1Complete(RunState& rs);

    static std::string ch2EventTitle();
    static std::string ch2EventText();
    static bool ch2TriggerCondition(const RunState& rs);
    static void ch2Complete(RunState& rs);

    static std::string ch3EventTitle();
    static std::string ch3EventText();
    static bool ch3TriggerCondition(const RunState& rs);
    static void ch3Complete(RunState& rs);

    static bool isComplete(const RunState& rs);
    static std::string statusString(const RunState& rs);
};
