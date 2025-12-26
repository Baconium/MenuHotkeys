#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GJDropDownLayer.hpp>
#include "Helpers.hpp"
#include "KeybindUtil.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        kbutil::resetAll();

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<MenuLayer>()) return ListenerResult::Propagate;
            if (kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!kbutil::isTopLayer(this)) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "main-menu", "icon-kit-button");
            return ListenerResult::Propagate;
        }, "main-menu-open-kit");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<MenuLayer>()) return ListenerResult::Propagate;
            if (kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!kbutil::isTopLayer(this)) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "main-menu", "editor-button");
            return ListenerResult::Propagate;
        }, "main-menu-open-online");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<MenuLayer>()) return ListenerResult::Propagate;
            if (kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!kbutil::isTopLayer(this)) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "main-menu", "play-button");
            return ListenerResult::Propagate;
        }, "main-menu-play");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<MenuLayer>()) return ListenerResult::Propagate;
            if (kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!kbutil::isTopLayer(this)) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "profile-menu", "profile-button");
            return ListenerResult::Propagate;
        }, "main-menu-profile");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<MenuLayer>()) return ListenerResult::Propagate;
            if (kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!kbutil::isTopLayer(this)) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "right-side-menu", "daily-chest-button");
            return ListenerResult::Propagate;
        }, "main-menu-chests");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<MenuLayer>()) return ListenerResult::Propagate;
            if (kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!kbutil::isTopLayer(this)) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "bottom-menu", "achievements-button");
            return ListenerResult::Propagate;
        }, "main-menu-achievements");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<MenuLayer>()) return ListenerResult::Propagate;
            if (kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!kbutil::isTopLayer(this)) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "bottom-menu", "settings-button");
            return ListenerResult::Propagate;
        }, "main-menu-settings");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<MenuLayer>()) return ListenerResult::Propagate;
            if (kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!kbutil::isTopLayer(this)) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "bottom-menu", "stats-button");
            return ListenerResult::Propagate;
        }, "main-menu-stats");

        return true;
    }
};
