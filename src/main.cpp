#include <Geode/Geode.hpp>

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/LevelAreaLayer.hpp>
#include <Geode/modify/LevelAreaInnerLayer.hpp>

#include <Geode/binding/CustomListView.hpp>
#include <Geode/binding/TableView.hpp>
#include <Geode/binding/LevelCell.hpp>

#include <geode.custom-keybinds/include/Keybinds.hpp>

#include <unordered_set>
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <limits>
#include <cmath>
#include <functional>

using namespace geode::prelude;
using namespace keybinds;

namespace kbutil {
    inline std::unordered_set<std::string> s_heldBinds;

    inline bool pressedOnce(std::string const& bindID, InvokeBindEvent* e) {
        if (!e) return false;

        if (e->isDown()) {
            return s_heldBinds.insert(bindID).second;
        } else {
            s_heldBinds.erase(bindID);
            return false;
        }
    }

    inline void resetAll() {
        s_heldBinds.clear();
    }

    template<typename T>
    inline bool isLayerActive() {
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        if (!scene) return false;

        std::function<bool(CCNode*)> walk = [&](CCNode* node) -> bool {
            if (!node) return false;

            if (typeinfo_cast<T*>(node)) {
                return true;
            }

            auto children = node->getChildren();
            if (!children) return false;

            CCObject* obj = nullptr;
            CCARRAY_FOREACH(children, obj) {
                if (walk(static_cast<CCNode*>(obj))) {
                    return true;
                }
            }
            return false;
        };

        return walk(scene);
    }
}

static CCNode* findByIDRecursive(CCNode* root, std::string const& id) {
    if (!root) return nullptr;
    if (root->getID() == id) return root;

    auto children = root->getChildren();
    if (!children) return nullptr;

    CCObject* obj = nullptr;
    CCARRAY_FOREACH(children, obj) {
        auto child = static_cast<CCNode*>(obj);
        if (auto found = findByIDRecursive(child, id)) return found;
    }
    return nullptr;
}

static void clickButtonRecursive(CCNode* root, std::string const& menuID, std::string const& buttonID) {
    if (!root) return;

    auto menu = findByIDRecursive(root, menuID);
    if (!menu) return;

    auto btn = findByIDRecursive(menu, buttonID);
    if (!btn) return;

    if (auto item = typeinfo_cast<CCMenuItemSpriteExtra*>(btn)) {
        item->activate();
    }
}

static void clickButton(CCNode* parentLayer, std::string const& menuID, std::string const& buttonID) {
    clickButtonRecursive(parentLayer, menuID, buttonID);
}

static bool openNthVisibleListEntry(LevelBrowserLayer* self, int n) {
    if (!self || n <= 0) return false;

    auto listViewNode = findByIDRecursive(self, "list-view");
    auto listView = typeinfo_cast<CustomListView*>(listViewNode);
    if (!listView || !listView->m_tableView || !listView->m_tableView->m_contentLayer) return false;

    auto content = listView->m_tableView->m_contentLayer;
    auto children = content->getChildren();
    if (!children) return false;

    std::vector<LevelCell*> cells;
    CCObject* obj = nullptr;
    CCARRAY_FOREACH(children, obj) {
        if (auto cell = typeinfo_cast<LevelCell*>(obj)) {
            cells.push_back(cell);
        }
    }

    if ((int)cells.size() < n) return false;

    std::sort(cells.begin(), cells.end(), [](LevelCell* a, LevelCell* b) {
        return a->getPositionY() > b->getPositionY();
    });

    auto target = cells[n - 1];

    clickButtonRecursive(target, "main-menu", "view-button");
    return true;
}

