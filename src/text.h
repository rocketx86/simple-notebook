/*
 * Simple Notebook
 * text.h
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

#ifndef __SN_TEXT_H__
#define __SN_TEXT_H__

extern gboolean on_text_view_focus(GtkWidget *widget, GdkEvent  *event,
					gpointer user_data);
extern gchar* strdelchr(gchar *dst, const gchar *src, const gchar *del);
extern gchar* get_auto_entry_name(GtkTextView *text_view);
extern gboolean select_all_text();
extern gboolean cut_text();
extern gboolean copy_text();
extern gboolean paste_text();
extern void on_paste_done(GtkTextBuffer *textbuffer, GtkClipboard *clipboard,
					gpointer user_data);
extern gboolean delete_text();
extern gboolean on_text_key_press(GtkTextView *text_view,
					GdkEventKey *event, gpointer data);
extern void set_text_view_focus(GtkTextView *text_view);
extern gboolean read_text(book_data *book, entry_data *entry);
extern gboolean write_text(book_data *book, entry_data *entry);

#endif //__SN_TEXT_H__
