#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/binding/CustomListView.hpp>
#include <Geode/binding/LevelCell.hpp>
#include <Geode/binding/LevelListCell.hpp>
#include <Geode/binding/MapPackCell.hpp>
#include <vector>
#include <string>
#include <limits>
#include <functional>

using namespace geode::prelude;
using namespace cocos2d;

// Safe node utilities

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

// Button manipulation

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
    if (!btn) return;

    if (auto item = typeinfo_cast<CCMenuItemSpriteExtra*>(btn)) {
        if (item->isEnabled() && item->isVisible()) item->activate();
    }
}