$execute {
    BindManager* manager = BindManager::get();

    manager->registerBindable({
        "main-menu-open-kit",
        "Open icon kit",
        "Opens the Icon Kit from the main menu",
        { Keybind::create(KEY_Left, Modifier::None) },
        "Main Menu"
    });

    manager->registerBindable({
        "main-menu-open-online",
        "Open online levels",
        "Opens the online menu from the main menu",
        { Keybind::create(KEY_Right, Modifier::None) },
        "Main Menu"
    });

    manager->registerBindable({
        "main-menu-play",
        "Play",
        "Play button",
        { Keybind::create(KEY_Up, Modifier::None) },
        "Main Menu"
    });

    manager->registerBindable({
        "main-menu-profile",
        "Profile",
        "Profile button",
        { Keybind::create(KEY_P, Modifier::None) },
        "Main Menu"
    });

    manager->registerBindable({
        "main-menu-chests",
        "Chests",
        "Chests button",
        { Keybind::create(KEY_C, Modifier::None) },
        "Main Menu"
    });

    manager->registerBindable({
        "main-menu-achievements",
        "Achievements",
        "Achievements button",
        { Keybind::create(KEY_A, Modifier::None) },
        "Main Menu"
    });

    manager->registerBindable({
        "main-menu-settings",
        "Settings",
        "Settings button",
        { Keybind::create(KEY_S, Modifier::None) },
        "Main Menu"
    });

    manager->registerBindable({
        "main-menu-stats",
        "Stats",
        "Stats button",
        { Keybind::create(KEY_I, Modifier::None) },
        "Main Menu"
    });

    manager->registerBindable({
        "icon-kit-prev-page",
        "Previous Page",
        "Go to the previous page in the Icon Kit",
        { Keybind::create(KEY_Left, Modifier::None) },
        "Icon Kit"
    });

    manager->registerBindable({
        "icon-kit-next-page",
        "Next Page",
        "Go to the next page in the Icon Kit",
        { Keybind::create(KEY_Right, Modifier::None) },
        "Icon Kit"
    });

    manager->registerBindable({
        "online-search",
        "Search",
        "Open the search menu in the online menu",
        { Keybind::create(KEY_F, Modifier::Control) },
        "Online Menu"
    });

    manager->registerBindable({
        "online-saved",
        "Saved",
        "Open the saved menu in the online menu",
        { Keybind::create(KEY_S, Modifier::None) },
        "Online Menu"
    });

    manager->registerBindable({
        "online-create",
        "Create",
        "Open the create menu in the online menu",
        { Keybind::create(KEY_C, Modifier::None) },
        "Online Menu"
    });

    manager->registerBindable({
        "online-quests",
        "Quests",
        "Open the quests menu in the online menu",
        { Keybind::create(KEY_Q, Modifier::None) },
        "Online Menu"
    });

    manager->registerBindable({
        "online-daily",
        "Daily",
        "Open the daily level menu in the online menu",
        { Keybind::create(KEY_D, Modifier::None) },
        "Online Menu"
    });

    manager->registerBindable({
        "online-weekly",
        "Weekly",
        "Open the weekly demon menu in the online menu",
        { Keybind::create(KEY_W, Modifier::None) },
        "Online Menu"
    });

    manager->registerBindable({
        "online-gauntlets",
        "Gauntlets",
        "Open the gauntlets menu in the online menu",
        { Keybind::create(KEY_G, Modifier::None) },
        "Online Menu"
    });

    manager->registerBindable({
        "online-featured",
        "Featured",
        "Open the featured menu in the online menu",
        { Keybind::create(KEY_F, Modifier::None) },
        "Online Menu"
    });

    manager->registerBindable({
        "online-lists",
        "Lists",
        "Open the lists menu in the online menu",
        { Keybind::create(KEY_L, Modifier::None) },
        "Online Menu"
    });

    manager->registerBindable({
        "online-paths",
        "Paths",
        "Open the paths menu in the online menu",
        { Keybind::create(KEY_P, Modifier::None) },
        "Online Menu"
    });

    manager->registerBindable({
        "online-map-packs",
        "Map Packs",
        "Open the map packs menu in the online menu",
        { Keybind::create(KEY_M, Modifier::None) },
        "Online Menu"
    });

    manager->registerBindable({
        "online-event",
        "Event",
        "Open the map event menu in the online menu",
        { Keybind::create(KEY_E, Modifier::None) },
        "Online Menu"
    });

    manager->registerBindable({
        "online-scores",
        "Leaderboards",
        "Open the leaderboards (scores) menu in the online menu",
        { Keybind::create(KEY_O, Modifier::None) },
        "Online Menu"
    });

    manager->registerBindable({
        "create-my-levels",
        "Uploaded",
        "Open your uploaded levels in the created levels menu",
        { Keybind::create(KEY_U, Modifier::None) },
        "Created Levels Menu"
    });

    manager->registerBindable({
        "create-search",
        "Search",
        "Open the search box in the created levels menu",
        { Keybind::create(KEY_F, Modifier::None) },
        "Created Levels Menu"
    });

    manager->registerBindable({
        "create-new-level",
        "Create New",
        "Open the create new level menu in the created levels menu",
        { Keybind::create(KEY_N, Modifier::Control) },
        "Created Levels Menu"
    });

    manager->registerBindable({
        "create-switch-mode",
        "Switch Modes",
        "Switches between your lists and levels in the created levels menu",
        { Keybind::create(KEY_T, Modifier::None) },
        "Created Levels Menu"
    });

    manager->registerBindable({
        "create-last-page",
        "Last Page",
        "Goes to the last page in the created levels menu",
        { Keybind::create(KEY_Right, Modifier::Control) },
        "Created Levels Menu"
    });

    manager->registerBindable({
        "edit-play",
        "Play",
        "Plays the level in the edit level menu",
        { Keybind::create(KEY_Up, Modifier::None) },
        "Edit Level Menu"
    });

    manager->registerBindable({
        "edit-edit",
        "Edit",
        "Opens the level editor in the edit level menu",
        { Keybind::create(KEY_Left, Modifier::None) },
        "Edit Level Menu"
    });

    manager->registerBindable({
        "edit-upload",
        "Upload",
        "Uploads the level in the edit level menu",
        { Keybind::create(KEY_Right, Modifier::None) },
        "Edit Level Menu"
    });

    manager->registerBindable({
        "edit-delete",
        "Delete",
        "Deletes the level in the edit level menu",
        { Keybind::create(KEY_Delete, Modifier::None) },
        "Edit Level Menu"
    });

    manager->registerBindable({
        "edit-duplicate",
        "Duplicate",
        "Duplicates the level in the edit level menu",
        { Keybind::create(KEY_D, Modifier::Control) },
        "Edit Level Menu"
    });

    manager->registerBindable({
        "edit-move-to-top",
        "Move to Top",
        "Moves the current level to the top of the levels list in the created levels menu",
        { Keybind::create(KEY_Up, Modifier::Control) },
        "Edit Level Menu"
    });

    manager->registerBindable({
        "edit-folder",
        "Folder",
        "Opens the select folder menu in the edit level menu",
        { Keybind::create(KEY_F, Modifier::None) },
        "Edit Level Menu"
    });

    manager->registerBindable({
        "pause-edit",
        "Edit",
        "Opens the level editor from the pause menu",
        { Keybind::create(KEY_E, Modifier::None) },
        "Pause Menu"
    });

    manager->registerBindable({
        "pause-play",
        "Play",
        "Plays the level from the pause menu",
        { Keybind::create(KEY_Up, Modifier::None) },
        "Pause Menu"
    });

    manager->registerBindable({
        "pause-practice",
        "Practice",
        "Toggles practice mode from the pause menu",
        { Keybind::create(KEY_P, Modifier::None) },
        "Pause Menu"
    });

    manager->registerBindable({
        "pause-retry",
        "Retry",
        "Retries the current level from the pause menu",
        { Keybind::create(KEY_R, Modifier::None) },
        "Pause Menu"
    });

    manager->registerBindable({
        "select-play",
        "Play",
        "Plays the current level in the level select menu",
        { Keybind::create(KEY_Up, Modifier::None) },
        "Level Select Menu"
    });

    manager->registerBindable({
        "select-tower",
        "Enter Tower",
        "Enters the tower in the enter the tower menu (I'm really good at writing these descriptions aren't I)",
        { Keybind::create(KEY_Up, Modifier::None) },
        "Level Select Menu"
    });

    manager->registerBindable({
        "tower-open-5001",
        "Open Tower Level 1",
        "Activates the Tower door: level-5001-button",
        { Keybind::create(KEY_One, Modifier::None) },
        "Tower"
    });

    manager->registerBindable({
        "tower-open-5002",
        "Open Tower Level 2",
        "Activates the Tower door: level-5002-button",
        { Keybind::create(KEY_Two, Modifier::None) },
        "Tower"
    });

    manager->registerBindable({
        "tower-open-5003",
        "Open Tower Level 3",
        "Activates the Tower door: level-5003-button",
        { Keybind::create(KEY_Three, Modifier::None) },
        "Tower"
    });

    manager->registerBindable({
        "tower-open-5004",
        "Open Tower Level 4",
        "Activates the Tower door: level-5004-button",
        { Keybind::create(KEY_Four, Modifier::None) },
        "Tower"
    });

    manager->registerBindable({
        "saved-delete",
        "Delete",
        "Deletes the level in the saved levels menu",
        { Keybind::create(KEY_Delete, Modifier::None) },
        "Saved Levels Menu"
    });

    manager->registerBindable({
        "saved-switch-mode",
        "Switch Mode",
        "Switches the mode in the saved levels menu",
        { Keybind::create(KEY_T, Modifier::None) },
        "Saved Levels Menu"
    });

    manager->registerBindable({
        "saved-favorite",
        "Favorite",
        "Shows favorited levels in the saved levels menu",
        { Keybind::create(KEY_F, Modifier::None) },
        "Saved Levels Menu"
    });

    manager->registerBindable({
        "saved-search",
        "Search",
        "Opens the search box in the saved levels menu",
        { Keybind::create(KEY_F, Modifier::Control) },
        "Saved Levels Menu"
    });

    manager->registerBindable({
        "saved-last-page",
        "Last Page",
        "Goes to the last page in the saved levels menu",
        { Keybind::create(KEY_Right, Modifier::Control) },
        "Saved Levels Menu"
    });

    manager->registerBindable({
        "saved-folder",
        "Folder",
        "Opens the folder menu in the saved levels menu",
        { Keybind::create(KEY_F, Modifier::None) },
        "Saved Levels Menu"
    });

    static constexpr auto listKeys = std::array{
        KEY_One, KEY_Two, KEY_Three, KEY_Four, KEY_Five,
        KEY_Six, KEY_Seven, KEY_Eight, KEY_Nine, KEY_Zero
    };

    for (int idx = 0; idx < 10; idx++) {
        int entryNum = idx + 1;
        manager->registerBindable({
            fmt::format("lists-open-{}", entryNum),
            fmt::format("Open Level {}", entryNum),
            "Opens the Nth visible entry in the Lists browser",
            { Keybind::create(listKeys[idx], Modifier::None) },
            "Created Levels Menu"
        });
    }
}

