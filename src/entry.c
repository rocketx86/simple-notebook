/*
 * Simple Notebook
 * entry.c
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

static const gchar *default_entry_name = "Entry";

static GtkTreeModel* create_entry_model(section_data *section);
static GtkTreeModel* create_book_section_model(master_data *master);
static gboolean init_book_section_view(GtkTreeView *section_view);
static void get_entry_name (GtkTreeViewColumn *tree_column, GtkCellRenderer *cell_rend,
					GtkTreeModel *model, GtkTreeIter *tree_iter, gpointer data);
static void get_book_section_name(GtkTreeViewColumn *tree_column,
					GtkCellRenderer *cell_rend, GtkTreeModel *tree_model,
					GtkTreeIter *tree_iter, gpointer data);
static gboolean on_entry_selection(GtkTreeSelection *selection, GtkTreeModel *entry_model,
					GtkTreePath *entry_path, gboolean path_currently_selected,
					gpointer user_data);
static GtkWidget* create_book_section_dialog(GtkWidget *section_view,
					const gchar *dialog_label);

/*
 * On entry selection
 */
static gboolean on_entry_selection(GtkTreeSelection *selection, GtkTreeModel *entry_model,
						GtkTreePath *entry_path, gboolean path_currently_selected,
						gpointer user_data)
{
	GtkTreeIter tree_iter;
	entry_data *entry = NULL;

	if(gtk_tree_model_get_iter(entry_model, &tree_iter, entry_path)) {
		gtk_tree_model_get(entry_model, &tree_iter, ENTRY_ITEM, &entry, END_OF_LIST);

		section_data *section = entry->parent_section;

		if(!path_currently_selected) {
			sn_trace(
			"? Entry [%s/%s] will be selected in %s",
			section->name, entry->name, __func__);
		} else {
			sn_trace(
			"? Entry [%s/%s] will be de-selected %s.",
			section->name, entry->name, __func__);
		}
	}
	return TRUE;
} // On entry selection

/*
 * On entry key press
 * Signal handler for "key-press-event"
 */
gboolean on_entry_key_press(GtkTreeView *entry_view,
					GdkEventKey *event, gpointer data)
{
	GtkTreePath *entry_path = NULL;
	GtkTreeModel *entry_model = NULL;
	GtkTreeViewColumn *entry_column = NULL;
	GtkTreeIter tree_iter;
	book_data *book = NULL;
	view_data *view = NULL;

	GdkModifierType modifiers;
	modifiers = gtk_accelerator_get_default_mod_mask();

	// Get currently selected
	book = (book_data*)master->curr_book;
	g_return_val_if_fail(book != NULL, FALSE);

	view = (view_data*)book->view;
	g_return_val_if_fail(view != NULL, FALSE);

	entry_model = gtk_tree_view_get_model(entry_view);

	switch (event->keyval) {
	case GDK_KEY_Up:
		if((event->state & modifiers) & GDK_SHIFT_MASK) {
			sn_trace("Shift+Up in %s", __func__);

			shift_entry_up();

		} else {
			sn_trace("Up in %s", __func__);

			gtk_tree_view_get_cursor
				(entry_view, &entry_path, &entry_column);

			if(entry_path != NULL) {
				if(gtk_tree_path_prev(entry_path)) {
					gtk_tree_view_set_cursor
						(entry_view, entry_path, entry_column, FALSE);
				}
			gtk_tree_path_free(entry_path);
			}
		}
		return TRUE;

	case GDK_KEY_Down:
		if((event->state & modifiers) & GDK_SHIFT_MASK) {
			sn_trace("Shift+Down in %s", __func__);

			shift_entry_down();

		} else {
			sn_trace("Down in %s", __func__);

			gtk_tree_view_get_cursor
				(entry_view, &entry_path, &entry_column);

			if(entry_path != NULL) {
				gtk_tree_path_next(entry_path);
				if(gtk_tree_model_get_iter(entry_model, &tree_iter, entry_path)) {
					gtk_tree_view_set_cursor
						(entry_view, entry_path, entry_column, FALSE);
				}
			gtk_tree_path_free(entry_path);
			}
		}
		return TRUE;

	case GDK_KEY_Left:
		if((event->state & modifiers) & GDK_CONTROL_MASK) {
			sn_trace("Ctrl+Left in %s", __func__);

			gtk_widget_grab_focus(GTK_WIDGET(main_view));
			on_book_key_press(GTK_NOTEBOOK(main_view), event, data);

		} else {
			sn_trace("Left in %s", __func__);

			gtk_widget_grab_focus(GTK_WIDGET(view->section_view));
			on_section_key_press(view->section_view, event, data);
		}
		return TRUE;

	case GDK_KEY_Right:
		if((event->state & modifiers) & GDK_CONTROL_MASK) {
			sn_trace("Ctrl+Right in %s", __func__);

			gtk_widget_grab_focus(GTK_WIDGET(main_view));
			on_book_key_press(GTK_NOTEBOOK(main_view), event, data);

		} else {
			sn_trace("Right in %s", __func__);

			gtk_widget_grab_focus(GTK_WIDGET(view->section_view));
			on_section_key_press(view->section_view, event, data);
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
			set_text_view_focus(view->text_view);
		}
		return TRUE;
	}
	return FALSE;
} // On entry keypress

/*
 * On entry change
 * Signal handler for view "cursor-changed" event
 * Called from create_entry in entry.c
 * Called from rename_entry in entry.c
 */
