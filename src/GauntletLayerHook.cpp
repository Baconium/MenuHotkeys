#include <Geode/modify/GauntletLayer.hpp>
#include "Helpers.hpp"
#include "KeybindUtil.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(MyGauntletLayer, GauntletLayer) {
    bool init(GauntletType type) {
        if (!GauntletLayer::init(type)) return false;
        kbutil::resetAll();

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GauntletLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "levels-menu", "level-1");
            return ListenerResult::Propagate;
        }, "gauntlet-1");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GauntletLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "levels-menu", "level-2");
            return ListenerResult::Propagate;
        }, "gauntlet-2");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GauntletLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "levels-menu", "level-3");
            return ListenerResult::Propagate;
        }, "gauntlet-3");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GauntletLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "levels-menu", "level-4");
            return ListenerResult::Propagate;
        }, "gauntlet-4");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GauntletLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "levels-menu", "level-5");
            return ListenerResult::Propagate;
        }, "gauntlet-5");

        return true;
    }
};
