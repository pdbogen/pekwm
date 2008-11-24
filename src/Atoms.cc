//
// Atoms.cc for pekwm
// Copyright © 2003-2008 Claes Nästén <me@pekdon.net>
//
// This program is licensed under the GNU GPL.
// See the LICENSE file for more information.
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream>

#include "Atoms.hh"
#include "PScreen.hh"
#include "Util.hh"

extern "C" {
#include <X11/Xutil.h>
}

using std::cerr;
using std::endl;
using std::string;
using std::map;

PekwmAtoms* PekwmAtoms::_instance = 0;
IcccmAtoms* IcccmAtoms::_instance = 0;
EwmhAtoms* EwmhAtoms::_instance = 0;
MiscAtoms* MiscAtoms::_instance = 0;

//! @brief PekwmAtoms constructor
PekwmAtoms::PekwmAtoms(void)
{
    if (_instance)
        throw string("PekwmAtoms, trying to create multiple instances");
    _instance = this;

    // pekwm atoms
    const char *names[] = {
                        "_PEKWM_FRAME_ID",
                        "_PEKWM_FRAME_DECOR",
                        "_PEKWM_FRAME_SKIP",
                        "_PEKWM_TITLE"
                    };
    const uint num = sizeof(names) / sizeof(char*);
    Atom atoms[num];

    XInternAtoms(PScreen::instance()->getDpy(),
                 const_cast<char**>(names), num, 0, atoms);

    // Insert all atoms into the _atoms map
    for (uint i = 0; i < num; ++i) {
        _atoms[PekwmAtomName(i)] = atoms[i];
    }
}

//! @brief PekwmAtoms destructor
PekwmAtoms::~PekwmAtoms(void)
{
    _instance = 0;
}

//! @brief IcccmAtoms constructor
IcccmAtoms::IcccmAtoms(void)
{
    if (_instance)
        throw string("IcccmAtoms, trying to create multiple instances");
    _instance = this;

    // ICCCM atoms
    const char *names[] = {
        "WM_NAME",
        "WM_ICON_NAME",
        "WM_STATE",
        "WM_CHANGE_STATE",
        "WM_PROTOCOLS",
        "WM_DELETE_WINDOW",
        "WM_COLORMAP_WINDOWS",
        "WM_TAKE_FOCUS",
        "WM_WINDOW_ROLE",
        "WM_CLIENT_MACHINE"
    };
    const uint num = sizeof(names) / sizeof(char*);
    Atom atoms[num];

    XInternAtoms(PScreen::instance()->getDpy(), const_cast<char**>(names), num, 0, atoms);

    // Insert all atoms into the _atoms map
    for (uint i = 0; i < num; ++i) {
        _atoms[IcccmAtomName(i)] = atoms[i];
    }
}

//! @brief IcccmAtoms destructor
IcccmAtoms::~IcccmAtoms(void)
{
    _instance = 0;
}

//! @brief EwmhAtoms constructor
EwmhAtoms::EwmhAtoms(void)
{
    if (_instance)
        throw string("EwmhAtoms, trying to create multiple instances");
    _instance = this;

    const char *names[] = {
                        "_NET_SUPPORTED",
                        "_NET_CLIENT_LIST", "_NET_CLIENT_LIST_STACKING",
                        "_NET_NUMBER_OF_DESKTOPS",
                        "_NET_DESKTOP_GEOMETRY", "_NET_DESKTOP_VIEWPORT",
                        "_NET_CURRENT_DESKTOP", "_NET_DESKTOP_NAMES",
                        "_NET_ACTIVE_WINDOW", "_NET_WORKAREA",
                        "_NET_DESKTOP_LAYOUT", "_NET_SUPPORTING_WM_CHECK",
                        "_NET_CLOSE_WINDOW",
                        "_NET_WM_NAME", "_NET_WM_VISIBLE_NAME",
                        "_NET_WM_ICON", "_NET_WM_DESKTOP",
                        "_NET_WM_STRUT", "_NET_WM_PID",

                        "_NET_WM_WINDOW_TYPE",
                        "_NET_WM_WINDOW_TYPE_DESKTOP", "_NET_WM_WINDOW_TYPE_DOCK",
                        "_NET_WM_WINDOW_TYPE_TOOLBAR", "_NET_WM_WINDOW_TYPE_MENU",
                        "_NET_WM_WINDOW_TYPE_UTILITY", "_NET_WM_WINDOW_TYPE_SPLASH",
                        "_NET_WM_WINDOW_TYPE_DIALOG", "_NET_WM_WINDOW_TYPE_NORMAL",

                        "_NET_WM_STATE",
                        "_NET_WM_STATE_MODAL", "_NET_WM_STATE_STICKY",
                        "_NET_WM_STATE_MAXIMIZED_VERT", "_NET_WM_STATE_MAXIMIZED_HORZ",
                        "_NET_WM_STATE_SHADED",
                        "_NET_WM_STATE_SKIP_TASKBAR", "_NET_WM_STATE_SKIP_PAGER",
                        "_NET_WM_STATE_HIDDEN", "_NET_WM_STATE_FULLSCREEN",
                        "_NET_WM_STATE_ABOVE", "_NET_WM_STATE_BELOW",

                        "_NET_WM_ALLOWED_ACTIONS",
                        "_NET_WM_ACTION_MOVE", "_NET_WM_ACTION_RESIZE",
                        "_NET_WM_ACTION_MINIMIZE", "_NET_WM_ACTION_SHADE",
                        "_NET_WM_ACTION_STICK",
                        "_NET_WM_ACTION_MAXIMIZE_VERT", "_NET_WM_ACTION_MAXIMIZE_HORZ",
                        "_NET_WM_ACTION_FULLSCREEN", "_NET_WM_ACTION_CHANGE_DESKTOP",
                        "_NET_WM_ACTION_CLOSE",
                        "UTF8_STRING"
                    };
    const uint num = sizeof(names) / sizeof(char*);
    Atom atoms[num];

    XInternAtoms(PScreen::instance()->getDpy(),
                 const_cast<char**>(names), num, 0, atoms);

    // Insert all items into the _atoms map
    for (uint i = 0; i < num; ++i) {
        _atoms[EwmhAtomName(i)] = atoms[i];
    }
}

