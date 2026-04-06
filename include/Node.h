#pragma once
#include <vector>
#include <string>

enum class NodeType {
    START,
    COMBAT,
    ELITE,
    EVENT,    // displayed as ? — random events + M'alid encounter
    SHOP,
    REST,
    BOSS
};

struct MapNode {
    int      id;
    NodeType type;
    int      row;
    int      col;
    bool     visited   = false;
    bool     available = false;      // player can move here now
    std::vector<int> nextIds;        // connections to next row

    std::string typeLabel() const;
    std::string typeSymbol() const;  // short ASCII tag e.g. [CMB]
};
