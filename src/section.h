/*
 * Simple Notebook
 * section.h
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

#ifndef __SN_SECTION_H__
#define __SN_SECTION_H__

enum { SECTION_ITEM = 0, N_SECTION_COLUMNS };

typedef struct _entry_data entry_data;

typedef struct _section_data {
	gchar name[MAX_NAME_LEN];
	GList *entry_list;
	book_data *parent_book;
	entry_data *curr_entry;
	GtkTreePath *curr_entry_path;
} section_data;

extern const gchar *trash_section_name;

extern gboolean on_section_key_press(GtkTreeView *section_view,
					GdkEventKey *event, gpointer data);
extern void on_section_change(GtkTreeView *section_view, gpointer data);
extern gboolean populate_sections(book_data *book);
extern gboolean init_section_view(GtkTreeView *section_view, book_data *book);
extern gboolean create_section();
extern gboolean rename_section();
extern gboolean clean_trash();
extern gboolean delete_section();
extern gboolean shift_section_up();
extern gboolean shift_section_down();
extern void free_section(gpointer);

#define get_current_section_or_return_with_warning()		\
	book->curr_section;										\
	if(section == NULL) {									\
		sn_warning("No current section in %s.", __func__);	\
		return 0;											\
	}

#endif //__SN_SECTION_H__
