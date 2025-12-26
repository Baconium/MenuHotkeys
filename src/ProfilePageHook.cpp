#include <Geode/modify/ProfilePage.hpp>
#include "Helpers.hpp"
#include "KeybindUtil.hpp"

using namespace geode::prelude;
using namespace keybinds;

class $modify(MyProfilePage, ProfilePage) {
    bool init(int accountID, bool ownProfile) {
        if (!ProfilePage::init(accountID, ownProfile)) return false;

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ProfilePage>()) return ListenerResult::Propagate;
            if (!kbutil::isTopLayer(this)) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (!kbutil::pressedOnce("profile-message", event)) return ListenerResult::Propagate;
            clickButton(this, "profile-info", "message-button");
            return ListenerResult::Propagate;
        }, "profile-message");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ProfilePage>()) return ListenerResult::Propagate;
            if (!kbutil::isTopLayer(this)) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (!kbutil::pressedOnce("profile-comment-history", event)) return ListenerResult::Propagate;
            clickButton(this, "profile-info", "comment-history-button");
            return ListenerResult::Propagate;
        }, "profile-comment-history");

        return true;
    }
};
