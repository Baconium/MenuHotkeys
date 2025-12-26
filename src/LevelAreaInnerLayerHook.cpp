#include <Geode/modify/LevelAreaInnerLayer.hpp>
#include "Helpers.hpp"
#include "KeybindUtil.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(MyLevelAreaInnerLayer, LevelAreaInnerLayer) {
    bool init(bool returning) {
        if (!LevelAreaInnerLayer::init(returning)) return false;
        kbutil::resetAll();

        auto sceneRoot = []() -> CCNode* {
            auto scene = CCDirector::sharedDirector()->getRunningScene();
            return scene ? static_cast<CCNode*>(scene) : nullptr;
        };
        this->template addEventListener<InvokeBindFilter>([sceneRoot](InvokeBindEvent* event) {
            if (kbutil::pressedOnce("select-tower", event)) clickButton(sceneRoot(), "enter-menu", "enter-btn");
            return ListenerResult::Propagate;
        }, "select-tower");

        this->template addEventListener<InvokeBindFilter>([sceneRoot](InvokeBindEvent* event) {
            if (kbutil::pressedOnce("tower-open-5001", event)) clickButton(sceneRoot(), "main-menu", "level-5001-button");
            return ListenerResult::Propagate;
        }, "tower-open-5001");

        this->template addEventListener<InvokeBindFilter>([sceneRoot](InvokeBindEvent* event) {
            if (kbutil::pressedOnce("tower-open-5002", event)) clickButton(sceneRoot(), "main-menu", "level-5002-button");
            return ListenerResult::Propagate;
        }, "tower-open-5002");

        this->template addEventListener<InvokeBindFilter>([sceneRoot](InvokeBindEvent* event) {
            if (kbutil::pressedOnce("tower-open-5003", event)) clickButton(sceneRoot(), "main-menu", "level-5003-button");
            return ListenerResult::Propagate;
        }, "tower-open-5003");

        this->template addEventListener<InvokeBindFilter>([sceneRoot](InvokeBindEvent* event) {
            if (kbutil::pressedOnce("tower-open-5004", event)) clickButton(sceneRoot(), "main-menu", "level-5004-button");
            return ListenerResult::Propagate;
        }, "tower-open-5004");

        return true;
    }
};
