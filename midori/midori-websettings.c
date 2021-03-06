/*
 Copyright (C) 2008 Christian Dywan <christian@twotoasts.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 See the file COPYING for the full license text.
*/

#include "midori-websettings.h"

#include "sokoke.h"

#include <glib/gi18n.h>
#include <string.h>

struct _MidoriWebSettings
{
    WebKitWebSettings parent_instance;

    gboolean remember_last_window_size;
    gint last_window_width;
    gint last_window_height;
    gint last_panel_position;
    gint last_panel_page;
    gint last_web_search;
    gchar* last_pageholder_uri;

    gboolean show_navigationbar;
    gboolean show_bookmarkbar;
    gboolean show_panel;
    gboolean show_statusbar;

    MidoriToolbarStyle toolbar_style;
    gboolean small_toolbar;
    gboolean show_new_tab;
    gboolean show_homepage;
    gboolean show_web_search;
    gboolean show_trash;

    MidoriStartup load_on_startup;
    gchar* homepage;
    gchar* download_folder;
    gchar* download_manager;
    gchar* location_entry_search;
    MidoriPreferredEncoding preferred_encoding;

    gint tab_label_size;
    gboolean close_buttons_on_tabs;
    MidoriNewPage open_new_pages_in;
    gboolean middle_click_opens_selection;
    gboolean open_tabs_in_the_background;
    gboolean open_popups_in_tabs;

    MidoriAcceptCookies accept_cookies;
    gboolean original_cookies_only;
    gint maximum_cookie_age;

    gboolean remember_last_visited_pages;
    gint maximum_history_age;
    gboolean remember_last_form_inputs;
    gboolean remember_last_downloaded_files;

    gchar* http_proxy;
    gint cache_size;
};

G_DEFINE_TYPE (MidoriWebSettings, midori_web_settings, WEBKIT_TYPE_WEB_SETTINGS)

enum
{
    PROP_0,

    PROP_REMEMBER_LAST_WINDOW_SIZE,
    PROP_LAST_WINDOW_WIDTH,
    PROP_LAST_WINDOW_HEIGHT,
    PROP_LAST_PANEL_POSITION,
    PROP_LAST_PANEL_PAGE,
    PROP_LAST_WEB_SEARCH,
    PROP_LAST_PAGEHOLDER_URI,

    PROP_SHOW_NAVIGATIONBAR,
    PROP_SHOW_BOOKMARKBAR,
    PROP_SHOW_PANEL,
    PROP_SHOW_STATUSBAR,

    PROP_TOOLBAR_STYLE,
    PROP_SMALL_TOOLBAR,
    PROP_SHOW_NEW_TAB,
    PROP_SHOW_HOMEPAGE,
    PROP_SHOW_WEB_SEARCH,
    PROP_SHOW_TRASH,

    PROP_LOAD_ON_STARTUP,
    PROP_HOMEPAGE,
    PROP_DOWNLOAD_FOLDER,
    PROP_DOWNLOAD_MANAGER,
    PROP_LOCATION_ENTRY_SEARCH,
    PROP_PREFERRED_ENCODING,

    PROP_TAB_LABEL_SIZE,
    PROP_CLOSE_BUTTONS_ON_TABS,
    PROP_OPEN_NEW_PAGES_IN,
    PROP_MIDDLE_CLICK_OPENS_SELECTION,
    PROP_OPEN_TABS_IN_THE_BACKGROUND,
    PROP_OPEN_POPUPS_IN_TABS,

    PROP_ACCEPT_COOKIES,
    PROP_ORIGINAL_COOKIES_ONLY,
    PROP_MAXIMUM_COOKIE_AGE,

    PROP_REMEMBER_LAST_VISITED_PAGES,
    PROP_MAXIMUM_HISTORY_AGE,
    PROP_REMEMBER_LAST_FORM_INPUTS,
    PROP_REMEMBER_LAST_DOWNLOADED_FILES,

    PROP_HTTP_PROXY,
    PROP_CACHE_SIZE
};

GType
midori_startup_get_type (void)
{
    static GType type = 0;
    if (!type)
    {
        static const GEnumValue values[] = {
         { MIDORI_STARTUP_BLANK_PAGE, "MIDORI_STARTUP_BLANK_PAGE", N_("Blank page") },
         { MIDORI_STARTUP_HOMEPAGE, "MIDORI_STARTUP_HOMEPAGE", N_("Homepage") },
         { MIDORI_STARTUP_LAST_OPEN_PAGES, "MIDORI_STARTUP_LAST_OPEN_PAGES", N_("Last open pages") },
         { 0, NULL, NULL }
        };
        type = g_enum_register_static ("MidoriStartup", values);
    }
    return type;
}

