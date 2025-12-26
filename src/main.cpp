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
#include <Geode/modify/ChallengesPage.hpp>
#include <Geode/modify/GauntletSelectLayer.hpp>
#include <Geode/modify/GauntletLayer.hpp>
#include <Geode/modify/ProfilePage.hpp>
#include <Geode/modify/GJDropDownLayer.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>

#include <Geode/binding/CustomListView.hpp>
#include <Geode/binding/TableView.hpp>
#include <Geode/binding/LevelCell.hpp>
#include <Geode/binding/LevelListCell.hpp>
#include <Geode/binding/MapPackCell.hpp>
#include <Geode/binding/ChallengeNode.hpp>
#include <Geode/binding/CCTextInputNode.hpp>

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
using namespace cocos2d;

// helpers

static inline std::vector<CCNode*> safeNodeChildren(CCNode* node) {
    std::vector<CCNode*> out;
    if (!node) return out;

    auto children = node->getChildren();
    if (!children) return out;

    out.reserve(static_cast<size_t>(children->count()));
    for (int i = 0; i < children->count(); i++) {
        auto obj = children->objectAtIndex(i);
        if (auto ch = typeinfo_cast<CCNode*>(obj)) {
            out.push_back(ch);
        }
    }
    return out;
}

static inline bool nodeLooksAlive(CCNode* n) {
    return n && n->getParent() != nullptr;
}

static inline bool nodeIsRunningVisible(CCNode* n) {
    return n && nodeLooksAlive(n) && n->isRunning() && n->isVisible();
}

template <typename T>
static inline T* findFirstOfTypeRecursive(CCNode* root) {
    if (!root) return nullptr;
    if (auto t = typeinfo_cast<T*>(root)) return t;
    for (auto ch : safeNodeChildren(root)) {
        if (auto found = findFirstOfTypeRecursive<T>(ch)) return found;
    }
    return nullptr;
}


template <typename T>
static inline T* getFrontmostLayerOfType() {
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    if (!scene) return nullptr;

    auto kids = safeNodeChildren(scene);
    if (kids.empty()) return nullptr;


    std::stable_sort(kids.begin(), kids.end(), [](CCNode* a, CCNode* b) {
        if (!a || !b) return false;
        return a->getZOrder() < b->getZOrder();
    });

    for (auto it = kids.rbegin(); it != kids.rend(); ++it) {
        auto node = *it;
        if (!nodeIsRunningVisible(node)) continue;
        if (auto t = findFirstOfTypeRecursive<T>(node)) {
            if (nodeIsRunningVisible(t)) return t;
        }
    }
    return nullptr;
}

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

    static bool isTopLayer(CCNode* layer) {

        auto scene = CCDirector::sharedDirector()->getRunningScene();
        if (!scene || !layer) return false;

        auto children = scene->getChildren();
        if (children && children->count() > 0) {
            CCObject* obj;
            CCARRAY_FOREACH(children, obj) {
                auto child = typeinfo_cast<CCNode*>(obj);
                if (!child) continue;

                if ((typeinfo_cast<GJDropDownLayer*>(child) || typeinfo_cast<FLAlertLayer*>(child)) 
                    && child->isVisible()) {
                    
                    return layer == child;
                }
            }
        }

    auto top = children->lastObject();
    return top == layer;
}

    template <typename T>
    inline bool isLayerActive() {
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        if (!scene) return false;

        std::function<bool(CCNode*)> walk = [&](CCNode* node) -> bool {
            if (!node) return false;
            if (typeinfo_cast<T*>(node)) return true;


            for (auto ch : safeNodeChildren(node)) {
                if (walk(ch)) return true;
            }
            return false;
        };

        return walk(scene);
    }
}

static CCNode* findByIDRecursive(CCNode* root, std::string const& id) {
    if (!root) return nullptr;

    if (root->getID() == id) return root;

    for (auto ch : safeNodeChildren(root)) {
        if (auto found = findByIDRecursive(ch, id)) return found;
    }
    return nullptr;
}

