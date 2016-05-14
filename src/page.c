/*
 * Simple Notebook
 * page.c
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

#include "common.h"

/*
 * Create book page
 * Called from create_book in book.c
 * Called from populate_books in book.c
 */
view_data* create_book_page(gpointer main_view, gpointer data)
{
	GtkWidget *book_page;
	GtkWidget *book_label;
	GtkWidget *left_pane;
	GtkWidget *section_frame;
	GtkWidget *entry_frame;
	GtkWidget *text_frame;
	GtkWidget *section_scroll;
	GtkWidget *entry_scroll;
	GtkWidget *text_scroll;
	GtkWidget *section_view;
	GtkWidget *entry_view;
	GtkWidget *text_view;
	book_data *book = NULL;
	view_data *view = NULL;

	book = (book_data*)data;

	sn_trace("Creating book page for [%s].", book->name);

	// Create Page
	book_page = gtk_hpaned_new ();
	book_label = gtk_label_new(book->name);
	gtk_notebook_append_page(GTK_NOTEBOOK(main_view), book_page, book_label);

	left_pane = gtk_vpaned_new ();
	gtk_paned_add1(GTK_PANED(book_page), left_pane);

	// Section View
	section_frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(section_frame), GTK_SHADOW_NONE);
	gtk_frame_set_label(GTK_FRAME(section_frame), "Section");
	gtk_container_set_border_width(GTK_CONTAINER (section_frame), 0);
	gtk_widget_set_size_request(section_frame, MAIN_WIDTH / 3, MAIN_HEIGHT / 3);
	gtk_paned_add1(GTK_PANED(left_pane), section_frame);

	section_scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_shadow_type
		(GTK_SCROLLED_WINDOW(section_scroll), GTK_SHADOW_IN);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(section_scroll),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(section_frame), section_scroll);

	section_view = gtk_tree_view_new ();
	gtk_container_add(GTK_CONTAINER(section_scroll), section_view);
	init_section_view(GTK_TREE_VIEW(section_view), book);

	// Entry View
	entry_frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(entry_frame), GTK_SHADOW_NONE);
	gtk_frame_set_label(GTK_FRAME(entry_frame), "Entry");
	gtk_container_set_border_width(GTK_CONTAINER(entry_frame), 0);
	gtk_widget_set_size_request(entry_frame, MAIN_WIDTH / 3, -1);
	gtk_paned_add2(GTK_PANED(left_pane), entry_frame);

	entry_scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_shadow_type
		(GTK_SCROLLED_WINDOW(entry_scroll), GTK_SHADOW_IN);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(entry_scroll),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_container_add(GTK_CONTAINER(entry_frame), entry_scroll);

	entry_view = gtk_tree_view_new ();
	gtk_container_add(GTK_CONTAINER(entry_scroll), entry_view);
	init_entry_view(GTK_TREE_VIEW(entry_view), book);

	// Text View
	text_frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(text_frame), GTK_SHADOW_NONE);
	gtk_frame_set_label(GTK_FRAME(text_frame), "Text");
	gtk_container_set_border_width(GTK_CONTAINER(text_frame), 0);
	gtk_paned_add2(GTK_PANED(book_page), text_frame);

	text_scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_shadow_type
		(GTK_SCROLLED_WINDOW(text_scroll), GTK_SHADOW_IN);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(text_scroll),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_container_add(GTK_CONTAINER(text_frame), text_scroll);

	text_view = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
	gtk_container_add(GTK_CONTAINER(text_scroll), text_view);

	// Set signal handlers
	g_signal_connect(text_view, "key-press-event",
		G_CALLBACK(on_text_key_press), book);

	g_signal_connect_swapped(section_view, "button-press-event",
		G_CALLBACK(on_main_view_mouse_button_press), sectionMenu);

	g_signal_connect_swapped(entry_view, "button-press-event",
		G_CALLBACK(on_main_view_mouse_button_press), entryMenu);

	g_signal_connect_swapped(text_view, "button-press-event",
		G_CALLBACK(on_main_view_mouse_button_press), textMenu);

	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer
		(GTK_TEXT_VIEW(text_view));
	g_signal_connect(text_buffer, "paste-done",
		G_CALLBACK(on_paste_done), NULL);

	g_signal_connect(text_view, "focus-in-event",
		G_CALLBACK(on_text_view_focus), NULL);

	view = g_new0(view_data, NEW_INSTANCE);
	view->book_page = book_page;
	view->section_view = GTK_TREE_VIEW(section_view);
	view->entry_view = GTK_TREE_VIEW(entry_view);
	view->text_view = GTK_TEXT_VIEW(text_view);

	return view;
}
