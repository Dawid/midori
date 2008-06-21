/*
 Copyright (C) 2008 Christian Dywan <christian@twotoasts.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 See the file COPYING for the full license text.
*/

#include "compat.h"

#if !GTK_CHECK_VERSION(2, 12, 0)

void
gtk_widget_set_tooltip_text (GtkWidget* widget, const gchar* text)
{
    static GtkTooltips* tooltips;
    if (!tooltips)
        tooltips = gtk_tooltips_new ();
    gtk_tooltips_set_tip (tooltips, widget, text, NULL);
}

void
gtk_tool_item_set_tooltip_text (GtkToolItem* toolitem,
                                const gchar* text)
{
    if (text && *text)
    {
        static GtkTooltips* tooltips = NULL;
        if (G_UNLIKELY (!tooltips))
            tooltips = gtk_tooltips_new();

        gtk_tool_item_set_tooltip (toolitem, tooltips, text, NULL);
    }
}

#endif

#ifndef WEBKIT_CHECK_VERSION

gfloat
webkit_web_view_get_zoom_level (WebKitWebView* web_view)
{
    g_return_val_if_fail (WEBKIT_IS_WEB_VIEW (web_view), 1.0);

    return 1.0;
}

void
webkit_web_view_set_zoom_level (WebKitWebView* web_view,
                                gfloat         zoom_level)
{
    g_return_if_fail (WEBKIT_IS_WEB_VIEW (web_view));
}

void
webkit_web_view_zoom_in (WebKitWebView* web_view)
{
    g_return_if_fail (WEBKIT_IS_WEB_VIEW (web_view));
}

void
webkit_web_view_zoom_out (WebKitWebView* web_view)
{
    g_return_if_fail (WEBKIT_IS_WEB_VIEW (web_view));
}

#endif
