/*
 Copyright (C) 2008 Christian Dywan <christian@twotoasts.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 See the file COPYING for the full license text.
*/

#ifndef __MIDORI_PANEL_H__
#define __MIDORI_PANEL_H__

#include <gtk/gtk.h>

#include <katze/katze.h>

G_BEGIN_DECLS

#define MIDORI_TYPE_PANEL \
    (midori_panel_get_type ())
#define MIDORI_PANEL(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST ((obj), MIDORI_TYPE_PANEL, MidoriPanel))
#define MIDORI_PANEL_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST ((klass), MIDORI_TYPE_PANEL, MidoriPanelClass))
#define MIDORI_IS_PANEL(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MIDORI_TYPE_PANEL))
#define MIDORI_IS_PANEL_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_TYPE ((klass), MIDORI_TYPE_PANEL))
#define MIDORI_PANEL_GET_CLASS(obj) \
    (G_TYPE_INSTANCE_GET_CLASS ((obj), MIDORI_TYPE_PANEL, MidoriPanelClass))

typedef struct _MidoriPanel                MidoriPanel;
typedef struct _MidoriPanelPrivate         MidoriPanelPrivate;
typedef struct _MidoriPanelClass           MidoriPanelClass;

struct _MidoriPanel
{
    GtkFrame parent_instance;

    MidoriPanelPrivate* priv;
};

struct _MidoriPanelClass
{
    GtkFrameClass parent_class;

    /* Signals */
    gboolean
    (*close)                  (MidoriPanel*          panel);

    void
    (*switch_page)            (MidoriPanel*          panel,
                               gint                  page);
};

GType
midori_panel_get_type               (void);

GtkWidget*
midori_panel_new                    (void);

gint
midori_panel_append_page            (MidoriPanel*       panel,
                                     GtkWidget*         child,
                                     const gchar*       icon,
                                     const gchar*       label);

gint
midori_panel_get_current_page       (MidoriPanel*       panel);

GtkWidget*
midori_panel_get_nth_page           (MidoriPanel*       panel,
                                     guint              page_num);

guint
midori_panel_get_n_pages            (MidoriPanel*       panel);

gint
midori_panel_page_num               (MidoriPanel*       panel,
                                     GtkWidget*         child);

void
midori_panel_set_current_page       (MidoriPanel*       panel,
                                     gint               n);

G_END_DECLS

#endif /* __MIDORI_PANEL_H__ */