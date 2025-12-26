#include <Geode/modify/CreatorLayer.hpp>
#include "Helpers.hpp"
#include "KeybindUtil.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(MyCreatorLayer, CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init()) return false;
        kbutil::resetAll();

        auto guard = [this]() {
            return kbutil::isLayerActive<CreatorLayer>() && nodeLooksAlive(this);
        };

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-search", event))
                clickButton(this, "creator-buttons-menu", "search-button");
            return ListenerResult::Propagate;
        }, "online-search");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-saved", event))
                clickButton(this, "creator-buttons-menu", "saved-button");
            return ListenerResult::Propagate;
        }, "online-saved");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-create", event))
                clickButton(this, "creator-buttons-menu", "create-button");
            return ListenerResult::Propagate;
        }, "online-create");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-quests", event))
                clickButton(this, "creator-buttons-menu", "quests-button");
            return ListenerResult::Propagate;
        }, "online-quests");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-daily", event))
                clickButton(this, "creator-buttons-menu", "daily-button");
            return ListenerResult::Propagate;
        }, "online-daily");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-weekly", event))
                clickButton(this, "creator-buttons-menu", "weekly-button");
            return ListenerResult::Propagate;
        }, "online-weekly");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-event", event))
                clickButton(this, "creator-buttons-menu", "event-button");
            return ListenerResult::Propagate;
        }, "online-event");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-gauntlets", event))
                clickButton(this, "creator-buttons-menu", "gauntlets-button");
            return ListenerResult::Propagate;
        }, "online-gauntlets");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-featured", event))
                clickButton(this, "creator-buttons-menu", "featured-button");
            return ListenerResult::Propagate;
        }, "online-featured");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-lists", event))
                clickButton(this, "creator-buttons-menu", "lists-button");
            return ListenerResult::Propagate;
        }, "online-lists");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-paths", event))
                clickButton(this, "creator-buttons-menu", "paths-button");
            return ListenerResult::Propagate;
        }, "online-paths");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-map-packs", event))
                clickButton(this, "creator-buttons-menu", "map-packs-button");
            return ListenerResult::Propagate;
        }, "online-map-packs");

        return true;
    }
};
