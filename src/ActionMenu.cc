//
// ActionMenu.cc for pekwm
// Copyright © 2002-2009 Claes Nästén <me@pekdon.net>
//
// This program is licensed under the GNU GPL.
// See the LICENSE file for more information.
//

#include "config.h"

#include <cstdio>
#include <iostream>
#include <set>

#include "PWinObj.hh"
#include "PDecor.hh"
#include "PMenu.hh"
#include "WORefMenu.hh"
#include "ActionMenu.hh"
#include "TextureHandler.hh"
#include "ImageHandler.hh"

#include "Config.hh"
#include "ActionHandler.hh"
#include "Client.hh"
#include "Frame.hh"
#include "WindowManager.hh"
#include "Util.hh"

using std::cerr;
using std::endl;
using std::find;
using std::string;
using std::wstring;

//! @brief ActionMenu constructor
//! @param type Type of menu
//! @param title Title of menu
//! @param name Name of the menu, empty for dynamic else should be unique
//! @param decor_name Name of decor to use, defaults to MENU.
ActionMenu::ActionMenu(MenuType type,
                       const std::wstring &title, const std::string &name,
                       const std::string &decor_name) :
    WORefMenu(WindowManager::instance()->getTheme(), title, name, decor_name),
    _act(WindowManager::instance()->getActionHandler()),
    _has_dynamic(false)
{
    // when creating dynamic submenus, this needs to be initialized as
    // dynamic inserting will be done
    _insert_at = 0;
    _menu_type = type;

    if (_menu_type == WINDOWMENU_TYPE) {
        _action_ok = WINDOWMENU_OK;
    } else if ((_menu_type == ROOTMENU_TYPE) || (_menu_type == ROOTMENU_STANDALONE_TYPE)) {
        _action_ok = ROOTMENU_OK;
    }
}

//! @brief ActionMenu destructor
ActionMenu::~ActionMenu(void)
{
    removeAll();
}

// START - PWinObj interface.

//! @brief Rebuilds the vmenu and if it has any items after it shows it.
void
ActionMenu::mapWindow(void)
{
    // find and rebuild the dynamic entries
    if (! isMapped() && _has_dynamic) {
        uint size_before = _items.size();
        rebuildDynamic();
        if (size_before != _items.size()) {
            buildMenu();
        }
    }

    PMenu::mapWindow();
}

//! @brief Hides and removes all items created by dynamic entries.
void
ActionMenu::unmapWindow(void)
{
    if (! isMapped()) {
        return;
    }

    if (_has_dynamic) {
        removeDynamic();
    }

    PMenu::unmapWindow();
}

// END - PWinObj interface.

//! @brief Handle item exec.
void
ActionMenu::handleItemExec(PMenu::Item *item)
{
    if (! item) {
        return;
    }

    ActionPerformed ap(getWORef(), item->getAE());
    _act->handleAction(ap);
}


/**
 * Re-reads the configuration clearing old entries from the menu. This
 * adds the ICON_PATH to the ImageHandler before loading to support
 * loading icons properly.
 *
 * @param section CfgParser::Entry with menu configuration
 */
void
ActionMenu::reload(CfgParser::Entry *section)
{
    // Clear menu
    removeAll();

    // Parse section (if any)
    ImageHandler::instance()->path_push_back(Config::instance()->getSystemIconPath());
    ImageHandler::instance()->path_push_back(Config::instance()->getIconPath());
    _insert_at = 0;
    parse(section);
    ImageHandler::instance()->path_pop_back();
    ImageHandler::instance()->path_pop_back();

    // Build menu from parsed content
    buildMenu();
}

//! @brief Checks if we have a position set for where to insert.
void
ActionMenu::insert(PMenu::Item *item)
{
    if (! item) {
        return;
    }

    checkItemWORef(item);

    _items.insert(_items.begin() + _insert_at, item);
    ++_insert_at;
}

//! @brief Removes a BaseMenuItem from the menu
void
ActionMenu::remove(PMenu::Item *item)
{
    if (! item) {
        return;
    }

    if (item->getIcon()) {
        TextureHandler::instance()->returnTexture(item->getIcon());
    }

    if (item->getWORef() && (item->getWORef()->getType() == WO_MENU)) {
        delete item->getWORef();
    }

    PMenu::remove(item);
}

//! @brief Removes all items from the menu
void
ActionMenu::removeAll(void)
{
    while (! _items.empty()) {
        remove(_items.back());
    }
    _item_curr = 0;
}

