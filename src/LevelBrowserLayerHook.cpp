#include <Geode/modify/LevelBrowserLayer.hpp>
#include "Helpers.hpp"
#include "KeybindUtil.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(MyLevelBrowserLayer, LevelBrowserLayer) {
    bool init(GJSearchObject* object) {
        if (!LevelBrowserLayer::init(object)) return false;
        kbutil::resetAll();

        auto guard = [this]() {
            return kbutil::isLayerActive<LevelBrowserLayer>() && nodeLooksAlive(this);
        };

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "my-levels-menu", "my-levels-button");
            return ListenerResult::Propagate;
        }, "create-my-levels");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "page-menu", "last-page-button");
            return ListenerResult::Propagate;
        }, "create-last-page");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "saved-menu", "delete-button");
            return ListenerResult::Propagate;
        }, "saved-delete");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "saved-menu", "switch-mode-button");
            return ListenerResult::Propagate;
        }, "saved-switch-mode");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "saved-menu", "favorite-button");
            return ListenerResult::Propagate;
        }, "saved-favorite");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "saved-menu", "search-button");
            return ListenerResult::Propagate;
        }, "saved-search");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "saved-menu", "last-page-button");
            return ListenerResult::Propagate;
        }, "saved-last-page");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "saved-menu", "folder-button");
            return ListenerResult::Propagate;
        }, "saved-folder");

        for (int i = 1; i <= 10; i++) {
            std::string bindID = fmt::format("lists-open-{}", i);
            this->template addEventListener<InvokeBindFilter>([this, i, bindID, guard](InvokeBindEvent* event) {
                if (!guard()) return ListenerResult::Propagate;
                if (kbutil::pressedOnce(bindID, event)) {
                    if (findByIDRecursive(this, "list-view")) {
                        if (listIsMapPacks(this)) openNthVisibleMapPackEntry(this, i);
                        else openNthVisibleListEntry(this, i);
                    }
                }
                return ListenerResult::Propagate;
            }, bindID);
        }

        return true;
    }
};