GType
midori_preferred_encoding_get_type (void)
{
    static GType type = 0;
    if (!type)
    {
        static const GEnumValue values[] = {
         { MIDORI_ENCODING_CHINESE, "MIDORI_ENCODING_CHINESE", N_("Chinese (BIG5)") },
         { MIDORI_ENCODING_JAPANESE, "MIDORI_ENCODING_JAPANESE", N_("Japanese (SHIFT_JIS)") },
         { MIDORI_ENCODING_RUSSIAN, "MIDORI_ENCODING_RUSSIAN", N_("Russian (KOI8-R)") },
         { MIDORI_ENCODING_UNICODE, "MIDORI_ENCODING_UNICODE", N_("Unicode (UTF-8)") },
         { MIDORI_ENCODING_WESTERN, "MIDORI_ENCODING_WESTERN", N_("Western (ISO-8859-1)") },
         { MIDORI_ENCODING_WESTERN, "MIDORI_ENCODING_CUSTOM", N_("Custom...") },
         { 0, NULL, NULL }
        };
        type = g_enum_register_static ("MidoriPreferredEncoding", values);
    }
    return type;
}

GType
midori_new_page_get_type (void)
{
    static GType type = 0;
    if (!type)
    {
        static const GEnumValue values[] = {
         { MIDORI_NEW_PAGE_TAB, "MIDORI_NEW_PAGE_TAB", N_("New tab") },
         { MIDORI_NEW_PAGE_WINDOW, "MIDORI_NEW_PAGE_WINDOW", N_("New window") },
         { MIDORI_NEW_PAGE_CURRENT, "MIDORI_NEW_PAGE_CURRENT", N_("Current tab") },
         { 0, NULL, NULL }
        };
        type = g_enum_register_static ("MidoriNewPage", values);
    }
    return type;
}

GType
midori_toolbar_style_get_type (void)
{
    static GType type = 0;
    if (!type)
    {
        static const GEnumValue values[] = {
         { MIDORI_TOOLBAR_DEFAULT, "MIDORI_TOOLBAR_DEFAULT", N_("Default") },
         { MIDORI_TOOLBAR_ICONS, "MIDORI_TOOLBAR_ICONS", N_("Icons") },
         { MIDORI_TOOLBAR_TEXT, "MIDORI_TOOLBAR_TEXT", N_("Text") },
         { MIDORI_TOOLBAR_BOTH, "MIDORI_TOOLBAR_BOTH", N_("Both") },
         { MIDORI_TOOLBAR_BOTH_HORIZ, "MIDORI_TOOLBAR_BOTH_HORIZ", N_("Both horizontal") },
         { 0, NULL, NULL }
        };
        type = g_enum_register_static ("MidoriToolbarStyle", values);
    }
    return type;
}

GType
midori_accept_cookies_get_type (void)
{
    static GType type = 0;
    if (!type)
    {
        static const GEnumValue values[] = {
         { MIDORI_ACCEPT_COOKIES_ALL, "MIDORI_ACCEPT_COOKIES_ALL", N_("All cookies") },
         { MIDORI_ACCEPT_COOKIES_SESSION, "MIDORI_ACCEPT_COOKIES_SESSION", N_("Session cookies") },
         { MIDORI_ACCEPT_COOKIES_NONE, "MIDORI_ACCEPT_COOKIES_NONE", N_("None") },
         { 0, NULL, NULL }
        };
        type = g_enum_register_static ("MidoriAcceptCookies", values);
    }
    return type;
}

static void
midori_web_settings_finalize (GObject* object);

static void
midori_web_settings_set_property (GObject*      object,
                                  guint         prop_id,
                                  const GValue* value,
                                  GParamSpec*   pspec);

static void
midori_web_settings_get_property (GObject*    object,
                                  guint       prop_id,
                                  GValue*     value,
                                  GParamSpec* pspec);

