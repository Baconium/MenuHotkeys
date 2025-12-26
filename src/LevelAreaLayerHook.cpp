#include <Geode/modify/LevelAreaLayer.hpp>
#include "Helpers.hpp"
#include "KeybindUtil.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(MyLevelAreaLayer, LevelAreaLayer) {
    bool init() {
        if (!LevelAreaLayer::init()) return false;
        kbutil::resetAll();

        auto sceneRoot = []() -> CCNode* {
            auto scene = CCDirector::sharedDirector()->getRunningScene();
            return scene ? static_cast<CCNode*>(scene) : nullptr;
        };

        this->template addEventListener<InvokeBindFilter>([sceneRoot](InvokeBindEvent* event) {
            if (kbutil::pressedOnce("select-tower", event)) clickButton(sceneRoot(), "enter-menu", "enter-btn");
            return ListenerResult::Propagate;
        }, "select-tower");

        return true;
    }
};