void on_entry_change(GtkTreeView *entry_view, gpointer data)
{
	book_data *book = NULL;
	section_data *section = NULL;
	entry_data *entry = NULL;
	GtkTreeSelection *selection = NULL;
	GtkTreeModel *entry_model = NULL;
	GtkTreeIter tree_iter;

	// Get currently selected
	book = (book_data*)data;

	// Write text of previous entry
	write_current_entry();

	// Get selected entry
	selection = gtk_tree_view_get_selection(entry_view);
	if(gtk_tree_selection_get_selected(selection, &entry_model, &tree_iter)) {
		gtk_tree_model_get(entry_model, &tree_iter, ENTRY_ITEM, &entry, END_OF_LIST);
		if(entry != NULL) {

			// Get entrys parent section
			section = entry->parent_section;

			// Set current entry in section
			sn_trace("* Selecting entry [%s/%s] in %s.",
				section->name, entry->name, __func__);

			if(section->curr_entry_path != NULL)
				gtk_tree_path_free(section->curr_entry_path);
			section->curr_entry_path = gtk_tree_model_get_path(entry_model, &tree_iter);
			section->curr_entry = entry;

			// Read text of current entry
			sn_trace("> Reading current entry [%s/%s] in %s.",
				section->name, entry->name, __func__);

			read_text(book, entry);

			// Add history
			if(book->add_history) {
				add_history();
			}
			book->add_history = FALSE;
		}
	}
	gtk_widget_grab_focus(GTK_WIDGET(entry_view));
	return;
} // On entry change

/*
 * Populate entries
 * Called from populate_sections in section.c
 * Called from on_section_change in section.c
 * Called from create_entry in entry.c
 */
gboolean populate_entries(book_data *book, section_data *section)
{
	GtkTreeModel *entry_model = NULL;
	GtkTreeView *entry_view = NULL;
	GtkTreeSelection *selection = NULL;
	GtkTreeIter tree_iter;
	entry_data *entry = NULL;
	view_data* view = NULL;

	// Get entry view
	view = book->view;
	entry_view = view->entry_view;

	// Create new entry model
	entry_model = create_entry_model(section);

	// Set entry model on entry view
	gtk_tree_view_set_model(entry_view, entry_model);

	// Does the section have entries?
	if(section->entry_list != NULL) {

		// Get current entry in section
		if(section->curr_entry != NULL) {
			entry = section->curr_entry;
			gtk_tree_model_get_iter(entry_model, &tree_iter, section->curr_entry_path);
		} else {
			entry = section->entry_list->data;
			gtk_tree_model_get_iter_first (entry_model, &tree_iter);
		}

		// Set current entry in section
		selection = gtk_tree_view_get_selection(entry_view);
		gtk_tree_selection_select_iter(selection, &tree_iter);
		if(section->curr_entry_path != NULL)
			gtk_tree_path_free(section->curr_entry_path);
		section->curr_entry_path = gtk_tree_model_get_path(entry_model, &tree_iter);
		section->curr_entry = entry;

		sn_trace("* Selectiong entry [%s/%s] in %s.",
			section->name, entry->name, __func__);

	} else {

		// Set current entry in section
		if(section->curr_entry_path != NULL)
			gtk_tree_path_free(section->curr_entry_path);
		section->curr_entry_path = NULL;
		section->curr_entry = NULL;

		sn_trace("* Selecting entry [(null)] in %s.", __func__);
	}

	// Read text of current entry
	sn_trace("> Reading current entry [%s/%s] in %s.",
		section->name, entry->name, __func__);

	read_text(book, entry);

	if (entry_model != NULL)
		g_object_unref(entry_model);

	return TRUE;
} // Populate entries

/*
 * Get entry name
 * Sets the text of cell to entry name
 */
static void get_entry_name (GtkTreeViewColumn *tree_column,
					GtkCellRenderer *cell_rend, GtkTreeModel *tree_model,
					GtkTreeIter *tree_iter, gpointer data)
{
	entry_data *entry = NULL;

	gtk_tree_model_get(tree_model, tree_iter, ENTRY_ITEM, &entry, END_OF_LIST);
	g_object_set(cell_rend, "text", entry->name, NULL);
	return;
} // Get entry name

/*
 * Get book section name
 * Sets the text of cell to book section
 */
static void get_book_section_name(GtkTreeViewColumn *tree_column,
					GtkCellRenderer *cell_rend, GtkTreeModel *tree_model,
					GtkTreeIter *tree_iter, gpointer data)
{
	gchar section_name[MAX_PATH_LEN];
	book_data *book = NULL;
	section_data *section = NULL;

	gtk_tree_model_get(tree_model, tree_iter, SECTION_ITEM, &section, END_OF_LIST);
	book = section->parent_book;
	g_snprintf(section_name, MAX_PATH_LEN-1, "%s/%s",
		book->name, section->name);
	g_object_set(cell_rend, "text", section_name, NULL);
	return;
} // Get section name

/*
 * Create entry model
 * Called from populate_entries in entry.c
 */
static GtkTreeModel* create_entry_model(section_data *section)
{
	GtkListStore *list_store = NULL;
	GtkTreeIter tree_iter;
	GList *entry_item = NULL;
	entry_data *entry = NULL;

	if (section->entry_list == NULL) {
		return NULL;
	}
	// Create list store
	list_store = gtk_list_store_new(N_ENTRY_COLUMNS, G_TYPE_POINTER);

	// Populate list store from entry list
	for(entry_item = section->entry_list;
	entry_item != NULL; entry_item = entry_item->next) {
		entry = entry_item->data;
		gtk_list_store_append(list_store, &tree_iter);
		gtk_list_store_set(list_store, &tree_iter,
			ENTRY_ITEM, entry, END_OF_LIST);
	}
	return GTK_TREE_MODEL(list_store);
} // Create entry model

/*
 * Create book section model
 * Called from copy_entry in entry.c
 * Called from move_entry in entry.c
 */
static GtkTreeModel* create_book_section_model(master_data *master)
{
	GtkListStore *list_store = NULL;
	GtkTreeIter tree_iter;
	GList *book_item = NULL;
	GList *section_item = NULL;
	book_data *book = NULL;
	section_data *section = NULL;

	// Create list store
	list_store = gtk_list_store_new(N_SECTION_COLUMNS, G_TYPE_POINTER);

	// Populate list store from book and section lists
	for(book_item = master->book_list;
	book_item != NULL; book_item = book_item->next) {
		book = book_item->data;
		for(section_item = book->section_list;
		section_item != NULL; section_item = section_item->next) {
			section = section_item->data;
			gtk_list_store_append(list_store, &tree_iter);
			gtk_list_store_set(list_store, &tree_iter,
				SECTION_ITEM, section, END_OF_LIST);
		}
	}
	return GTK_TREE_MODEL(list_store);
} // Create book section model