//! @brief EwmhAtoms destructor
EwmhAtoms::~EwmhAtoms(void)
{
    _instance = 0;
}

//! @brief Builds a array of all atoms in the map.
Atom*
EwmhAtoms::getAtomArray(void) const
{
    Atom *atoms = new Atom[_atoms.size()];

    map<EwmhAtomName, Atom>::const_iterator it = _atoms.begin();
    for (uint i = 0; it != _atoms.end(); ++i, ++it)
        atoms[i] = it->second;

    return atoms;
}

/**
 * MiscAtoms constructor, initializes hints.
 */
MiscAtoms::MiscAtoms(void)
{
    if (_instance) {
        throw string("MiscAtoms, trying to create multiple instances");
    }
    _instance = this;

    // Misc atoms
    const char *names[] = {
        "_MOTIF_WM_HINTS"
    };
    const unsigned int num = sizeof(names) / sizeof(char*);
    Atom atoms[num];

    XInternAtoms(PScreen::instance()->getDpy(), const_cast<char**>(names), num, 0, atoms);

    // Insert all atoms into the _atoms map
    for (unsigned int i = 0; i < num; ++i) {
        _atoms[MiscAtomName(i)] = atoms[i];
    }
}

/**
 * MiscAtoms destructor, clears instance pointer.
 */
MiscAtoms::~MiscAtoms(void)
{
    _instance = 0;
}

