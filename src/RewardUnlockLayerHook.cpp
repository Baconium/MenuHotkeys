#include <Geode/modify/RewardUnlockLayer.hpp>
#include "Helpers.hpp"
#include "KeybindUtil.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(MyRewardUnlockLayer, RewardUnlockLayer) {
    bool init(int type, RewardsPage* page) {
        if (!RewardUnlockLayer::init(type, page)) return false;
        kbutil::resetAll();

        auto canOperate = [this]() -> bool {
            if (!kbutil::isLayerActive<RewardUnlockLayer>()) return false;
            if (!nodeLooksAlive(this)) return false;
            return true;
        };

        this->template addEventListener<InvokeBindFilter>([this, canOperate](InvokeBindEvent* e) {
            if (!canOperate()) return ListenerResult::Propagate;
            if (!kbutil::pressedOnce("rewards-claim", e)) return ListenerResult::Propagate;
            auto btn = this->m_rewardBtn;
            if (btn && nodeIsRunningVisible(btn) && btn->isVisible() && btn->isEnabled()) {
                kbutil::resetAll();
                btn->activate();
            }
            return ListenerResult::Propagate;
        }, "rewards-claim");

        return true;
    }
};