// small helper because ExtendedLayer "level-pages" keeps 3 levels loaded at the same time
// (I am aware that this is ABSOLUTELY the worst way to implement a fix to this)

static void clickCurrentLevel(LevelSelectLayer* self) {
    if (!self) return;

    auto pages = findByIDRecursive(self, "level-pages");
    if (!pages) return;

    auto children = pages->getChildren();
    if (!children || children->count() == 0) return;

    auto win = CCDirector::sharedDirector()->getWinSize();
    CCPoint screenCenterWorld = ccp(win.width * 0.5f, win.height * 0.5f);

    CCPoint centerLocal = pages->convertToNodeSpace(screenCenterWorld);

    CCNode* bestPage = nullptr;
    float bestDist = std::numeric_limits<float>::infinity();

    CCObject* obj = nullptr;
    CCARRAY_FOREACH(children, obj) {
        auto node = typeinfo_cast<CCNode*>(obj);
        if (!node) continue;

        if (!findByIDRecursive(node, "level-menu")) continue;

        auto sz = node->getContentSize();
        CCPoint pageCenter = node->getPosition() + ccp(sz.width * 0.5f, sz.height * 0.5f);

        float dx = pageCenter.x - centerLocal.x;
        float dy = pageCenter.y - centerLocal.y;
        float dist = dx * dx + dy * dy;

        if (dist < bestDist) {
            bestDist = dist;
            bestPage = node;
        }
    }

    if (!bestPage) return;

    auto menu = findByIDRecursive(bestPage, "level-menu");
    if (!menu) return;

    auto btn = findByIDRecursive(menu, "level-button");
    if (!btn) return;

    if (auto item = typeinfo_cast<CCMenuItemSpriteExtra*>(btn)) {
        item->activate();
    }
}