static void clickButtonRecursive(CCNode* root, std::string const& menuID, std::string const& buttonID) {
    if (!root) return;
    if (!nodeLooksAlive(root)) return;

    auto menu = findByIDRecursive(root, menuID);
    if (!menu) return;

    auto btn = findByIDRecursive(menu, buttonID);
    if (!btn) return;

    if (auto item = typeinfo_cast<CCMenuItemSpriteExtra*>(btn)) {
        if (item->isEnabled() && item->isVisible()) {
            kbutil::resetAll();
            item->activate();
        }
    }
}

static void clickButton(CCNode* parentLayer, std::string const& menuID, std::string const& buttonID) {
    clickButtonRecursive(parentLayer, menuID, buttonID);
}

static CCMenuItemSpriteExtra* findFirstMenuItemSpriteExtra(CCNode* root) {
    if (!root) return nullptr;

    if (auto item = typeinfo_cast<CCMenuItemSpriteExtra*>(root)) return item;

    for (auto ch : safeNodeChildren(root)) {
        if (auto found = findFirstMenuItemSpriteExtra(ch)) return found;
    }
    return nullptr;
}


static bool listIsMapPacks(LevelBrowserLayer* self) {
    if (!self) return false;
    if (!nodeLooksAlive(self)) return false;

    auto listViewNode = findByIDRecursive(self, "list-view");
    auto listView = typeinfo_cast<CustomListView*>(listViewNode);
    if (!listView || !listView->m_tableView || !listView->m_tableView->m_contentLayer) return false;

    for (auto ch : safeNodeChildren(listView->m_tableView->m_contentLayer)) {
        if (typeinfo_cast<MapPackCell*>(ch)) return true;
    }
    return false;
}

static bool openNthVisibleMapPackEntry(LevelBrowserLayer* self, int n) {
    if (!self || n <= 0) return false;
    if (!nodeLooksAlive(self)) return false;

    auto listViewNode = findByIDRecursive(self, "list-view");
    auto listView = typeinfo_cast<CustomListView*>(listViewNode);
    if (!listView || !listView->m_tableView || !listView->m_tableView->m_contentLayer) return false;

    auto content = listView->m_tableView->m_contentLayer;

    std::vector<MapPackCell*> cells;
    for (auto ch : safeNodeChildren(content)) {
        if (auto c = typeinfo_cast<MapPackCell*>(ch)) cells.push_back(c);
    }

    if ((int)cells.size() < n) return false;

    std::sort(cells.begin(), cells.end(), [](CCNode* a, CCNode* b) {
        return a->getPositionY() > b->getPositionY();
    });

    auto target = cells[n - 1];
    if (!target) return false;

    if (auto item = findFirstMenuItemSpriteExtra(target)) {
        if (item->isEnabled() && item->isVisible()) {
            kbutil::resetAll();
            item->activate();
            return true;
        }
    }

    return false;
}

static bool openNthVisibleListEntry(LevelBrowserLayer* self, int n) {
    if (!self || n <= 0) return false;
    if (!nodeLooksAlive(self)) return false;

    auto listViewNode = findByIDRecursive(self, "list-view");
    auto listView = typeinfo_cast<CustomListView*>(listViewNode);
    if (!listView || !listView->m_tableView || !listView->m_tableView->m_contentLayer) return false;

    auto content = listView->m_tableView->m_contentLayer;

    std::vector<CCNode*> cells;
    for (auto ch : safeNodeChildren(content)) {
        if (typeinfo_cast<LevelCell*>(ch) || typeinfo_cast<LevelListCell*>(ch)) {
            cells.push_back(ch);
        }
    }

    if ((int)cells.size() < n) return false;

    std::sort(cells.begin(), cells.end(), [](CCNode* a, CCNode* b) {
        return a->getPositionY() > b->getPositionY();
    });

    auto target = cells[n - 1];
    if (!target) return false;

    clickButtonRecursive(target, "main-menu", "view-button");
    return true;
}

static void collectByIDRecursive(CCNode* root, std::string const& id, std::vector<CCNode*>& out) {
    if (!root) return;


    if (root->getID() == id) out.push_back(root);


    for (auto ch : safeNodeChildren(root)) {
        collectByIDRecursive(ch, id, out);
    }
}

