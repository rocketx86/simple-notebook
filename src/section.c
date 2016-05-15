/*
 * Simple Notebook
 * section.c
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

const gchar *trash_section_name = "Trash";
static const gchar *default_section_name = "Section";

static GtkTreeModel* create_section_model(book_data *book);
static gboolean on_section_selection(GtkTreeSelection *selection, GtkTreeModel *section_model,
					GtkTreePath *section_path, gboolean path_currently_selected,
					gpointer user_data);
static void get_section_name(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell_rend,
					GtkTreeModel *model, GtkTreeIter *tree_iter, gpointer data);

/*
 * On section selection
 */
static gboolean on_section_selection(GtkTreeSelection *selection, GtkTreeModel *section_model,
					GtkTreePath *section_path, gboolean path_currently_selected,
					gpointer user_data)
{
	GtkTreeIter tree_iter;
	section_data *section = NULL;

	if(gtk_tree_model_get_iter(section_model, &tree_iter, section_path)) {
		gtk_tree_model_get(section_model, &tree_iter, SECTION_ITEM, &section, END_OF_LIST);

		if(!path_currently_selected) {
			sn_trace(
			"? Section [%s] will be selected in %s.",
			section->name, __func__);
		} else {
			sn_trace(
			"? Section [%s] will be de-selected in %s.",
			section->name, __func__);
		}
	}
	return TRUE;
} // On section selection

/*
 * On section key press
 * Signal handler for "key-press-event"
 */
gboolean on_section_key_press(GtkTreeView *section_view,
					GdkEventKey *event, gpointer data)
{
	GtkTreePath *section_path = NULL;
	GtkTreeModel *section_model = NULL;
	GtkTreeViewColumn *section_column = NULL;
	GtkTreeIter tree_iter;
	GdkModifierType modifiers;
	book_data *book = NULL;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	// Get section model
	section_model = gtk_tree_view_get_model(section_view);

	// Get default modifiers
	modifiers = gtk_accelerator_get_default_mod_mask();

	switch (event->keyval) {
	case GDK_KEY_Up:
		sn_trace("Up in %s", __func__);

		gtk_widget_grab_focus(GTK_WIDGET(get_entry_view(book)));
		on_entry_key_press(get_entry_view(book), event, data);

		return TRUE;

	case GDK_KEY_Down:
			sn_trace("Down in %s", __func__);

		gtk_widget_grab_focus(GTK_WIDGET(get_entry_view(book)));
		on_entry_key_press(get_entry_view(book), event, data);

		return TRUE;

	case GDK_KEY_Left:
		if((event->state & modifiers) & GDK_CONTROL_MASK) {
			sn_trace("Ctrl+Left in %s", __func__);

			gtk_widget_grab_focus(GTK_WIDGET(main_view));
			on_book_key_press(GTK_NOTEBOOK(main_view), event, data);

		} else if((event->state & modifiers) & GDK_SHIFT_MASK) {
			sn_trace("Shift+Left in %s", __func__);

			shift_section_up();

		} else {
			sn_trace("Left in %s", __func__);

			gtk_tree_view_get_cursor
				(section_view, &section_path, &section_column);

			if(section_path != NULL) {
				if(gtk_tree_path_prev(section_path)) {
					gtk_tree_view_set_cursor
						(section_view, section_path, section_column, FALSE);
				}
			gtk_tree_path_free(section_path);
			}
		}
		return TRUE;

	case GDK_KEY_Right:
		if((event->state & modifiers) & GDK_CONTROL_MASK) {
			sn_trace("Ctrl+Right in %s", __func__);

			gtk_widget_grab_focus(GTK_WIDGET(main_view));
			on_book_key_press(GTK_NOTEBOOK(main_view), event, data);

		} else if((event->state & modifiers) & GDK_SHIFT_MASK) {
			sn_trace("Shift+Right in %s", __func__);

			shift_section_down();

		} else {
			sn_trace("Right in %s", __func__);

			gtk_tree_view_get_cursor
				(section_view, &section_path, &section_column);

			if(section_path != NULL) {
				gtk_tree_path_next(section_path);
				if(gtk_tree_model_get_iter(section_model, &tree_iter, section_path)) {
					gtk_tree_view_set_cursor
						(section_view, section_path, section_column, FALSE);
				}
			gtk_tree_path_free(section_path);
			}
		}
		return TRUE;

	case GDK_KEY_Tab:
		/* no break */
	case GDK_KEY_Return:
		/* no break */
	case GDK_KEY_KP_Enter:
		if((event->state & modifiers) & GDK_SHIFT_MASK) {
			sn_trace("Shift+Enter in %s", __func__);
			add_history();
		} else {
			sn_trace("Enter in %s", __func__);
			set_text_view_focus(get_text_view(book));
		}
		return TRUE;
	}
	return FALSE;
} // On section key press

