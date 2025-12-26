#include <Geode/modify/GauntletSelectLayer.hpp>
#include <algorithm>
#include "Helpers.hpp"
#include "KeybindUtil.hpp"

using namespace geode::prelude;
using namespace keybinds;

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