static void clickCurrentLevel(LevelSelectLayer* self) {
    if (!self) return;
    if (!nodeLooksAlive(self)) return;

    auto pages = findByIDRecursive(self, "level-pages");
    if (!pages) return;

    auto children = safeNodeChildren(pages);
    if (children.empty()) return;

    auto win = CCDirector::sharedDirector()->getWinSize();
    CCPoint screenCenterWorld = ccp(win.width * 0.5f, win.height * 0.5f);
    CCPoint centerLocal = pages->convertToNodeSpace(screenCenterWorld);

    CCNode* bestPage = nullptr;
    float bestDist = std::numeric_limits<float>::infinity();

    for (auto node : children) {
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
        if (item->isEnabled() && item->isVisible()) item->activate();
    }
}

static void collectMenuItems(CCNode* root, std::vector<CCMenuItemSpriteExtra*>& out) {
    if (!root) return;
    if (!nodeLooksAlive(root)) return;
    
    geode::Ref<CCNode> rootHold = root;


    if (auto item = typeinfo_cast<CCMenuItemSpriteExtra*>(root)) {
        out.push_back(item);
    }


    for (auto ch : safeNodeChildren(root)) {
        collectMenuItems(ch, out);
    }
}

static void clickButtonDirect(CCNode* root, std::string const& buttonID) {
    if (!root) return;

    auto btn = findByIDRecursive(root, buttonID);
    
    if (auto item = typeinfo_cast<CCMenuItemSpriteExtra*>(btn)) {
        if (item->isVisible() && item->isEnabled()) {
            item->activate();
        }
    }
}