/*
 * On section change
 * Signal handler for view "cursor-changed" event
 * Called from create_section in section.c
 * Called from rename_section in section.c
 */
void on_section_change(GtkTreeView *section_view, gpointer data)
{
	book_data* book = NULL;
	section_data *section = NULL;
	GtkTreeSelection *selection = NULL;
	GtkTreeModel *section_model = NULL;
	GtkTreeIter tree_iter;

	// Get book from data
	book = (book_data*)data;
	if(book == NULL) {
		sn_warning("No current book in %s.", __func__);
		return;
	}

	// Write text of previous entry
	write_current_entry();

	// Get selected section
	selection = gtk_tree_view_get_selection(section_view);
	if(gtk_tree_selection_get_selected(selection, &section_model, &tree_iter)) {
		gtk_tree_model_get(section_model, &tree_iter, SECTION_ITEM, &section, END_OF_LIST);
		if(section != NULL) {

			// Set current section in book;
			sn_trace("* Selecting section [%s] in %s.",
				section->name, __func__);

			if(book->curr_section_path != NULL)
				gtk_tree_path_free(book->curr_section_path);
			book->curr_section_path = gtk_tree_model_get_path(section_model, &tree_iter);
			book->curr_section = section;

			// Populate entry view
			sn_trace("# Populating entries for section [%s] in %s.",
				section->name, __func__);

			populate_entries(book, section);
		}
	}
	gtk_widget_grab_focus(GTK_WIDGET(section_view));
	return;
} // On section change

/*
 * Populate sections
 * Called from populate_books in book.c
 * Called from create_section in section.c
 */
gboolean populate_sections(book_data *book)
{
	GtkTreeModel *section_model = NULL;
	GtkTreeView *section_view = NULL;
	GtkTreeSelection *selection = NULL;
	GtkTreeIter tree_iter;
	section_data *section = NULL;

	// Get section view
	section_view = get_section_view(book);

	// Create new section model
	section_model = create_section_model(book);

	// Set section model on section view
	gtk_tree_view_set_model(section_view, section_model);

	// Does the book have sections?
	if(book->section_list != NULL) {

		// Get current section in book
		if(book->curr_section != NULL) {
			section = book->curr_section;
			gtk_tree_model_get_iter(section_model, &tree_iter, book->curr_section_path);
		} else {
			section = book->section_list->data;
			gtk_tree_model_get_iter_first (section_model, &tree_iter);
		}

		// Set current section in book
		selection = gtk_tree_view_get_selection(section_view);
		gtk_tree_selection_select_iter(selection, &tree_iter);
		if(book->curr_section_path != NULL)
			gtk_tree_path_free(book->curr_section_path);
		book->curr_section_path = gtk_tree_model_get_path(section_model, &tree_iter);
		book->curr_section = section;

		sn_trace("* Selecting section [%s] in %s.",
			section->name, __func__);

		// Populate entry view
		sn_trace("# Populating entries for section [%s] in %s.",
			section->name, __func__);

		populate_entries(book, section);

	} else {

		// Set current section in book
		if(book->curr_section_path != NULL)
			gtk_tree_path_free(book->curr_section_path);
		book->curr_section_path = NULL;
		book->curr_section = NULL;

		sn_trace("* Selecting section [(null)] in %s.", __func__);
	}

	if (section_model != NULL)
		g_object_unref(section_model);

	return TRUE;
} // Populate sections

/*
 * Get section name
 * Sets the text of cell to section name
 */
static void get_section_name (GtkTreeViewColumn *tree_column,
					GtkCellRenderer *cell_rend, GtkTreeModel *model,
					GtkTreeIter *tree_iter, gpointer data)
{
	section_data *section = NULL;

	gtk_tree_model_get(model, tree_iter, SECTION_ITEM, &section, END_OF_LIST);
	g_object_set(cell_rend, "text", section->name, NULL);
	return;
} // Get section name

