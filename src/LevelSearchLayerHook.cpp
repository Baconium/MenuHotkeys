#include <Geode/modify/LevelSearchLayer.hpp>
#include "Helpers.hpp"
#include "KeybindUtil.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(MyLevelSearchLayer, LevelSearchLayer) {
    struct Fields {
        bool m_ctrlHeld  = false;
        bool m_shiftHeld = false;
        bool m_altHeld   = false;
    };

    bool init(int p0) {
        if (!LevelSearchLayer::init(p0)) return false;
        kbutil::resetAll();

        this->setKeyboardEnabled(true);
        this->scheduleOnce(schedule_selector(MyLevelSearchLayer::doAutoFocus), 0.05f);

        auto guard = [this]() {
            return kbutil::isLayerActive<LevelSearchLayer>() && nodeLooksAlive(this);
        };

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "search-button-menu", "search-level-button");
            return ListenerResult::Propagate;
        }, "search-level");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "search-button-menu", "search-user-button");
            return ListenerResult::Propagate;
        }, "search-user");

        this->template addEventListener<InvokeBindFilter>([this, guard](InvokeBindEvent* event) {
            if (!guard()) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "search-button-menu", "clear-search-button");
            return ListenerResult::Propagate;
        }, "search-clear");

        return true;
    }

    void doAutoFocus(float) {
        auto searchBar = typeinfo_cast<CCTextInputNode*>(this->getChildByID("search-bar"));
        if (!searchBar) return;

        searchBar->m_selected = true;

        if (searchBar->m_textField) {
            searchBar->m_textField->attachWithIME();
            return;
        }

        if (auto textField = findFirstOfTypeRecursive<CCTextFieldTTF>(searchBar)) {
            textField->attachWithIME();
        }
    }

    Modifier currentMods() {
        int mods = 0;
        if (m_fields->m_ctrlHeld)  mods |= (int)Modifier::Control;
        if (m_fields->m_shiftHeld) mods |= (int)Modifier::Shift;
        if (m_fields->m_altHeld)   mods |= (int)Modifier::Alt;
        return (Modifier)mods;
    }

    bool shouldForward(enumKeyCodes key, Modifier mods) {
        if ((int)mods != 0) return true;

        switch (key) {
            case KEY_Enter:
            case KEY_Escape:
            case KEY_Tab:
            case KEY_Left:
            case KEY_Right:
            case KEY_Up:
            case KEY_Down:
            case KEY_Delete:
            case KEY_Backspace:
                return true;
            default:
                return false;
        }
    }

    void forwardToKeybinds(enumKeyCodes key, bool isDown) {
        auto mods = currentMods();
        if (!shouldForward(key, mods)) return;
        PressBindEvent(Keybind::create(key, mods), isDown).post();
    }

    void keyDown(enumKeyCodes key) override {
        if (key == KEY_Control) m_fields->m_ctrlHeld  = true;
        if (key == KEY_Shift)   m_fields->m_shiftHeld = true;
        if (key == KEY_Alt)     m_fields->m_altHeld   = true;
        forwardToKeybinds(key, true);
        LevelSearchLayer::keyDown(key);
    }

    void keyUp(enumKeyCodes key) override {
        forwardToKeybinds(key, false);
        if (key == KEY_Control) m_fields->m_ctrlHeld  = false;
        if (key == KEY_Shift)   m_fields->m_shiftHeld = false;
        if (key == KEY_Alt)     m_fields->m_altHeld   = false;
        LevelSearchLayer::keyUp(key);
    }
};