/*
 * Init entry view
 * Called from create_book_page in page.c
 */
gboolean init_entry_view(GtkTreeView *entry_view, book_data *book)
{
	GtkTreeViewColumn *view_col = NULL;
	GtkCellRenderer *cell_rend = NULL;
	GtkTreeSelection *selection = NULL;

	// Create entry column
	view_col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(view_col, "Name");
	gtk_tree_view_set_headers_visible(entry_view, FALSE);
	gtk_tree_view_append_column(entry_view, view_col);

	// Create entry cell_rend with get_entry_name callback
	cell_rend = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(view_col, cell_rend, TRUE);
	gtk_tree_view_column_set_cell_data_func
		(view_col, cell_rend, get_entry_name, NULL, NULL);

	// Set selection function
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(entry_view));
	gtk_tree_selection_set_select_function
		(selection, on_entry_selection, NULL, NULL);

	// Set signal handlers
	g_signal_connect(entry_view, "cursor-changed",
		G_CALLBACK(on_entry_change), book);

	g_signal_connect(entry_view, "key-press-event",
		G_CALLBACK(on_entry_key_press), book);

	return TRUE;
} // Init entry view

/*
 * Init book section view
 * Called from copy_entry in entry.c
 * Called from move_entry in entry.c
 */
static gboolean init_book_section_view(GtkTreeView *section_view)
{
	GtkTreeViewColumn *view_col = NULL;
	GtkCellRenderer *cell_rend = NULL;

	// Create section column
	view_col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(view_col, "Section");
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(section_view), FALSE);
	gtk_tree_view_append_column(GTK_TREE_VIEW(section_view), view_col);

	// Create section cell_rend with get_book_section_name callback
	cell_rend = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(view_col, cell_rend, TRUE);
	gtk_tree_view_column_set_cell_data_func
		(view_col, cell_rend, get_book_section_name, NULL, NULL);

	return TRUE;
} // Init book section view

/*
 * Create book section dialog
 * Called from copy_entry in entry.c
 * Called from move_entry in entry.c
 */
static GtkWidget* create_book_section_dialog(GtkWidget *section_view,
					const gchar *dialog_label)
{
	GtkWidget *section_dialog = NULL;
	GtkWidget *section_scroll = NULL;
	GtkWidget *hbox = NULL;
	GtkWidget *vbox = NULL;
	GtkWidget *label = NULL;

	section_dialog = gtk_dialog_new_with_buttons(app_name, GTK_WINDOW(main_window),
			GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	gtk_window_set_type_hint(GTK_WINDOW(section_dialog), GDK_WINDOW_TYPE_HINT_MENU);
	gtk_window_set_resizable(GTK_WINDOW(section_dialog), FALSE);

	gtk_dialog_set_default_response(GTK_DIALOG(section_dialog),
		GTK_RESPONSE_ACCEPT);

	// Set label
	label = gtk_label_new(dialog_label);
	hbox = gtk_hbox_new(FALSE, 0);
	vbox = gtk_dialog_get_content_area(GTK_DIALOG(section_dialog));
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 5);

	// Create scroll view
	section_scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(section_scroll, MAIN_WIDTH / 2, MAIN_HEIGHT / 3);
	gtk_scrolled_window_set_shadow_type
		(GTK_SCROLLED_WINDOW(section_scroll), GTK_SHADOW_IN);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(section_scroll),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), section_scroll, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 5);
	gtk_container_add(GTK_CONTAINER(section_scroll), section_view);

	// Set mouse button press signal handler
	g_signal_connect(section_view, "button-press-event",
		G_CALLBACK(on_section_mouse_button_press), section_dialog);

	return section_dialog;
} // Create book section dialog

/*
 * Create entry
 * Signal handler for "activate" create entry
 */
gboolean create_entry()
{
	FILE *fp = NULL;
	gchar entry_name[MAX_NAME_LEN];
	gchar filename[MAX_PATH_LEN];
	GtkTreeModel *entry_model = NULL;
	GtkTreeView *entry_view = NULL;
	GtkTreeSelection *selection = NULL;
	GtkTreeIter tree_iter;
	GList *entry_item = NULL;
	GtkTreePath *tree_path = NULL;
	entry_data *curr_entry = NULL;
	book_data *book = NULL;
	section_data *section = NULL;
	entry_data *entry = NULL;
	view_data *view = NULL;
	gint cindex = 0;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();
	section = get_current_section_or_return_with_warning();

	while (++cindex < MAX_TRIES) {
		// Create entry name
		g_snprintf(entry_name, sizeof(entry_name), "%s%d",
			default_entry_name, cindex);

		// Create entry text file
		g_snprintf(filename, sizeof(filename),
			"%s%s%s%s%s%s%s.txt",
			note_dir, G_DIR_SEPARATOR_S,
			book->name, G_DIR_SEPARATOR_S,
			section->name, G_DIR_SEPARATOR_S,
			entry_name);

		fp = fopen(filename, "wx");
		if (fp == NULL) {
			continue;
		}
		fclose(fp);
		break;
	}

	sn_trace("Creating entry text file [%s].", filename);

	// Create entry
	entry = g_new0(entry_data, NEW_INSTANCE);
	strcpy(entry->name, entry_name);
	entry->parent_section = section;

	// Get selected entry
	view = book->view;
	entry_view = view->entry_view;
	entry_model = gtk_tree_view_get_model(entry_view);
	selection = gtk_tree_view_get_selection(entry_view);

	// Update model
	if(gtk_tree_selection_get_selected(selection, &entry_model, &tree_iter)) {
		gtk_tree_model_get(entry_model, &tree_iter, ENTRY_ITEM, &curr_entry, END_OF_LIST);
		tree_path = gtk_tree_model_get_path(entry_model, &tree_iter);
		entry_item = g_list_find(section->entry_list, curr_entry);
		section->entry_list = g_list_insert_before(section->entry_list, entry_item, entry);
	} else {
		section->entry_list = g_list_append(section->entry_list, entry);
	}

	// Write book
	write_book(book, note_dir);

	// Update view
	populate_entries(book, section);
	if(tree_path != NULL)
		gtk_tree_selection_select_path(selection, tree_path);
	on_entry_change(entry_view, book);

	// Set text focus
	set_text_view_focus(view->text_view);

	// Name entry
	if(options.auto_name_entry == TRUE) {
		entry->need_rename = TRUE;
	} else {
		rename_entry();
	}

	return TRUE;
} // Create entry

