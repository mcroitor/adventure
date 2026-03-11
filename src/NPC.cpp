#include "NPC.hpp"

#include <utility>

NPC::NPC()
    : MapObject(),
      name("Villager"),
      dialogues({"Hello."}),
      nextDialogueIndex(0) {}

NPC::NPC(const Point& pos, const std::string& npcName, const std::vector<std::string>& npcDialogues)
    : MapObject(pos, 0),
      name(npcName),
      dialogues(npcDialogues),
      nextDialogueIndex(0) {
    if (dialogues.empty()) {
        dialogues.push_back("...");
    }
}

void NPC::SetDialogueCursor(size_t cursor) {
    if (dialogues.empty()) {
        nextDialogueIndex = 0;
        return;
    }

    nextDialogueIndex = cursor % dialogues.size();
}

std::string NPC::Talk() {
    if (dialogues.empty()) {
        return name + ": ...";
    }

    size_t idx = nextDialogueIndex % dialogues.size();
    std::string line = dialogues[idx];
    nextDialogueIndex = (idx + 1) % dialogues.size();
    return name + ": " + line;
}
