#include "../include/Quest.h"

// ── Chapter 1: Torn Banner ────────────────────────────────────────────────────

std::string BannerQuest::ch1EventTitle() {
    return "Ruins of Ashveil";
}

std::string BannerQuest::ch1EventText() {
    return
        "Among the scorched timbers you find a tattered banner — the colours are ash-grey "
        "and deep red, the stitching done by someone who cared about it.\n"
        "In the corner: a symbol you half-recognise from old fireside stories. A peasant's "
        "crook, wreathed in thorns. The banner of the Supreme Peasant.\n"
        "You fold it carefully and tuck it inside your coat.\n"
        "[BANNER FOUND — Quest: The Supreme Peasant's Road, Chapter 1/3]";
}

bool BannerQuest::ch1TriggerCondition(const RunState& rs) {
    return !rs.bannerFound;
}

void BannerQuest::ch1Complete(RunState& rs) {
    rs.bannerFound = true;
}

// ── Chapter 2: The Veteran ────────────────────────────────────────────────────

std::string BannerQuest::ch2EventTitle() {
    return "Marsh Exile";
}

std::string BannerQuest::ch2EventText() {
    return
        "A hollow-eyed old soldier sits on a root above the waterline. He stares at the "
        "banner before you say a word.\n"
        "\"Where did you find that?\" His voice is careful — not threatening. Afraid.\n"
        "\"Ashveil. I found it in the ruins.\"\n"
        "He is quiet for a long time. \"The Supreme Peasant was real. Not a story. He walked "
        "this road. All three chapters of it. And he did not come back.\"\n"
        "\"He left that banner at the threshold of the Obsidian Reach. Said if anyone ever "
        "carried it to the end, the dead would know they hadn't wandered for nothing.\"\n"
        "He stands, joints cracking. \"There's a grave at the foot of the Reach. His grave. "
        "You'll know it — thorn-wreath carved in the stone.\"\n"
        "[QUEST ADVANCED — The Supreme Peasant's Road, Chapter 2/3]";
}

bool BannerQuest::ch2TriggerCondition(const RunState& rs) {
    return rs.bannerFound && !rs.veteranMet;
}

void BannerQuest::ch2Complete(RunState& rs) {
    rs.veteranMet = true;
}

// ── Chapter 3: The Grave ──────────────────────────────────────────────────────

std::string BannerQuest::ch3EventTitle() {
    return "The Peasant's Grave";
}

std::string BannerQuest::ch3EventText() {
    return
        "You find it at the foot of a black cliff — a flat stone, rough-cut, with a "
        "thorn-wreath carved into its face. No name. Just the wreath.\n"
        "You lay the banner across the stone. The fabric goes still in the wind.\n"
        "From somewhere beneath the rock — you feel it more than hear it — a sound like "
        "a door unlocking.\n"
        "Then: three faint shapes behind you. Not solid. Not threatening. Just present.\n"
        "The three champions. Waiting.\n"
        "One of them — the tallest — nods once.\n"
        "[QUEST COMPLETE — The Supreme Peasant's Banner now carries their blessing]\n"
        "[TRUE ENDING UNLOCKED — Face Slave King Amax after the Darkblood Lord]";
}

bool BannerQuest::ch3TriggerCondition(const RunState& rs) {
    return rs.bannerFound && rs.veteranMet && !rs.bannerComplete;
}

void BannerQuest::ch3Complete(RunState& rs) {
    rs.bannerComplete = true;
}

bool BannerQuest::isComplete(const RunState& rs) {
    return rs.bannerComplete;
}

std::string BannerQuest::statusString(const RunState& rs) {
    if (!rs.bannerFound)    return "[ ] The Supreme Peasant's Road (not started)";
    if (!rs.veteranMet)     return "[>] The Supreme Peasant's Road (1/3 — banner found)";
    if (!rs.bannerComplete) return "[>] The Supreme Peasant's Road (2/3 — veteran met)";
    return "[x] The Supreme Peasant's Road (COMPLETE)";
}