/*
 * Rename entry
 * Signal handler for "activate" rename entry
 */
gboolean rename_entry()
{
	FILE *fp = NULL;
	GtkWidget *name_dialog = NULL;
	GtkWidget *msg_dialog = NULL;
	GtkWidget *vbox = NULL;
	GtkWidget *hbox = NULL;
	GtkWidget *label = NULL;
	GtkWidget *name_entry = NULL;
	const gchar *entry_text = NULL;
	gchar old_entry_display_name[MAX_NAME_LEN];
	gchar new_entry_display_name[MAX_NAME_LEN];
	gchar new_entry_name[MAX_PATH_LEN];
	gchar old_entry_name[MAX_PATH_LEN];
	book_data *book = NULL;
	section_data *section = NULL;
	entry_data *entry = NULL;
	view_data *view = NULL;
	gchar *temp_string;
	gint result;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();
	section = get_current_section_or_return_with_warning();
	entry = get_current_entry_or_return_with_warning();

	// Get view data
	view = book->view;

	// Write current entry
	write_current_entry();

	// Create name dialog
	name_dialog = gtk_dialog_new_with_buttons(app_name, GTK_WINDOW(main_window),
			GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	gtk_widget_set_size_request(name_dialog, SM_DLG_WIDTH, SM_DLG_HEIGHT);
	gtk_window_set_type_hint(GTK_WINDOW(name_dialog), GDK_WINDOW_TYPE_HINT_MENU);
	gtk_window_set_resizable(GTK_WINDOW(name_dialog), FALSE);

	label = gtk_label_new("Enter new entry name...");
	name_entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(name_entry), MAX_NAME_LEN-5);

	// Get auto name
	if(options.auto_name_entry == TRUE) {

		temp_string = g_malloc0(MAX_NAME_LEN);
		strdelchr(temp_string, get_auto_entry_name(view->text_view), ILLEGAL_CHARS);
		temp_string[MAX_NAME_LEN-5] = 0;
		if(strcmp(temp_string, "") == 0)
			gtk_entry_set_text(GTK_ENTRY(name_entry), entry->name);
		else
			gtk_entry_set_text(GTK_ENTRY(name_entry), temp_string);
		g_free(temp_string);
	} else {

		gtk_entry_set_text(GTK_ENTRY(name_entry), entry->name);
	}

	vbox = gtk_dialog_get_content_area(GTK_DIALOG(name_dialog));

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 5);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), name_entry, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 5);

	gtk_entry_set_activates_default(GTK_ENTRY(name_entry), TRUE);
	gtk_dialog_set_default_response (GTK_DIALOG(name_dialog),GTK_RESPONSE_ACCEPT);
	gtk_widget_show_all(name_dialog);

	// Run dialog
	result = gtk_dialog_run(GTK_DIALOG(name_dialog));

	switch (result) {
	case GTK_RESPONSE_ACCEPT:

		entry_text = gtk_entry_get_text(GTK_ENTRY(name_entry));

		// Scan name for illegal characters
		temp_string = g_malloc0(MAX_NAME_LEN);
		strdelchr(temp_string, entry_text, ILLEGAL_CHARS);
		gtk_entry_set_text(GTK_ENTRY(name_entry), temp_string);
		g_free(temp_string);
		entry_text = gtk_entry_get_text(GTK_ENTRY(name_entry));

		// Old entry file
		g_snprintf(old_entry_name, sizeof(old_entry_name),
			"%s%s%s%s%s%s%s.txt",
			note_dir, G_DIR_SEPARATOR_S,
			book->name, G_DIR_SEPARATOR_S,
			section->name, G_DIR_SEPARATOR_S,
			entry->name);

		// New entry file
		g_snprintf(new_entry_name, sizeof(new_entry_name),
			"%s%s%s%s%s%s%s.txt",
			note_dir, G_DIR_SEPARATOR_S,
			book->name, G_DIR_SEPARATOR_S,
			section->name, G_DIR_SEPARATOR_S,
			entry_text);

		// Ignore rename if names match
		if(strcmp(entry->name, entry_text) == 0) {
			gtk_widget_destroy(name_dialog);
			return TRUE;
		}

		// Set display name
		strncpy(old_entry_display_name, entry->name, MAX_NAME_LEN-5);
		if(strlen(old_entry_display_name) > 25)
			strcpy(old_entry_display_name+25, "...\0");

		strncpy(new_entry_display_name, entry_text, MAX_NAME_LEN-5);
		if(strlen(new_entry_display_name) > 25)
			strcpy(new_entry_display_name+25, "...\0");

		// Check that new entry name is valid
		if(strlen(entry_text) < 1) {

			sn_warning("Unable to rename entry [%s] to [%s].",
				entry->name, entry_text);

			msg_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window),
				GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
				"Unable to rename entry \"%s\" to \"%s\".",
				old_entry_display_name, new_entry_display_name);
			gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msg_dialog),
				"Please ensure the entry name contains a valid character.");
			gtk_window_set_title(GTK_WINDOW(msg_dialog), app_name);
			gtk_window_set_type_hint(GTK_WINDOW(msg_dialog), GDK_WINDOW_TYPE_HINT_MENU);
			gtk_window_set_resizable(GTK_WINDOW(msg_dialog), FALSE);
			result = gtk_dialog_run(GTK_DIALOG(msg_dialog));

			gtk_widget_destroy(name_dialog);
			gtk_widget_destroy(msg_dialog);
			return FALSE;
		}

		// Check that new entry name is valid
		fp = fopen(new_entry_name, "wx");
		if (fp == NULL) {
			sn_warning("Unable to rename entry [%s] to [%s].",
				entry->name, entry_text);

			msg_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_MODAL,
				GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
				"Unable to rename entry \"%s\" to \"%s\".",
				old_entry_display_name, new_entry_display_name);
			gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msg_dialog),
				"Please ensure the entry name doesn't aleady exist.");
			gtk_window_set_title(GTK_WINDOW(msg_dialog), app_name);
			gtk_window_set_type_hint(GTK_WINDOW(msg_dialog), GDK_WINDOW_TYPE_HINT_MENU);
			gtk_window_set_resizable(GTK_WINDOW(msg_dialog), FALSE);
			result = gtk_dialog_run(GTK_DIALOG(msg_dialog));

			gtk_widget_destroy(name_dialog);
			gtk_widget_destroy(msg_dialog);
			return FALSE;

		}
		fclose(fp);

		sn_trace("Renaming entry [%s] to [%s].", entry->name, entry_text);

		// Rename entry file
		result = rename(old_entry_name, new_entry_name);

		if(result == 0) {

			// Remove history
			remove_history();

			// Update entry
			strcpy(entry->name, entry_text);

			// Update book
			write_book(book, note_dir);

			// Update view
			populate_entries(book, section);
			on_entry_change(view->entry_view, book);

			gtk_widget_destroy(name_dialog);
			return TRUE;
		} else {
			sn_warning("Unable to rename entry [%s].", entry->name);
			gtk_widget_destroy(name_dialog);
			return FALSE;
		}

	default:
		gtk_widget_destroy(name_dialog);
		return FALSE;

	} // End switch

	return FALSE;
} // Rename entry

