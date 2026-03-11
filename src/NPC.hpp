#pragma once

#include "MapObject.hpp"

#include <string>
#include <vector>

/**
 * @brief Non-player character placed on map and used for dialogues.
 */
class NPC : public MapObject {
    std::string name;
    std::vector<std::string> dialogues;
    size_t nextDialogueIndex = 0;

public:
    NPC();
    NPC(const Point& pos, const std::string& npcName, const std::vector<std::string>& npcDialogues);

    virtual char GetSymbol() const override { return 'N'; }
    virtual bool IsPassable() const override { return false; }

    const std::string& GetName() const { return name; }
    const std::vector<std::string>& GetDialogues() const { return dialogues; }
    size_t GetDialogueCursor() const { return nextDialogueIndex; }
    void SetDialogueCursor(size_t cursor);

    std::string Talk();
};
