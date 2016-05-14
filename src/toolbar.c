/*
 * Simple Notebook
 * toolbar.c
 *
 * Copyright 2016 Roy Richardson <rocketx86@users.github.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include "common.h"

// Toolbar order
enum {bh, fh, s1, cb, rb, ob, lb, db, vh, vb, s2, cs, rs, os, ms, nt, ds, s3,
	ce, re, oe, me, te, de, be, se, pe, s4, ft, ut, rt, xt, ct, vt, dt, s5, en};

/*
 * Create toolbar
 */
GtkWidget* create_main_toolbar()
{
	GtkWidget *iconw;
	GtkToolItem *button;

	main_toolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(main_toolbar), GTK_TOOLBAR_ICONS);
	gtk_container_set_border_width(GTK_CONTAINER(main_toolbar), 0);
	gtk_widget_set_size_request(main_toolbar, MAIN_WIDTH - 20, -1);

	// Back button
	iconw = create_icon_widget("back-history.xpm");
	button = gtk_tool_button_new(iconw, "Back History");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, bh);
	gtk_tool_item_set_proxy_menu_item (button, "back-history", backHistoryMi);
	gtk_tool_item_set_tooltip_markup(button, "Back History");
	gtk_tool_item_set_is_important(button, TRUE);
	g_signal_connect(button, "clicked", G_CALLBACK(back_history), NULL);

	// Forward button
	iconw = create_icon_widget("forward-history.xpm");
	button = gtk_tool_button_new(iconw, "Forward History");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, fh);
	gtk_tool_item_set_proxy_menu_item (button, "back-history", forwardHistoryMi);
	gtk_tool_item_set_tooltip_markup(button, "Forward History");
	g_signal_connect(button, "clicked", G_CALLBACK(forward_history), NULL);

	// Separator
	button = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, s1);

	// Create book button
	iconw = create_icon_widget("create-book.xpm");
	button = gtk_tool_button_new(iconw, "Create Notebook");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, cb);
	gtk_tool_item_set_proxy_menu_item (button, "create-book", createBookMi);
	gtk_tool_item_set_tooltip_markup(button, "Create Notebook");
	g_signal_connect(button, "clicked", G_CALLBACK(create_book), NULL);

	// Rename book button
	iconw = create_icon_widget("rename-book.xpm");
	button = gtk_tool_button_new(iconw, "Rename Notebook");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, rb);
	gtk_tool_item_set_proxy_menu_item (button, "rename-book", renameBookMi);
	gtk_tool_item_set_tooltip_markup(button, "Rename Notebook");
	g_signal_connect(button, "clicked", G_CALLBACK(rename_book), NULL);

	// Open book button
	iconw = create_icon_widget("open-book.xpm");
	button = gtk_tool_button_new(iconw, "Open Notebook");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, ob);
	gtk_tool_item_set_proxy_menu_item (button, "open-book", openBookMi);
	gtk_tool_item_set_tooltip_markup(button, "Open Notebook");
	//g_signal_connect(button, "clicked", NULL, NULL);

	// Close book button
	iconw = create_icon_widget("close-book.xpm");
	button = gtk_tool_button_new(iconw, "Close Notebook");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, lb);
	gtk_tool_item_set_proxy_menu_item (button, "close-book", closeBookMi);
	gtk_tool_item_set_tooltip_markup(button, "Close Notebook");
	//g_signal_connect(button, "clicked", NULL, NULL);

	// Delete book button
	iconw = create_icon_widget("delete-book.xpm");
	button = gtk_tool_button_new(iconw, "Delete Notebook");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, db);
	gtk_tool_item_set_proxy_menu_item (button, "delete-book", deleteBookMi);
	gtk_tool_item_set_tooltip_markup(button, "Delete Notebook");
	g_signal_connect(button, "clicked",  G_CALLBACK(delete_book), NULL);

	// View history button
	iconw = create_icon_widget("view-history.xpm");
	button = gtk_tool_button_new(iconw, "View History");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, vh);
	gtk_tool_item_set_proxy_menu_item (button, "view-history", viewHistoryMi);
	gtk_tool_item_set_tooltip_markup(button, "View history");
	g_signal_connect(button, "clicked",  G_CALLBACK(view_history), NULL);

	// View bookmarks button
	iconw = create_icon_widget("view-bookmarks.xpm");
	button = gtk_tool_button_new(iconw, "View Bookmarks");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, vb);
	gtk_tool_item_set_proxy_menu_item (button, "view-bookmarks", viewBookmarksMi);
	gtk_tool_item_set_tooltip_markup(button, "View Bookmarks");
	//g_signal_connect(button, "clicked", NULL, NULL);

	// Separator
	button = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, s2);

	// Create section button
	iconw = create_icon_widget("create-section.xpm");
	button = gtk_tool_button_new(iconw, "Create Section");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, cs);
	gtk_tool_item_set_proxy_menu_item (button, "create-section", createSectionMi);
	gtk_tool_item_set_tooltip_markup(button, "Create Section");
	g_signal_connect(button, "clicked", G_CALLBACK(create_section), NULL);

	// Rename section button
	iconw = create_icon_widget("rename-section.xpm");
	button = gtk_tool_button_new(iconw, "Rename Section");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, rs);
	gtk_tool_item_set_proxy_menu_item (button, "rename-section", renameSectionMi);
	gtk_tool_item_set_tooltip_markup(button, "Rename Section");
	g_signal_connect(button, "clicked", G_CALLBACK(rename_section), NULL);

	// Copy section button
	iconw = create_icon_widget("copy-section.xpm");
	button = gtk_tool_button_new(iconw, "Copy Section");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, os);
	gtk_tool_item_set_proxy_menu_item (button, "copy-section", copySectionMi);
	gtk_tool_item_set_tooltip_markup(button, "Copy Section");
	//g_signal_connect(button, "clicked", NULL, NULL);

	// Move section button
	iconw = create_icon_widget("move-section.xpm");
	button = gtk_tool_button_new(iconw, "Move Section");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, ms);
	gtk_tool_item_set_proxy_menu_item (button, "move-section", moveSectionMi);
	gtk_tool_item_set_tooltip_markup(button, "Move Section");
	//g_signal_connect(button, "clicked", NULL, NULL);

	// Clean trash button
	iconw = create_icon_widget("clean-trash.xpm");
	button = gtk_tool_button_new(iconw, "Clean Trash");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, nt);
	gtk_tool_item_set_proxy_menu_item (button, "clean-trash", cleanTrashMi);
	gtk_tool_item_set_tooltip_markup(button, "Clean Trash");
	g_signal_connect(button, "clicked", G_CALLBACK(clean_trash), NULL);

	// Delete section button
	iconw = create_icon_widget("delete-section.xpm");
	button = gtk_tool_button_new(iconw, "Delete Section");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, ds);
	gtk_tool_item_set_proxy_menu_item (button, "delete-section", deleteSectionMi);
	gtk_tool_item_set_tooltip_markup(button, "Delete Section");
	g_signal_connect(button, "clicked", G_CALLBACK(delete_section), NULL);

	// Separator
	button = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, s3);

	// Create entry button
	iconw = create_icon_widget("create-entry.xpm");
	button = gtk_tool_button_new(iconw, "Create Entry");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, ce);
	gtk_tool_item_set_proxy_menu_item (button, "create-entry", createEntryMi);
	gtk_tool_item_set_tooltip_markup(button, "Create Entry");
	g_signal_connect(button, "clicked", G_CALLBACK(create_entry), NULL);

	// Rename entry button
	iconw = create_icon_widget("rename-entry.xpm");
	button = gtk_tool_button_new(iconw, "Rename Entry");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, re);
	gtk_tool_item_set_proxy_menu_item (button, "rename-entry", renameEntryMi);
	gtk_tool_item_set_tooltip_markup(button, "Rename Entry");
	g_signal_connect(button, "clicked", G_CALLBACK(rename_entry), NULL);

	// Copy entry button
	iconw = create_icon_widget("copy-entry.xpm");
	button = gtk_tool_button_new(iconw, "Copy Entry");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, oe);
	gtk_tool_item_set_proxy_menu_item (button, "copy-entry", copyEntryMi);
	gtk_tool_item_set_tooltip_markup(button, "Copy Entry");
	g_signal_connect(button, "clicked", G_CALLBACK(copy_entry), NULL);

	// Move entry button
	iconw = create_icon_widget("move-entry.xpm");
	button = gtk_tool_button_new(iconw, "Move Entry");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, me);
	gtk_tool_item_set_proxy_menu_item (button, "move-entry", moveEntryMi);
	gtk_tool_item_set_tooltip_markup(button, "Move Entry");
	g_signal_connect(button, "clicked", G_CALLBACK(move_entry), NULL);

	// Trash entry button
	iconw = create_icon_widget("trash-entry.xpm");
	button = gtk_tool_button_new(iconw, "Trash Entry");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, te);
	gtk_tool_item_set_proxy_menu_item (button, "trash-entry", trashEntryMi);
	gtk_tool_item_set_tooltip_markup(button, "Trash Entry");
	g_signal_connect(button, "clicked", G_CALLBACK(trash_entry), NULL);

	// Delete entry button
	iconw = create_icon_widget("delete-entry.xpm");
	button = gtk_tool_button_new(iconw, "Delete Entry");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, de);
	gtk_tool_item_set_proxy_menu_item (button, "delete-entry", deleteEntryMi);
	gtk_tool_item_set_tooltip_markup(button, "Delete Entry");
	g_signal_connect(button, "clicked", G_CALLBACK(delete_entry), NULL);

	// Bookmark entry button
	iconw = create_icon_widget("bookmark-entry.xpm");
	button = gtk_tool_button_new(iconw, "Bookmark Entry");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, be);
	gtk_tool_item_set_proxy_menu_item (button, "bookmark-entry", bookmarkEntryMi);
	gtk_tool_item_set_tooltip_markup(button, "Bookmark Entry");
	//g_signal_connect(button, "clicked", NULL, NULL);

	// Search entry button
	iconw = create_icon_widget("search-entry.xpm");
	button = gtk_tool_button_new(iconw, "Search Entry");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, se);
	gtk_tool_item_set_proxy_menu_item (button, "search-entry", searchEntryMi);
	gtk_tool_item_set_tooltip_markup(button, "Search Entry");
	//g_signal_connect(button, "clicked", NULL, NULL);

	// Print entry button
	iconw = create_icon_widget("print.xpm");
	button = gtk_tool_button_new(iconw, "Print Entry");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, pe);
	gtk_tool_item_set_proxy_menu_item (button, "print-entry", printEntryMi);
	gtk_tool_item_set_tooltip_markup(button, "Print Entry");
	//g_signal_connect(button, "clicked", NULL, NULL);

	// Separator
	button = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, s4);

	// Find text button
	iconw = create_icon_widget("replace-text.xpm");
	button = gtk_tool_button_new(iconw, "Replace Text");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, ft);
	gtk_tool_item_set_proxy_menu_item (button, "replace-text", replaceTextMi);
	gtk_tool_item_set_tooltip_markup(button, "Replace Text");
	//g_signal_connect(button, "clicked", NULL, NULL);

	// Undo text button
	iconw = create_icon_widget("undo-text.xpm");
	button = gtk_tool_button_new(iconw, "Undo Text");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, ut);
	gtk_tool_item_set_proxy_menu_item (button, "undo-text", undoTextMi);
	gtk_tool_item_set_tooltip_markup(button, "Undo Text");
	g_signal_connect(button, "clicked", G_CALLBACK(undo_text), NULL);

	// Redo text button
	iconw = create_icon_widget("redo-text.xpm");
	button = gtk_tool_button_new(iconw, "Redo Text");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, rt);
	gtk_tool_item_set_proxy_menu_item (button, "redo-text", redoTextMi);
	gtk_tool_item_set_tooltip_markup(button, "Redo Text");
	g_signal_connect(button, "clicked", G_CALLBACK(redo_text), NULL);

	// Cut text button
	iconw = create_icon_widget("cut-text.xpm");
	button = gtk_tool_button_new(iconw, "Cut Text");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, xt);
	gtk_tool_item_set_proxy_menu_item (button, "cut-text", cutTextMi);
	gtk_tool_item_set_tooltip_markup(button, "Cut Text");
	g_signal_connect(button, "clicked", G_CALLBACK(cut_text), NULL);

	// Copy text button
	iconw = create_icon_widget("copy-text.xpm");
	button = gtk_tool_button_new(iconw, "Copy Text");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, ct);
	gtk_tool_item_set_proxy_menu_item (button, "copy-text", copyTextMi);
	gtk_tool_item_set_tooltip_markup(button, "Copy Text");
	g_signal_connect(button, "clicked", G_CALLBACK(copy_text), NULL);

	// Paste text button
	iconw = create_icon_widget("paste-text.xpm");
	button = gtk_tool_button_new(iconw, "Paste Text");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, vt);
	gtk_tool_item_set_proxy_menu_item (button, "paste-text", pasteTextMi);
	gtk_tool_item_set_tooltip_markup(button, "Paste Text");
	g_signal_connect(button, "clicked", G_CALLBACK(paste_text), NULL);

	// Delete text button
	iconw = create_icon_widget("delete-text.xpm");
	button = gtk_tool_button_new(iconw, "Delete Text");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, dt);
	gtk_tool_item_set_proxy_menu_item (button, "delete-text", deleteTextMi);
	gtk_tool_item_set_tooltip_markup(button, "Delete Text");
	g_signal_connect(button, "clicked", G_CALLBACK(delete_text), NULL);

	// Separator
	button = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, s5);

	// Exit notebook button
	iconw = create_icon_widget("exit.xpm");
	button = gtk_tool_button_new(iconw, "Exit Notebook");
	gtk_toolbar_insert(GTK_TOOLBAR(main_toolbar), button, en);
	gtk_tool_item_set_proxy_menu_item (button, "exit-book", pasteTextMi);
	gtk_tool_item_set_tooltip_markup(button, "Exit Notebook");
	g_signal_connect(button, "clicked", G_CALLBACK(on_main_window_delete), NULL);

	return main_toolbar;
}