// main menu hook

class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        kbutil::resetAll();

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<MenuLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "main-menu", "icon-kit-button");
            }
            return ListenerResult::Propagate;
        }, "main-menu-open-kit");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<MenuLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "main-menu", "editor-button");
            }
            return ListenerResult::Propagate;
        }, "main-menu-open-online");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<MenuLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "main-menu", "play-button");
            }
            return ListenerResult::Propagate;
        }, "main-menu-play");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<MenuLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "profile-menu", "profile-button");
            }
            return ListenerResult::Propagate;
        }, "main-menu-profile");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<MenuLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "right-side-menu", "daily-chest-button");
            }
            return ListenerResult::Propagate;
        }, "main-menu-chests");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<MenuLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "bottom-menu", "achievements-button");
            }
            return ListenerResult::Propagate;
        }, "main-menu-achievements");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<MenuLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "bottom-menu", "settings-button");
            }
            return ListenerResult::Propagate;
        }, "main-menu-settings");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<MenuLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "bottom-menu", "stats-button");
            }
            return ListenerResult::Propagate;
        }, "main-menu-stats");

        return true;
    }
};

// icon kit hook

class $modify(MyGarageLayer, GJGarageLayer) {
    bool init() {
        if (!GJGarageLayer::init()) return false;

        kbutil::resetAll();

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJGarageLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "prev-page-menu", "prev-button");
            }
            return ListenerResult::Propagate;
        }, "icon-kit-prev-page");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJGarageLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "next-page-menu", "next-button");
            }
            return ListenerResult::Propagate;
        }, "icon-kit-next-page");

        return true;
    }
};