//! @brief Parse config and push items into menu
//! @param cs Section object to read config from
//! @param menu BaseMenu object to push object in
//! @param has_dynamic If true the menu being parsed is dynamic, defaults to false.
void
ActionMenu::parse(CfgParser::Entry *section, PMenu::Item *parent)
{
    if (! section) {
        return;
    }

    if (section->getValue().size()) {
        wstring title(Util::to_wide_str(section->getValue()));
        setTitle(title);
        _title_base = title;
    }

    CfgParser::Entry *value;
    ActionEvent ae;

    ActionMenu *submenu = 0;
    PMenu::Item *item = 0;
    PTexture *icon = 0;

    CfgParser::iterator it(section->begin());
    for (; it != section->end(); ++it) {
        item = 0;

        if (*(*it) == "SUBMENU") {
            CfgParser::Entry *sub_section = (*it)->getSection();
            if (sub_section) {
                icon = getIcon(sub_section->findEntry("ICON"));

                submenu = new ActionMenu(_menu_type, Util::to_wide_str((*it)->getValue()), (*it)->getValue());
                submenu->_menu_parent = this;
                submenu->parse(sub_section, parent);
                submenu->buildMenu();

                item = new PMenu::Item(Util::to_wide_str(sub_section->getValue()), submenu, icon);
                item->setCreator(parent);
            } else {
                cerr << " *** WARNING: submenu entry does not contain any section." << endl;
            }
        } else if (*(*it) == "SEPARATOR") {
            // No icon support on separators.
            item = new PMenu::Item(L"", 0, 0);
            item->setType(PMenu::Item::MENU_ITEM_SEPARATOR);
            item->setCreator(parent);

        } else {
            CfgParser::Entry *sub_section = (*it)->getSection();
            if (sub_section) {
                // Inside of the Entry = "foo" { ... } section, here
                // Actions and Icon are the valid options.
                value = sub_section->findEntry("ACTIONS");
                if (value && Config::instance()->parseActions(value->getValue(), ae, _action_ok)) {
                    icon = getIcon(sub_section->findEntry("ICON"));

                    item = new PMenu::Item(Util::to_wide_str(sub_section->getValue()), 0, icon);
                    item->setCreator(parent);
                    item->setAE(ae);

                    if (ae.isOnlyAction(ACTION_MENU_DYN)) {
                        _has_dynamic = true;
                        item->setType(PMenu::Item::MENU_ITEM_HIDDEN);
                    }
                }
            }
        }

        // If an item was successfully created, insert it to the menu.
        if (item) {
            insert(item);
        }
    }
}

/**
 * Get icon texture from parser value.
 *
 * @param value Entry to get icon name from.
 * @return PTexture if icon was loaded, else 0.
 */
PTexture*
ActionMenu::getIcon(CfgParser::Entry *value)
{
    // Skip blank icons
    if (! value || ! value->getValue().size()) {
        return 0;
    }

    PTexture *icon = 0;

    // Try to load the icon as a complete texture specification, if
    // that fails load is an scaled image.
    icon = TextureHandler::instance()->getTexture(value->getValue());
    if (! icon) {
        icon = TextureHandler::instance()->getTexture("IMAGE " + value->getValue() + "#SCALED");
    }

    return icon;
}

/**
 * Executes all Dynamic entries in the menu.
 */
void
ActionMenu::rebuildDynamic(void)
{
    PWinObj *wo_ref = getWORef();

    // Export environment before to dynamic script.
    Client *client = 0;
    if (wo_ref && wo_ref->getType() == WO_CLIENT) {
        client = static_cast<Client*>(wo_ref);
    }
    Client::setClientEnvironment(client);

    // Setup icon path before parsing.
    ImageHandler::instance()->path_push_back(Config::instance()->getSystemIconPath());
    ImageHandler::instance()->path_push_back(Config::instance()->getIconPath());

    PMenu::Item* item = 0;
    vector<PMenu::Item*>::iterator it;
    for (it = _items.begin(); it != _items.end(); ++it) {
        if ((*it)->getAE().isOnlyAction(ACTION_MENU_DYN)) {
            _insert_at = it - _items.begin();

            item = *it;

            CfgParser dynamic;
            if (dynamic.parse((*it)->getAE().action_list.front().getParamS(),
                              CfgParserSource::SOURCE_COMMAND)) {
                _has_dynamic = true;
                parse(dynamic.getEntryRoot()->findSection("DYNAMIC"), *it);
            }

            it = find(_items.begin(), _items.end(), item);
        }
    }
    _insert_at = _items.size();

    // Cleanup icon path
    ImageHandler::instance()->path_pop_back();
    ImageHandler::instance()->path_pop_back();
}

//! @brief Remove all entries from the menu created by dynamic entries.
void
ActionMenu::removeDynamic(void)
{
    std::set<PMenu::Item *> dynlist;

    vector<PMenu::Item*>::iterator it(_items.begin());
    for (; it != _items.end(); ++it) {
        if ((*it)->getType() == PMenu::Item::MENU_ITEM_HIDDEN) {
            dynlist.insert(*it);
        }
    }

    it = _items.begin();
    for (; it != _items.end();) {
        if (dynlist.find((*it)->getCreator()) != dynlist.end()) {
            if ((*it)->getWORef() && ((*it)->getWORef()->getType() == WO_MENU)) {
                delete (*it)->getWORef();
            }
            delete (*it);
            it = _items.erase(it);
        } else {
            ++it;
        }
    }
}