namespace AtomUtil {

//! @brief Get unknown property
bool
getProperty(Window win, Atom atom, Atom type, ulong expected, uchar** data, ulong *actual)
{
    Atom r_type;
    int r_format, status;
    ulong read = 0, left = 0;

    *data = 0;
    do {
        if (*data) {
            XFree(*data);
            *data = 0;
        }
        expected += left;

        status =
            XGetWindowProperty(PScreen::instance()->getDpy(), win, atom,
                               0L, expected, False, type,
                               &r_type, &r_format, &read, &left, data);

        if (status != Success || type != r_type || read == 0) {
            if (*data) {
                XFree(*data);
                *data = 0;
            }
            left = 0;
        }
    } while (left);

    if (actual) {
        * actual = read;
    }

    return (*data != 0);
}

//! @brief Set XA_ATOM, one value
void
setAtom(Window win, Atom atom, Atom value)
{
    XChangeProperty(PScreen::instance()->getDpy(), win, atom, XA_ATOM, 32,
                    PropModeReplace, (uchar *) &value, 1);
}

//! @brief Set XA_ATOM, multiple values
void
setAtoms(Window win, Atom atom, Atom *values, int size)
{
    XChangeProperty(PScreen::instance()->getDpy(), win, atom, XA_ATOM, 32,
                    PropModeReplace, (uchar *) values, size);
}

//! @brief Set XA_WINDOW, one value
void
setWindow(Window win, Atom atom, Window value)
{
    XChangeProperty(PScreen::instance()->getDpy(), win, atom, XA_WINDOW, 32,
                    PropModeReplace, (uchar *) &value, 1);
}

//! @brief Set XA_WINDOW, multiple values
void
setWindows(Window win, Atom atom, Window *values, int size)
{
    XChangeProperty(PScreen::instance()->getDpy(), win, atom, XA_WINDOW, 32,
                    PropModeReplace, (uchar *) values, size);
}

//! @brief Get XA_CARDINAL
bool
getLong(Window win, Atom atom, long &value)
{
    long *data = 0;
    uchar *udata = 0;

    if (getProperty(win, atom, XA_CARDINAL, 1L, &udata)) {
        data = reinterpret_cast<long*>(udata);
        value = *data;
        XFree(udata);

        return true;
    }

    return false;
}

//! @brief Set XA_CARDINAL
void
setLong(Window win, Atom atom, long value)
{
    XChangeProperty(PScreen::instance()->getDpy(), win, atom, XA_CARDINAL, 32,
                    PropModeReplace, (uchar *) &value, 1);
}

//! @brief Get XA_STRING property
bool
getString(Window win, Atom atom, string &value)
{
    uchar *data = 0;

    if (getProperty(win, atom, XA_STRING, 64L, &data)) {
        value = string((const char*) data);
        XFree(data);

        return true;
    }

    return false;
}

//! @brief Get UTF-8 string.
bool
getUtf8String(Window win, Atom atom, std::wstring &value)
{
    bool status = false;
    unsigned char *data = 0;

    if (getProperty(win, atom, EwmhAtoms::instance()->getAtom(UTF8_STRING),
                    32, &data)) {
        status = true;

        string utf8_str(reinterpret_cast<char*>(data));
        value = Util::from_utf8_str(utf8_str);
        XFree(data);
    }

    return status;
}

//! @brief Set UTF-8 string.
void
setUtf8String(Window win, Atom atom, const std::wstring &value)
{
    string utf8_string(Util::to_utf8_str(value));

    XChangeProperty(PScreen::instance()->getDpy(), win, atom,
                    EwmhAtoms::instance()->getAtom(UTF8_STRING), 8,
                    PropModeReplace,
                    reinterpret_cast<const uchar*>(utf8_string.c_str()),
                    utf8_string.size());
}

    /**
     * Set array of UTF-8 strings.
     *
     * @param win Window to set string on.
     * @param atom Atom to set array value.
     * @param values Array of strings.
     * @param length Number of elements in value.
     */
    void
    setUtf8StringArray(Window win, Atom atom, unsigned char *values, unsigned int length)
    {
        XChangeProperty(PScreen::instance()->getDpy(), win, atom, EwmhAtoms::instance()->getAtom(UTF8_STRING),
                        8, PropModeReplace, values, length);
    }

//! @brief Set XA_STRING property
void
setString(Window win, Atom atom, const string &value)
{
    XChangeProperty(PScreen::instance()->getDpy(), win, atom, XA_STRING, 8,
                    PropModeReplace, (uchar*) value.c_str(), value.size());
}

/**
 * Read text property.
 *
 * @param win Window to get property from.
 * @param atom Atom holding the property.
 * @param value Return string.
 * @return true if property was successfully read.
 */
bool
getTextProperty(Window win, Atom atom, std::string &value)
{
    // Read text property, return if it fails.
    XTextProperty text_property;
    if (! XGetTextProperty(PScreen::instance()->getDpy(), win, &text_property, atom)
        || ! text_property.value || ! text_property.nitems) {
        return false;
    }

    if (text_property.encoding == XA_STRING) {
        value = reinterpret_cast<const char*>(text_property.value);
    } else {
        char **mb_list;
        int num;

        XmbTextPropertyToTextList(PScreen::instance()->getDpy(), &text_property, &mb_list, &num);
        if (mb_list && num > 0) {
            value = *mb_list;
            XFreeStringList(mb_list);
        }
    }

    XFree(text_property.value);

    return true;
}

//! @brief
void*
getEwmhPropData(Window win, Atom prop, Atom type, int &num)
{
    Atom type_ret;
    int format_ret;
    ulong items_ret, after_ret;
    uchar *prop_data = 0;

    XGetWindowProperty(PScreen::instance()->getDpy(), win, prop, 0, 0x7fffffff,
                       False, type, &type_ret, &format_ret, &items_ret,
                       &after_ret, &prop_data);

    num = items_ret;

    return prop_data;
}

/**
 * Remove property from window.
 */
void
unsetProperty(Window win, Atom prop)
{
    XDeleteProperty(PScreen::instance()->getDpy(), win, prop);
}

} // end namespace AtomUtil
