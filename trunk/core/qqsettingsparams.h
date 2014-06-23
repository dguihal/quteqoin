#ifndef QQSETTINGSPARAMS_H
#define QQSETTINGSPARAMS_H

#include "core/qqtypes.h"

#include <QtGlobal>

#ifdef Q_OS_UNIX
const char notif_name[] = "QuteQoin";
#endif

#define MAX_CACHE_AGE_DAYS 60

#define SETTINGS_TOTOZ_PRESETS_PATH ":/totoz"

// MAIN WINDOW
#define SETTINGS_MAINWINDOW_STATE "mainwindow_state"
#define SETTINGS_MAINWINDOW_GEOMETRY "mainwindow_geometry"

// GENERAL
#define SETTINGS_GENERAL_MAX_HISTLEN "max_hist_len"
#define DEFAULT_GENERAL_MAX_HISTLEN 500

#define SETTINGS_GENERAL_DEFAULT_UA "default_ua"
#define DEFAULT_GENERAL_DEFAULT_UA "quteqoin crash edition"

#define SETTINGS_GENERAL_DEFAULT_LOGIN "default_login"
#define DEFAULT_GENERAL_DEFAULT_LOGIN ""

#define SETTINGS_GENERAL_WEBSEARCH_URL "default_websearch_url"
#define DEFAULT_GENERAL_WEBSEARCH_URL "http://www.google.fr/search?q=%s&ie=utf-8&oe=utf-8"

#define SETTINGS_GENERAL_DEFAULT_FONT "default_font"
#define DEFAULT_GENERAL_DEFAULT_FONT QFont().toString()

#define SETTINGS_GENERAL_HIGHLIGHT_COLOR "highlight_color"
#define DEFAULT_GENERAL_HIGHLIGHT_COLOR "#FFE940"

// TOTOZ
#define TOTOZ_BOOKMARKS_FILE "totozBookmarks"

//Temporaire
#define SETTINGS_TOTOZ_BOOKMARKLIST "totoz_bookmarklist"

#define SETTINGS_TOTOZ_SERVER_URL "totoz_server_url"
#define DEFAULT_TOTOZ_SERVER_URL "http://totoz.eu"

#define SETTINGS_TOTOZ_SERVER_BASE_IMG "totoz_server_base_img"
#define DEFAULT_TOTOZ_SERVER_BASE_IMG "img"

#define SETTINGS_TOTOZ_SERVER_NAME_SUFFIX "totoz_server_name_suffix"
#define DEFAULT_TOTOZ_SERVER_NAME_SUFFIX ""

#define SETTINGS_TOTOZ_SERVER_ALLOW_SEARCH "totoz_serveur_allow_search"
#define DEFAULT_TOTOZ_SERVER_ALLOW_SEARCH true

#define SETTINGS_TOTOZ_SERVER_QUERY_PATTERN "totoz_query_pattern"
#define DEFAULT_TOTOZ_SERVER_QUERY_PATTERN "search.xml?terms=%t&offset=%o"

#define SETTINGS_TOTOZ_VISUAL_MODE "totoz_visual_mode"
#define TOTOZ_VISUAL_MODE_DISABLED "Disabled"
#define TOTOZ_VISUAL_MODE_INLINE "Inline"
#define TOTOZ_VISUAL_MODE_ON_HOVER "On hover"
//#define DEFAULT_TOTOZ_VISUAL_MODES (QStringList() << TOTOZ_VISUAL_MODE_DISABLED << TOTOZ_VISUAL_MODE_INLINE << TOTOZ_VISUAL_MODE_ON_HOVER)
#define DEFAULT_TOTOZ_VISUAL_MODES (QStringList() << TOTOZ_VISUAL_MODE_DISABLED << TOTOZ_VISUAL_MODE_ON_HOVER)
#define DEFAULT_TOTOZ_VISUAL_MODE TOTOZ_VISUAL_MODE_ON_HOVER

#ifdef Q_OS_UNIX
#define SETTINGS_BIGORNOTIFY_ENABLED "bigornotify_enabled"
#define DEFAULT_BIGORNOTIFY_ENABLED true
#endif

#define SETTINGS_WEB_IMAGE_VIEWER_ENABLED "web_image_viewer_enabled"
#define DEFAULT_WEB_IMAGE_VIEWER_ENABLED true

#define SETTINGS_WEB_IMAGE_PREVIEW_SIZE "web_image_preview_size"
#define DEFAULT_WEB_IMAGE_PREVIEW_SIZE 100

// BOUCHOTS
#define BOUCHOTS_SPLIT_CHAR ';'

#define SETTINGS_LIST_BOUCHOTS "bouchots"

#define SETTINGS_BOUCHOT_COLOR "color"
#define SETTINGS_BOUCHOT_ALIASES "aliases"
#define SETTINGS_BOUCHOT_REFRESH "refresh"
#define DEFAULT_BOUCHOT_REFRESH "30"
#define SETTINGS_BOUCHOT_LOGIN "login"
#define SETTINGS_BOUCHOT_COOKIES "cookies"
#define SETTINGS_BOUCHOT_UA "ua"
#define SETTINGS_BOUCHOT_BACKENDURL "backendUrl"
#define SETTINGS_BOUCHOT_POSTURL "postUrl"
#define SETTINGS_BOUCHOT_POSTDATA "postData"
#define SETTINGS_BOUCHOT_GROUP "group"
#define SETTINGS_BOUCHOT_SLIPTYPE "slipType"
#define SETTINGS_BOUCHOT_STRICTHTTPSCERTIF "strictHttpsCertif"

// PALMI
#define SETTINGS_PALMI_MINI "palmi_minimized"
#define DEFAULT_PALMI_MINI false
#define SETTINGS_PALMI_SHORCUTS "palmi_shortcuts"
#define SETTINGS_PALMI_SHORTCUTS_MIN_KEY Qt::Key_Space
#define SETTINGS_PALMI_SHORTCUTS_MAX_KEY Qt::Key_AsciiTilde

// FILTERS
#define SETTINGS_FILTER_SMART_URL_TRANSFORMER "filter_smart_url_transformer"
#define DEFAULT_FILTER_SMART_URL_TRANSFORMER true

#define SETTINGS_FILTER_SMART_URL_TRANSFORM_TYPE "filter_smart_url_transform_type"
#define DEFAULT_FILTER_SMART_URL_TRANSFORM_TYPE QuteQoin::Shorter

// HUNT
#define SETTINGS_HUNT_MODE "hunt_mode"
#define DEFAULT_HUNT_MODE QuteQoin::HuntMode_Manual

#define SETTINGS_SL_HUNT_MODE "sl_hunt_mode"
#define DEFAULT_SL_HUNT_MODE QuteQoin::SLHuntMode_Disabled

#define SETTINGS_HUNT_SILENT_ENABLED "hunt_silent_enabled"
#define DEFAULT_HUNT_SILENT_ENABLED false

#define SETTINGS_HUNT_MAX_ITEMS "hunt_max_items"
#define DEFAULT_HUNT_MAX_ITEMS 5
#endif // QQSETTINGSPARAMS_H
