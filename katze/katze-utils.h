/*
 Copyright (C) 2007 Christian Dywan <christian@twotoasts.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 See the file COPYING for the full license text.
*/

#ifndef __KATZE_UTILS_H__
#define __KATZE_UTILS_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * katze_assign:
 * @lvalue: a pointer
 * @rvalue: the new value
 *
 * Frees @lvalue if needed and assigns it the value of @rvalue.
 **/
#define katze_assign(lvalue, rvalue) \
    if (1) \
    { \
        g_free (lvalue); \
        lvalue = rvalue; \
    }

/**
 * katze_object_assign:
 * @lvalue: a gobject
 * @rvalue: the new value
 *
 * Unrefs @lvalue if needed and assigns it the value of @rvalue.
 **/
#define katze_object_assign(lvalue, rvalue) \
    if (1) \
    { \
        if (lvalue) \
            g_object_unref (lvalue); \
        lvalue = rvalue; \
    }

G_END_DECLS

#endif /* __KATZE_UTILS_H__ */