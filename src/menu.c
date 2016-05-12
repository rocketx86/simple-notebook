/*
 * Simple Notebook
 * menu.c
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

GtkWidget *bookMenu = NULL;
GtkWidget *sectionMenu = NULL;
GtkWidget *entryMenu = NULL;
GtkWidget *textMenu = NULL;

GtkWidget *bookBookMi = NULL;
GtkWidget *createBookMi = NULL;
GtkWidget *renameBookMi = NULL;
GtkWidget *openBookMi = NULL;
GtkWidget *closeBookMi = NULL;
GtkWidget *deleteBookMi = NULL;
GtkWidget *sep01Mi = NULL;
GtkWidget *viewHistoryMi = NULL;
GtkWidget *clearHistoryMi = NULL;
GtkWidget *viewBookmarksMi = NULL;
GtkWidget *clearBookmarksMi = NULL;
GtkWidget *sep02Mi = NULL;
GtkWidget *searchBookMi = NULL;
GtkWidget *printBookMi = NULL;
GtkWidget *sep03Mi = NULL;
GtkWidget *exitBookMi = NULL;

GtkWidget *sectionSectionMi = NULL;
GtkWidget *createSectionMi = NULL;
GtkWidget *renameSectionMi = NULL;
GtkWidget *copySectionMi = NULL;
GtkWidget *moveSectionMi = NULL;
GtkWidget *cleanTrashMi = NULL;
GtkWidget *deleteSectionMi = NULL;
GtkWidget *sep04Mi = NULL;
GtkWidget *searchSectionMi = NULL;
GtkWidget *printSectionMi = NULL;

GtkWidget *entryEntryMi = NULL;
GtkWidget *createEntryMi = NULL;
GtkWidget *renameEntryMi = NULL;
GtkWidget *copyEntryMi = NULL;
GtkWidget *moveEntryMi = NULL;
GtkWidget *trashEntryMi = NULL;
GtkWidget *deleteEntryMi = NULL;
GtkWidget *sep05Mi = NULL;
GtkWidget *backHistoryMi = NULL;
GtkWidget *forwardHistoryMi = NULL;
GtkWidget *bookmarkEntryMi = NULL;
GtkWidget *sep06Mi = NULL;
GtkWidget *searchEntryMi = NULL;
GtkWidget *printEntryMi = NULL;

GtkWidget *textTextMi = NULL;
GtkWidget *replaceTextMi = NULL;
GtkWidget *undoTextMi = NULL;
GtkWidget *redoTextMi = NULL;
GtkWidget *selectTextMi = NULL;
GtkWidget *cutTextMi = NULL;
GtkWidget *copyTextMi = NULL;
GtkWidget *pasteTextMi = NULL;
GtkWidget *deleteTextMi = NULL;

GtkWidget* create_main_menu (void)
{
	GtkWidget *iconw;

	// Setup menu
	main_menu = gtk_menu_bar_new();
	bookMenu = gtk_menu_new();
	sectionMenu = gtk_menu_new();
	entryMenu = gtk_menu_new();
	textMenu = gtk_menu_new();


	// Notebook menu items
	bookBookMi = gtk_menu_item_new_with_mnemonic("_Notebook");

	createBookMi = gtk_image_menu_item_new_with_mnemonic("_Create Notebook");
	iconw = create_icon_widget("create-book.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(createBookMi), iconw);

	renameBookMi = gtk_image_menu_item_new_with_mnemonic("_Rename Notebook");
	iconw = create_icon_widget("rename-book.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(renameBookMi), iconw);

	openBookMi = gtk_image_menu_item_new_with_mnemonic("_Open Notebook...");
	iconw = create_icon_widget("open-book.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(openBookMi), iconw);

	closeBookMi = gtk_image_menu_item_new_with_mnemonic("C_lose Notebook");
	iconw = create_icon_widget("close-book.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(closeBookMi), iconw);

	deleteBookMi = gtk_image_menu_item_new_with_mnemonic("_Delete Notebook");
	iconw = create_icon_widget("delete-book.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(deleteBookMi), iconw);

	sep01Mi = gtk_separator_menu_item_new();

	viewHistoryMi = gtk_image_menu_item_new_with_mnemonic("View _History...");
	iconw = create_icon_widget("view-history.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(viewHistoryMi), iconw);

	clearHistoryMi = gtk_image_menu_item_new_with_mnemonic("Cl_ear History");
	iconw = create_icon_widget("clean.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(clearHistoryMi), iconw);

	viewBookmarksMi = gtk_image_menu_item_new_with_mnemonic("View _Bookmarks...");
	iconw = create_icon_widget("view-bookmarks.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(viewBookmarksMi), iconw);

	clearBookmarksMi = gtk_image_menu_item_new_with_mnemonic("Cle_ar Bookmarks");
	iconw = create_icon_widget("clean.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(clearBookmarksMi), iconw);

	sep02Mi = gtk_separator_menu_item_new();

	searchBookMi = gtk_image_menu_item_new_with_mnemonic("_Search Notebook...");
	iconw = create_icon_widget("search.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(searchBookMi), iconw);

	printBookMi = gtk_image_menu_item_new_with_mnemonic("_Print Notebook...");
	iconw = create_icon_widget("print.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(printBookMi), iconw);

	sep03Mi = gtk_separator_menu_item_new();

	exitBookMi = gtk_image_menu_item_new_with_mnemonic("E_xit Notebook");
	iconw = create_icon_widget("exit.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(exitBookMi), iconw);


	// Section menu items
	sectionSectionMi = gtk_menu_item_new_with_mnemonic("_Section");

	createSectionMi = gtk_image_menu_item_new_with_mnemonic("_Create Section");
	iconw = create_icon_widget("create-section.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(createSectionMi), iconw);

	renameSectionMi = gtk_image_menu_item_new_with_mnemonic("_Rename Section...");
	iconw = create_icon_widget("rename-section.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(renameSectionMi), iconw);

	copySectionMi = gtk_image_menu_item_new_with_mnemonic("C_opy Section...");
	iconw = create_icon_widget("copy-section.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(copySectionMi), iconw);

	moveSectionMi = gtk_image_menu_item_new_with_mnemonic("_Move Section...");
	iconw = create_icon_widget("move-section.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(moveSectionMi), iconw);

	cleanTrashMi = gtk_image_menu_item_new_with_mnemonic("C_lean Trash");
	iconw = create_icon_widget("clean-trash.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(cleanTrashMi), iconw);

	deleteSectionMi = gtk_image_menu_item_new_with_mnemonic("_Delete Section");
	iconw = create_icon_widget("delete-section.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(deleteSectionMi), iconw);

	sep04Mi = gtk_separator_menu_item_new();

	searchSectionMi = gtk_image_menu_item_new_with_mnemonic("_Search Section...");
	iconw = create_icon_widget("search.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(searchSectionMi), iconw);

	printSectionMi = gtk_image_menu_item_new_with_mnemonic("_Print Section...");
	iconw = create_icon_widget("print.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(printSectionMi), iconw);


	// Entry menu items
	entryEntryMi = gtk_menu_item_new_with_mnemonic("_Entry");

	createEntryMi = gtk_image_menu_item_new_with_mnemonic("_Create Entry");
	iconw = create_icon_widget("create-entry.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(createEntryMi), iconw);

	renameEntryMi = gtk_image_menu_item_new_with_mnemonic("_Rename Entry...");
	iconw = create_icon_widget("rename-entry.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(renameEntryMi), iconw);

	copyEntryMi = gtk_image_menu_item_new_with_mnemonic("_Copy Entry...");
	iconw = create_icon_widget("copy-entry.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(copyEntryMi), iconw);

	moveEntryMi = gtk_image_menu_item_new_with_mnemonic("_Move Entry...");
	iconw = create_icon_widget("move-entry.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(moveEntryMi), iconw);

	trashEntryMi = gtk_image_menu_item_new_with_mnemonic("_Trash Entry");
	iconw = create_icon_widget("trash-entry.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(trashEntryMi), iconw);

	deleteEntryMi = gtk_image_menu_item_new_with_mnemonic("_Delete Entry");
	iconw = create_icon_widget("delete-entry.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(deleteEntryMi), iconw);

	sep05Mi = gtk_separator_menu_item_new();

	backHistoryMi = gtk_image_menu_item_new_with_mnemonic("_Back History");
	iconw = create_icon_widget("back-history.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(backHistoryMi), iconw);

	forwardHistoryMi = gtk_image_menu_item_new_with_mnemonic("_Forward History");
	iconw = create_icon_widget("forward-history.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(forwardHistoryMi), iconw);

	bookmarkEntryMi = gtk_image_menu_item_new_with_mnemonic("Boo_kmark Entry...");
	iconw = create_icon_widget("bookmark-entry.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(bookmarkEntryMi), iconw);

	sep06Mi = gtk_separator_menu_item_new();

	searchEntryMi = gtk_image_menu_item_new_with_mnemonic("_Search Entry...");
	iconw = create_icon_widget("search-entry.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(searchEntryMi), iconw);

	printEntryMi = gtk_image_menu_item_new_with_mnemonic("_Print Entry...");
	iconw = create_icon_widget("print.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(printEntryMi), iconw);


	// Text menu items
	textTextMi = gtk_menu_item_new_with_mnemonic("_Text");

	replaceTextMi = gtk_image_menu_item_new_with_mnemonic("_Replace Text");
	iconw = create_icon_widget("replace-text.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(replaceTextMi), iconw);

	undoTextMi = gtk_image_menu_item_new_with_mnemonic("_Undo Text");
	iconw = create_icon_widget("undo-text.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(undoTextMi), iconw);

	redoTextMi = gtk_image_menu_item_new_with_mnemonic("R_edo Text");
	iconw = create_icon_widget("redo-text.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(redoTextMi), iconw);

	selectTextMi = gtk_image_menu_item_new_with_mnemonic("Select _All");
	iconw = create_icon_widget("select-all.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(selectTextMi), iconw);

	cutTextMi = gtk_image_menu_item_new_with_mnemonic("_Cut Text");
	iconw = create_icon_widget("cut-text.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(cutTextMi), iconw);

	copyTextMi = gtk_image_menu_item_new_with_mnemonic("C_opy Text");
	iconw = create_icon_widget("copy-text.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(copyTextMi), iconw);

	pasteTextMi = gtk_image_menu_item_new_with_mnemonic("_Paste Text");
	iconw = create_icon_widget("paste-text.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(pasteTextMi), iconw);

	deleteTextMi = gtk_image_menu_item_new_with_mnemonic("_Delete Text");
	iconw = create_icon_widget("delete-text.xpm");
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM(deleteTextMi), iconw);


	// Notebook menu
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(bookBookMi), bookMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMenu), createBookMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMenu), renameBookMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMenu), openBookMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMenu), closeBookMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMenu), deleteBookMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMenu), sep01Mi);
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMenu), viewHistoryMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMenu), clearHistoryMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMenu), viewBookmarksMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMenu), clearBookmarksMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMenu), sep02Mi);
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMenu), searchBookMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMenu), printBookMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMenu), sep03Mi);
	gtk_menu_shell_append(GTK_MENU_SHELL(bookMenu), exitBookMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(main_menu), bookBookMi);


	// Section menu
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(sectionSectionMi), sectionMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(sectionMenu), createSectionMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(sectionMenu), renameSectionMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(sectionMenu), copySectionMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(sectionMenu), moveSectionMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(sectionMenu), cleanTrashMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(sectionMenu), deleteSectionMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(sectionMenu), sep04Mi);
	gtk_menu_shell_append(GTK_MENU_SHELL(sectionMenu), searchSectionMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(sectionMenu), printSectionMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(main_menu), sectionSectionMi);


	// Entry menu
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(entryEntryMi), entryMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(entryMenu), createEntryMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(entryMenu), renameEntryMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(entryMenu), copyEntryMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(entryMenu), moveEntryMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(entryMenu), trashEntryMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(entryMenu), deleteEntryMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(entryMenu), sep05Mi);
	gtk_menu_shell_append(GTK_MENU_SHELL(entryMenu), backHistoryMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(entryMenu), forwardHistoryMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(entryMenu), bookmarkEntryMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(entryMenu), sep06Mi);
	gtk_menu_shell_append(GTK_MENU_SHELL(entryMenu), searchEntryMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(entryMenu), printEntryMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(main_menu), entryEntryMi);


	// Text menu
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(textTextMi), textMenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(textMenu), replaceTextMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(textMenu), undoTextMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(textMenu), redoTextMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(textMenu), selectTextMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(textMenu), cutTextMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(textMenu), copyTextMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(textMenu), pasteTextMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(textMenu), deleteTextMi);
	gtk_menu_shell_append(GTK_MENU_SHELL(main_menu), textTextMi);


	// Notebook menu signal handlers
	g_signal_connect(G_OBJECT(createBookMi), "activate",
		G_CALLBACK(create_book), NULL);
	g_signal_connect(G_OBJECT(renameBookMi), "activate",
		G_CALLBACK(rename_book), NULL);
	g_signal_connect(G_OBJECT(deleteBookMi), "activate",
		G_CALLBACK(delete_book), NULL);
	g_signal_connect(G_OBJECT(viewHistoryMi), "activate",
		G_CALLBACK(view_history), NULL);
	g_signal_connect(G_OBJECT(clearHistoryMi), "activate",
		G_CALLBACK(clear_history), NULL);
	g_signal_connect(G_OBJECT(exitBookMi), "activate",
		G_CALLBACK(on_main_window_delete), NULL);


	// Section menu signal handlers
	g_signal_connect(G_OBJECT(createSectionMi), "activate",
		G_CALLBACK(create_section), NULL);
	g_signal_connect(G_OBJECT(renameSectionMi), "activate",
		G_CALLBACK(rename_section), NULL);
	g_signal_connect(G_OBJECT(deleteSectionMi), "activate",
		G_CALLBACK(delete_section), NULL);


	// Entry menu signal handlers
	g_signal_connect(G_OBJECT(createEntryMi), "activate",
		G_CALLBACK(create_entry), NULL);
	g_signal_connect(G_OBJECT(renameEntryMi), "activate",
		G_CALLBACK(rename_entry), NULL);
	g_signal_connect(G_OBJECT(copyEntryMi), "activate",
		G_CALLBACK(copy_entry), NULL);
	g_signal_connect(G_OBJECT(moveEntryMi), "activate",
		G_CALLBACK(move_entry), NULL);
	g_signal_connect(G_OBJECT(deleteEntryMi), "activate",
		G_CALLBACK(delete_entry), NULL);
	g_signal_connect(G_OBJECT(forwardHistoryMi), "activate",
		G_CALLBACK(forward_history), NULL);
	g_signal_connect(G_OBJECT(backHistoryMi), "activate",
		G_CALLBACK(back_history), NULL);


	// Text menu signal handlers
	g_signal_connect(G_OBJECT(undoTextMi), "activate",
		G_CALLBACK(undo_text), NULL);
	g_signal_connect(G_OBJECT(redoTextMi), "activate",
		G_CALLBACK(redo_text), NULL);
	g_signal_connect(G_OBJECT(selectTextMi), "activate",
		G_CALLBACK(select_all_text), NULL);
	g_signal_connect(G_OBJECT(cutTextMi), "activate",
		G_CALLBACK(cut_text), NULL);
	g_signal_connect(G_OBJECT(copyTextMi), "activate",
		G_CALLBACK(copy_text), NULL);
	g_signal_connect(G_OBJECT(pasteTextMi), "activate",
		G_CALLBACK(paste_text), NULL);
	g_signal_connect(G_OBJECT(deleteTextMi), "activate",
		G_CALLBACK(delete_text), NULL);


	// Notebook menu accelerators
	gtk_widget_add_accelerator(createBookMi, "activate", accel_group,
						GDK_KEY_F1, GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(renameBookMi, "activate", accel_group,
						GDK_KEY_F2, GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(deleteBookMi, "activate", accel_group,
						GDK_KEY_F4, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(viewHistoryMi, "activate", accel_group,
						GDK_KEY_h, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(clearHistoryMi, "activate", accel_group,
						GDK_KEY_g, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(exitBookMi, "activate", accel_group,
						GDK_KEY_Escape, 0, GTK_ACCEL_VISIBLE);


	// Section menu accelerators
	gtk_widget_add_accelerator(createSectionMi, "activate", accel_group,
						GDK_F5, GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(renameSectionMi, "activate", accel_group,
						GDK_F6, GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(deleteSectionMi, "activate", accel_group,
						GDK_F8, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);


	// Entry menu accelerators
	gtk_widget_add_accelerator(createEntryMi, "activate", accel_group,
						GDK_F9, GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(renameEntryMi, "activate", accel_group,
						GDK_F10, GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(copyEntryMi, "activate", accel_group,
						GDK_F11, GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(moveEntryMi, "activate", accel_group,
						GDK_F11, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(deleteEntryMi, "activate", accel_group,
						GDK_F12, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(forwardHistoryMi, "activate", accel_group,
						GDK_Right, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(backHistoryMi, "activate", accel_group,
						GDK_Left, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);


	// Text menu accelerators
	gtk_widget_add_accelerator(replaceTextMi, "activate", accel_group,
						GDK_f, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(selectTextMi, "activate", accel_group,
						GDK_a, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(undoTextMi, "activate", accel_group,
						GDK_z, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(redoTextMi, "activate", accel_group,
						GDK_y, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(selectTextMi, "activate", accel_group,
						GDK_a, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(cutTextMi, "activate", accel_group,
						GDK_x, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(copyTextMi, "activate", accel_group,
						GDK_c, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(pasteTextMi, "activate", accel_group,
						GDK_v, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(deleteTextMi, "activate", accel_group,
						GDK_Delete, 0, GTK_ACCEL_VISIBLE);

	return main_menu;
}
