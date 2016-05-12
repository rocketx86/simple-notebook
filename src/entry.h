/*
 * Simple Notebook
 * entry.h
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

#ifndef __SN_ENTRY_H__
#define __SN_ENTRY_H__

enum { ENTRY_ITEM = 0, N_ENTRY_COLUMNS };

typedef struct _edit_data edit_data;

typedef struct _entry_data {
	gchar name[MAX_NAME_LEN];
	GList *edit_list;
	edit_data *curr_edit;
	section_data *parent_section;
	gboolean need_rename;
} entry_data;

extern gboolean on_entry_key_press(GtkTreeView *entry_view,
					GdkEventKey *event, gpointer data);
extern void on_entry_change(GtkTreeView *entry_view, gpointer data);
extern gboolean populate_entries(book_data *book, section_data *section);
extern gboolean init_entry_view(GtkTreeView *entry_view, book_data *book);
extern gboolean create_entry();
extern gboolean rename_entry();
extern gboolean copy_entry();
extern gboolean move_entry();
extern gboolean delete_entry();
extern gboolean shift_entry_up();
extern gboolean shift_entry_down();
extern void write_current_entry();
extern gboolean on_section_mouse_button_press(GtkWidget *view,
					GdkEvent *event, gpointer data);
extern void free_entry(gpointer);

#define get_current_entry_or_return_with_warning()			\
	section->curr_entry;									\
	if(entry == NULL) {										\
		sn_warning("No current entry in %s.", __func__);	\
		return 0;											\
	}

#endif //__SN_ENTRY_H__
