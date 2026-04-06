#pragma once
#include "Node.h"
#include <vector>

class ChapterMap {
private:
    int                  chapter;
    std::vector<MapNode> nodes;
    int                  currentNodeId;
    int                  totalRows;

    void        generate(unsigned int seed);
    void        markAvailable();
    void        connectRows(int fromRow, int toRow);
    std::string buildConnectorLine(int lowerRow, float t) const;

public:
    explicit ChapterMap(int chapter, unsigned int seed = 0);

    // Returns ids of nodes the player can move to next
    std::vector<int> getAvailableIds() const;

    // Move to node; throws if not available
    void selectNode(int id);

    int             getCurrentId()   const;
    const MapNode&  getNode(int id)  const;
    const MapNode&  getCurrentNode() const;
    bool            atBoss()         const;
    int             getChapter()     const;

    // ASCII render to stdout
    void render() const;
};