// online hook

class $modify(MyCreatorLayer, CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init()) return false;

        kbutil::resetAll();

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<CreatorLayer>()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-search", event)) {
                clickButton(this, "creator-buttons-menu", "search-button");
            }
            return ListenerResult::Propagate;
        }, "online-search");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<CreatorLayer>()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-saved", event)) {
                clickButton(this, "creator-buttons-menu", "saved-button");
            }
            return ListenerResult::Propagate;
        }, "online-saved");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<CreatorLayer>()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-create", event)) {
                clickButton(this, "creator-buttons-menu", "create-button");
            }
            return ListenerResult::Propagate;
        }, "online-create");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<CreatorLayer>()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-quests", event)) {
                clickButton(this, "creator-buttons-menu", "quests-button");
            }
            return ListenerResult::Propagate;
        }, "online-quests");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<CreatorLayer>()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-daily", event)) {
                clickButton(this, "creator-buttons-menu", "daily-button");
            }
            return ListenerResult::Propagate;
        }, "online-daily");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<CreatorLayer>()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-weekly", event)) {
                clickButton(this, "creator-buttons-menu", "weekly-button");
            }
            return ListenerResult::Propagate;
        }, "online-weekly");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<CreatorLayer>()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-event", event)) {
                clickButton(this, "creator-buttons-menu", "event-button");
            }
            return ListenerResult::Propagate;
        }, "online-event");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<CreatorLayer>()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-gauntlets", event)) {
                clickButton(this, "creator-buttons-menu", "gauntlets-button");
            }
            return ListenerResult::Propagate;
        }, "online-gauntlets");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<CreatorLayer>()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-featured", event)) {
                clickButton(this, "creator-buttons-menu", "featured-button");
            }
            return ListenerResult::Propagate;
        }, "online-featured");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<CreatorLayer>()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-lists", event)) {
                clickButton(this, "creator-buttons-menu", "lists-button");
            }
            return ListenerResult::Propagate;
        }, "online-lists");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<CreatorLayer>()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-paths", event)) {
                clickButton(this, "creator-buttons-menu", "paths-button");
            }
            return ListenerResult::Propagate;
        }, "online-paths");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<CreatorLayer>()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("online-map-packs", event)) {
                clickButton(this, "creator-buttons-menu", "map-packs-button");
            }
            return ListenerResult::Propagate;
        }, "online-map-packs");

        return true;
    }
};

