#include <Geode/modify/GJDropDownLayer.hpp>
#include "Helpers.hpp"
#include "KeybindUtil.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(MyDropDownLayer, GJDropDownLayer) {
    bool init(char const* title, float unk1, bool unk2) {
        if (!GJDropDownLayer::init(title, unk1, unk2)) return false;
        kbutil::resetAll();

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "options-menu", "account-button");
            return ListenerResult::Propagate;
        }, "settings-account");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "options-menu", "how-to-play-button");
            return ListenerResult::Propagate;
        }, "settings-how-to-play");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "options-menu", "options-button");
            return ListenerResult::Propagate;
        }, "settings-options");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "options-menu", "graphics-button");
            return ListenerResult::Propagate;
        }, "settings-graphics");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "options-menu", "rate-button");
            return ListenerResult::Propagate;
        }, "settings-rate");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "options-menu", "songs-button");
            return ListenerResult::Propagate;
        }, "settings-songs");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "options-menu", "help-button");
            return ListenerResult::Propagate;
        }, "settings-help");

        return true;
    }
};