// keybinds

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

    manager->registerBindable({
        "quests-claim-top",
        "Claim Quest 1 (Top)",
        "Claims the top quest reward (if available)",
        { Keybind::create(KEY_One, Modifier::None) },
        "Quests Menu"
    });

    manager->registerBindable({
        "quests-claim-mid",
        "Claim Quest 2 (Middle)",
        "Claims the middle quest reward (if available)",
        { Keybind::create(KEY_Two, Modifier::None) },
        "Quests Menu"
    });

    manager->registerBindable({
        "quests-claim-bot",
        "Claim Quest 3 (Bottom)",
        "Claims the bottom quest reward (if available)",
        { Keybind::create(KEY_Three, Modifier::None) },
        "Quests Menu"
    });

    manager->registerBindable({
        "quests-claim-all",
        "Claim All Quests",
        "Claims all available quest rewards",
        { Keybind::create(KEY_Enter, Modifier::None) },
        "Quests Menu"
    });

    manager->registerBindable({
        "gauntlet-left",
        "Previous Page",
        "Goes back a page in the gauntlets menu",
        { Keybind::create(KEY_Left, Modifier::None) },
        "Gauntlets Menu"
    });

    manager->registerBindable({
        "gauntlet-right",
        "Next Page",
        "Goes forward a page in the gauntlets menu",
        { Keybind::create(KEY_Right, Modifier::None) },
        "Gauntlets Menu"
    });

    manager->registerBindable({
        "gauntlet-1",
        "Level 1",
        "Selects level 1 in the gauntlets menu",
        { Keybind::create(KEY_One, Modifier::None) },
        "Gauntlets Menu"
    });

    manager->registerBindable({
        "gauntlet-2",
        "Level 2",
        "Selects level 2 in the gauntlets menu",
        { Keybind::create(KEY_Two, Modifier::None) },
        "Gauntlets Menu"
    });

    manager->registerBindable({
        "gauntlet-3",
        "Level 3",
        "Selects level 3 in the gauntlets menu",
        { Keybind::create(KEY_Three, Modifier::None) },
        "Gauntlets Menu"
    });

    manager->registerBindable({
        "gauntlet-4",
        "Level 4",
        "Selects level 4 in the gauntlets menu",
        { Keybind::create(KEY_Four, Modifier::None) },
        "Gauntlets Menu"
    });

    manager->registerBindable({
        "gauntlet-5",
        "Level 5",
        "Selects level 5 in the gauntlets menu",
        { Keybind::create(KEY_Five, Modifier::None) },
        "Gauntlets Menu"
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

    manager->registerBindable({
        "profile-message",
        "Messages",
        "Opens the messages menu in the profile page",
        { Keybind::create(KEY_M, Modifier::None) },
        "Profile Menu"
    });

    manager->registerBindable({
        "profile-friend",
        "Friends",
        "Opens the friends menu in the profile page",
        { Keybind::create(KEY_F, Modifier::None) },
        "Profile Menu"
    });

    manager->registerBindable({
        "profile-requests",
        "Requests",
        "Opens the requests menu in the profile page",
        { Keybind::create(KEY_R, Modifier::None) },
        "Profile Menu"
    });

    manager->registerBindable({
        "profile-settings",
        "Settings",
        "Opens the settings menu in the profile page",
        { Keybind::create(KEY_S, Modifier::None) },
        "Profile Menu"
    });

    manager->registerBindable({
        "profile-next-page",
        "Next Page",
        "Goes to the next page in the profile page",
        { Keybind::create(KEY_Right, Modifier::None) },
        "Profile Menu"
    });

    manager->registerBindable({
        "profile-prev-page",
        "Previous Page",
        "Goes to the previous page in the profile page",
        { Keybind::create(KEY_Left, Modifier::None) },
        "Profile Menu"
    });

    manager->registerBindable({
        "profile-comment",
        "Comment",
        "Opens the comment menu in the profile page",
        { Keybind::create(KEY_C, Modifier::Control) },
        "Profile Menu"
    });

    manager->registerBindable({
        "profile-comment-history",
        "Comment History",
        "Opens the comment history menu in the profile page",
        { Keybind::create(KEY_C, Modifier::None) },
        "Profile Menu"
    });

    manager->registerBindable({
        "profile-refresh",
        "Refresh",
        "Refreshes the profile page",
        { Keybind::create(KEY_R, Modifier::Control) },
        "Profile Menu"
    });

    manager->registerBindable({
        "profile-info",
        "Info",
        "Opens the info menu in the profile page",
        { Keybind::create(KEY_I, Modifier::None) },
        "Profile Menu"
    });

    manager->registerBindable({
        "settings-account",
        "Account",
        "Opens the account menu in the settings menu",
        { Keybind::create(KEY_A, Modifier::None) },
        "Settings Menu"
    });

    manager->registerBindable({
        "settings-how-to-play",
        "How to Play",
        "Opens the how to play menu in the settings menu",
        { Keybind::create(KEY_H, Modifier::Control) },
        "Settings Menu"
    });

    manager->registerBindable({
        "settings-options",
        "Options",
        "Opens the options menu in the settings menu",
        { Keybind::create(KEY_O, Modifier::None) },
        "Settings Menu"
    });

    manager->registerBindable({
        "settings-graphics",
        "Graphics",
        "Opens the graphics menu in the settings menu",
        { Keybind::create(KEY_G, Modifier::None) },
        "Settings Menu"
    });

    manager->registerBindable({
        "settings-rate",
        "Rate",
        "Opens the rate menu in the settings menu",
        { Keybind::create(KEY_R, Modifier::None) },
        "Settings Menu"
    });

    manager->registerBindable({
        "settings-songs",
        "Songs",
        "Opens the songs menu in the settings menu",
        { Keybind::create(KEY_S, Modifier::None) },
        "Settings Menu"
    });

    manager->registerBindable({
        "settings-help",
        "Help",
        "Opens the help menu in the settings menu",
        { Keybind::create(KEY_H, Modifier::None) },
        "Settings Menu"
    });

    manager->registerBindable({
        "search-level",
        "Search Level",
        "Searches for levels in the search menu",
        { Keybind::create(KEY_Enter, Modifier::None) },
        "Search Menu"
    });

    manager->registerBindable({
        "search-user",
        "Search User",
        "Searches for users in the search menu",
        { Keybind::create(KEY_Enter, Modifier::Control) },
        "Search Menu"
    });

    manager->registerBindable({
        "search-clear",
        "Clear Search",
        "Clears the current search in the search menu",
        { Keybind::create(KEY_Enter, Modifier::Control) },
        "Search Menu"
    });

    manager->registerBindable({
        "quick-friends",
        "Friends",
        "Searches for levels made by your friends in the search menu",
        { Keybind::create(KEY_F, Modifier::Control) },
        "Search Menu"
    });

    manager->registerBindable({
        "quick-followed",
        "Followed",
        "Searches for levels made by people you follow in the search menu",
        { Keybind::create(KEY_F, Modifier::Control | Modifier::Shift) },
        "Search Menu"
    });

    manager->registerBindable({
        "quick-downloaded",
        "Most Downloaded",
        "Searches for the most downloaded levels in the search menu",
        { Keybind::create(KEY_D, Modifier::Control) },
        "Search Menu"
    });

    manager->registerBindable({
        "quick-liked",
        "Most Liked",
        "Searches for the most liked levels in the search menu",
        { Keybind::create(KEY_L, Modifier::Control) },
        "Search Menu"
    });

    manager->registerBindable({
        "quick-sent",
        "Sent",
        "Searches for levels sent by moderators in the search menu",
        { Keybind::create(KEY_S, Modifier::Control) },
        "Search Menu"
    });

    manager->registerBindable({
        "quick-trending",
        "Trending",
        "Searches for trending levels in the search menu",
        { Keybind::create(KEY_T, Modifier::Control) },
        "Search Menu"
    });

    manager->registerBindable({
        "quick-recent",
        "Recent",
        "TO THE RECENT TAAAAAAB -evw",
        { Keybind::create(KEY_R, Modifier::Control) },
        "Search Menu"
    });

    manager->registerBindable({
        "quick-magic",
        "Magic",
        "Searches for fancy potentially rateworthy levels in the search menu",
        { Keybind::create(KEY_M, Modifier::Control) },
        "Search Menu"
    });

    manager->registerBindable({
        "quick-awarded",
        "Awarded",
        "Searches for rated levels in the search menu",
        { Keybind::create(KEY_A, Modifier::Control) },
        "Search Menu"
    });
}

// main menu hook

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

// icon kit hook

class $modify(MyGarageLayer, GJGarageLayer) {
    bool init() {
        if (!GJGarageLayer::init()) return false;
        kbutil::resetAll();
        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJGarageLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "prev-page-menu", "prev-button");
            return ListenerResult::Propagate;
        }, "icon-kit-prev-page");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJGarageLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "next-page-menu", "next-button");
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