/*
 * Create section model
 * Called by populate_sections in section.c
 */
static GtkTreeModel* create_section_model(book_data *book)
{
	GtkListStore *list_store = NULL;
	GtkTreeIter tree_iter;
	GList *section_item = NULL;
	section_data *section = NULL;

	// Create list store
	list_store = gtk_list_store_new(N_SECTION_COLUMNS, G_TYPE_POINTER);

	// Populate list store from section list
	for(section_item = book->section_list;
	section_item != NULL; section_item = section_item->next) {
		section = section_item->data;
		gtk_list_store_append(list_store, &tree_iter);
		gtk_list_store_set(list_store, &tree_iter, SECTION_ITEM, section, END_OF_LIST);
	}
	return GTK_TREE_MODEL(list_store);
} // Create section model

/*
 * Init section view
 * Called from create_book_page in page.c
 */
gboolean init_section_view(GtkTreeView *section_view, book_data *book)
{
	GtkTreeViewColumn *view_col = NULL;
	GtkCellRenderer *cell_rend = NULL;
	GtkTreeSelection *selection = NULL;

	// Create section column
	view_col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(view_col, "Name");
	gtk_tree_view_set_headers_visible(section_view, FALSE);
	gtk_tree_view_append_column(section_view, view_col);

	// Create section cell_rend with get_section_name callback
	cell_rend = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(view_col, cell_rend, TRUE);
	gtk_tree_view_column_set_cell_data_func
		(view_col, cell_rend, get_section_name, NULL, NULL);

	// Set selection function
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(section_view));
	gtk_tree_selection_set_select_function
		(selection, on_section_selection, NULL, NULL);

	// Set signal handlers
	g_signal_connect(section_view, "cursor-changed",
		G_CALLBACK(on_section_change), book);

	g_signal_connect(section_view, "key-press-event",
		G_CALLBACK(on_section_key_press), book);

	return TRUE;
} // Init section view

/*
 * Create section
 * Signal handler for "activate" create section
 */
gboolean create_section()
{
	gchar section_name[MAX_NAME_LEN];
	gchar filename[MAX_PATH_LEN];
	GtkTreeModel *section_model = NULL;
	GtkTreeView *section_view = NULL;
	GtkTreeSelection *selection = NULL;
	GtkTreeIter tree_iter;
	GList *section_item = NULL;
	GtkTreePath *tree_path = NULL;
	section_data *curr_section = NULL;
	book_data *book = NULL;
	section_data *section = NULL;
	gint cindex = 0;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	while (++cindex < MAX_TRIES) {
		// Create section name
		g_snprintf(section_name, sizeof(section_name), "%s%d",
			default_section_name, cindex);

		// Create directory
		g_snprintf(filename, sizeof(filename), "%s%s%s%s%s",
			note_dir, G_DIR_SEPARATOR_S,
			book->name, G_DIR_SEPARATOR_S,
			section_name);

		if(mkdir(filename, S_IRWXU | S_IRWXG) == 0) {
			break;
		}
	}

	sn_trace("Creating section dir [%s].", filename);

	// Create section
	section = g_new0(section_data, NEW_INSTANCE);
	strcpy(section->name, section_name);
	section->parent_book = book;

	// Get selected section
	section_view = get_section_view(book);
	section_model = gtk_tree_view_get_model(section_view);
	selection = gtk_tree_view_get_selection(section_view);

	// Update model
	if(gtk_tree_selection_get_selected(selection, &section_model, &tree_iter)) {
		gtk_tree_model_get(section_model, &tree_iter, SECTION_ITEM, &curr_section, END_OF_LIST);
		tree_path = gtk_tree_model_get_path(section_model, &tree_iter);
		section_item = g_list_find(book->section_list, curr_section);
		book->section_list = g_list_insert_before(book->section_list, section_item, section);
	} else {
		book->section_list = g_list_append(book->section_list, section);
	}

	// Write book
	write_book(book, note_dir);

	// Write text of previous entry
	write_current_entry();

	// Update view
	populate_sections(book);
	if(tree_path != NULL)
		gtk_tree_selection_select_path(selection, tree_path);
	on_section_change(section_view, book);

	// Rename section
	rename_section();

	return TRUE;
} // Create section

