/*
 Copyright (C) 2007-2008 Christian Dywan <christian@twotoasts.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 See the file COPYING for the full license text.
*/

#include "midori-panel.h"

#include "sokoke.h"
#include <glib/gi18n.h>

G_DEFINE_TYPE (MidoriPanel, midori_panel, GTK_TYPE_HBOX)

struct _MidoriPanelPrivate
{
    GtkWidget* toolbar;
    GtkWidget* toolbar_label;
    GtkWidget* frame;
    GtkWidget* toolbook;
    GtkWidget* notebook;
    GSList*    group;
    GtkMenu*   menu;
};

#define MIDORI_PANEL_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
     MIDORI_TYPE_PANEL, MidoriPanelPrivate))

enum
{
    PROP_0,

    PROP_SHADOW_TYPE,
    PROP_MENU,
    PROP_PAGE
};

enum {
    CLOSE,
    SWITCH_PAGE,

    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];

static void
midori_panel_finalize (GObject* object);

static void
midori_panel_set_property (GObject*      object,
                           guint         prop_id,
                           const GValue* value,
                           GParamSpec*   pspec);

static void
midori_panel_get_property (GObject*    object,
                           guint       prop_id,
                           GValue*     value,
                           GParamSpec* pspec);

static gboolean
midori_panel_close_cb (MidoriPanel* panel)
{
    gtk_widget_hide (GTK_WIDGET (panel));
    return FALSE;
}

static void
midori_cclosure_marshal_BOOLEAN__VOID (GClosure*     closure,
                                       GValue*       return_value,
                                       guint         n_param_values,
                                       const GValue* param_values,
                                       gpointer      invocation_hint,
                                       gpointer      marshal_data)
{
    typedef gboolean(*GMarshalFunc_BOOLEAN__VOID) (gpointer  data1,
                                                   gpointer  data2);
    register GMarshalFunc_BOOLEAN__VOID callback;
    register GCClosure* cc = (GCClosure*) closure;
    register gpointer data1, data2;
    gboolean v_return;

    g_return_if_fail (return_value != NULL);
    g_return_if_fail (n_param_values == 1);

    if (G_CCLOSURE_SWAP_DATA (closure))
    {
        data1 = closure->data;
        data2 = g_value_peek_pointer (param_values + 0);
    }
    else
    {
        data1 = g_value_peek_pointer (param_values + 0);
        data2 = closure->data;
    }
    callback = (GMarshalFunc_BOOLEAN__VOID) (marshal_data
        ? marshal_data : cc->callback);
    v_return = callback (data1, data2);
    g_value_set_boolean (return_value, v_return);
}

