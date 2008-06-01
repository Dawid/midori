/*
 Copyright (C) 2007-2008 Christian Dywan <christian@twotoasts.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 See the file COPYING for the full license text.
*/

#include "sokoke.h"

#include "search.h"

#include "config.h"
#include "main.h"

#include <string.h>
#include <gdk/gdkkeysyms.h>
#include <glib/gi18n.h>
#include <glib/gprintf.h>

gchar*
sokoke_magic_uri (const gchar* uri, const gchar* default_search_uri)
{
    // Add file:// if we have a local path
    if (g_path_is_absolute (uri))
        return g_strconcat ("file://", uri, NULL);
    // Do we need to add a protocol?
    if (!strstr (uri, "://"))
    {
        // Do we have a domain, ip address or localhost?
        if (strchr (uri, '.') != NULL || !strcmp (uri, "localhost"))
            return g_strconcat ("http://", uri, NULL);
        // We don't want to search? So return early.
        if (!default_search_uri)
            return g_strdup (uri);
        gchar* search;
        const gchar* search_uri = NULL;
        // Do we have a keyword and a string?
        gchar** parts = g_strsplit (uri, " ", 2);
        if (parts[0] && parts[1])
        {
            guint n = g_list_length (searchEngines);
            guint i;
            for (i = 0; i < n; i++)
            {
                SearchEngine* search_engine = (SearchEngine*)g_list_nth_data (
                    searchEngines, i);
                if (!strcmp (search_engine_get_keyword (search_engine),
                                                        parts[0]))
                    search_uri = search_engine->url;
            }
        if (search_uri)
            search = g_strdup_printf (search_uri, parts[1]);
        }
        // We only have a word or there is no matching keyword, so search for it
        if (!search_uri)
            search = g_strdup_printf (default_search_uri, uri);
        return search;
    }
    return g_strdup (uri);
}

void
sokoke_entry_setup_completion (GtkEntry* entry)
{
    /* TODO: The current behavior works only with the beginning of strings
             But we want to match "localhost" with "loc" and "hos" */
    GtkEntryCompletion* completion = gtk_entry_completion_new ();
    gtk_entry_completion_set_model (completion,
        GTK_TREE_MODEL (gtk_list_store_new (1, G_TYPE_STRING)));
    gtk_entry_completion_set_text_column (completion, 0);
    gtk_entry_completion_set_minimum_key_length (completion, 3);
    gtk_entry_set_completion (entry, completion);
    gtk_entry_completion_set_popup_completion (completion, FALSE); //...
}

void
sokoke_entry_append_completion (GtkEntry* entry, const gchar* text)
{
    GtkEntryCompletion* completion = gtk_entry_get_completion (entry);
    GtkTreeModel* completion_store = gtk_entry_completion_get_model (completion);
    GtkTreeIter iter;
    gtk_list_store_insert (GTK_LIST_STORE (completion_store), &iter, 0);
    gtk_list_store_set (GTK_LIST_STORE (completion_store), &iter, 0, text, -1);
}

void sokoke_combo_box_add_strings(GtkComboBox* combobox
 , const gchar* labelFirst, ...)
{
    // Add a number of strings to a combobox, terminated with NULL
    // This works only for text comboboxes
    va_list args;
    va_start(args, labelFirst);

    const gchar* label;
    for(label = labelFirst; label; label = va_arg(args, const gchar*))
        gtk_combo_box_append_text(combobox, label);

    va_end(args);
}

void sokoke_widget_set_visible(GtkWidget* widget, gboolean visible)
{
    // Show or hide the widget
    if(visible)
        gtk_widget_show(widget);
    else
        gtk_widget_hide(widget);
}

void sokoke_container_show_children(GtkContainer* container)
{
    // Show every child but not the container itself
    gtk_container_foreach(container, (GtkCallback)(gtk_widget_show_all), NULL);
}

void sokoke_widget_set_tooltip_text(GtkWidget* widget, const gchar* text)
{
    #if GTK_CHECK_VERSION(2, 12, 0)
    gtk_widget_set_tooltip_text(widget, text);
    #else
    static GtkTooltips* tooltips;
    if(!tooltips)
        tooltips = gtk_tooltips_new();
    gtk_tooltips_set_tip(tooltips, widget, text, NULL);
    #endif
}