/*
 * Copy entry
 * Signal handler for "activate" copy entry
 */
gboolean copy_entry()
{
	FILE *fp = NULL;
	GtkWidget *msg_dialog = NULL;
	GtkWidget *section_dialog = NULL;
	GtkWidget *section_view = NULL;
	GtkTreeModel *section_model = NULL;
	GtkTreeSelection *section_selection = NULL;
	GtkTreeIter section_iter;
	gchar entry_display_name[MAX_NAME_LEN];
	gchar curr_entry_path[MAX_PATH_LEN];
	gchar copy_entry_path[MAX_PATH_LEN];
	book_data *book = NULL;
	section_data *section = NULL;
	entry_data *entry = NULL;
	book_data *book_copy = NULL;
	section_data *section_copy = NULL;
	entry_data *entry_copy = NULL;
	gint result;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();
	section = get_current_section_or_return_with_warning();
	entry = get_current_entry_or_return_with_warning();

	// Create section view
	section_view = gtk_tree_view_new();

	// Create section dialog
	section_dialog = create_book_section_dialog(section_view,
		"Select the section to copy the entry to...");

	// Init section view
	init_book_section_view(GTK_TREE_VIEW(section_view));

	// Create section entry model / store
	section_model = create_book_section_model(master);

	// Set section model on section view
	gtk_tree_view_set_model(GTK_TREE_VIEW(section_view),
		GTK_TREE_MODEL(section_model));
	if(section_model != NULL)
		g_object_unref(section_model);

	// Show and run view section dialog
	gtk_widget_show_all(section_dialog);
	result = gtk_dialog_run(GTK_DIALOG(section_dialog));

	switch (result) {
	case GTK_RESPONSE_ACCEPT:

		section_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(section_view));
		if(gtk_tree_selection_get_selected(section_selection,
				&section_model, &section_iter)) {

			gtk_tree_model_get(section_model, &section_iter, SECTION_ITEM,
				&section_copy, END_OF_LIST);

			if(section == NULL) {
				gtk_widget_destroy(section_dialog);
				return FALSE;
			}
		} else {
			gtk_widget_destroy(section_dialog);
			return FALSE;
		}

		book_copy = section_copy->parent_book;

		// Curr entry file path
		g_snprintf(curr_entry_path, sizeof(curr_entry_path),
			"%s%s%s%s%s%s%s.txt",
			note_dir, G_DIR_SEPARATOR_S,
			book->name, G_DIR_SEPARATOR_S,
			section->name, G_DIR_SEPARATOR_S,
			entry->name);

		// Copy entry file path
		g_snprintf(copy_entry_path, sizeof(copy_entry_path),
			"%s%s%s%s%s%s%s.txt",
			note_dir, G_DIR_SEPARATOR_S,
			book_copy->name, G_DIR_SEPARATOR_S,
			section_copy->name, G_DIR_SEPARATOR_S,
			entry->name);

		// Set display name
		strncpy(entry_display_name, entry->name, MAX_NAME_LEN-5);
		if(strlen(entry_display_name) > 25)
			strcpy(entry_display_name+25, "...\0");

		// Check that new entry path is valid
		fp = fopen(copy_entry_path, "wx");

		if (fp == NULL) {
			sn_warning("Unable to copy entry [%s].", copy_entry_path);

			msg_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_MODAL,
				GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
				"Unable to copy entry \"%s\".", entry_display_name);
			gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msg_dialog),
				"Please ensure the entry name doesn't aleady exist.");
			gtk_window_set_title(GTK_WINDOW(msg_dialog), app_name);
			gtk_window_set_type_hint(GTK_WINDOW(msg_dialog), GDK_WINDOW_TYPE_HINT_MENU);
			gtk_window_set_resizable(GTK_WINDOW(msg_dialog), FALSE);
			result = gtk_dialog_run(GTK_DIALOG(msg_dialog));

			gtk_widget_destroy(msg_dialog);
			gtk_widget_destroy(section_dialog);
			return FALSE;
		}
		fclose(fp);

		// Remove file created by previous open
		result = remove(copy_entry_path);

		sn_trace("Copying entry [%s] to [%s].", curr_entry_path, copy_entry_path);

		// Copy the entry file to the new path
		result = copy_file(copy_entry_path, curr_entry_path);

		if(result == 0) {

			// Create entry
			entry_copy = g_new0(entry_data, NEW_INSTANCE);
			strcpy(entry_copy->name, entry->name);
			entry_copy->parent_section = section_copy;

			// Update model
			section_copy->entry_list = g_list_append(section_copy->entry_list, entry_copy);

			// Update book
			write_book(book, note_dir);

			gtk_widget_destroy(section_dialog);
			return TRUE;

		} else {
			gtk_widget_destroy(section_dialog);
			sn_warning("Unable to copy entry [%s].", curr_entry_path);
			return FALSE;
		}
		gtk_widget_destroy(section_dialog);
		return FALSE;

	default:
		gtk_widget_destroy(section_dialog);
		return FALSE;

	} // End switch

	return FALSE;
} // Copy entry

