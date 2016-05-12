/*
 * Simple Notebook
 * book.h
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

#ifndef __SN_BOOK_H__
#define __SN_BOOK_H__

typedef struct _section_data section_data;
typedef struct _history_data history_data;
typedef struct _view_data view_data;

typedef struct _book_data {
	gchar name[MAX_NAME_LEN];
	GList *section_list;
	GList *history_list;
	view_data *view;
	gboolean is_open;
	gboolean add_history;
	history_data *curr_history;
	section_data *curr_section;
	section_data *trash_section;
	GtkTreePath *curr_section_path;
} book_data;

extern gboolean on_book_key_press(GtkNotebook *notebook,
					GdkEventKey *event, gpointer data);
extern void on_book_change(GtkNotebook *notebook, gpointer page,
					guint page_num, gpointer user_data);
extern gboolean populate_books(gchar *note_dir);
extern gboolean create_book();
extern gboolean rename_book();
extern gboolean open_book();
extern gboolean close_book();
extern gboolean delete_book();

extern book_data* new_book(master_data *master, gchar *note_dir);
extern gboolean read_book(book_data *book, gchar *note_dir);
extern gboolean write_book(book_data *book, gchar *note_dir);
extern void free_book(gpointer book);

#define get_current_book_or_return_with_warning()			\
	master->curr_book;										\
	if(book == NULL) {										\
		sn_warning("No current book in %s.", __func__);		\
		return 0;											\
	}

#endif //__SN_BOOK_H__