void
sokoke_tool_item_set_tooltip_text (GtkToolItem* toolitem, const gchar* text)
{
    // TODO: Use 2.12 api if available
    static GtkTooltips* tooltips = NULL;

    if (G_UNLIKELY (!tooltips))
        tooltips = gtk_tooltips_new();

    if (text && *text)
        gtk_tool_item_set_tooltip (toolitem, tooltips, text, NULL);
}

typedef struct
{
     GtkWidget* widget;
     SokokeMenuPos position;
} SokokePopupInfo;

static void
sokoke_widget_popup_position_menu (GtkMenu*  menu,
                                   gint*     x,
                                   gint*     y,
                                   gboolean* push_in,
                                   gpointer  user_data)
{
    gint wx, wy;
    gint menu_width;
    GtkRequisition menu_req;
    GtkRequisition widget_req;
    SokokePopupInfo* info = user_data;
    GtkWidget* widget = info->widget;

    // Retrieve size and position of both widget and menu
    if (GTK_WIDGET_NO_WINDOW (widget))
    {
        gdk_window_get_position (widget->window, &wx, &wy);
        wx += widget->allocation.x;
        wy += widget->allocation.y;
    }
    else
        gdk_window_get_origin (widget->window, &wx, &wy);
    gtk_widget_size_request (GTK_WIDGET (menu), &menu_req);
    gtk_widget_size_request (widget, &widget_req);
    menu_width = menu_req.width;
    gint widget_height = widget_req.height; // Better than allocation.height

    // Calculate menu position
    if (info->position == SOKOKE_MENU_POSITION_CURSOR)
        ; // Do nothing?
    else if (info->position == SOKOKE_MENU_POSITION_RIGHT)
    {
        *x = wx + widget->allocation.width - menu_width;
        *y = wy + widget_height;
    } else if (info->position == SOKOKE_MENU_POSITION_LEFT)
    {
        *x = wx;
        *y = wy + widget_height;
    }

    *push_in = TRUE;
}


void
sokoke_widget_popup (GtkWidget*      widget,
                     GtkMenu*        menu,
                     GdkEventButton* event,
                     SokokeMenuPos   pos)
{
    int button, event_time;
    if (event)
    {
        button = event->button;
        event_time = event->time;
    }
    else
    {
        button = 0;
        event_time = gtk_get_current_event_time ();
    }

    if (!gtk_menu_get_attach_widget(menu))
        gtk_menu_attach_to_widget (menu, widget, NULL);

    if (widget)
    {
        SokokePopupInfo info = { widget, pos };
        gtk_menu_popup (menu, NULL, NULL,
                        sokoke_widget_popup_position_menu, &info,
                        button, event_time);
    }
    else
        gtk_menu_popup (menu, NULL, NULL, NULL, NULL, button, event_time);
}

typedef enum
{
    SOKOKE_DESKTOP_UNTESTED,
    SOKOKE_DESKTOP_XFCE,
    SOKOKE_DESKTOP_UNKNOWN
} SokokeDesktop;

static SokokeDesktop sokoke_get_desktop(void)
{
    static SokokeDesktop desktop = SOKOKE_DESKTOP_UNTESTED;
    if(G_UNLIKELY(desktop == SOKOKE_DESKTOP_UNTESTED))
    {
        // Are we running in Xfce?
        gint result; gchar* out; gchar* err;
        gboolean success = g_spawn_command_line_sync(
         "xprop -root _DT_SAVE_MODE | grep -q xfce4"
         , &out, &err, &result, NULL);
        if(success && !result)
            desktop = SOKOKE_DESKTOP_XFCE;
        else
            desktop = SOKOKE_DESKTOP_UNKNOWN;
    }

    return desktop;
}

