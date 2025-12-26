#include <Geode/modify/EditLevelLayer.hpp>
#include "Helpers.hpp"
#include "KeybindUtil.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(MyEditLevelLayer, EditLevelLayer) {
    bool init(GJGameLevel* level) {
        if (!EditLevelLayer::init(level)) return false;
        kbutil::resetAll();

        auto guard = [this]() {
            return kbutil::isLayerActive<EditLevelLayer>() && nodeLooksAlive(this);
        };

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "level-edit-menu", "play-button");
            return ListenerResult::Propagate;
        }, "edit-play");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "level-edit-menu", "edit-button");
            return ListenerResult::Propagate;
        }, "edit-edit");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "level-edit-menu", "share-button");
            return ListenerResult::Propagate;
        }, "edit-upload");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "level-actions-menu", "delete-button");
            return ListenerResult::Propagate;
        }, "edit-delete");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "level-actions-menu", "duplicate-button");
            return ListenerResult::Propagate;
        }, "edit-duplicate");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "level-actions-menu", "move-to-top-button");
            return ListenerResult::Propagate;
        }, "edit-move-to-top");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "folder-menu", "folder-button");
            return ListenerResult::Propagate;
        }, "edit-folder");

        return true;
    }
};