/*
 * Move entry
 * Signal handler for "activate" move entry
 */
gboolean move_entry()
{
	FILE *fp = NULL;
	GtkWidget *msg_dialog = NULL;
	GtkWidget *section_dialog = NULL;
	GtkWidget *section_view = NULL;
	GtkTreeModel *section_model = NULL;
	GtkTreeSelection *section_selection = NULL;
	GtkTreeIter section_iter;
	gchar entry_display_name[MAX_NAME_LEN];
	gchar curr_entry_path[MAX_PATH_LEN];
	gchar move_entry_path[MAX_PATH_LEN];
	GList *entry_item = NULL;
	book_data *book = NULL;
	section_data *section = NULL;
	entry_data *entry = NULL;
	book_data *book_copy = NULL;
	section_data *section_copy = NULL;
	gint result;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();
	section = get_current_section_or_return_with_warning();
	entry = get_current_entry_or_return_with_warning();

	// Create section view
	section_view = gtk_tree_view_new();

	// Create section dialog
	section_dialog = create_book_section_dialog(section_view,
		"Select the section to move the entry to...");

	// Init section view
	init_book_section_view(GTK_TREE_VIEW(section_view));

	// Create section entry model / store
	section_model = create_book_section_model(master);

	// Set section model on section view
	gtk_tree_view_set_model(GTK_TREE_VIEW(section_view),
		GTK_TREE_MODEL(section_model));
	if(section_model != NULL)
		g_object_unref(section_model);

	// Show and run view section dialog
	gtk_widget_show_all(section_dialog);
	result = gtk_dialog_run(GTK_DIALOG(section_dialog));

	switch (result) {
	case GTK_RESPONSE_ACCEPT:

		section_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(section_view));
		if(gtk_tree_selection_get_selected(section_selection,
				&section_model, &section_iter)) {

			gtk_tree_model_get(section_model, &section_iter, SECTION_ITEM,
				&section_copy, END_OF_LIST);

			if(section == NULL) {
				gtk_widget_destroy(section_dialog);
				return FALSE;
			}
		} else {
			gtk_widget_destroy(section_dialog);
			return FALSE;
		}

		book_copy = section_copy->parent_book;

		// Curr entry file path
		g_snprintf(curr_entry_path, sizeof(curr_entry_path),
			"%s%s%s%s%s%s%s.txt",
			note_dir, G_DIR_SEPARATOR_S,
			book->name, G_DIR_SEPARATOR_S,
			section->name, G_DIR_SEPARATOR_S,
			entry->name);

		// Move entry file path
		g_snprintf(move_entry_path, sizeof(move_entry_path),
			"%s%s%s%s%s%s%s.txt",
			note_dir, G_DIR_SEPARATOR_S,
			book_copy->name, G_DIR_SEPARATOR_S,
			section_copy->name, G_DIR_SEPARATOR_S,
			entry->name);

		// Set display name
		strncpy(entry_display_name, entry->name, MAX_NAME_LEN-5);
		if(strlen(entry_display_name) > 25)
			strcpy(entry_display_name+25, "...\0");

		// Check that new entry path is valid
		fp = fopen(move_entry_path, "wx");

		if (fp == NULL) {
			sn_warning("Unable to move entry [%s].", move_entry_path);

			msg_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_MODAL,
				GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
				"Unable to move entry \"%s\".", entry_display_name);
			gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msg_dialog),
				"Please ensure the entry name doesn't aleady exist.");
			gtk_window_set_title(GTK_WINDOW(msg_dialog), app_name);
			gtk_window_set_type_hint(GTK_WINDOW(msg_dialog), GDK_WINDOW_TYPE_HINT_MENU);
			gtk_window_set_resizable(GTK_WINDOW(msg_dialog), FALSE);
			result = gtk_dialog_run(GTK_DIALOG(msg_dialog));

			gtk_widget_destroy(msg_dialog);
			gtk_widget_destroy(section_dialog);
			return FALSE;
		}
		fclose(fp);

		// Remove file created by previous open
		result = remove(move_entry_path);

		sn_trace("Moving entry [%s] to [%s].", curr_entry_path, move_entry_path);

		// Move the entry file to the new path
		result = rename(curr_entry_path, move_entry_path);

		if(result == 0) {

			// Remove history
			remove_history();

			// Select next entry
			entry_item = g_list_find(section->entry_list, entry);
			entry_item = entry_item->next;
			if(entry_item != NULL)
				section->curr_entry = entry_item->data;
			else
				section->curr_entry = NULL;

			// Remove entry
			section->entry_list = g_list_remove(section->entry_list, entry);

			// Append entry
			section_copy->entry_list = g_list_append(section_copy->entry_list, entry);

			// Update entry
			entry->parent_section = section_copy;

			// Write book
			write_book(book, note_dir);

			// Update view
			populate_entries(book, section);
			
			gtk_widget_destroy(section_dialog);
			return TRUE;

		} else {
			gtk_widget_destroy(section_dialog);
			sn_warning("Unable to move entry [%s].", curr_entry_path);
			return FALSE;
		}
		gtk_widget_destroy(section_dialog);
		return FALSE;

	default:
		gtk_widget_destroy(section_dialog);
		return FALSE;

	} // End switch

	return FALSE;
} // Move entry

/*
 * Trash entry
 * Signal handler for "activate" trash entry
 */
