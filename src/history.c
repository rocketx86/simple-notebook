/*
 * Simple Notebook
 * history.c
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
 *
 */

#include "common.h"

static void get_history_name (GtkTreeViewColumn *tree_column,
					GtkCellRenderer *cell_rend, GtkTreeModel *model,
					GtkTreeIter *tree_iter, gpointer data);
static gboolean select_history(history_data *history);

/*
 * Get history name
 * Sets the text of cell_rend to history
 */
static void get_history_name (GtkTreeViewColumn *tree_column,
					GtkCellRenderer *cell_rend, GtkTreeModel *model,
					GtkTreeIter *tree_iter, gpointer data)
{
	gchar history_name[MAX_PATH_LEN];
	history_data *history = NULL;

	gtk_tree_model_get(model, tree_iter, HISTORY_ITEM, &history, END_OF_LIST);
	g_snprintf(history_name, MAX_PATH_LEN-1, "%s/%s",
		history->section_name, history->entry_name);
	g_object_set(cell_rend, "text", history_name, NULL);
	return;
} // Get history name

/*
 * View history
 */
gboolean view_history()
{
	GtkWidget *history_dialog = NULL;
	GtkWidget *history_scroll = NULL;
	GtkWidget *history_view = NULL;
	GtkTreeModel *history_model = NULL;
	GtkListStore *history_store = NULL;
	GtkTreeSelection *history_selection = NULL;
	GtkCellRenderer *cell_rend = NULL;
	GtkTreeViewColumn *view_col = NULL;
	GtkTreeIter history_iter;
	GtkWidget *hbox = NULL;
	GtkWidget *vbox = NULL;
	GtkWidget *label = NULL;
	GList *history_item = NULL;
	history_data *history = NULL;
	book_data *book = NULL;
	gint result;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	sn_trace0("View history.");

	// Create history dialog
	history_dialog = gtk_dialog_new_with_buttons(app_name, GTK_WINDOW(main_window),
			GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	gtk_window_set_type_hint(GTK_WINDOW(history_dialog), GDK_WINDOW_TYPE_HINT_MENU);
	gtk_window_set_resizable(GTK_WINDOW(history_dialog), FALSE);

	gtk_dialog_set_default_response(GTK_DIALOG(history_dialog),
		GTK_RESPONSE_ACCEPT);

	// Set label
	label = gtk_label_new("Select entry to view from history...");
	hbox = gtk_hbox_new(FALSE, 0);
	vbox = gtk_dialog_get_content_area(GTK_DIALOG(history_dialog));
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 5);

	// Create scroll view
	history_scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(history_scroll, MAIN_WIDTH / 2, MAIN_HEIGHT / 3);
	gtk_scrolled_window_set_shadow_type
		(GTK_SCROLLED_WINDOW(history_scroll), GTK_SHADOW_IN);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(history_scroll),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), history_scroll, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 5);

	// Create tree view
	history_view = gtk_tree_view_new();
	gtk_container_add(GTK_CONTAINER(history_scroll), history_view);

	// Create history column
	view_col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(view_col, "History");
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(history_view), FALSE);
	gtk_tree_view_append_column(GTK_TREE_VIEW(history_view), view_col);

	// Create history cell_rend with get_history_name callback
	cell_rend = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(view_col, cell_rend, TRUE);
	gtk_tree_view_column_set_cell_data_func
		(view_col, cell_rend, get_history_name, NULL, NULL);

	// Create history model / store
	history_store = gtk_list_store_new(N_HISTORY_COLUMNS, G_TYPE_POINTER);
	for(history_item = book->history_list;
	history_item != NULL; history_item = history_item->next) {
		history = history_item->data;
		gtk_list_store_append(history_store, &history_iter);
		gtk_list_store_set(history_store, &history_iter,
			HISTORY_ITEM, history, END_OF_LIST);
	}

	// Set history model on history view
	gtk_tree_view_set_model(GTK_TREE_VIEW(history_view),
		GTK_TREE_MODEL(history_store));
	if(history_store != NULL)
		g_object_unref(history_store);

	// Set mouse button press signal handler
	g_signal_connect(history_view, "button-press-event",
		G_CALLBACK(on_history_mouse_button_press), history_dialog);

	// Show and run view history dialog
	gtk_widget_show_all(history_dialog);
	result = gtk_dialog_run(GTK_DIALOG(history_dialog));

	switch (result) {
	case GTK_RESPONSE_ACCEPT:

		history_model = GTK_TREE_MODEL(history_store);
		history_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(history_view));
		if(gtk_tree_selection_get_selected(history_selection,
				&history_model, &history_iter)) {

			gtk_tree_model_get(history_model, &history_iter, HISTORY_ITEM,
				&history, END_OF_LIST);

			if(history != NULL) {
				select_history(history);
			}
		}
		gtk_widget_destroy(history_dialog);
		return TRUE;

	default:
		gtk_widget_destroy(history_dialog);
		return FALSE;

	} // End switch

	return FALSE;
} // View history

