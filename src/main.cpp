#include <Geode/Geode.hpp>
#include <fmt/format.h>
#include <geode.custom-keybinds/include/Keybinds.hpp>
#include <array>
#include "Helpers.hpp"
#include "KeybindUtil.hpp"

using namespace geode::prelude;
using namespace keybinds;

//keybind registrations

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
        { Keybind::create(KEY_Delete, Modifier::Control) },
        "Search Menu"
    });
}
