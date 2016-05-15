/*
 * Simple Notebook
 * page.h
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

#ifndef __SN_PAGE_H__
#define __SN_PAGE_H__

typedef struct _view_data {
	GtkWidget *book_page;
	GtkTreeView *section_view;
	GtkTreeView *entry_view;
	GtkTextView *text_view;
} view_data;

extern view_data* create_book_page (gpointer main_view, gpointer data);
extern GtkWidget* get_book_page(book_data *book);
extern GtkTreeView* get_section_view(book_data *book);
extern GtkTreeView* get_entry_view(book_data *book);
extern GtkTextView* get_text_view(book_data *book);

#endif //__SN_PAGE_H__
