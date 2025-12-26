#pragma once

#include <geode.custom-keybinds/include/Keybinds.hpp>
#include <unordered_set>
#include <string>

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