/*
 * Clear history
 */
gboolean clear_history()
{
	GtkWidget *msg_dialog = NULL;
	book_data *book = NULL;
	GList *history_list = NULL;
	gint result = 0;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	// Confirm clear action
	msg_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window),
		GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
		"Are you sure you want to clear history?");
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msg_dialog),
		"If you clear history, it is permanently lost.");
	gtk_window_set_title(GTK_WINDOW(msg_dialog), app_name);

	result = gtk_dialog_run(GTK_DIALOG(msg_dialog));

	switch (result) {
	case GTK_RESPONSE_YES:

		// Free history list
		history_list = book->history_list;
		g_list_free_full(history_list, free_history);
		book->history_list = NULL;
		book->curr_history = NULL;
		gtk_widget_destroy(msg_dialog);

		// TODO Write history

		return TRUE;

	default:
		gtk_widget_destroy(msg_dialog);
		return FALSE;
	}
	return TRUE;
} // Clear history

/*
 * Add history
 * Called from on_text_view_focus
 * Called from on_book_key_press
 * Called from on_section_key_press
 * Called from on_entry_key_press
 */
gboolean add_history()
{
	history_data *history = NULL;
	book_data *book = NULL;
	section_data *section = NULL;
	entry_data *entry = NULL;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();
	section = get_current_section_or_return_with_warning();
	entry = get_current_entry_or_return_with_warning();

	// Is this the trash section?
	if(strcmp(section->name, trash_section_name) == 0) {
		return FALSE;
	}

	// Remove history
	remove_history();

	// Create history
	history = g_new0(history_data, NEW_INSTANCE);

	// Set history names
	strncpy(history->section_name, section->name, MAX_NAME_LEN-5);
	strncpy(history->entry_name, entry->name, MAX_NAME_LEN-5);

	// Insert history into list
	book->history_list = g_list_insert(book->history_list, history, 0);

	// Set current history
	book->curr_history = history;

	sn_trace("Adding history [%s/%s].",
		history->section_name,
		history->entry_name);

	// TODO Write history

	return TRUE;
} // Add history

/*
 * Remove history
 * Called by add_history
 * Called by delete_entry
 */
gboolean remove_history()
{
	GList *history_item = NULL;
	history_data *history = NULL;
	book_data *book = NULL;
	section_data *section = NULL;
	entry_data *entry = NULL;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();
	section = get_current_section_or_return_with_warning();
	entry = get_current_entry_or_return_with_warning();

	history_item = book->history_list;

	while(history_item != NULL) {

		history = (history_data*)history_item->data;
		if((strcmp(history->section_name, section->name) == 0)
		&& (strcmp(history->entry_name, entry->name) == 0)) {

			// Is this the current history
			if(book->curr_history == history) {

				// Set current history
				if(history_item->next) {
					book->curr_history = history_item->next->data;
				} else if(history_item->prev) {
					book->curr_history = history_item->prev->data;
				} else {
					book->curr_history = NULL;
				}
			}
			sn_trace("Removing history [%s/%s].",
				history->section_name,
				history->entry_name);

			// Remove history
			book->history_list = g_list_remove(book->history_list, history);
			free_history(history);
			break;
		}
		history_item = history_item->next;
	}

	return TRUE;
} // Remove history

/*
 * Remove section history
 * Called by delete_section
 */
