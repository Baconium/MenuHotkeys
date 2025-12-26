#include <Geode/modify/ChallengesPage.hpp>
#include "Helpers.hpp"
#include "KeybindUtil.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(MyChallengesPage, ChallengesPage) {
    bool init() {
        if (!ChallengesPage::init()) return false;
        kbutil::resetAll();

        auto claimQuest = [this](std::string const& questNodeID) {
            if (!nodeLooksAlive(this)) return;
            auto mainLayer = this->getChildByID("main-layer");
            if (!mainLayer) return;

            auto questNode = mainLayer->getChildByID(questNodeID);
            if (questNode) clickButtonRecursive(questNode, "claim-menu", "claim-button");
        };

        this->template addEventListener<InvokeBindFilter>([this, claimQuest](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ChallengesPage>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;

            if (event->isDown()) {
                claimQuest("top-quest");
                claimQuest("middle-quest");
                claimQuest("bottom-quest");
            }
            return ListenerResult::Propagate;
        }, "quests-claim-all");

        this->template addEventListener<InvokeBindFilter>([this, claimQuest](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ChallengesPage>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) claimQuest("top-quest");
            return ListenerResult::Propagate;
        }, "quests-claim-top");

        this->template addEventListener<InvokeBindFilter>([this, claimQuest](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ChallengesPage>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) claimQuest("middle-quest");
            return ListenerResult::Propagate;
        }, "quests-claim-mid");

        this->template addEventListener<InvokeBindFilter>([this, claimQuest](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ChallengesPage>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) claimQuest("bottom-quest");
            return ListenerResult::Propagate;
        }, "quests-claim-bot");

        return true;
    }
};