static void
midori_panel_class_init (MidoriPanelClass* class)
{

    signals[CLOSE] = g_signal_new (
        "close",
        G_TYPE_FROM_CLASS (class),
        (GSignalFlags)(G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION),
        G_STRUCT_OFFSET (MidoriPanelClass, close),
        g_signal_accumulator_true_handled,
        NULL,
        midori_cclosure_marshal_BOOLEAN__VOID,
        G_TYPE_BOOLEAN, 0);

    signals[SWITCH_PAGE] = g_signal_new (
        "switch-page",
        G_TYPE_FROM_CLASS (class),
        (GSignalFlags)(G_SIGNAL_RUN_LAST),
        G_STRUCT_OFFSET (MidoriPanelClass, switch_page),
        0,
        NULL,
        g_cclosure_marshal_VOID__INT,
        G_TYPE_NONE, 1,
        G_TYPE_INT);

    class->close = midori_panel_close_cb;

    GObjectClass* gobject_class = G_OBJECT_CLASS (class);
    gobject_class->finalize = midori_panel_finalize;
    gobject_class->set_property = midori_panel_set_property;
    gobject_class->get_property = midori_panel_get_property;

    GParamFlags flags = G_PARAM_READWRITE | G_PARAM_CONSTRUCT;

    g_object_class_install_property (gobject_class,
                                     PROP_SHADOW_TYPE,
                                     g_param_spec_enum (
                                     "shadow-type",
                                     "Shadow Type",
                                     _("Appearance of the shadow around each panel"),
                                     GTK_TYPE_SHADOW_TYPE,
                                     GTK_SHADOW_NONE,
                                     flags));

    g_object_class_install_property (gobject_class,
                                     PROP_MENU,
                                     g_param_spec_object (
                                     "menu",
                                     "Menu",
                                     _("Menu to hold panel items"),
                                     GTK_TYPE_MENU,
                                     G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class,
                                     PROP_PAGE,
                                     g_param_spec_int (
                                     "page",
                                     "Page",
                                     _("The index of the current page"),
                                     -1, G_MAXINT, -1,
                                     flags));

    g_type_class_add_private (class, sizeof (MidoriPanelPrivate));
}

static void
midori_panel_button_close_clicked_cb (GtkWidget*   toolitem,
                                      MidoriPanel* panel)
{
    gboolean return_value;
    g_signal_emit (panel, signals[CLOSE], 0, &return_value);
}

static void
midori_panel_init (MidoriPanel* panel)
{
    panel->priv = MIDORI_PANEL_GET_PRIVATE (panel);

    MidoriPanelPrivate* priv = panel->priv;

    // Create the sidebar
    priv->toolbar = gtk_toolbar_new ();
    gtk_toolbar_set_style (GTK_TOOLBAR (priv->toolbar), GTK_TOOLBAR_BOTH);
    gtk_toolbar_set_icon_size (GTK_TOOLBAR (priv->toolbar),
                               GTK_ICON_SIZE_BUTTON);
    gtk_toolbar_set_orientation (GTK_TOOLBAR (priv->toolbar),
                                 GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start (GTK_BOX (panel), priv->toolbar, FALSE, FALSE, 0);
    gtk_widget_show_all (priv->toolbar);
    GtkWidget* vbox = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (panel), vbox, TRUE, TRUE, 0);

    // Create the titlebar
    GtkWidget* labelbar = gtk_toolbar_new ();
    gtk_toolbar_set_icon_size (GTK_TOOLBAR (labelbar), GTK_ICON_SIZE_MENU);
    gtk_toolbar_set_style (GTK_TOOLBAR (labelbar), GTK_TOOLBAR_ICONS);
    GtkToolItem* toolitem = gtk_tool_item_new ();
    gtk_tool_item_set_expand (toolitem, TRUE);
    priv->toolbar_label = gtk_label_new (NULL);
    gtk_misc_set_alignment (GTK_MISC (priv->toolbar_label), 0, 0.5);
    gtk_container_add (GTK_CONTAINER (toolitem), priv->toolbar_label);
    gtk_container_set_border_width (GTK_CONTAINER (toolitem), 6);
    gtk_toolbar_insert (GTK_TOOLBAR (labelbar), toolitem, -1);
    toolitem = gtk_tool_button_new_from_stock (GTK_STOCK_CLOSE);
    gtk_tool_button_set_label (GTK_TOOL_BUTTON (toolitem), _("Close panel"));
    sokoke_tool_item_set_tooltip_text (GTK_TOOL_ITEM (toolitem), _("Close panel"));
    g_signal_connect (toolitem, "clicked",
        G_CALLBACK (midori_panel_button_close_clicked_cb), panel);
    gtk_toolbar_insert (GTK_TOOLBAR (labelbar), toolitem, -1);
    gtk_box_pack_start (GTK_BOX (vbox), labelbar, FALSE, FALSE, 0);
    gtk_widget_show_all (vbox);

    // Create the toolbook
    priv->toolbook = gtk_notebook_new ();
    gtk_notebook_set_show_border (GTK_NOTEBOOK (priv->toolbook), FALSE);
    gtk_notebook_set_show_tabs (GTK_NOTEBOOK (priv->toolbook), FALSE);
    gtk_box_pack_start (GTK_BOX (vbox), priv->toolbook, FALSE, FALSE, 0);
    gtk_widget_show (priv->toolbook);

    // Create the notebook
    priv->notebook = gtk_notebook_new ();
    gtk_notebook_set_show_border (GTK_NOTEBOOK (priv->notebook), FALSE);
    gtk_notebook_set_show_tabs (GTK_NOTEBOOK (priv->notebook), FALSE);
    priv->frame = gtk_frame_new (NULL);
    gtk_container_add (GTK_CONTAINER (priv->frame), priv->notebook);
    gtk_box_pack_start (GTK_BOX (vbox), priv->frame, TRUE, TRUE, 0);
    gtk_widget_show_all (priv->frame);
}

static void
midori_panel_finalize (GObject* object)
{
    MidoriPanel* panel = MIDORI_PANEL (object);
    MidoriPanelPrivate* priv = panel->priv;

    if (priv->menu)
    {
        // FIXME: Remove all menu items
    }

    G_OBJECT_CLASS (midori_panel_parent_class)->finalize (object);
}

static void
midori_panel_set_property (GObject*      object,
                           guint         prop_id,
                           const GValue* value,
                           GParamSpec*   pspec)
{
    MidoriPanel* panel = MIDORI_PANEL (object);
    MidoriPanelPrivate* priv = panel->priv;

    switch (prop_id)
    {
    case PROP_SHADOW_TYPE:
        gtk_frame_set_shadow_type (GTK_FRAME (priv->frame),
                                   g_value_get_enum (value));
        break;
    case PROP_MENU:
        katze_object_assign (priv->menu, g_value_get_object (value));
        // FIXME: Move existing items to the new menu
        break;
    case PROP_PAGE:
        midori_panel_set_current_page (panel, g_value_get_int (value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
midori_panel_get_property (GObject*    object,
                           guint       prop_id,
                           GValue*     value,
                           GParamSpec* pspec)
{
    MidoriPanel* panel = MIDORI_PANEL (object);
    MidoriPanelPrivate* priv = panel->priv;

    switch (prop_id)
    {
    case PROP_SHADOW_TYPE:
        g_value_set_enum (value,
            gtk_frame_get_shadow_type (GTK_FRAME (priv->frame)));
        break;
    case PROP_MENU:
        g_value_set_object (value, priv->menu);
        break;
    case PROP_PAGE:
        g_value_set_int (value, midori_panel_get_current_page (panel));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

/**
 * midori_panel_new:
 *
 * Creates a new empty panel.
 *
 * Return value: a new #MidoriPanel
 **/
GtkWidget*
midori_panel_new (void)
{
    MidoriPanel* panel = g_object_new (MIDORI_TYPE_PANEL,
                                       NULL);

    return GTK_WIDGET (panel);
}

static void
midori_panel_menu_item_activate_cb (GtkWidget*   widget,
                                    MidoriPanel* panel)
{
    GtkWidget* child = g_object_get_data (G_OBJECT (widget), "page");
    guint n = midori_panel_page_num (panel, child);
    midori_panel_set_current_page (panel, n);
    g_signal_emit (panel, signals[SWITCH_PAGE], 0, n);
}

/**
 * midori_panel_append_page:
 * @panel: a #MidoriPanel
 * @child: the child widget
 * @toolbar: a toolbar widget, or %NULL
 * @icon: a stock ID or icon name, or %NULL
 * @label: a string to use as the label, or %NULL
 *
 * Appends a new page to the panel. If @toolbar is specified it will
 * be packaged above @child.
 *
 * If @icon is an icon name, the according image is used as an
 * icon for this page.
 *
 * If @label is given, it is used as the label of this page.
 *
 * In the case of an error, -1 is returned.
 *
 * Return value: the index of the new page, or -1
 **/
gint
midori_panel_append_page (MidoriPanel* panel,
                          GtkWidget*   child,
                          GtkWidget*   toolbar,
                          const gchar* icon,
                          const gchar* label)
{
    g_return_val_if_fail (MIDORI_IS_PANEL (panel), -1);
    g_return_val_if_fail (GTK_IS_WIDGET (child), -1);
    g_return_val_if_fail (!toolbar || GTK_IS_WIDGET (toolbar), -1);

    MidoriPanelPrivate* priv = panel->priv;

    GtkWidget* scrolled = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC);
    GTK_WIDGET_SET_FLAGS (scrolled, GTK_CAN_FOCUS);
    gtk_widget_show (scrolled);
    GtkWidget* widget;
    GObjectClass* gobject_class = G_OBJECT_GET_CLASS (child);
    if (GTK_WIDGET_CLASS (gobject_class)->set_scroll_adjustments_signal)
        widget = child;
    else
    {
        widget = gtk_viewport_new (NULL, NULL);
        gtk_widget_show (widget);
        gtk_container_add (GTK_CONTAINER (widget), child);
    }
    gtk_container_add (GTK_CONTAINER (scrolled), widget);
    gtk_container_add (GTK_CONTAINER (priv->notebook), scrolled);

    if (!toolbar)
        toolbar = gtk_event_box_new ();
    gtk_widget_show (toolbar);
    gtk_container_add (GTK_CONTAINER (priv->toolbook), toolbar);

    guint n = midori_panel_page_num (panel, child);

    const gchar* text = label ? label : _("Untitled");
    g_object_set_data (G_OBJECT (child), "label", (gchar*)text);

    GtkWidget* image;
    GtkToolItem* toolitem = gtk_radio_tool_button_new (priv->group);
    priv->group = gtk_radio_tool_button_get_group (GTK_RADIO_TOOL_BUTTON (
                                                   toolitem));
    gtk_tool_button_set_label (GTK_TOOL_BUTTON (toolitem), text);
    if (icon)
    {
        image = gtk_image_new_from_icon_name (icon, GTK_ICON_SIZE_BUTTON);
        gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (toolitem), image);
    }
    g_object_set_data (G_OBJECT (toolitem), "page", child);
    g_signal_connect (toolitem, "clicked",
                      G_CALLBACK (midori_panel_menu_item_activate_cb), panel);
    gtk_widget_show_all (GTK_WIDGET (toolitem));
    gtk_toolbar_insert (GTK_TOOLBAR (priv->toolbar), toolitem, -1);

    if (priv->menu)
    {
        GtkWidget* menuitem = gtk_image_menu_item_new_with_label (text);
        if (icon)
        {
            image = gtk_image_new_from_icon_name (icon, GTK_ICON_SIZE_MENU);
            gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuitem),
                                           image);
        }
        gtk_widget_show_all (menuitem);
        g_object_set_data (G_OBJECT (menuitem), "page", child);
        g_signal_connect (menuitem, "activate",
                          G_CALLBACK (midori_panel_menu_item_activate_cb),
                          panel);
        gtk_menu_shell_append (GTK_MENU_SHELL (priv->menu), menuitem);
    }

    return n;
}

/**
 * midori_panel_get_current_page:
 * @panel: a #MidoriPanel
 *
 * Retrieves the index of the currently selected page.
 *
 * If @panel has no children, -1 is returned.
 *
 * Return value: the index of the current page, or -1
 **/
gint
midori_panel_get_current_page (MidoriPanel* panel)
{
    g_return_val_if_fail (MIDORI_IS_PANEL (panel), -1);

    MidoriPanelPrivate* priv = panel->priv;

    return gtk_notebook_get_current_page (GTK_NOTEBOOK (priv->notebook));
}

static GtkWidget*
_midori_panel_child_for_scrolled (MidoriPanel* panel,
                                  GtkWidget*   scrolled)
{
    GtkWidget* child = gtk_bin_get_child (GTK_BIN (scrolled));
    if (GTK_IS_VIEWPORT (child))
        child = gtk_bin_get_child (GTK_BIN (child));
    return child;
}

/**
 * midori_panel_get_nth_page:
 * @panel: a #MidoriPanel
 *
 * Retrieves the child widget of the nth page.
 *
 * If @panel has no children, %NULL is returned.
 *
 * Return value: the child widget of the new page, or %NULL
 **/
GtkWidget*
midori_panel_get_nth_page (MidoriPanel* panel,
                           guint        page_num)
{
    g_return_val_if_fail (MIDORI_IS_PANEL (panel), NULL);

    MidoriPanelPrivate* priv = panel->priv;

    GtkWidget* scrolled = gtk_notebook_get_nth_page (
        GTK_NOTEBOOK (priv->notebook), page_num);
    if (scrolled)
        return _midori_panel_child_for_scrolled (panel, scrolled);
    return NULL;
}

/**
 * midori_panel_get_n_pages:
 * @panel: a #MidoriPanel
 *
 * Retrieves the number of pages contained in the panel.
 *
 * Return value: the number of pages
 **/
guint
midori_panel_get_n_pages (MidoriPanel* panel)
{
    g_return_val_if_fail (MIDORI_IS_PANEL (panel), 0);

    MidoriPanelPrivate* priv = panel->priv;

    return gtk_notebook_get_n_pages (GTK_NOTEBOOK (priv->notebook));
}

static GtkWidget*
_midori_panel_scrolled_for_child (MidoriPanel* panel,
                                  GtkWidget*   child)
{
    GtkWidget* scrolled = gtk_widget_get_parent (GTK_WIDGET (child));
    if (GTK_IS_VIEWPORT (scrolled))
        scrolled = gtk_widget_get_parent (scrolled);
    return scrolled;
}

/**
 * midori_panel_page_num:
 * @panel: a #MidoriPanel
 *
 * Retrieves the index of the page associated to @widget.
 *
 * If @panel has no children, -1 is returned.
 *
 * Return value: the index of page associated to @widget, or -1
 **/
gint
midori_panel_page_num (MidoriPanel* panel,
                       GtkWidget*   child)
{
    g_return_val_if_fail (MIDORI_IS_PANEL (panel), -1);

    MidoriPanelPrivate* priv = panel->priv;

    GtkWidget* scrolled = _midori_panel_scrolled_for_child (panel, child);
    return gtk_notebook_page_num (GTK_NOTEBOOK (priv->notebook), scrolled);
}

/**
 * midori_panel_set_current_page:
 * @panel: a #MidoriPanel
 * @n: index of the page to switch to, or -1 to mean the last page
 *
 * Switches to the page with the given index.
 *
 * The child must be visible, otherwise the underlying GtkNotebook will
 * silently ignore the attempt to switch the page.
 **/
void
midori_panel_set_current_page (MidoriPanel* panel,
                               gint         n)
{
    g_return_if_fail (MIDORI_IS_PANEL (panel));

    MidoriPanelPrivate* priv = panel->priv;

    gtk_notebook_set_current_page (GTK_NOTEBOOK (priv->toolbook), n);
    gtk_notebook_set_current_page (GTK_NOTEBOOK (priv->notebook), n);
    GtkWidget* child = midori_panel_get_nth_page (panel, n);
    if (child)
    {
        const gchar* label = g_object_get_data (G_OBJECT (child), "label");
        g_object_set (priv->toolbar_label, "label", label, NULL);
    }
}