gboolean remove_section_history()
{
	GList *history_item = NULL;
	history_data *history = NULL;
	book_data *book = NULL;
	section_data *section = NULL;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();
	section = get_current_section_or_return_with_warning();

	history_item = book->history_list;

	while(history_item != NULL) {

		history = (history_data*)history_item->data;
		if((strcmp(history->section_name, section->name) == 0)) {

			// Is this the current history
			if(book->curr_history == history) {

				// Set current history
				if(history_item->next) {
					book->curr_history = history_item->next->data;
				} else if(history_item->prev) {
					book->curr_history = history_item->prev->data;
				} else {
					book->curr_history = NULL;
				}
			}
			sn_trace("Removing history [%s/%s].",
				history->section_name,
				history->entry_name);

			// Remove history
			book->history_list = g_list_remove(book->history_list, history);
			free_history(history);
			break;
		}
		history_item = history_item->next;
	}

	return TRUE;
} // Remove section history

/*
 * Forward history
 */
gboolean forward_history()
{
	GList *history_item = NULL;
	history_data *history = NULL;
	book_data *book = NULL;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	sn_trace0("Forward history.");

	// Move history forward
	history_item = g_list_find(book->history_list, book->curr_history);
	if(history_item == NULL) {
		return FALSE;
	}
	if(history_item->prev == NULL) {
		history = history_item->data;
	} else {
		history_item = history_item->prev;
		history = history_item->data;
	}

	select_history(history);

	return TRUE;
} // Forward history

/*
 * Back history
 */
gboolean back_history()
{
	GList *history_item = NULL;
	history_data *history = NULL;
	book_data *book = NULL;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	sn_trace0("Back history.");

	// Move history backward
	history_item = g_list_find(book->history_list, book->curr_history);
	if(history_item == NULL) {
		return FALSE;
	}

	if(history_item->next == NULL) {
		history = history_item->data;
	} else {
		history_item = history_item->next;
		history = history_item->data;
	}

	select_history(history);

	return TRUE;
} // Back history

/*
 * Select history
 */
static gboolean select_history(history_data *history)
{
	GtkTreePath *tree_path = NULL;
	GtkTreeView *tree_view = NULL;
	GtkTreeModel *tree_model = NULL;
	GtkTreeIter tree_iter;
	book_data *book = NULL;
	section_data *section = NULL;
	entry_data *entry = NULL;
	view_data *view = NULL;
	gboolean more = FALSE;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	sn_trace0("Select history.");

	// Get model and view
	view = book->view;
	tree_view = view->section_view;
	tree_model = gtk_tree_view_get_model(tree_view);

	// Find history section
	more = gtk_tree_model_get_iter_first(tree_model, &tree_iter);
	while (more == TRUE) {
		gtk_tree_model_get(tree_model, &tree_iter,
					SECTION_ITEM, &section, END_OF_LIST);
		if(strcmp(history->section_name, section->name) == 0) {
			tree_path = gtk_tree_model_get_path(tree_model, &tree_iter);
			gtk_tree_view_set_cursor(tree_view, tree_path, NULL, FALSE);
			break;
		}
		more = gtk_tree_model_iter_next(tree_model, &tree_iter);
	}

	// Get model and view
	tree_view = view->entry_view;
	tree_model = gtk_tree_view_get_model(tree_view);

	// Find history entry
	more = gtk_tree_model_get_iter_first(tree_model, &tree_iter);
	while (more == TRUE) {
		gtk_tree_model_get(tree_model, &tree_iter,
					ENTRY_ITEM, &entry, END_OF_LIST);
		if(strcmp(history->entry_name, entry->name) == 0) {
			tree_path = gtk_tree_model_get_path(tree_model, &tree_iter);
			gtk_tree_view_set_cursor(tree_view, tree_path, NULL, FALSE);
			book->curr_history = history;
			break;
		}
		more = gtk_tree_model_iter_next(tree_model, &tree_iter);
	}

	return TRUE;
} // Select history

/*
 * On history mouse button press
 * Signal handler for mouse clicks on history view
 */
gboolean on_history_mouse_button_press(GtkWidget *view,
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
} // On history mouse button press

/*
 * Free history
 */
void free_history(gpointer history)
{
	if(history == NULL) {
		sn_warning0("Freeing history [(null)].");
		return;
	}

	sn_trace("Freeing history [%s/%s].",
		((history_data*)history)->section_name,
		((history_data*)history)->entry_name);

	// Free history
	g_free(history);

	return;
} // Free history