gpointer sokoke_xfce_header_new(const gchar* icon, const gchar* title)
{
    // Create an xfce header with icon and title
    // This returns NULL if the desktop is not xfce
    if(sokoke_get_desktop() == SOKOKE_DESKTOP_XFCE)
    {
        GtkWidget* entry = gtk_entry_new();
        gchar* markup;
        GtkWidget* xfce_heading = gtk_event_box_new();
        gtk_widget_modify_bg(xfce_heading, GTK_STATE_NORMAL
         , &entry->style->base[GTK_STATE_NORMAL]);
        GtkWidget* hbox = gtk_hbox_new(FALSE, 12);
        gtk_container_set_border_width(GTK_CONTAINER(hbox), 6);
        GtkWidget* image = gtk_image_new_from_icon_name(icon, GTK_ICON_SIZE_DIALOG);
        gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);
        GtkWidget* label = gtk_label_new(NULL);
        gtk_widget_modify_fg(label, GTK_STATE_NORMAL
         , &entry->style->text[GTK_STATE_NORMAL]);
        markup = g_strdup_printf("<span size='large' weight='bold'>%s</span>", title);
        gtk_label_set_markup(GTK_LABEL(label), markup);
        gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
        gtk_container_add(GTK_CONTAINER(xfce_heading), hbox);
        g_free(markup);
        return xfce_heading;
    }
    return NULL;
}

gpointer sokoke_superuser_warning_new(void)
{
    // Create a horizontal bar with a security warning
    // This returns NULL if the user is no superuser
    #ifdef HAVE_UNISTD_H
    if(G_UNLIKELY(!geteuid())) // effective superuser?
    {
        GtkWidget* hbox = gtk_event_box_new();
        gtk_widget_modify_bg(hbox, GTK_STATE_NORMAL
         , &hbox->style->bg[GTK_STATE_SELECTED]);
        GtkWidget* label = gtk_label_new(_("Warning: You are using a superuser account!"));
        gtk_misc_set_padding(GTK_MISC(label), 0, 2);
        gtk_widget_modify_fg(GTK_WIDGET(label), GTK_STATE_NORMAL
         , &GTK_WIDGET(label)->style->fg[GTK_STATE_SELECTED]);
        gtk_container_add(GTK_CONTAINER(hbox), GTK_WIDGET(label));
        return hbox;
    }
    #endif
    return NULL;
}

GtkWidget* sokoke_hig_frame_new(const gchar* title)
{
    // Create a frame with no actual frame but a bold label and indentation
    GtkWidget* frame = gtk_frame_new(NULL);
    gchar* titleBold = g_strdup_printf("<b>%s</b>", title);
    GtkWidget* label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), titleBold);
    g_free(titleBold);
    gtk_frame_set_label_widget(GTK_FRAME(frame), label);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);
    return frame;
}

void sokoke_widget_set_pango_font_style(GtkWidget* widget, PangoStyle style)
{
    // Conveniently change the pango font style
    // For some reason we need to reset if we actually want the normal style
    if(style == PANGO_STYLE_NORMAL)
        gtk_widget_modify_font(widget, NULL);
    else
    {
        PangoFontDescription* fontDescription = pango_font_description_new();
        pango_font_description_set_style(fontDescription, PANGO_STYLE_ITALIC);
        gtk_widget_modify_font(widget, fontDescription);
        pango_font_description_free(fontDescription);
    }
}

static gboolean
sokoke_on_entry_focus_in_event (GtkEntry*      entry,
                                GdkEventFocus* event,
                                gpointer       userdata)
{
    gint default_text = GPOINTER_TO_INT (
        g_object_get_data (G_OBJECT (entry), "sokoke_has_default"));
    if (default_text)
    {
        gtk_entry_set_text (entry, "");
        g_object_set_data (G_OBJECT(entry), "sokoke_has_default",
                           GINT_TO_POINTER (0));
        sokoke_widget_set_pango_font_style (GTK_WIDGET (entry),
                                            PANGO_STYLE_NORMAL);
    }
    return FALSE;
}