static void
midori_web_settings_class_init (MidoriWebSettingsClass* class)
{
    GObjectClass* gobject_class = G_OBJECT_CLASS (class);
    gobject_class->finalize = midori_web_settings_finalize;
    gobject_class->set_property = midori_web_settings_set_property;
    gobject_class->get_property = midori_web_settings_get_property;

    GParamFlags flags = G_PARAM_READWRITE | G_PARAM_CONSTRUCT;

    g_object_class_install_property (gobject_class,
                                     PROP_REMEMBER_LAST_WINDOW_SIZE,
                                     g_param_spec_boolean (
                                     "remember-last-window-size",
                                     _("Remember last window size"),
                                     _("Whether to save the last window size"),
                                     TRUE,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_LAST_WINDOW_WIDTH,
                                     g_param_spec_int (
                                     "last-window-width",
                                     _("Last window width"),
                                     _("The last saved window width"),
                                     0, G_MAXINT, 0,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_LAST_WINDOW_HEIGHT,
                                     g_param_spec_int (
                                     "last-window-height",
                                     _("Last window height"),
                                     _("The last saved window height"),
                                     0, G_MAXINT, 0,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_LAST_PANEL_POSITION,
                                     g_param_spec_int (
                                     "last-panel-position",
                                     _("Last panel position"),
                                     _("The last saved panel position"),
                                     0, G_MAXINT, 0,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_LAST_PANEL_PAGE,
                                     g_param_spec_int (
                                     "last-panel-page",
                                     _("Last panel page"),
                                     _("The last saved panel page"),
                                     0, G_MAXINT, 0,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_LAST_WEB_SEARCH,
                                     g_param_spec_int (
                                     "last-web-search",
                                     _("Last Web search"),
                                     _("The last saved Web search"),
                                     0, G_MAXINT, 0,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_LAST_PAGEHOLDER_URI,
                                     g_param_spec_string (
                                     "last-pageholder-uri",
                                     _("Last pageholder URI"),
                                     _("The URI last opened in the pageholder"),
                                     "",
                                     flags));



    g_object_class_install_property (gobject_class,
                                     PROP_SHOW_NAVIGATIONBAR,
                                     g_param_spec_boolean (
                                     "show-navigationbar",
                                     _("Show Navigationbar"),
                                     _("Whether to show the navigationbar"),
                                     TRUE,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_SHOW_BOOKMARKBAR,
                                     g_param_spec_boolean (
                                     "show-bookmarkbar",
                                     _("Show Bookmarkbar"),
                                     _("Whether to show the bookmarkbar"),
                                     FALSE,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_SHOW_PANEL,
                                     g_param_spec_boolean (
                                     "show-panel",
                                     _("Show Panel"),
                                     _("Whether to show the panel"),
                                     FALSE,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_SHOW_STATUSBAR,
                                     g_param_spec_boolean (
                                     "show-statusbar",
                                     _("Show Statusbar"),
                                     _("Whether to show the statusbar"),
                                     TRUE,
                                     flags));


    g_object_class_install_property (gobject_class,
                                     PROP_TOOLBAR_STYLE,
                                     g_param_spec_enum (
                                     "toolbar-style",
                                     _("Toolbar Style"),
                                     _("The style of the toolbar"),
                                     MIDORI_TYPE_TOOLBAR_STYLE,
                                     MIDORI_TOOLBAR_DEFAULT,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_SMALL_TOOLBAR,
                                     g_param_spec_boolean (
                                     "small-toolbar",
                                     _("Small toolbar"),
                                     _("Use small toolbar icons"),
                                     FALSE,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_SHOW_NEW_TAB,
                                     g_param_spec_boolean (
                                     "show-new-tab",
                                     _("Show New Tab"),
                                     _("Show the New Tab button in the toolbar"),
                                     TRUE,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_SHOW_HOMEPAGE,
                                     g_param_spec_boolean (
                                     "show-homepage",
                                     _("Show Homepage"),
                                     _("Show the Homepage button in the toolbar"),
                                     TRUE,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_SHOW_WEB_SEARCH,
                                     g_param_spec_boolean (
                                     "show-web-search",
                                     _("Show Web search"),
                                     _("Show the Web search entry in the toolbar"),
                                     TRUE,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_SHOW_TRASH,
                                     g_param_spec_boolean (
                                     "show-trash",
                                     _("Show Trash"),
                                     _("Show the Trash button in the toolbar"),
                                     TRUE,
                                     flags));



    g_object_class_install_property (gobject_class,
                                     PROP_LOAD_ON_STARTUP,
                                     g_param_spec_enum (
                                     "load-on-startup",
                                     _("Load on Startup"),
                                     _("What to load on startup"),
                                     MIDORI_TYPE_STARTUP,
                                     MIDORI_STARTUP_HOMEPAGE,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_HOMEPAGE,
                                     g_param_spec_string (
                                     "homepage",
                                     _("Homepage"),
                                     _("The homepage"),
                                     "http://www.google.com",
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_DOWNLOAD_FOLDER,
                                     g_param_spec_string (
                                     "download-folder",
                                     _("Download Folder"),
                                     _("The folder downloaded files are saved to"),
                                     g_get_home_dir (),
                                     G_PARAM_READABLE));

    g_object_class_install_property (gobject_class,
                                     PROP_DOWNLOAD_MANAGER,
                                     g_param_spec_string (
                                     "download-manager",
                                     _("Download Manager"),
                                     _("An external download manager"),
                                     NULL,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_LOCATION_ENTRY_SEARCH,
                                     g_param_spec_string (
                                     "location-entry-search",
                                     _("Location entry Search"),
                                     _("The search to perform inside the location entry"),
                                     "http://www.google.com/search?q=%s",
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_PREFERRED_ENCODING,
                                     g_param_spec_enum (
                                     "preferred-encoding",
                                     _("Preferred Encoding"),
                                     _("The preferred character encoding"),
                                     MIDORI_TYPE_PREFERRED_ENCODING,
                                     MIDORI_ENCODING_WESTERN,
                                     flags));



    g_object_class_install_property (gobject_class,
                                     PROP_TAB_LABEL_SIZE,
                                     g_param_spec_int (
                                     "tab-label-size",
                                     _("Tab Label Size"),
                                     _("The desired tab label size"),
                                     0, G_MAXINT, 10,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_CLOSE_BUTTONS_ON_TABS,
                                     g_param_spec_boolean (
                                     "close-buttons-on-tabs",
                                     _("Close Buttons on Tabs"),
                                     _("Whether tabs have close buttons"),
                                     TRUE,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_OPEN_NEW_PAGES_IN,
                                     g_param_spec_enum (
                                     "open-new-pages-in",
                                     _("Open new pages in"),
                                     _("Where to open new pages"),
                                     MIDORI_TYPE_NEW_PAGE,
                                     MIDORI_NEW_PAGE_TAB,
                                     G_PARAM_READABLE));

    g_object_class_install_property (gobject_class,
                                     PROP_MIDDLE_CLICK_OPENS_SELECTION,
                                     g_param_spec_boolean (
                                     "middle-click-opens-selection",
                                     _("Middle click opens Selection"),
                                     _("Load an URL from the selection via middle click"),
                                     FALSE,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_OPEN_TABS_IN_THE_BACKGROUND,
                                     g_param_spec_boolean (
                                     "open-tabs-in-the-background",
                                     _("Open tabs in the background"),
                                     _("Whether to open new tabs in the background"),
                                     FALSE,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_OPEN_POPUPS_IN_TABS,
                                     g_param_spec_boolean (
                                     "open-popups-in-tabs",
                                     _("Open popups in tabs"),
                                     _("Whether to open popup windows in tabs"),
                                     TRUE,
                                     flags));



    g_object_class_install_property (gobject_class,
                                     PROP_ACCEPT_COOKIES,
                                     g_param_spec_enum (
                                     "accept-cookies",
                                     _("Accept cookies"),
                                     _("What type of cookies to accept"),
                                     MIDORI_TYPE_ACCEPT_COOKIES,
                                     MIDORI_ACCEPT_COOKIES_ALL,
                                     G_PARAM_READABLE));

    g_object_class_install_property (gobject_class,
                                     PROP_ORIGINAL_COOKIES_ONLY,
                                     g_param_spec_boolean (
                                     "original-cookies-only",
                                     _("Original cookies only"),
                                     _("Accept cookies from the original website only"),
                                     FALSE,
                                     G_PARAM_READABLE));

    g_object_class_install_property (gobject_class,
                                     PROP_MAXIMUM_COOKIE_AGE,
                                     g_param_spec_int (
                                     "maximum-cookie-age",
                                     _("Maximum cookie age"),
                                     _("The maximum number of days to save cookies for"),
                                     0, G_MAXINT, 30,
                                     G_PARAM_READABLE));



    g_object_class_install_property (gobject_class,
                                     PROP_REMEMBER_LAST_VISITED_PAGES,
                                     g_param_spec_boolean (
                                     "remember-last-visited-pages",
                                     _("Remember last visited pages"),
                                     _("Whether the last visited pages are saved"),
                                     TRUE,
                                     G_PARAM_READABLE));

    g_object_class_install_property (gobject_class,
                                     PROP_MAXIMUM_HISTORY_AGE,
                                     g_param_spec_int (
                                     "maximum-history-age",
                                     _("Maximum history age"),
                                     _("The maximum number of days to save the history for"),
                                     0, G_MAXINT, 30,
                                     G_PARAM_READABLE));

    g_object_class_install_property (gobject_class,
                                     PROP_REMEMBER_LAST_FORM_INPUTS,
                                     g_param_spec_boolean (
                                     "remember-last-form-inputs",
                                     _("Remember last form inputs"),
                                     _("Whether the last form inputs are saved"),
                                     TRUE,
                                     G_PARAM_READABLE));

    g_object_class_install_property (gobject_class,
                                     PROP_REMEMBER_LAST_DOWNLOADED_FILES,
                                     g_param_spec_boolean (
                                     "remember-last-downloaded-files",
                                     _("Remember last downloaded files"),
                                     _("Whether the last downloaded files are saved"),
                                     TRUE,
                                     G_PARAM_READABLE));



    g_object_class_install_property (gobject_class,
                                     PROP_HTTP_PROXY,
                                     g_param_spec_string (
                                     "http-proxy",
                                     _("HTTP Proxy"),
                                     _("The proxy used for HTTP connections"),
                                     g_getenv ("http_proxy"),
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_CACHE_SIZE,
                                     g_param_spec_int (
                                     "cache-size",
                                     _("Cache size"),
                                     _("The allowed size of the cache"),
                                     0, G_MAXINT, 100,
                                     G_PARAM_READABLE));
}

static void
notify_default_encoding_cb (GObject* object, GParamSpec* pspec)
{
    MidoriWebSettings* web_settings = MIDORI_WEB_SETTINGS (object);

    const gchar* string;
    g_object_get (object, "default-encoding", &string, NULL);
    const gchar* encoding = string ? string : "";
    if (!strcmp (encoding, "BIG5"))
        web_settings->preferred_encoding = MIDORI_ENCODING_CHINESE;
    else if (!strcmp (encoding, "SHIFT_JIS"))
        web_settings->preferred_encoding = MIDORI_ENCODING_JAPANESE;
    else if (!strcmp (encoding, "KOI8-R"))
        web_settings->preferred_encoding = MIDORI_ENCODING_RUSSIAN;
    else if (!strcmp (encoding, "UTF-8"))
        web_settings->preferred_encoding = MIDORI_ENCODING_UNICODE;
    else if (!strcmp (encoding, "ISO-8859-1"))
        web_settings->preferred_encoding = MIDORI_ENCODING_WESTERN;
    else
        web_settings->preferred_encoding = MIDORI_ENCODING_CUSTOM;
    g_object_notify (object, "preferred-encoding");
}

static void
midori_web_settings_init (MidoriWebSettings* web_settings)
{
    g_signal_connect (web_settings, "notify::default-encoding",
                      G_CALLBACK (notify_default_encoding_cb), NULL);
}

static void
midori_web_settings_finalize (GObject* object)
{
    G_OBJECT_CLASS (midori_web_settings_parent_class)->finalize (object);
}

static void
midori_web_settings_set_property (GObject*      object,
                                  guint         prop_id,
                                  const GValue* value,
                                  GParamSpec*   pspec)
{
    MidoriWebSettings* web_settings = MIDORI_WEB_SETTINGS (object);

    switch (prop_id)
    {
    case PROP_REMEMBER_LAST_WINDOW_SIZE:
        web_settings->remember_last_window_size = g_value_get_boolean (value);
        break;
    case PROP_LAST_WINDOW_WIDTH:
        web_settings->last_window_width = g_value_get_int (value);
        break;
    case PROP_LAST_WINDOW_HEIGHT:
        web_settings->last_window_height = g_value_get_int (value);
        break;
    case PROP_LAST_PANEL_POSITION:
        web_settings->last_panel_position = g_value_get_int (value);
        break;
    case PROP_LAST_PANEL_PAGE:
        web_settings->last_panel_page = g_value_get_int (value);
        break;
    case PROP_LAST_WEB_SEARCH:
        web_settings->last_web_search = g_value_get_int (value);
        break;
    case PROP_LAST_PAGEHOLDER_URI:
        katze_assign (web_settings->last_pageholder_uri, g_value_dup_string (value));
        break;

    case PROP_SHOW_NAVIGATIONBAR:
        web_settings->show_navigationbar = g_value_get_boolean (value);
        break;
    case PROP_SHOW_BOOKMARKBAR:
        web_settings->show_bookmarkbar = g_value_get_boolean (value);
        break;
    case PROP_SHOW_PANEL:
        web_settings->show_panel = g_value_get_boolean (value);
        break;
    case PROP_SHOW_STATUSBAR:
        web_settings->show_statusbar = g_value_get_boolean (value);
        break;

    case PROP_TOOLBAR_STYLE:
        web_settings->toolbar_style = g_value_get_enum (value);
        break;
    case PROP_SMALL_TOOLBAR:
        web_settings->small_toolbar = g_value_get_boolean (value);
        break;
    case PROP_SHOW_NEW_TAB:
        web_settings->show_new_tab = g_value_get_boolean (value);
        break;
    case PROP_SHOW_HOMEPAGE:
        web_settings->show_homepage = g_value_get_boolean (value);
        break;
    case PROP_SHOW_WEB_SEARCH:
        web_settings->show_web_search = g_value_get_boolean (value);
        break;
    case PROP_SHOW_TRASH:
        web_settings->show_trash = g_value_get_boolean (value);
        break;

    case PROP_LOAD_ON_STARTUP:
        web_settings->load_on_startup = g_value_get_enum (value);
        break;
    case PROP_HOMEPAGE:
        katze_assign (web_settings->homepage, g_value_dup_string (value));
        break;
    case PROP_DOWNLOAD_FOLDER:
        katze_assign (web_settings->download_folder, g_value_dup_string (value));
        break;
    case PROP_DOWNLOAD_MANAGER:
        katze_assign (web_settings->download_manager, g_value_dup_string (value));
        break;
    case PROP_LOCATION_ENTRY_SEARCH:
        katze_assign (web_settings->location_entry_search, g_value_dup_string (value));
        break;
    case PROP_PREFERRED_ENCODING:
        web_settings->preferred_encoding = g_value_get_enum (value);
        switch (web_settings->preferred_encoding)
        {
        case MIDORI_ENCODING_CHINESE:
            g_object_set (object, "default-encoding", "BIG5", NULL);
            break;
        case MIDORI_ENCODING_JAPANESE:
            g_object_set (object, "default-encoding", "SHIFT_JIS", NULL);
            break;
        case MIDORI_ENCODING_RUSSIAN:
            g_object_set (object, "default-encoding", "KOI8-R", NULL);
            break;
        case MIDORI_ENCODING_UNICODE:
            g_object_set (object, "default-encoding", "UTF-8", NULL);
            break;
        case MIDORI_ENCODING_WESTERN:
            g_object_set (object, "default-encoding", "ISO-8859-1", NULL);
            break;
        case MIDORI_ENCODING_CUSTOM:
            g_object_set (object, "default-encoding", "", NULL);
        }
        break;

    case PROP_TAB_LABEL_SIZE:
        web_settings->tab_label_size = g_value_get_int (value);
        break;
    case PROP_CLOSE_BUTTONS_ON_TABS:
        web_settings->close_buttons_on_tabs = g_value_get_boolean (value);
        break;
    case PROP_OPEN_NEW_PAGES_IN:
        web_settings->open_new_pages_in = g_value_get_enum (value);
        break;
    case PROP_MIDDLE_CLICK_OPENS_SELECTION:
        web_settings->middle_click_opens_selection = g_value_get_boolean (value);
        break;
    case PROP_OPEN_TABS_IN_THE_BACKGROUND:
        web_settings->open_tabs_in_the_background = g_value_get_boolean (value);
        break;
    case PROP_OPEN_POPUPS_IN_TABS:
        web_settings->open_popups_in_tabs = g_value_get_boolean (value);
        break;

    case PROP_ACCEPT_COOKIES:
        web_settings->accept_cookies = g_value_get_enum (value);
        break;
    case PROP_ORIGINAL_COOKIES_ONLY:
        web_settings->original_cookies_only = g_value_get_boolean (value);
        break;
    case PROP_MAXIMUM_COOKIE_AGE:
        web_settings->maximum_cookie_age = g_value_get_int (value);
        break;

    case PROP_REMEMBER_LAST_VISITED_PAGES:
        web_settings->remember_last_visited_pages = g_value_get_boolean (value);
        break;
    case PROP_MAXIMUM_HISTORY_AGE:
        web_settings->maximum_history_age = g_value_get_int (value);
        break;
    case PROP_REMEMBER_LAST_FORM_INPUTS:
        web_settings->remember_last_form_inputs = g_value_get_boolean (value);
        break;
    case PROP_REMEMBER_LAST_DOWNLOADED_FILES:
        web_settings->remember_last_downloaded_files = g_value_get_boolean (value);
        break;

    case PROP_HTTP_PROXY:
        katze_assign (web_settings->http_proxy, g_value_dup_string (value));
        g_setenv ("http_proxy", web_settings->http_proxy ? web_settings->http_proxy : "", TRUE);
        break;
    case PROP_CACHE_SIZE:
        web_settings->cache_size = g_value_get_int (value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
midori_web_settings_get_property (GObject*    object,
                                  guint       prop_id,
                                  GValue*     value,
                                  GParamSpec* pspec)
{
    MidoriWebSettings* web_settings = MIDORI_WEB_SETTINGS (object);

    switch (prop_id)
    {
    case PROP_REMEMBER_LAST_WINDOW_SIZE:
        g_value_set_boolean (value, web_settings->remember_last_window_size);
        break;
    case PROP_LAST_WINDOW_WIDTH:
        g_value_set_int (value, web_settings->last_window_width);
        break;
    case PROP_LAST_WINDOW_HEIGHT:
        g_value_set_int (value, web_settings->last_window_height);
        break;
    case PROP_LAST_PANEL_POSITION:
        g_value_set_int (value, web_settings->last_panel_position);
        break;
    case PROP_LAST_PANEL_PAGE:
        g_value_set_int (value, web_settings->last_panel_page);
        break;
    case PROP_LAST_WEB_SEARCH:
        g_value_set_int (value, web_settings->last_web_search);
        break;
    case PROP_LAST_PAGEHOLDER_URI:
        g_value_set_string (value, web_settings->last_pageholder_uri);
        break;

    case PROP_SHOW_NAVIGATIONBAR:
        g_value_set_boolean (value, web_settings->show_navigationbar);
        break;
    case PROP_SHOW_BOOKMARKBAR:
        g_value_set_boolean (value, web_settings->show_bookmarkbar);
        break;
    case PROP_SHOW_PANEL:
        g_value_set_boolean (value, web_settings->show_panel);
        break;
    case PROP_SHOW_STATUSBAR:
        g_value_set_boolean (value, web_settings->show_statusbar);
        break;

    case PROP_TOOLBAR_STYLE:
        g_value_set_enum (value, web_settings->toolbar_style);
        break;
    case PROP_SMALL_TOOLBAR:
        g_value_set_boolean (value, web_settings->small_toolbar);
        break;
    case PROP_SHOW_NEW_TAB:
        g_value_set_boolean (value, web_settings->show_new_tab);
        break;
    case PROP_SHOW_HOMEPAGE:
        g_value_set_boolean (value, web_settings->show_homepage);
        break;
    case PROP_SHOW_WEB_SEARCH:
        g_value_set_boolean (value, web_settings->show_web_search);
        break;
    case PROP_SHOW_TRASH:
        g_value_set_boolean (value, web_settings->show_trash);
        break;

    case PROP_LOAD_ON_STARTUP:
        g_value_set_enum (value, web_settings->load_on_startup);
        break;
    case PROP_HOMEPAGE:
        g_value_set_string (value, web_settings->homepage);
        break;
    case PROP_DOWNLOAD_FOLDER:
        g_value_set_string (value, web_settings->download_folder);
        break;
    case PROP_DOWNLOAD_MANAGER:
        g_value_set_string (value, web_settings->download_manager);
        break;
    case PROP_LOCATION_ENTRY_SEARCH:
        g_value_set_string (value, web_settings->location_entry_search);
        break;
    case PROP_PREFERRED_ENCODING:
        g_value_set_enum (value, web_settings->preferred_encoding);
        break;

    case PROP_TAB_LABEL_SIZE:
        g_value_set_int (value, web_settings->tab_label_size);
        break;
    case PROP_CLOSE_BUTTONS_ON_TABS:
        g_value_set_boolean (value, web_settings->close_buttons_on_tabs);
        break;
    case PROP_OPEN_NEW_PAGES_IN:
        g_value_set_enum (value, web_settings->open_new_pages_in);
        break;
    case PROP_MIDDLE_CLICK_OPENS_SELECTION:
        g_value_set_boolean (value, web_settings->middle_click_opens_selection);
        break;
    case PROP_OPEN_TABS_IN_THE_BACKGROUND:
        g_value_set_boolean (value, web_settings->open_tabs_in_the_background);
        break;
    case PROP_OPEN_POPUPS_IN_TABS:
        g_value_set_boolean (value, web_settings->open_popups_in_tabs);
        break;

    case PROP_ACCEPT_COOKIES:
        g_value_set_enum (value, web_settings->accept_cookies);
        break;
    case PROP_ORIGINAL_COOKIES_ONLY:
        g_value_set_boolean (value, web_settings->original_cookies_only);
        break;
    case PROP_MAXIMUM_COOKIE_AGE:
        g_value_set_int (value, web_settings->maximum_cookie_age);
        break;

    case PROP_REMEMBER_LAST_VISITED_PAGES:
        g_value_set_boolean (value, web_settings->remember_last_visited_pages);
        break;
    case PROP_MAXIMUM_HISTORY_AGE:
        g_value_set_int (value, web_settings->maximum_history_age);
        break;
    case PROP_REMEMBER_LAST_FORM_INPUTS:
        g_value_set_boolean (value, web_settings->remember_last_form_inputs);
        break;
    case PROP_REMEMBER_LAST_DOWNLOADED_FILES:
        g_value_set_boolean (value, web_settings->remember_last_downloaded_files);
        break;

    case PROP_HTTP_PROXY:
        g_value_set_string (value, web_settings->http_proxy);
        break;
    case PROP_CACHE_SIZE:
        g_value_set_int (value, web_settings->cache_size);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

/**
 * midori_web_settings_new:
 *
 * Creates a new #MidoriWebSettings instance with default values.
 *
 * You will typically want to assign this to a #MidoriWebView or #MidoriBrowser.
 *
 * Return value: a new #MidoriWebSettings
 **/
MidoriWebSettings*
midori_web_settings_new (void)
{
    MidoriWebSettings* web_settings = g_object_new (MIDORI_TYPE_WEB_SETTINGS,
                                                    NULL);

    return web_settings;
}

/**
 * midori_web_settings_copy:
 *
 * Copies an existing #MidoriWebSettings instance.
 *
 * Return value: a new #MidoriWebSettings
 **/
MidoriWebSettings*
midori_web_settings_copy (MidoriWebSettings* web_settings)
{
    g_return_val_if_fail (MIDORI_IS_WEB_SETTINGS (web_settings), NULL);

    MidoriWebSettings* copy;
    copy = MIDORI_WEB_SETTINGS (webkit_web_settings_copy (
        WEBKIT_WEB_SETTINGS (web_settings)));
    g_object_set (copy,
                  "load-on-startup", web_settings->load_on_startup,
                  "homepage", web_settings->homepage,
                  "download-folder", web_settings->download_folder,
                  "download-manager", web_settings->download_manager,
                  "location-entry-search", web_settings->location_entry_search,
                  "preferred-encoding", web_settings->preferred_encoding,

                  "toolbar-style", web_settings->toolbar_style,
                  "small-toolbar", web_settings->small_toolbar,
                  "show-web-search", web_settings->show_web_search,
                  "show-new-tab", web_settings->show_new_tab,
                  "show-trash", web_settings->show_trash,

                  "tab-label-size", web_settings->tab_label_size,
                  "close-buttons-on-tabs", web_settings->close_buttons_on_tabs,
                  "open-new-pages-in", web_settings->open_new_pages_in,
                  "middle-click-opens-selection", web_settings->middle_click_opens_selection,
                  "open-tabs-in-the-background", web_settings->open_tabs_in_the_background,
                  "open-popups-in-tabs", web_settings->open_popups_in_tabs,

                  "accept-cookies", web_settings->accept_cookies,
                  "original-cookies-only", web_settings->original_cookies_only,
                  "maximum-cookie-age", web_settings->maximum_cookie_age,

                  "remember-last-visited-pages", web_settings->remember_last_visited_pages,
                  "maximum-history-age", web_settings->maximum_history_age,
                  "remember-last-form-inputs", web_settings->remember_last_form_inputs,
                  "remember-last-downloaded-files", web_settings->remember_last_downloaded_files,

                  "http-proxy", web_settings->http_proxy,
                  "cache-size", web_settings->cache_size,
                  NULL);

    return copy;
}