/*
 * Rename section
 * Signal handler for "activate" rename section
 * Called from create_section in section.c
 */
gboolean rename_section()
{
	GtkWidget *name_dialog = NULL;
	GtkWidget *msg_dialog = NULL;
	GtkWidget *vbox = NULL;
	GtkWidget *hbox = NULL;
	GtkWidget *label = NULL;
	GtkWidget *name_entry = NULL;
	const gchar *entry_text = NULL;
	gchar old_section_display_name[MAX_NAME_LEN];
	gchar new_section_display_name[MAX_NAME_LEN];
	gchar new_section_name[MAX_PATH_LEN];
	gchar old_section_name[MAX_PATH_LEN];
	book_data *book = NULL;
	section_data *section = NULL;
	gchar *temp_string;
	gint result;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();
	section = get_current_section_or_return_with_warning();

	// Write current entry
	write_current_entry();

	// Check for trash section
	if(section == book->trash_section) {

		msg_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_MODAL,
			GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
			"Unable to rename trash section.");
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msg_dialog),
			"Trash is a special section used to soft delete entries.");
		gtk_window_set_title(GTK_WINDOW(msg_dialog), app_name);
		gtk_window_set_type_hint(GTK_WINDOW(msg_dialog), GDK_WINDOW_TYPE_HINT_MENU);
		gtk_window_set_resizable(GTK_WINDOW(msg_dialog), FALSE);
		result = gtk_dialog_run(GTK_DIALOG(msg_dialog));

		gtk_widget_destroy(msg_dialog);
		return FALSE;
	}

	// Create name dialog
	name_dialog = gtk_dialog_new_with_buttons(app_name, GTK_WINDOW(main_window),
			GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	gtk_widget_set_size_request(name_dialog, SM_DLG_WIDTH, SM_DLG_HEIGHT);
	gtk_window_set_type_hint(GTK_WINDOW(name_dialog), GDK_WINDOW_TYPE_HINT_MENU);
	gtk_window_set_resizable(GTK_WINDOW(name_dialog), FALSE);

	label = gtk_label_new("Enter new section name...");
	name_entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(name_entry), MAX_NAME_LEN-5);
	gtk_entry_set_text(GTK_ENTRY(name_entry), section->name);

	vbox = gtk_dialog_get_content_area(GTK_DIALOG(name_dialog));

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 5);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), name_entry, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 5);

	gtk_entry_set_activates_default(GTK_ENTRY(name_entry), TRUE);
	gtk_dialog_set_default_response(GTK_DIALOG(name_dialog),GTK_RESPONSE_ACCEPT);
	gtk_widget_show_all(name_dialog);

	result = gtk_dialog_run(GTK_DIALOG(name_dialog));

	switch (result) {
	case GTK_RESPONSE_ACCEPT:

		entry_text = gtk_entry_get_text(GTK_ENTRY(name_entry));

		// Scan name for invalid characters
		temp_string = g_malloc0(MAX_NAME_LEN);
		strdelchr(temp_string, entry_text, ILLEGAL_CHARS);
		gtk_entry_set_text(GTK_ENTRY(name_entry), temp_string);
		g_free(temp_string);
		entry_text = gtk_entry_get_text(GTK_ENTRY(name_entry));

		// Old section directory
		g_snprintf(old_section_name, sizeof(old_section_name),
			"%s%s%s%s%s",
			note_dir, G_DIR_SEPARATOR_S,
			book->name, G_DIR_SEPARATOR_S,
			section->name);

		// New section directory
		g_snprintf(new_section_name, sizeof(new_section_name),
			"%s%s%s%s%s",
			note_dir, G_DIR_SEPARATOR_S,
			book->name, G_DIR_SEPARATOR_S,
			entry_text);

		// Ignore rename if names match
		if(strcmp(section->name, entry_text) == 0) {
			gtk_widget_destroy(name_dialog);
			return TRUE;
		}

		// Set display name
		strncpy(old_section_display_name, section->name, MAX_NAME_LEN-5);
		if(strlen(old_section_display_name) > 25)
			strcpy(old_section_display_name+25, "...\0");

		strncpy(new_section_display_name, entry_text, MAX_NAME_LEN-5);
		if(strlen(new_section_display_name) > 25)
			strcpy(new_section_display_name+25, "...\0");

		// Check that new section name is valid
		if(strlen(entry_text) < 1) {

			sn_warning("Unable to rename section [%s] to [%s].",
				section->name, entry_text);

			msg_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window),
				GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
				"Unable to rename section \"%s\" \nto \"%s\".",
				old_section_display_name, new_section_display_name);
			gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msg_dialog),
				"Please ensure the section name contains a valid character.");
			gtk_window_set_title(GTK_WINDOW(msg_dialog), app_name);
			gtk_window_set_type_hint(GTK_WINDOW(msg_dialog), GDK_WINDOW_TYPE_HINT_MENU);
			gtk_window_set_resizable(GTK_WINDOW(msg_dialog), FALSE);
			result = gtk_dialog_run(GTK_DIALOG(msg_dialog));

			gtk_widget_destroy(name_dialog);
			gtk_widget_destroy(msg_dialog);
			return FALSE;
		}

		// Check that new section name is valid
		if(mkdir(new_section_name, S_IRWXU | S_IRWXG) != 0) {

			sn_warning("Unable to rename section [%s] to [%s]",
				section->name, entry_text);

			msg_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_MODAL,
				GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
				"Unable to rename section \"%s\" \nto \"%s\".",
				old_section_display_name, new_section_display_name);
			gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msg_dialog),
				"Please ensure the section name doesn't aleady exist.");
			gtk_window_set_title(GTK_WINDOW(msg_dialog), app_name);
			gtk_window_set_type_hint(GTK_WINDOW(msg_dialog), GDK_WINDOW_TYPE_HINT_MENU);
			gtk_window_set_resizable(GTK_WINDOW(msg_dialog), FALSE);
			result = gtk_dialog_run(GTK_DIALOG(msg_dialog));

			gtk_widget_destroy(name_dialog);
			gtk_widget_destroy(msg_dialog);
			return FALSE;
		}

		sn_trace("Renaming section [%s] to [%s].", section->name, entry_text);

		result = rename(old_section_name, new_section_name);

		if(result == 0) {

			// Remove section history
			remove_section_history();

			// Update section
			strcpy(section->name, entry_text);

			// Update book
			write_book(book, note_dir);

			// Update view
			populate_sections(book);
			on_section_change(get_section_view(book), book);

			gtk_widget_destroy(name_dialog);
			return TRUE;
		} else {
			sn_warning("Unable to rename section [%s].", section->name);
			gtk_widget_destroy(name_dialog);
			return FALSE;
		}

	default:
		gtk_widget_destroy(name_dialog);
		return FALSE;

	} // End switch

	return FALSE;
} // Rename section