static gboolean
sokoke_on_entry_focus_out_event (GtkEntry*      entry,
                                 GdkEventFocus* event,
                                 gpointer       userdata)
{
    const gchar* text = gtk_entry_get_text (entry);
    if (text && !*text)
    {
        const gchar* defaultText = (const gchar*)g_object_get_data (
         G_OBJECT (entry), "sokoke_default_text");
        gtk_entry_set_text (entry, defaultText);
        g_object_set_data (G_OBJECT(entry),
                           "sokoke_has_default", GINT_TO_POINTER (1));
        sokoke_widget_set_pango_font_style (GTK_WIDGET(entry),
                                            PANGO_STYLE_ITALIC);
    }
    return FALSE;
}

void
sokoke_entry_set_default_text (GtkEntry*    entry,
                               const gchar* default_text)
{
    // Note: The default text initially overwrites any previous text
    gchar* old_value = g_object_get_data (G_OBJECT (entry),
                                          "sokoke_default_text");
    if (!old_value)
    {
        g_object_set_data (G_OBJECT (entry), "sokoke_has_default",
                           GINT_TO_POINTER (1));
        sokoke_widget_set_pango_font_style (GTK_WIDGET (entry),
                                            PANGO_STYLE_ITALIC);
        gtk_entry_set_text (entry, default_text);
    }
    g_object_set_data (G_OBJECT (entry), "sokoke_default_text",
                       (gpointer)default_text);
    g_signal_connect (entry, "focus-in-event",
        G_CALLBACK (sokoke_on_entry_focus_in_event), NULL);
    g_signal_connect (entry, "focus-out-event",
        G_CALLBACK (sokoke_on_entry_focus_out_event), NULL);
}

gchar* sokoke_key_file_get_string_default(GKeyFile* keyFile
 , const gchar* group, const gchar* key, const gchar* defaultValue, GError** error)
{
    gchar* value = g_key_file_get_string(keyFile, group, key, error);
    return value == NULL ? g_strdup(defaultValue) : value;
}

gint sokoke_key_file_get_integer_default(GKeyFile* keyFile
 , const gchar* group, const gchar* key, const gint defaultValue, GError** error)
{
    if(!g_key_file_has_key(keyFile, group, key, NULL))
        return defaultValue;
    return g_key_file_get_integer(keyFile, group, key, error);
}

gdouble
sokoke_key_file_get_double_default (GKeyFile*     key_file,
                                    const gchar*  group,
                                    const gchar*  key,
                                    const gdouble default_value,
                                    GError**      error)
{
    if (!g_key_file_has_key (key_file, group, key, NULL))
        return default_value;
    return g_key_file_get_double (key_file, group, key, error);
}

gboolean
sokoke_key_file_get_boolean_default (GKeyFile*      key_file,
                                     const gchar*   group,
                                     const gchar*   key,
                                     const gboolean default_value,
                                     GError**       error)
{
    if (!g_key_file_has_key (key_file, group, key, NULL))
        return default_value;
    return g_key_file_get_boolean (key_file, group, key, error);
}

gboolean sokoke_key_file_save_to_file(GKeyFile* keyFile
 , const gchar* filename, GError** error)
{
    gchar* data = g_key_file_to_data(keyFile, NULL, error);
    if(!data)
        return FALSE;
    FILE* fp;
    if(!(fp = fopen(filename, "w")))
    {
        *error = g_error_new(G_FILE_ERROR, G_FILE_ERROR_ACCES
         , _("Writing failed."));
        return FALSE;
    }
    fputs(data, fp);
    fclose(fp);
    g_free(data);
    return TRUE;
}

void sokoke_widget_get_text_size(GtkWidget* widget, const gchar* text
 , gint* w, gint* h)
{
    PangoLayout* layout = gtk_widget_create_pango_layout(widget, text);
    pango_layout_get_pixel_size(layout, w, h);
    g_object_unref(layout);
}

void sokoke_menu_item_set_accel(GtkMenuItem* menuitem, const gchar* path
 , const gchar* key, GdkModifierType modifiers)
{
    if(path && *path)
    {
        gchar* accel = g_strconcat("<", g_get_prgname(), ">/", path, NULL);
        gtk_menu_item_set_accel_path(GTK_MENU_ITEM(menuitem), accel);
        guint keyVal = key ? gdk_keyval_from_name(key) : 0;
        gtk_accel_map_add_entry(accel, keyVal, modifiers);
        g_free(accel);
    }
}