// created levels hook (and saved levels hook)

class $modify(MyLevelBrowserLayer, LevelBrowserLayer) {
    bool init(GJSearchObject* object) {
        if (!LevelBrowserLayer::init(object)) return false;

        kbutil::resetAll();

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<LevelBrowserLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "my-levels-menu", "my-levels-button");
            }
            return ListenerResult::Propagate;
        }, "create-my-levels");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<LevelBrowserLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "page-menu", "last-page-button");
            }
            return ListenerResult::Propagate;
        }, "create-last-page");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<LevelBrowserLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "saved-menu", "delete-button");
            }
            return ListenerResult::Propagate;
        }, "saved-delete");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<LevelBrowserLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "saved-menu", "switch-mode-button");
            }
            return ListenerResult::Propagate;
        }, "saved-switch-mode");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<LevelBrowserLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "saved-menu", "favorite-button");
            }
            return ListenerResult::Propagate;
        }, "saved-favorite");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<LevelBrowserLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "saved-menu", "search-button");
            }
            return ListenerResult::Propagate;
        }, "saved-search");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<LevelBrowserLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "saved-menu", "last-page-button");
            }
            return ListenerResult::Propagate;
        }, "saved-last-page");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<LevelBrowserLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "saved-folder", "folder-button");
            }
            return ListenerResult::Propagate;
        }, "saved-folder");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<LevelBrowserLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "saved-menu", "folder-button");
            }
            return ListenerResult::Propagate;
        }, "saved-folder");

        for (int i = 1; i <= 10; i++) {
            std::string bindID = fmt::format("lists-open-{}", i);

            this->template addEventListener<InvokeBindFilter>([this, i, bindID](InvokeBindEvent* event) {
                if (!kbutil::isLayerActive<LevelBrowserLayer>()) return ListenerResult::Propagate;
                if (kbutil::pressedOnce(bindID, event)) {
                    if (findByIDRecursive(this, "list-view")) {
                        openNthVisibleListEntry(this, i);
                    }
                }
                return ListenerResult::Propagate;
            }, bindID);
        }

        return true;
    }
};

// level edit page hook

class $modify(MyEditLevelLayer, EditLevelLayer) {
    bool init(GJGameLevel* level) {
        if (!EditLevelLayer::init(level)) return false;

        kbutil::resetAll();

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<EditLevelLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "level-edit-menu", "play-button");
            }
            return ListenerResult::Propagate;
        }, "edit-play");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<EditLevelLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "level-edit-menu", "edit-button");
            }
            return ListenerResult::Propagate;
        }, "edit-edit");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<EditLevelLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "level-edit-menu", "share-button");
            }
            return ListenerResult::Propagate;
        }, "edit-upload");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<EditLevelLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "level-actions-menu", "delete-button");
            }
            return ListenerResult::Propagate;
        }, "edit-delete");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<EditLevelLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "level-actions-menu", "duplicate-button");
            }
            return ListenerResult::Propagate;
        }, "edit-duplicate");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<EditLevelLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "level-actions-menu", "move-to-top-button");
            }
            return ListenerResult::Propagate;
        }, "edit-move-to-top");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<EditLevelLayer>()) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "folder-menu", "folder-button");
            }
            return ListenerResult::Propagate;
        }, "edit-folder");

        return true;
    }
};