// created levels hook

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

// level edit page hook

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

// level select hook

class $modify(MyLevelSelectLayer, LevelSelectLayer) {
    bool init(int page) {
        if (!LevelSelectLayer::init(page)) return false;
        kbutil::resetAll();

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<LevelSelectLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;

            if (kbutil::pressedOnce("select-play", event)) clickCurrentLevel(this);
            return ListenerResult::Propagate;
        }, "select-play");

        return true;
    }
};

// tower hook

class $modify(MyLevelAreaLayer, LevelAreaLayer) {
    bool init() {
        if (!LevelAreaLayer::init()) return false;
        kbutil::resetAll();

        auto sceneRoot = []() -> CCNode* {
            auto scene = CCDirector::sharedDirector()->getRunningScene();
            return scene ? static_cast<CCNode*>(scene) : nullptr;
        };

        this->template addEventListener<InvokeBindFilter>([sceneRoot](InvokeBindEvent* event) {
            if (kbutil::pressedOnce("select-tower", event)) clickButton(sceneRoot(), "enter-menu", "enter-btn");
            return ListenerResult::Propagate;
        }, "select-tower");

        return true;
    }
};

// tower hook 2: electric boogaloo

class $modify(MyLevelAreaInnerLayer, LevelAreaInnerLayer) {
    bool init(bool returning) {
        if (!LevelAreaInnerLayer::init(returning)) return false;
        kbutil::resetAll();

        auto sceneRoot = []() -> CCNode* {
            auto scene = CCDirector::sharedDirector()->getRunningScene();
            return scene ? static_cast<CCNode*>(scene) : nullptr;
        };
        this->template addEventListener<InvokeBindFilter>([sceneRoot](InvokeBindEvent* event) {
            if (kbutil::pressedOnce("select-tower", event)) clickButton(sceneRoot(), "enter-menu", "enter-btn");
            return ListenerResult::Propagate;
        }, "select-tower");

        this->template addEventListener<InvokeBindFilter>([sceneRoot](InvokeBindEvent* event) {
            if (kbutil::pressedOnce("tower-open-5001", event)) clickButton(sceneRoot(), "main-menu", "level-5001-button");
            return ListenerResult::Propagate;
        }, "tower-open-5001");

        this->template addEventListener<InvokeBindFilter>([sceneRoot](InvokeBindEvent* event) {
            if (kbutil::pressedOnce("tower-open-5002", event)) clickButton(sceneRoot(), "main-menu", "level-5002-button");
            return ListenerResult::Propagate;
        }, "tower-open-5002");

        this->template addEventListener<InvokeBindFilter>([sceneRoot](InvokeBindEvent* event) {
            if (kbutil::pressedOnce("tower-open-5003", event)) clickButton(sceneRoot(), "main-menu", "level-5003-button");
            return ListenerResult::Propagate;
        }, "tower-open-5003");

        this->template addEventListener<InvokeBindFilter>([sceneRoot](InvokeBindEvent* event) {
            if (kbutil::pressedOnce("tower-open-5004", event)) clickButton(sceneRoot(), "main-menu", "level-5004-button");
            return ListenerResult::Propagate;
        }, "tower-open-5004");

        return true;
    }
};