/*
 * Clean trash
 * Signal handler for "activate" clean trash
 */
gboolean clean_trash()
{
	GtkWidget *msg_dialog = NULL;
	GList *section_item = NULL;
	gchar filename[MAX_PATH_LEN];
	book_data *book = NULL;
	section_data *section = NULL;
	entry_data *entry = NULL;
	gint result;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	// Get trash section
	section = book->trash_section;

	// Confirm clean trash action
	msg_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_MODAL,
		GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
		"Are you sure you want to clean trash and delete and all of its entries?");
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msg_dialog),
		"If you clean the trash, its entries are permanently lost.");
	gtk_window_set_title(GTK_WINDOW(msg_dialog), app_name);
	gtk_window_set_type_hint(GTK_WINDOW(msg_dialog), GDK_WINDOW_TYPE_HINT_MENU);
	gtk_window_set_resizable(GTK_WINDOW(msg_dialog), FALSE);

	result = gtk_dialog_run(GTK_DIALOG(msg_dialog));
	switch (result) {
	case GTK_RESPONSE_YES:

		// Delete entries
		for(section_item = section->entry_list;
		section_item != NULL; section_item = section_item->next) {

			entry = section_item->data;

			// Delete entry text file
			g_snprintf(filename, sizeof(filename),
				"%s%s%s%s%s%s%s.txt",
				note_dir, G_DIR_SEPARATOR_S,
				book->name, G_DIR_SEPARATOR_S,
				section->name, G_DIR_SEPARATOR_S,
				entry->name);

			sn_trace("Deleting entry [%s] in %s.",
				filename, __func__);

			result = remove(filename);

			if(result != 0) {
				sn_warning("Unable to delete entry [%s].", entry->name);
				sn_warning("Unable to delete section [%s].", section->name);
				gtk_widget_destroy(msg_dialog);
				return FALSE;
			}
		}

		// Free entries
		g_list_free_full(section->entry_list, free_entry);
		section->entry_list = NULL;

		// Write book
		write_book(book, note_dir);

		// Update view
		populate_sections(book);
		
		gtk_widget_grab_focus(GTK_WIDGET(get_section_view(book)));
		gtk_widget_destroy(msg_dialog);
		return TRUE;

	default:
		gtk_widget_destroy(msg_dialog);
		return FALSE;
	}
	return FALSE;
} // Clean trash