// level select hook

class $modify(MyLevelSelectLayer, LevelSelectLayer) {
    bool init(int page) {
        if (!LevelSelectLayer::init(page)) return false;

        kbutil::resetAll();

        this->template addEventListener<InvokeBindFilter>(
            [this](InvokeBindEvent* event) {
                if (!kbutil::isLayerActive<LevelSelectLayer>())
                    return ListenerResult::Propagate;

                if (kbutil::pressedOnce("select-play", event)) {
                    clickCurrentLevel(this);
                }

                return ListenerResult::Propagate;
            },
            "select-play"
        );

        return true;
    }
};

// tower menu hook

class $modify(MyLevelAreaLayer, LevelAreaLayer) {
    bool init() {
        if (!LevelAreaLayer::init()) return false;

        kbutil::resetAll();

        auto sceneRoot = []() -> CCNode* {
            auto scene = CCDirector::sharedDirector()->getRunningScene();
            return scene ? static_cast<CCNode*>(scene) : nullptr;
        };

        this->template addEventListener<InvokeBindFilter>([sceneRoot](InvokeBindEvent* event) {
            if (kbutil::pressedOnce("select-tower", event)) {
                clickButton(sceneRoot(), "enter-menu", "enter-btn");
            }
            return ListenerResult::Propagate;
        }, "select-tower");

        return true;
    }
};

// tower menu hook 2: electric boogaloo

class $modify(MyLevelAreaInnerLayer, LevelAreaInnerLayer) {
    bool init(bool returning) {
        if (!LevelAreaInnerLayer::init(returning)) return false;

        kbutil::resetAll();

        auto sceneRoot = []() -> CCNode* {
            auto scene = CCDirector::sharedDirector()->getRunningScene();
            return scene ? static_cast<CCNode*>(scene) : nullptr;
        };

        this->template addEventListener<InvokeBindFilter>([sceneRoot](InvokeBindEvent* event) {
            if (kbutil::pressedOnce("select-tower", event)) {
                clickButton(sceneRoot(), "enter-menu", "enter-btn");
            }
            return ListenerResult::Propagate;
        }, "select-tower");

        this->template addEventListener<InvokeBindFilter>([sceneRoot](InvokeBindEvent* event) {
            if (kbutil::pressedOnce("tower-open-5001", event)) {
                clickButton(sceneRoot(), "main-menu", "level-5001-button");
            }
            return ListenerResult::Propagate;
        }, "tower-open-5001");

        this->template addEventListener<InvokeBindFilter>([sceneRoot](InvokeBindEvent* event) {
            if (kbutil::pressedOnce("tower-open-5002", event)) {
                clickButton(sceneRoot(), "main-menu", "level-5002-button");
            }
            return ListenerResult::Propagate;
        }, "tower-open-5002");

        this->template addEventListener<InvokeBindFilter>([sceneRoot](InvokeBindEvent* event) {
            if (kbutil::pressedOnce("tower-open-5003", event)) {
                clickButton(sceneRoot(), "main-menu", "level-5003-button");
            }
            return ListenerResult::Propagate;
        }, "tower-open-5003");

        this->template addEventListener<InvokeBindFilter>([sceneRoot](InvokeBindEvent* event) {
            if (kbutil::pressedOnce("tower-open-5004", event)) {
                clickButton(sceneRoot(), "main-menu", "level-5004-button");
            }
            return ListenerResult::Propagate;
        }, "tower-open-5004");

        return true;
    }
};
