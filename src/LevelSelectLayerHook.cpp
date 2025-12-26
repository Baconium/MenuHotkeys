#include <Geode/modify/LevelSelectLayer.hpp>
#include "Helpers.hpp"
#include "KeybindUtil.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(MyLevelSelectLayer, LevelSelectLayer) {
    bool init(int page) {
        if (!LevelSelectLayer::init(page)) return false;
        kbutil::resetAll();

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<LevelSelectLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;

            if (kbutil::pressedOnce("select-play", event)) clickCurrentLevel(this);
            return ListenerResult::Propagate;
        }, "select-play");

        return true;
    }
};