/*
 * Delete section
 * Signal handler for "activate" delete section
 */
gboolean delete_section()
{
	GtkWidget *msg_dialog = NULL;
	GList *section_item = NULL;
	gchar filename[MAX_PATH_LEN];
	gchar section_display_name[MAX_NAME_LEN];
	book_data *book = NULL;
	section_data *section = NULL;
	entry_data *entry = NULL;
	gint result;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();
	section = get_current_section_or_return_with_warning();

	// Check for trash section
	if(book->trash_section == section) {

		msg_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_MODAL,
			GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
			"Unable to delete trash section.");
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msg_dialog),
			"Trash is a special section used to soft delete entries.");
		gtk_window_set_title(GTK_WINDOW(msg_dialog), app_name);
		gtk_window_set_type_hint(GTK_WINDOW(msg_dialog), GDK_WINDOW_TYPE_HINT_MENU);
		gtk_window_set_resizable(GTK_WINDOW(msg_dialog), FALSE);
		result = gtk_dialog_run(GTK_DIALOG(msg_dialog));

		gtk_widget_destroy(msg_dialog);
		return FALSE;
	}

	// Set display name
	strncpy(section_display_name, section->name, MAX_NAME_LEN-5);
	if(strlen(section_display_name) > 25)
		strcpy(section_display_name+25, "...\0");

	// Confirm delete action
	msg_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_MODAL,
		GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
		"Are you sure you want to permanenty delete\n\"%s\" and all of its entries?",
		section_display_name);
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msg_dialog),
		"If you delete a section, it is permanently lost.");
	gtk_window_set_title(GTK_WINDOW(msg_dialog), app_name);
	gtk_window_set_type_hint(GTK_WINDOW(msg_dialog), GDK_WINDOW_TYPE_HINT_MENU);
	gtk_window_set_resizable(GTK_WINDOW(msg_dialog), FALSE);

	result = gtk_dialog_run(GTK_DIALOG(msg_dialog));
	switch (result) {
	case GTK_RESPONSE_YES:

		// Delete entries
		for(section_item = section->entry_list;
		section_item != NULL; section_item = section_item->next) {

			entry = section_item->data;

			// Delete entry text file
			g_snprintf(filename, sizeof(filename),
				"%s%s%s%s%s%s%s.txt",
				note_dir, G_DIR_SEPARATOR_S,
				book->name, G_DIR_SEPARATOR_S,
				section->name, G_DIR_SEPARATOR_S,
				entry->name);

			sn_trace("Deleting entry [%s] in %s.",
				filename, __func__);

			result = remove(filename);

			if(result != 0) {
				sn_warning("Unable to delete entry [%s].", entry->name);
				sn_warning("Unable to delete section [%s].", section->name);
				gtk_widget_destroy(msg_dialog);
				return FALSE;
			}
		}

		// Free entries
		g_list_free_full(section->entry_list, free_entry);
		section->entry_list = NULL;

		// Delete section directory
		g_snprintf(filename, sizeof(filename), "%s%s%s%s%s",
			note_dir, G_DIR_SEPARATOR_S,
			book->name, G_DIR_SEPARATOR_S,
			section->name);

		sn_trace("Deleting section [%s] in %s.",
			filename, __func__);

		result = rmdir(filename);

		if (result == 0) {

			// Remove section history
			remove_section_history();

			// Select next section
			section_item = g_list_find(book->section_list, section);
			section_item = section_item->next;
			if(section_item != NULL)
				book->curr_section = section_item->data;
			else
				book->curr_section = NULL;

			// Remove section
			book->section_list = g_list_remove(book->section_list, section);
			free_section(section);

			// Write book
			write_book(book, note_dir);

			// Update view
			populate_sections(book);
			gtk_widget_grab_focus(GTK_WIDGET(get_section_view(book)));
			gtk_widget_destroy(msg_dialog);
			return TRUE;

		} else {
			sn_warning("Unable to delete section [%s].", section->name);
			gtk_widget_destroy(msg_dialog);
			return FALSE;
		}

	default:
		gtk_widget_destroy(msg_dialog);
		return FALSE;
	}
	return FALSE;
} // Delete section

