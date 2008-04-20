/*
 Copyright (C) 2007-2008 Christian Dywan <christian@twotoasts.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 See the file COPYING for the full license text.
*/

#ifndef __SOKOKE_H__
#define __SOKOKE_H__ 1

#include <gtk/gtk.h>

// Many themes need this hack for small toolbars to work
#define GTK_ICON_SIZE_SMALL_TOOLBAR GTK_ICON_SIZE_BUTTON

gchar*
sokoke_magic_uri               (const gchar* uri,
                                const gchar* search);

void
sokoke_entry_setup_completion  (GtkEntry* entry);

void
sokoke_entry_append_completion (GtkEntry* entry,
                                const gchar* text);

typedef enum {
    SOKOKE_MENU_POSITION_CURSOR = 0,
    SOKOKE_MENU_POSITION_LEFT,
    SOKOKE_MENU_POSITION_RIGHT
} SokokeMenuPos;

void
sokoke_combo_box_add_strings(GtkComboBox*, const gchar*, ...);

void
sokoke_widget_set_visible(GtkWidget*, gboolean);

void
sokoke_container_show_children(GtkContainer*);

void
sokoke_widget_set_tooltip_text(GtkWidget*, const gchar*);

void
sokoke_tool_item_set_tooltip_text(GtkToolItem*, const gchar*);

void
sokoke_widget_popup(GtkWidget*, GtkMenu*, GdkEventButton*, SokokeMenuPos pos);

gpointer
sokoke_xfce_header_new(const gchar*, const gchar*);

gpointer
sokoke_superuser_warning_new(void);

GtkWidget*
sokoke_hig_frame_new(const gchar*);

void
sokoke_widget_set_pango_font_style(GtkWidget*, PangoStyle);

void
sokoke_entry_set_default_text(GtkEntry*, const gchar*);

gchar*
sokoke_key_file_get_string_default(GKeyFile*, const gchar*, const gchar*
 , const gchar*, GError**);

gint
sokoke_key_file_get_integer_default(GKeyFile*, const gchar*, const gchar*
 , const gint, GError**);

gboolean
sokoke_key_file_get_boolean_default (GKeyFile*    key_file,
                                     const gchar* group,
                                     const gchar* key,
                                     gboolean     default_value,
                                     GError**     error);

gboolean
sokoke_key_file_save_to_file(GKeyFile*, const gchar*, GError**);

void
sokoke_widget_get_text_size(GtkWidget*, const gchar*, gint*, gint*);

void
sokoke_menu_item_set_accel(GtkMenuItem*, const gchar*, const gchar*, GdkModifierType);

#endif /* !__SOKOKE_H__ */
