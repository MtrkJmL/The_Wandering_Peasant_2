#pragma once
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"
#include "ChapterMap.h"
#include "RunState.h"
#include "Quest.h"
#include "Discoveries.h"
#include "Achievement.h"
#include <memory>
#include <random>

class Game {
private:
    Player       player;
    RunState     runState;
    Discoveries  discoveries;
    Achievements achievements;
    bool         isRunning;
    std::mt19937 rng;

    // ── Menus ──────────────────────────────────────────────────
    void displayMainMenu();
    void startNewGame();
    void characterCreation();
    void choosePerk();

    // ── Chapter flow ───────────────────────────────────────────
    void runChapter(int chapter);
    void resolveNode(const MapNode& node, int chapter);

    // ── Node handlers ──────────────────────────────────────────
    void handleCombatNode(int chapter, bool isElite);
    void handleEventNode(int chapter);
    void handleShopNode();
    void handleRestNode();
    void handleBossNode(int chapter);

    // ── Between-chapter hub ────────────────────────────────────
    void handleTownOfDeadBallads(int afterChapter);

    // ── Combat ─────────────────────────────────────────────────
    bool runCombat(Enemy& enemy);
    bool runBossFight(Boss& boss);
    void playerAttackTurn(Enemy& enemy, bool& enemyDead);
    void playerAttackTurnBoss(Boss& boss, bool& bossDead);
    void enemyAttackTurn(Enemy& enemy);
    void enemyAttackTurnBoss(Boss& boss);
    void showCombatStatus(const Enemy& enemy) const;
    void showCombatStatusBoss(const Boss& boss) const;
    int  runTimingBar(bool isHeavy, int& outCritPos, int critWidthBonus = 0);

    // ── Potion / bargain UI ────────────────────────────────────
    void potionMenu(Enemy* enemy = nullptr); // null = out of combat
    bool attemptBargain(Enemy& enemy);
    bool attemptTalk(Enemy& enemy);

    // ── Shop ───────────────────────────────────────────────────
    void shopMenu();
    void blacksmithMenu();

    // ── Narrative helpers ──────────────────────────────────────
    void showChapterIntro(int chapter) const;
    void showTwistReveal();
    void showVictory(bool trueEnding);
    void showTutorial();
    void gameOver();
    void showQuestLog() const;
    void showDiscoveries();

    // ── Persistence ────────────────────────────────────────────
    void saveDiscoveries();
    void saveAchievements();
    void notifyAchievement(AchievementID id);

    // ── Achievement screen ─────────────────────────────────────
    void showAchievements();

    // ── Utility ────────────────────────────────────────────────
    void clearScreen() const;
    void pressEnter()  const;
    void slowPrint(const std::string& text, int delayMs = 18) const;
    void slowPrintSkippable(const std::string& text, int delayMs, bool& skipped) const;
    int  randInt(int lo, int hi);          // inclusive

public:
    Game();
    void run();
};
