/*
 * Simple Notebook
 * edit.h
 *
 * Copyright 2016 Roy Richardson <rocketx86@users.github.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */

#ifndef __SN_EDIT_H__
#define __SN_EDIT_H__

enum {edit_insert, edit_delete};

typedef struct _edit_data {
	guint32 edit_type;
	guint32 edit_action;
	guint32 edit_start;
	guint32 edit_end;
	guint32 edit_length;
	gchar *edit_text;
} edit_data;

extern edit_data* new_edit(guint32 edit_type, GtkTextIter *edit_start,
				GtkTextIter *edit_end, gchar *edit_text, gboolean same_action);
extern edit_data* add_edit(edit_data *edit);
extern edit_data* get_current_edit();
extern gboolean undo_text();
extern gboolean redo_text();
extern gint copy_file(const gchar *to, const gchar *from);
extern void free_edit(gpointer edit);

#endif //__SN_EDIT_H__