gboolean trash_entry()
{
	FILE *fp = NULL;
	GtkWidget *msg_dialog = NULL;
	gchar entry_display_name[MAX_NAME_LEN];
	gchar curr_entry_path[MAX_PATH_LEN];
	gchar trash_entry_path[MAX_PATH_LEN];
	GList *entry_item = NULL;
	book_data *book = NULL;
	section_data *section = NULL;
	section_data *trash_section = NULL;
	entry_data *entry = NULL;
	gint result;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();
	section = get_current_section_or_return_with_warning();
	entry = get_current_entry_or_return_with_warning();
	
	// Get trash section
	trash_section = book->trash_section;
	
	// Curr entry file path
	g_snprintf(curr_entry_path, sizeof(curr_entry_path),
		"%s%s%s%s%s%s%s.txt",
		note_dir, G_DIR_SEPARATOR_S,
		book->name, G_DIR_SEPARATOR_S,
		section->name, G_DIR_SEPARATOR_S,
		entry->name);

	// Trash entry file path
	g_snprintf(trash_entry_path, sizeof(trash_entry_path),
		"%s%s%s%s%s%s%s.txt",
		note_dir, G_DIR_SEPARATOR_S,
		book->name, G_DIR_SEPARATOR_S,
		trash_section_name, G_DIR_SEPARATOR_S,
		entry->name);

	// Set display name
	strncpy(entry_display_name, entry->name, MAX_NAME_LEN-5);
	if(strlen(entry_display_name) > 25)
		strcpy(entry_display_name+25, "...\0");

	// Check that new entry path is valid
	fp = fopen(trash_entry_path, "wx");

	if (fp == NULL) {
		sn_warning("Unable to trash entry [%s].", trash_entry_path);

		msg_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_MODAL,
			GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
			"Unable to trash entry \"%s\".", entry_display_name);
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msg_dialog),
			"Please ensure the entry name doesn't aleady exist.");
		gtk_window_set_type_hint(GTK_WINDOW(msg_dialog), GDK_WINDOW_TYPE_HINT_MENU);
		gtk_window_set_resizable(GTK_WINDOW(msg_dialog), FALSE);
		gtk_window_set_title(GTK_WINDOW(msg_dialog), app_name);
		result = gtk_dialog_run(GTK_DIALOG(msg_dialog));

		gtk_widget_destroy(msg_dialog);
		return FALSE;
	}
	fclose(fp);

	// Remove file created by previous open
	result = remove(trash_entry_path);

	sn_trace("Trashing entry [%s] to [%s].", curr_entry_path, trash_entry_path);

	// Move the entry file to the new path
	result = rename(curr_entry_path, trash_entry_path);

	if(result == 0) {

		// Remove history
		remove_history();

		// Select next entry
		entry_item = g_list_find(section->entry_list, entry);
		entry_item = entry_item->next;
		if(entry_item != NULL)
			section->curr_entry = entry_item->data;
		else
			section->curr_entry = NULL;

		// Remove entry
		section->entry_list = g_list_remove(section->entry_list, entry);

		// Append entry
		trash_section->entry_list = g_list_append(trash_section->entry_list, entry);

		// Update entry
		entry->parent_section = trash_section;

		// Write book
		write_book(book, note_dir);

		// Update view
		populate_entries(book, section);

		return TRUE;

	} else {
		sn_warning("Unable to trash entry [%s].", curr_entry_path);
		return FALSE;
	}

	return FALSE;
} // Trash entry

/*
 * Delete entry
 * Signal handler for "activate" delete entry
 */
gboolean delete_entry()
{
	GtkWidget *msg_dialog;
	gchar filename[MAX_PATH_LEN];
	gchar entry_display_name[MAX_NAME_LEN];
	GList *entry_item = NULL;
	book_data *book = NULL;
	section_data *section = NULL;
	entry_data *entry = NULL;
	view_data *view = NULL;
	gint result;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();
	section = get_current_section_or_return_with_warning();
	entry = get_current_entry_or_return_with_warning();

	// Get view data
	view = book->view;

	// Set display name
	strncpy(entry_display_name, entry->name, MAX_NAME_LEN-5);
	if(strlen(entry_display_name) > 25)
		strcpy(entry_display_name+25, "...\0");

	// Confirm delete action
	msg_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_MODAL,
		GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
		"Are you sure you want to delete\n\"%s\"?", entry_display_name);
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msg_dialog),
		"If you delete an entry, it is permanently lost.");
	gtk_window_set_title(GTK_WINDOW(msg_dialog), app_name);
	gtk_window_set_type_hint(GTK_WINDOW(msg_dialog), GDK_WINDOW_TYPE_HINT_MENU);
	gtk_window_set_resizable(GTK_WINDOW(msg_dialog), FALSE);

	result = gtk_dialog_run(GTK_DIALOG(msg_dialog));
	switch (result) {
	case GTK_RESPONSE_YES:

		// Delete entry text file
		g_snprintf(filename, sizeof(filename),
			"%s%s%s%s%s%s%s.txt",
			note_dir, G_DIR_SEPARATOR_S,
			book->name, G_DIR_SEPARATOR_S,
			section->name, G_DIR_SEPARATOR_S,
			entry->name);

		sn_trace("Deleting entry [%s] in delete_entry().",
			filename);

		result = remove(filename);

		if (result == 0) {

			// Remove history
			remove_history();

			// Select next entry
			entry_item = g_list_find(section->entry_list, entry);
			entry_item = entry_item->next;
			if(entry_item != NULL)
				section->curr_entry = entry_item->data;
			else
				section->curr_entry = NULL;

			// Remove entry
			section->entry_list = g_list_remove(section->entry_list, entry);
			free_entry(entry);

			// Write book
			write_book(book, note_dir);

			// Update view
			populate_entries(book, section);
			gtk_widget_grab_focus(GTK_WIDGET(view->entry_view));
			gtk_widget_destroy(msg_dialog);
			return TRUE;

		} else {
			sn_warning("Unable to delete entry [%s].", entry->name);
			gtk_widget_destroy(msg_dialog);
			return FALSE;
		}

	default:
		gtk_widget_destroy(msg_dialog);
		return FALSE;
	}
	return FALSE;
} // Delete entry

