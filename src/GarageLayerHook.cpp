#include <Geode/modify/GJGarageLayer.hpp>
#include "Helpers.hpp"
#include "KeybindUtil.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(MyGJGarageLayer, GJGarageLayer) {
    bool init() {
        if (!GJGarageLayer::init()) return false;
        kbutil::resetAll();

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJGarageLayer>()) return ListenerResult::Propagate;
            if (!kbutil::isTopLayer(this)) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "prev-page-menu", "prev-button");
            return ListenerResult::Propagate;
        }, "icon-kit-prev-page");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJGarageLayer>()) return ListenerResult::Propagate;
            if (!kbutil::isTopLayer(this)) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "next-page-menu", "next-button");
            return ListenerResult::Propagate;
        }, "icon-kit-next-page");

        return true;
    }
};