/*
 * Shift section up
 */
gboolean shift_section_up()
{
	GtkTreePath *section_path = NULL;
	GtkTreeView *section_view = NULL;
	GtkTreeModel *section_model = NULL;
	GtkTreeSelection *section_selection = NULL;
	GtkTreeViewColumn *section_column = NULL;
	GtkTreeIter section_iter;
	book_data *book = NULL;
	section_data *section = NULL;
	guint section_index = 0;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	// Get section model and view
	section_view = get_section_view(book);
	section_model = gtk_tree_view_get_model(section_view);

	sn_trace0("Shifting section up.");

	// Make sure we are not at the top
	gtk_tree_view_get_cursor
		(section_view, &section_path, &section_column);
	if(section_path != NULL) {
		if(gtk_tree_path_prev(section_path)) {

			// Get the section to move up
			section_selection = gtk_tree_view_get_selection(section_view);
			if(gtk_tree_selection_get_selected
				(section_selection, &section_model, &section_iter)) {
				gtk_tree_model_get(section_model, &section_iter,
					SECTION_ITEM, &section, END_OF_LIST);

				// Move section up
				if(section != NULL) {
					section_index = g_list_index(book->section_list, section);
					book->section_list = g_list_remove(book->section_list, section);
					book->section_list = g_list_insert
						(book->section_list, section, --section_index);

					// Write book
					write_book(book, note_dir);
				}
			}
			// Populate sections
			populate_sections(book);
			gtk_tree_view_set_cursor
				(section_view, section_path, section_column, FALSE);
		}
	gtk_tree_path_free(section_path);
	}

	return TRUE;
} // Shift section up

/*
 * Shift section down
 */
gboolean shift_section_down()
{
	GtkTreePath *section_path = NULL;
	GtkTreeView *section_view = NULL;
	GtkTreeModel *section_model = NULL;
	GtkTreeSelection *section_selection = NULL;
	GtkTreeViewColumn *section_column = NULL;
	GtkTreeIter section_iter;
	book_data *book = NULL;
	section_data *section = NULL;
	guint section_index = 0;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	// Get section model and view
	section_view = get_section_view(book);
	section_model = gtk_tree_view_get_model(section_view);

	sn_trace0("Shifting section down.");

	// Make sure we are not at the bottom
	gtk_tree_view_get_cursor
		(section_view, &section_path, &section_column);
	if(section_path != NULL) {
		gtk_tree_path_next(section_path);
		if(gtk_tree_model_get_iter(section_model, &section_iter, section_path)) {

			// Get the section to move down
			section_selection = gtk_tree_view_get_selection(section_view);
			if(gtk_tree_selection_get_selected
				(section_selection, &section_model, &section_iter)) {
				gtk_tree_model_get(section_model, &section_iter,
					SECTION_ITEM, &section, END_OF_LIST);

				// Move section down
				if(section != NULL) {
					section_index = g_list_index(book->section_list, section);
					book->section_list = g_list_remove(book->section_list, section);
					book->section_list = g_list_insert
						(book->section_list, section, ++section_index);

					// Write book
					write_book(book, note_dir);
				}
			}
			// Populate sections
			populate_sections(book);
			gtk_tree_view_set_cursor
				(section_view, section_path, section_column, FALSE);
		}
	gtk_tree_path_free(section_path);
	}

	return TRUE;
} // Shift section down

/*
 * Free section
 */
void free_section(gpointer section)
{
	GList *entry_list = NULL;

	if(section == NULL) {
		sn_warning0("Freeing section [(null)].");
		return;
	}
	// Free entries
	entry_list = ((section_data*)section)->entry_list;
	g_list_free_full(entry_list, free_entry);

	sn_trace("Freeing section [%#lx:%s].",
		(ulong)section, ((section_data*)section)->name);

	// Free section
	g_free(section);

	return;
} // Free section