// quests hook

class $modify(MyChallengesPage, ChallengesPage) {
    bool init() {
        if (!ChallengesPage::init()) return false;
        kbutil::resetAll();

        auto claimQuest = [this](std::string const& questNodeID) {
            if (!nodeLooksAlive(this)) return;
            auto mainLayer = this->getChildByID("main-layer");
            if (!mainLayer) return;

            auto questNode = mainLayer->getChildByID(questNodeID);
            if (questNode) clickButtonRecursive(questNode, "claim-menu", "claim-button");
        };

        this->template addEventListener<InvokeBindFilter>([this, claimQuest](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ChallengesPage>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;

            if (event->isDown()) {
                claimQuest("top-quest");
                claimQuest("middle-quest");
                claimQuest("bottom-quest");
            }
            return ListenerResult::Propagate;
        }, "quests-claim-all");

        this->template addEventListener<InvokeBindFilter>([this, claimQuest](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ChallengesPage>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) claimQuest("top-quest");
            return ListenerResult::Propagate;
        }, "quests-claim-top");

        this->template addEventListener<InvokeBindFilter>([this, claimQuest](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ChallengesPage>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) claimQuest("middle-quest");
            return ListenerResult::Propagate;
        }, "quests-claim-mid");

        this->template addEventListener<InvokeBindFilter>([this, claimQuest](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ChallengesPage>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) claimQuest("bottom-quest");
            return ListenerResult::Propagate;
        }, "quests-claim-bot");

        return true;
    }
};

// gauntlet select hook

