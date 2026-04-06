#pragma once

// Persistent flags for the current run.
// All flags reset on death. Banner chain flags persist across chapters within a run.
struct RunState {
    // Banner side-quest chain
    bool bannerFound    = false; // Ch1: found torn banner in ruins
    bool veteranMet     = false; // Ch2: veteran recognised the banner
    bool bannerComplete = false; // Ch3: found the Supreme Peasant's grave

    // Chapter progression
    int  currentChapter = 1;    // 1, 2, or 3
    bool ch1BossDefeated = false;
    bool ch2BossDefeated = false;
    bool ch3BossDefeated = false;

    // Narrative flags
    bool twistRevealed  = false; // Ch2 mid-chapter twist shown
    bool witchDefeated  = false;

    // Loot / economy helpers
    int  totalGoldEarned = 0;
    int  enemiesDefeated = 0;

    // Plague Shaman: permanently locks one potion slot for the entire run
    bool shamanCurseApplied = false;

    // Quest 1: The Debt Ledger (Ch1 start → kill-count complete)
    bool debtLedgerFound = false;
    int  debtKillCount   = 0;
    bool debtQuestDone   = false;

    // Quest 2: The Third Face (Ch1 shrine → Ch2 vandal event)
    bool shrineQuestActive = false;
    bool shrineQuestDone   = false;

    // Quest 3: Something from the East (Ch2 doc → Ch3 shard event)
    bool easternWarningFound = false;
    bool easternWarningDone  = false;

    // Quest 4: The Deserter's Cipher (Ch2 start → Ch3 key event)
    bool deserterMet   = false;
    bool cipherDecoded = false;

    // Quest 5: What They Built (Ch3 instant-complete)
    bool architectureFound = false;

    void reset() { *this = RunState{}; }
};