/*
 * Shift entry up
 */
gboolean shift_entry_up()
{
	GtkTreePath *entry_path = NULL;
	GtkTreeView *entry_view = NULL;
	GtkTreeModel *entry_model = NULL;
	GtkTreeSelection *entry_selection = NULL;
	GtkTreeViewColumn *entry_column = NULL;
	GtkTreeIter entry_iter;
	book_data *book = NULL;
	section_data *section = NULL;
	entry_data *entry = NULL;
	view_data *view = NULL;
	guint entry_index = 0;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();
	section = get_current_section_or_return_with_warning();

	// Get entry view & model
	view = (view_data*)book->view;
	entry_view = view->entry_view;
	entry_model = gtk_tree_view_get_model(entry_view);

	sn_trace0("Shifting entry up.");

	// Make sure we are not at the top
	gtk_tree_view_get_cursor
		(entry_view, &entry_path, &entry_column);
	if(entry_path != NULL) {
		if(gtk_tree_path_prev(entry_path)) {

			// Get the entry to move up
			entry_selection = gtk_tree_view_get_selection(entry_view);
			if(gtk_tree_selection_get_selected
				(entry_selection, &entry_model, &entry_iter)) {
				gtk_tree_model_get(entry_model, &entry_iter,
					ENTRY_ITEM, &entry, END_OF_LIST);

				// Move entry up
				if(entry != NULL) {
					entry_index = g_list_index(section->entry_list, entry);
					section->entry_list = g_list_remove(section->entry_list, entry);
					section->entry_list = g_list_insert
						(section->entry_list, entry, --entry_index);

					// Write book
					write_book(book, note_dir);
				}
			}
			// Populate entries
			populate_entries(book, section);
			gtk_tree_view_set_cursor
				(entry_view, entry_path, entry_column, FALSE);
		}
	gtk_tree_path_free(entry_path);
	}

	return TRUE;
} // Shift entry up

/*
 * Shift entry down
 */
gboolean shift_entry_down()
{
	GtkTreePath *entry_path = NULL;
	GtkTreeView *entry_view = NULL;
	GtkTreeModel *entry_model = NULL;
	GtkTreeSelection *entry_selection = NULL;
	GtkTreeViewColumn *entry_column = NULL;
	GtkTreeIter entry_iter;
	book_data *book = NULL;
	section_data *section = NULL;
	entry_data *entry = NULL;
	view_data *view = NULL;
	guint entry_index = 0;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();
	section = get_current_section_or_return_with_warning();

	// Get entry view & model
	view = (view_data*)book->view;
	entry_view = view->entry_view;
	entry_model = gtk_tree_view_get_model(entry_view);

	sn_trace0("Shifting entry down.");

	// Make sure we are not at the bottom
	gtk_tree_view_get_cursor
		(entry_view, &entry_path, &entry_column);
	if(entry_path != NULL) {
		gtk_tree_path_next(entry_path);
		if(gtk_tree_model_get_iter(entry_model, &entry_iter, entry_path)) {

			// Get the entry to move down
			entry_selection = gtk_tree_view_get_selection(entry_view);
			if(gtk_tree_selection_get_selected
				(entry_selection, &entry_model, &entry_iter)) {
				gtk_tree_model_get(entry_model, &entry_iter,
					ENTRY_ITEM, &entry, END_OF_LIST);

				// Move entry down
				if(entry != NULL) {
					entry_index = g_list_index(section->entry_list, entry);
					section->entry_list = g_list_remove(section->entry_list, entry);
					section->entry_list = g_list_insert
						(section->entry_list, entry, ++entry_index);

					// Write book
					write_book(book, note_dir);
				}
			}
			// Populate entries
			populate_entries(book, section);
			gtk_tree_view_set_cursor
				(entry_view, entry_path, entry_column, FALSE);
		}
	gtk_tree_path_free(entry_path);
	}

	return TRUE;
} // Shift entry down

/*
 * Write current entry
 */
void write_current_entry()
{
	section_data *curr_section = NULL;
	entry_data *curr_entry = NULL;
	book_data *curr_book = NULL;

	// Assert master exists
	g_assert_nonnull(master);

	// If no current selection then
	// Return without warning
	curr_book = master->curr_book;
	if(curr_book == NULL) return;

	curr_section = curr_book->curr_section;
	if(curr_section == NULL) return;

	curr_entry = curr_section->curr_entry;
	if(curr_entry == NULL) return;

	sn_trace("< Writing current entry [%s/%s].",
		curr_section->name, curr_entry->name);

	write_text(curr_book, curr_entry);

	if(curr_entry->need_rename == TRUE) {
		curr_entry->need_rename = FALSE;
		if(options.auto_name_entry == TRUE)
			rename_entry();
	}

	return;
} // Write current entry

/*
 * On section mouse button press
 * Signal handler for "button-press-event" in book section view
 */
gboolean on_section_mouse_button_press(GtkWidget *view,
					GdkEvent *event, gpointer data)
{
	const gushort LEFT_CLICK = 1;

	if (event->type == GDK_2BUTTON_PRESS) {
		GdkEventButton *bevent = (GdkEventButton*) event;

		if (bevent->button == LEFT_CLICK) {
			sn_trace("Left Double Click in %s.", __func__);
			gtk_dialog_response(GTK_DIALOG(data), GTK_RESPONSE_ACCEPT);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
} // On section mouse button press

/*
 * Free entry
 */
void free_entry(gpointer entry)
{
	GList *edit_list = NULL;

	if(entry == NULL) {
		sn_warning0("Freeing entry [(null)].");
		return;
	}
	// Free edits
	edit_list = ((entry_data*)entry)->edit_list;
	g_list_free_full(edit_list, free_edit);

	sn_trace("Freeing entry [%#lx:%s].",
		(ulong)entry, ((entry_data*)entry)->name);

	// Free entry
	g_free(entry);

	return;
} // Free entry