class $modify(MyGauntletSelectLayer, GauntletSelectLayer) {
    bool init(int p0) {
        if (!GauntletSelectLayer::init(p0)) return false;
        kbutil::resetAll();

        auto canOperate = [this]() {
            auto front = getFrontmostLayerOfType<GauntletSelectLayer>();
            if (front != this) return false;

            return nodeIsRunningVisible(this)
                && this->m_scrollLayer != nullptr
                && nodeIsRunningVisible(this->m_scrollLayer);
        };

        auto activateGauntletAtIndex = [this, canOperate](int idxZeroBased) {
            if (!canOperate()) return;

            int pageNum = this->m_scrollLayer->m_page;
            auto page = this->m_scrollLayer->getPage(pageNum);
            if (!nodeIsRunningVisible(page)) return;

            geode::Ref<CCNode> pageHold = page;

            std::vector<CCMenuItemSpriteExtra*> items;
            collectMenuItems(page, items);

            items.erase(
                std::remove_if(items.begin(), items.end(), [](CCMenuItemSpriteExtra* it) {
                    if (!it) return true;
                    if (!nodeIsRunningVisible(it)) return true;


                    auto parent = it->getParent();
                    if (!nodeIsRunningVisible(parent)) return true;


                    if (!it->isVisible()) return true;
                    if (!it->isEnabled()) return true;
                    return !typeinfo_cast<CCMenu*>(parent);
                }),
                items.end()
            );

            if (items.empty()) return;

            std::sort(items.begin(), items.end(), [](CCMenuItemSpriteExtra* a, CCMenuItemSpriteExtra* b) {
                if (!a || !b) return false;
                if (a->getPositionX() == b->getPositionX())
                    return a->getPositionY() > b->getPositionY();
                return a->getPositionX() < b->getPositionX();
            });

            if (idxZeroBased < 0 || idxZeroBased >= static_cast<int>(items.size())) return;

            auto target = items[idxZeroBased];
            if (!nodeIsRunningVisible(target)) return;
            kbutil::resetAll();
            target->activate();
        };

        this->template addEventListener<InvokeBindFilter>([this, canOperate](InvokeBindEvent* e) {
            if (!canOperate()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("gauntlet-left", e) && this->m_leftButton && nodeIsRunningVisible(this->m_leftButton)) {
                kbutil::resetAll();
                this->m_leftButton->activate();
            }
            return ListenerResult::Propagate;
        }, "gauntlet-left");

        this->template addEventListener<InvokeBindFilter>([this, canOperate](InvokeBindEvent* e) {
            if (!canOperate()) return ListenerResult::Propagate;
            if (kbutil::pressedOnce("gauntlet-right", e) && this->m_rightButton && nodeIsRunningVisible(this->m_rightButton)) {
                kbutil::resetAll();
                this->m_rightButton->activate();
            }
            return ListenerResult::Propagate;
        }, "gauntlet-right");

        this->template addEventListener<InvokeBindFilter>([activateGauntletAtIndex](InvokeBindEvent* e) {
            if (kbutil::pressedOnce("gauntlet-1", e)) activateGauntletAtIndex(0);
            return ListenerResult::Propagate;
        }, "gauntlet-1");

        this->template addEventListener<InvokeBindFilter>([activateGauntletAtIndex](InvokeBindEvent* e) {
            if (kbutil::pressedOnce("gauntlet-2", e)) activateGauntletAtIndex(1);
            return ListenerResult::Propagate;
        }, "gauntlet-2");

        this->template addEventListener<InvokeBindFilter>([activateGauntletAtIndex](InvokeBindEvent* e) {
            if (kbutil::pressedOnce("gauntlet-3", e)) activateGauntletAtIndex(2);
            return ListenerResult::Propagate;
        }, "gauntlet-3");

        this->template addEventListener<InvokeBindFilter>([activateGauntletAtIndex](InvokeBindEvent* e) {
            if (kbutil::pressedOnce("gauntlet-4", e)) activateGauntletAtIndex(3);
            return ListenerResult::Propagate;
        }, "gauntlet-4");

        this->template addEventListener<InvokeBindFilter>([activateGauntletAtIndex](InvokeBindEvent* e) {
            if (kbutil::pressedOnce("gauntlet-5", e)) activateGauntletAtIndex(4);
            return ListenerResult::Propagate;
        }, "gauntlet-5");

        return true;
    }
};

// gauntlet hook

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

// profile page hook

class $modify(MyProfilePage, ProfilePage) {
    bool init(int accountID, bool ownProfile) {
        if (!ProfilePage::init(accountID, ownProfile)) return false;
        
        kbutil::resetAll();
        
        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ProfilePage>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "bottom-menu", "message-button");
            return ListenerResult::Propagate;
        }, "profile-message");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ProfilePage>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "bottom-menu", "friend-button");
            return ListenerResult::Propagate;
        }, "profile-friend");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ProfilePage>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "bottom-menu", "requests-button");
            return ListenerResult::Propagate;
        }, "profile-requests");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ProfilePage>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButton(this, "bottom-menu", "settings-button");
            return ListenerResult::Propagate;
        }, "profile-settings");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ProfilePage>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButtonDirect(this, "next-page-button");
            return ListenerResult::Propagate;
        }, "profile-next-page");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ProfilePage>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButtonDirect(this, "prev-page-button");
            return ListenerResult::Propagate;
        }, "profile-prev-page");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ProfilePage>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButtonDirect(this, "comment-button");
            return ListenerResult::Propagate;
        }, "profile-comment");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ProfilePage>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButtonDirect(this, "comment-history-button");
            return ListenerResult::Propagate;
        }, "profile-comment-history");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ProfilePage>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButtonDirect(this, "refresh-button");
            return ListenerResult::Propagate;
        }, "profile-refresh");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<ProfilePage>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) clickButtonDirect(this, "info-button");
            return ListenerResult::Propagate;
        }, "profile-info");

        return true;
    }
};

