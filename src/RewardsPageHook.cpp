#include <Geode/modify/RewardsPage.hpp>
#include "Helpers.hpp"
#include "KeybindUtil.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(MyRewardsPage, RewardsPage) {
    bool init() {
        if (!RewardsPage::init()) return false;
        kbutil::resetAll();

        auto canOperate = [this]() -> bool {
            if (!kbutil::isLayerActive<RewardsPage>()) return false;
            if (!nodeLooksAlive(this)) return false;
            // Avoid double-activations while unlock layer is open
            if (this->m_openLayer != nullptr) return false;
            return true;
        };

        auto tryOpen = [this](CCMenuItemSpriteExtra* chest) {
            if (!chest) return;
            if (!nodeIsRunningVisible(chest)) return;
            if (!chest->isVisible() || !chest->isEnabled()) return;
            kbutil::resetAll();
            // Call the page's handler to ensure correct flow
            this->onReward(chest);
        };

        this->template addEventListener<InvokeBindFilter>([this, canOperate, tryOpen](InvokeBindEvent* e) {
            if (!canOperate()) return ListenerResult::Propagate;
            if (!kbutil::pressedOnce("rewards-open-left", e)) return ListenerResult::Propagate;
            tryOpen(this->m_leftChest);
            return ListenerResult::Propagate;
        }, "rewards-open-left");

        this->template addEventListener<InvokeBindFilter>([this, canOperate, tryOpen](InvokeBindEvent* e) {
            if (!canOperate()) return ListenerResult::Propagate;
            if (!kbutil::pressedOnce("rewards-open-right", e)) return ListenerResult::Propagate;
            tryOpen(this->m_rightChest);
            return ListenerResult::Propagate;
        }, "rewards-open-right");

        this->template addEventListener<InvokeBindFilter>([this, canOperate, tryOpen](InvokeBindEvent* e) {
            if (!canOperate()) return ListenerResult::Propagate;
            if (!kbutil::pressedOnce("rewards-open-both", e)) return ListenerResult::Propagate;
            // Open left first, then right if no unlock layer popped
            tryOpen(this->m_leftChest);
            if (this->m_openLayer == nullptr) tryOpen(this->m_rightChest);
            return ListenerResult::Propagate;
        }, "rewards-open-both");

        return true;
    }
};