// settings hook

class $modify(MyDropDownLayer, GJDropDownLayer) {
    bool init(char const* title, float unk1, bool unk2) {
        if (!GJDropDownLayer::init(title, unk1, unk2)) return false;
        
        kbutil::resetAll();

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "options-menu", "account-button");
            }
            return ListenerResult::Propagate;
        }, "settings-account");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "options-menu", "how-to-play-button");
            }
            return ListenerResult::Propagate;
        }, "settings-how-to-play");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "options-menu", "options-button");
            }
            return ListenerResult::Propagate;
        }, "settings-options");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "options-menu", "graphics-button");
            }
            return ListenerResult::Propagate;
        }, "settings-graphics");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "options-menu", "rate-button");
            }
            return ListenerResult::Propagate;
        }, "settings-rate");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "options-menu", "songs-button");
            }
            return ListenerResult::Propagate;
        }, "settings-songs");

        this->template addEventListener<InvokeBindFilter>([this](InvokeBindEvent* event) {
            if (!kbutil::isLayerActive<GJDropDownLayer>()) return ListenerResult::Propagate;
            if (!nodeLooksAlive(this)) return ListenerResult::Propagate;
            if (event->isDown()) {
                clickButton(this, "options-menu", "help-button");
            }
            return ListenerResult::Propagate;
        }, "settings-help");

        return true;
    }
};

class $modify(MyLevelSearchLayer, LevelSearchLayer) {
    struct Fields {
        bool m_ctrlHeld = false;
        bool m_shiftHeld = false;
    };

    bool init(int p0) {
        log::info("MyLevelSearchLayer::init called!");
        if (!LevelSearchLayer::init(p0)) return false;
        kbutil::resetAll();

        // 2. Also enable it here just in case
        this->setKeyboardEnabled(true);
        log::info("MyLevelSearchLayer::init - set keyboard enabled");
        
        // Schedule the auto-focus for the next frame to ensure full initialization
        // Use a longer delay (0.05s instead of 0.01s) to avoid conflicts with keybind event processing
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
        
        auto bindQuick = [this, guard](std::string const& bindID, std::string const& btnID) {
            this->template addEventListener<InvokeBindFilter>([this, guard, btnID](InvokeBindEvent* event) {
                if (!guard()) return ListenerResult::Propagate;
                if (event->isDown()) clickButton(this, "quick-search-menu", btnID);
                return ListenerResult::Propagate;
            }, bindID);
        };

        bindQuick("quick-downloaded", "most-downloaded-button");
        bindQuick("quick-liked",      "most-liked-button");
        bindQuick("quick-sent",       "sent-button");
        bindQuick("quick-trending",   "trending-button");
        bindQuick("quick-recent",     "recent-button");
        bindQuick("quick-magic",      "magic-button");
        bindQuick("quick-awarded",    "awarded-button");
        bindQuick("quick-followed",   "followed-button");
        bindQuick("quick-friends",    "friends-button");

        return true;
    }
    
    void doAutoFocus(float) {
        log::info("doAutoFocus: Attempting to auto-focus search-bar...");
        auto searchBar = typeinfo_cast<CCTextInputNode*>(this->getChildByID("search-bar"));
        if (searchBar) {
            log::info("doAutoFocus: Found search-bar, setting m_selected=true");
            searchBar->m_selected = true;
            
            // Try to attach IME to the internal text field
            auto textField = findFirstOfTypeRecursive<CCTextFieldTTF>(searchBar);
            if (textField) {
                log::info("doAutoFocus: Found text field, calling attachWithIME()");
                textField->attachWithIME();
            }
        } else {
            log::info("doAutoFocus: ERROR - search-bar not found!");
        }
    }

    void keyUp(enumKeyCodes key) {
        if (key == KEY_Control) m_fields->m_ctrlHeld = false;
        if (key == KEY_Shift)   m_fields->m_shiftHeld = false;
        LevelSearchLayer::keyUp(key);
    }

    void keyDown(enumKeyCodes key) {
        // Pass through to parent
        LevelSearchLayer::keyDown(key);
    }
};