/*
 * Simple Notebook
 * book.c
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

static const gchar *default_book_name = "Notebook";

/*
 * On book key press
 * Signal handler for "key-press-event"
 */
gboolean on_book_key_press(GtkNotebook *notebook,
					GdkEventKey *event, gpointer data)
{
	GdkModifierType modifiers;
	book_data *book = NULL;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	// Get default modifiers
	modifiers = gtk_accelerator_get_default_mod_mask();

	switch (event->keyval) {
	case GDK_KEY_Up:
		sn_trace("Up in %s", __func__);

		gtk_widget_grab_focus(GTK_WIDGET(get_entry_view(book)));
		on_entry_key_press(get_entry_view(book), event, book);

		return TRUE;

	case GDK_KEY_Down:
		sn_trace("Down in %s", __func__);

		gtk_widget_grab_focus(GTK_WIDGET(get_entry_view(book)));
		on_entry_key_press(get_entry_view(book), event, book);

		return TRUE;

	case GDK_KEY_Left:
		if((event->state & modifiers) & GDK_CONTROL_MASK) {
			if((event->state & modifiers) & GDK_SHIFT_MASK) {
				sn_trace("Ctrl+Shift+Left in %s", __func__);
				// TODO Shift Book Left
			} else {
				sn_trace("Ctrl+Left in %s", __func__);

				gtk_notebook_prev_page(notebook);
			}
		} else {
			sn_trace("Left in %s", __func__);

			gtk_widget_grab_focus(GTK_WIDGET(get_section_view(book)));
			on_section_key_press(get_section_view(book), event, book);
		}
		return TRUE;

	case GDK_KEY_Right:
		if((event->state & modifiers) & GDK_CONTROL_MASK) {
			if((event->state & modifiers) & GDK_SHIFT_MASK) {
				sn_trace("Ctrl+Shift+Right in %s", __func__);
				// TODO Shift Book Right

			} else {
				sn_trace("Ctrl+Right in %s", __func__);

				gtk_notebook_next_page(notebook);
			}
		} else {
			sn_trace("Right in %s", __func__);

			gtk_widget_grab_focus(GTK_WIDGET(get_section_view(book)));
			on_section_key_press(get_section_view(book), event, book);
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
} // On book key press

/*
 * On book change
 * Signal handler for "switch-page" event
 * Triggered by delete_book in book.c
 * Triggered by create_book_page in page.c
 */
void on_book_change(GtkNotebook *notebook, gpointer page,
					guint page_num, gpointer user_data)
{
	book_data* book = NULL;

	// Assert master exists
	g_assert_nonnull(master);
	
	book = master->curr_book;
	if(book != NULL) {
		// Write current entry
		write_current_entry();

		// Write current book
		write_book(book, note_dir);
	}

	// Get selected book
	book = g_list_nth_data(master->book_list, page_num);

	// Set current book
	if(book != NULL) {
		master->curr_book = book;

		sn_trace("Setting notebook to [%s] on page [%d] in %s.",
			book->name, page_num, __func__);
	
		// Return if book view doesn't exist
		g_return_if_fail(book->view != NULL);

		populate_sections(book);

		gtk_widget_grab_focus(GTK_WIDGET(get_entry_view(book)));

	} else {
		sn_warning("No notebook found for page [%d].", page_num);
	}
	return;
} // On book change

/*
 * Populate books
 * Called from main in main.c
 * Creates book tabs/pages for each open book
 */
gboolean populate_books(gchar *note_dir)
{
	GList *book_item;
	book_data *book;

	// Assert master exists
	g_assert_nonnull(master);

	// For each book in the book list
	for(book_item = master->book_list;
	book_item != NULL; book_item = book_item->next) {

		// Read book from file
		book = book_item->data;
		read_book(book, note_dir);

		// Is book marked open?
		if(book->is_open) {

			// Create new book page
			book->view = create_book_page(main_view, book);

		} else {
			// TODO update MRU for closed books
		}
	}
	return TRUE;
} // Populate books

/*
 * Create book
 * Signal handler for "activate" create notebook
 */
gboolean create_book()
{
	book_data *book = NULL;
	gint page_num;

	// Assert master exists
	g_assert_nonnull(master);

	// Create new book
	book = new_book(master, note_dir);

	// Read book
	read_book(book, note_dir);

	// Create new book page
	book->view = create_book_page(main_view, book);

	// Show book page
	gtk_widget_show_all(main_view);

	// Select new book
	page_num = g_list_index(master->book_list, book);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(main_view), page_num);

	// Update master
	write_master(master, note_dir);

	// Rename book
	rename_book();

	return TRUE;
} // Create book

/*
 * Rename book 
 * Signal handler for "activate" rename notebook
 * Called from rename_book in book.c
 */
gboolean rename_book()
{
	GtkWidget *name_dialog = NULL;
	GtkWidget *msg_dialog = NULL;
	GtkWidget *vbox = NULL;
	GtkWidget *hbox = NULL;
	GtkWidget *label = NULL;
	GtkWidget *name_entry = NULL;
	const gchar *entry_text = NULL;
	gchar old_book_display_name[MAX_NAME_LEN];
	gchar new_book_display_name[MAX_NAME_LEN];
	gchar new_book_name[MAX_PATH_LEN];
	gchar old_book_name[MAX_PATH_LEN];
	book_data *book = NULL;
	gchar *temp_string;
	gint result;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

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

	label = gtk_label_new("Enter new notebook name... ");
	name_entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(name_entry), MAX_NAME_LEN-5);
	gtk_entry_set_text(GTK_ENTRY(name_entry), book->name);

	vbox = gtk_dialog_get_content_area(GTK_DIALOG(name_dialog));

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 5);
	gtk_box_pack_start (GTK_BOX(vbox), hbox, TRUE, TRUE, 5);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), name_entry, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 5);

	gtk_entry_set_activates_default(GTK_ENTRY(name_entry), TRUE);
	gtk_dialog_set_default_response (GTK_DIALOG(name_dialog),GTK_RESPONSE_ACCEPT);
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

		// Old book directory
		g_snprintf(old_book_name, sizeof(old_book_name), "%s%s%s",
			note_dir, G_DIR_SEPARATOR_S,
			book->name);

		// New book directory
		g_snprintf(new_book_name, sizeof(new_book_name), "%s%s%s",
			note_dir, G_DIR_SEPARATOR_S,
			entry_text);

		// Ignore rename if names match
		if(strcmp(book->name, entry_text) == 0) {
			gtk_widget_destroy(name_dialog);
			return TRUE;
		}

		// Set display name
		strncpy(old_book_display_name, book->name, MAX_NAME_LEN-5);
		if(strlen(old_book_display_name) > 25)
			strcpy(old_book_display_name+25, "...\0");

		strncpy(new_book_display_name, entry_text, MAX_NAME_LEN-5);
		if(strlen(new_book_display_name) > 25)
			strcpy(new_book_display_name+25, "...\0");

		// Check that new book name is valid
		if(strlen(entry_text) < 1) {

			sn_warning("Unable to rename notebook [%s] to [%s].",
				book->name, entry_text);

			msg_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window),
				GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
				"Unable to rename notebook \"%s\" to \"%s\".",
				old_book_display_name, new_book_display_name);
			gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msg_dialog),
				"Please ensure the notebook name contains a valid character.");
			gtk_window_set_title(GTK_WINDOW(msg_dialog), app_name);
			gtk_window_set_type_hint(GTK_WINDOW(msg_dialog), GDK_WINDOW_TYPE_HINT_MENU);
			gtk_window_set_resizable(GTK_WINDOW(msg_dialog), FALSE);
			result = gtk_dialog_run(GTK_DIALOG(msg_dialog));

			gtk_widget_destroy(name_dialog);
			gtk_widget_destroy(msg_dialog);
			return FALSE;
		}

		// Check that new book name is valid
		if(mkdir(new_book_name, S_IRWXU | S_IRWXG) != 0) {

			sn_warning("Unable to rename notebook [%s] to [%s].",
				book->name, entry_text);

			msg_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window),
				GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
				"Unable to rename notebook \"%s\" to \"%s\".",
				old_book_display_name, new_book_display_name);
			gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msg_dialog),
				"Please ensure the notebook name doesn't aleady exist.");
			gtk_window_set_title(GTK_WINDOW(msg_dialog), app_name);
			gtk_window_set_type_hint(GTK_WINDOW(msg_dialog), GDK_WINDOW_TYPE_HINT_MENU);
			gtk_window_set_resizable(GTK_WINDOW(msg_dialog), FALSE);
			result = gtk_dialog_run(GTK_DIALOG(msg_dialog));

			gtk_widget_destroy(name_dialog);
			gtk_widget_destroy(msg_dialog);
			return FALSE;
		}

		sn_trace("Renaming notebook [%s] to [%s].", book->name, entry_text);

		// Rename directory
		rename(old_book_name, new_book_name);

		// Old book index file
		g_snprintf(old_book_name, sizeof(old_book_name),
			"%s%s%s%s%s.idx",
			note_dir, G_DIR_SEPARATOR_S,
			entry_text, G_DIR_SEPARATOR_S,
			book->name);

		// New book index file
		g_snprintf(new_book_name, sizeof(new_book_name),
			"%s%s%s%s%s.idx",
			note_dir, G_DIR_SEPARATOR_S,
			entry_text, G_DIR_SEPARATOR_S,
			entry_text);

		// Rename index
		rename(old_book_name, new_book_name);

		// Update book
		strcpy(book->name, entry_text);

		// Update master
		write_master(master, note_dir);

		// Update view
		label = gtk_label_new(entry_text);
		gtk_notebook_set_tab_label(GTK_NOTEBOOK(main_view),
			GTK_WIDGET(get_book_page(book)), GTK_WIDGET(label));

		gtk_widget_destroy(name_dialog);
		return TRUE;

	default:
		gtk_widget_destroy(name_dialog);
		return FALSE;

	} // End switch

	return FALSE;
} // Rename book

/*
 * Delete book
 * Signal handler for "activate" delete notebook
 */
gboolean delete_book()
{
	GtkWidget *msg_dialog = NULL;
	GList *entry_item = NULL;
	GList *section_item = NULL;
	gchar book_name[MAX_PATH_LEN];
	gchar section_name[MAX_PATH_LEN];
	gchar entry_name[MAX_PATH_LEN];
	gchar book_display_name[MAX_NAME_LEN];
	book_data *book = NULL;
	section_data *section = NULL;
	entry_data *entry = NULL;
	gint page_num;
	gint result;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	// Set display name
	strncpy(book_display_name, book->name, MAX_NAME_LEN-5);
	if(strlen(book_display_name) > 25)
		strcpy(book_display_name+25, "...\0");

	// Confirm delete action
	msg_dialog = gtk_message_dialog_new(GTK_WINDOW(main_window), GTK_DIALOG_MODAL,
		GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
		"Are you sure you want to permanenty delete\n\"%s\" and all of its"
		" sections and entries?", book_display_name);
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(msg_dialog),
		"If you delete a notebook, it is permanently lost.");
	gtk_window_set_title(GTK_WINDOW(msg_dialog), app_name);
	gtk_window_set_type_hint(GTK_WINDOW(msg_dialog), GDK_WINDOW_TYPE_HINT_MENU);
	gtk_window_set_resizable(GTK_WINDOW(msg_dialog), FALSE);

	result = gtk_dialog_run(GTK_DIALOG(msg_dialog));
	switch (result) {
	case GTK_RESPONSE_YES:

		// Delete Do Not Delete file
		g_snprintf(entry_name, sizeof(entry_name),
			"%s%s%s%s%s%sDo Not Delete",
			note_dir, G_DIR_SEPARATOR_S,
			book->name, G_DIR_SEPARATOR_S,
			trash_section_name, G_DIR_SEPARATOR_S);

		remove(entry_name);

		// Delete sections
		for(section_item = book->section_list;
		section_item != NULL; section_item = section_item->next) {

			section = section_item->data;

			// Delete entries
			for(entry_item = section->entry_list;
			entry_item != NULL; entry_item = entry_item->next) {

				entry = entry_item->data;

				// Delete entry text file
				g_snprintf(entry_name, sizeof(entry_name),
					"%s%s%s%s%s%s%s.txt",
					note_dir, G_DIR_SEPARATOR_S,
					book->name, G_DIR_SEPARATOR_S,
					section->name, G_DIR_SEPARATOR_S,
					entry->name);

				sn_trace("Deleting entry [%s] in delete_book().",
					entry_name);

				result = remove(entry_name);

				if(result != 0) {
					sn_warning("Unable to delete entry [%s].", entry_name);
				}

			} // Delete entries

			// Free entries
			g_list_free_full(section->entry_list, free_entry);
			section->entry_list = NULL;

			// Delete section directory
			g_snprintf(section_name, sizeof(section_name),
				"%s%s%s%s%s",
				note_dir, G_DIR_SEPARATOR_S,
				book->name, G_DIR_SEPARATOR_S,
				section->name);

			sn_trace("Deleting section [%s] in delete_book().", section_name);

			result = rmdir(section_name);

			if(result != 0) {
				sn_warning("Unable to delete section [%s].", section_name);
			}

		} // Delete sections

		// Free sections
		g_list_free_full(book->section_list, free_section);
		book->section_list = NULL;

		// Delete book index file
		g_snprintf(entry_name, sizeof(entry_name), "%s%s%s%s%s.idx",
			note_dir, G_DIR_SEPARATOR_S,
			book->name, G_DIR_SEPARATOR_S,
			book->name);

		remove(entry_name);

		// Delete book directory
		g_snprintf(book_name, sizeof(book_name), "%s%s%s",
			note_dir, G_DIR_SEPARATOR_S,
			book->name);

		sn_trace("Deleting notebook [%s] in delete_book().",
			book_name);

		result = rmdir(book_name);

		if(result != 0) {
			sn_warning("Unable to delete notebook [%s].", book_name);
		}

		// Get page to remove
		page_num = g_list_index(master->book_list, book);

		// Update master
		master->book_list = g_list_remove(master->book_list, book);
		master->curr_book = NULL;

		// Remove book page
		// Triggers on_book_change setting new curr_book
		gtk_notebook_remove_page(GTK_NOTEBOOK(main_view), page_num);

		sn_trace("Removing notebook [%s] in delete_book().", book_name);

		// Write master
		write_master(master, note_dir);

		// Handle case if last book deleted and on_book_change not called
		if (master->curr_book == book)
			master->curr_book = NULL;

		// Free book
		free_book(book);

		gtk_widget_destroy(msg_dialog);
		return TRUE;

	default:
		gtk_widget_destroy(msg_dialog);
		return FALSE;

	} // End switch
	return FALSE;
} // Delete book

/*
 * New book index
 * Called from main in main.c
 * Called from create_book in book.c
 */
book_data* new_book(master_data* master, gchar* note_dir)
{
	FILE *fp = NULL;
	gchar book_name[MAX_NAME_LEN];
	gchar section_name[MAX_PATH_LEN];
	gchar filename[MAX_PATH_LEN];
	book_data *book = NULL;
	gint cindex = 0;

	// Assert master exists
	g_assert_nonnull(master);

	// Write current book
	if(master->curr_book != NULL) {
		write_book(master->curr_book, note_dir);
	}

	// Create book directory
	while (++cindex < MAX_TRIES) {
		// Create book name
		g_snprintf(book_name, sizeof(book_name), "%s%d",
			default_book_name, cindex);

		// Create directory
		g_snprintf(filename, sizeof(filename), "%s%s%s",
			note_dir, G_DIR_SEPARATOR_S,
			book_name);

		if(mkdir(filename, S_IRWXU | S_IRWXG) == 0) {
			break;
		}
	}

	// Create filename
	g_snprintf(filename, sizeof(filename), "%s%s%s%s%s.idx",
		note_dir, G_DIR_SEPARATOR_S,
		book_name, G_DIR_SEPARATOR_S,
		book_name);

	sn_trace("Creating notebook index [%s].", filename);

	// Create book index
	fp = fopen(filename, "w");
	if (fp != NULL) {

		// Create book
		book = g_new0(book_data, NEW_INSTANCE);
		strcpy(book->name, book_name);
		book->is_open = TRUE;

		// Create trash directory
		g_snprintf(section_name, sizeof(section_name),
			"%s%s%s%s%s",
			note_dir, G_DIR_SEPARATOR_S,
			book->name, G_DIR_SEPARATOR_S,
			trash_section_name);

		mkdir(section_name, S_IRWXU | S_IRWXG);

		// Create Do Not Delete file
		g_snprintf(section_name, sizeof(section_name),
			"%s%s%s%s%s%sDo Not Delete",
			note_dir, G_DIR_SEPARATOR_S,
			book->name, G_DIR_SEPARATOR_S,
			trash_section_name, G_DIR_SEPARATOR_S);

		fclose(fopen(section_name, "w"));

		// Update master
		master->book_list = g_list_append(master->book_list, book);
		master->book_count++;
		master->open_count++;

		// Write to file
		fprintf(fp, "%s\r\n", file_header);
		fprintf(fp, "%s\r\n", trash_section_name);
		fclose(fp);

	} else {
		sn_error("Unable to create notebook index [%s].", filename);
		return NULL;
	}
	return book;
} // New book

/*
 * Read book index
 * Called from create_book in book.c
 * Called from populate_books in book.c
 * Reads book index from file and creates
 * section and entry data structures
 */
gboolean read_book(book_data *book, gchar *note_dir)
{
	FILE *fp = NULL;
	gchar filename[MAX_PATH_LEN];
	gchar *buffer = NULL;
	size_t bufsize = MAX_NAME_LEN;
	section_data *section = NULL;
	entry_data *entry = NULL;
	glong num_read = 0;

	// Create filename
	g_snprintf(filename, sizeof(filename),
		"%s%s%s%s%s.idx",
		note_dir, G_DIR_SEPARATOR_S,
		book->name, G_DIR_SEPARATOR_S,
		book->name);

	sn_trace("Reading notebook index [%s].", filename);

	// Open notebook file
	fp = fopen(filename, "r");
	if (fp != NULL) {

		// Read header
		buffer = g_malloc0(bufsize);
		num_read = getline(&buffer, &bufsize, fp);

		if(num_read > 1) {
			buffer[num_read-2] = 0;
			if(strcmp(buffer, file_header)) {
				sn_warning("File header mismatch in [%s].", filename);
			}
		}
		// Read groups and entries
		num_read = getline(&buffer, &bufsize, fp);

		while(num_read > 0) {

			// Check for length
			if(num_read > MAX_NAME_LEN-3 || num_read < 3) {
				// Get next line
				num_read = getline(&buffer, &bufsize, fp);
				continue;
			}

			// Is this a section or entry?
			if(buffer[0] != '\\') {

				// Create section
				section = g_new0(section_data, NEW_INSTANCE);

				// Set section name
				buffer[num_read-2] = 0;
				strncpy(section->name, buffer, MAX_NAME_LEN-5);

				sn_trace("Creating section [%s] in %s", section->name, __func__);

				// Set sections parent book
				section->parent_book = book;

				// Add section to books section list
				book->section_list = g_list_append(book->section_list, section);

				// Is this the trash section?
				if (strcmp(section->name, trash_section_name) == 0) {
					book->trash_section = section;
				}
			} else {

				// Create entry
				entry = g_new0(entry_data, NEW_INSTANCE);

				// Set entry name
				buffer[num_read-2] = 0;
				strncpy(entry->name, buffer+1, MAX_NAME_LEN-5);

				sn_trace("Creating entry [%s/%s] in %s.",
					section->name, entry->name, __func__);

				// Make sure we read a section
				if (section != NULL) {

					// Set parent section
					entry->parent_section = section;

					// Add entry to sections entry list
					section->entry_list = g_list_append(section->entry_list, entry);
				} else {
					sn_warning("Entry [%s] found before Section!", entry->name);
				}
			}
			num_read = getline(&buffer, &bufsize, fp);
		}
		// TODO Read memory
		// TODO Read history
		g_free(buffer);
		fclose(fp);
		return TRUE;
	} else {
		sn_warning("Unable to read notebook index [%s].", filename);
		return FALSE;
	}
	return FALSE;
} // Read book

/*
 * Write book index
 * Called from most signal handlers
 */
gboolean write_book(book_data *book, gchar *note_dir)
{
	FILE *fp = NULL;
	gchar filename[MAX_PATH_LEN];
	section_data *section = NULL;
	entry_data *entry = NULL;
	GList *section_item = NULL;
	GList *entry_item = NULL;

	// Create filename
	g_snprintf(filename, sizeof(filename),
		"%s%s%s%s%s.idx",
		note_dir, G_DIR_SEPARATOR_S,
		book->name, G_DIR_SEPARATOR_S,
		book->name);

	sn_trace("Writing notebook index [%s].", filename);

	// Open notebook file
	fp = fopen(filename, "w");
	if (fp != NULL) {

		// Write header
		fprintf(fp, "%s\r\n", file_header);

		// Write section to file
		for(section_item = book->section_list;
			section_item != NULL; section_item = section_item->next) {

			section = section_item->data;
			fprintf(fp, "%s\r\n", section->name);

			// Write entries to file
			for(entry_item = section->entry_list;
				entry_item != NULL; entry_item = entry_item->next) {

				entry = entry_item->data;
				fprintf(fp, "\\%s\r\n", entry->name);
			}
		}
		fclose(fp);
		return TRUE;
	} else {
		sn_error("Unable to write notebook index [%s].", filename);
		return FALSE;
	}
	return FALSE;
} // Write book

/*
 * Free book
 */
void free_book(gpointer book)
{
	GList *section_list = NULL;
	GList *history_list = NULL;

	if(book == NULL) {
		sn_warning0("Freeing notebook [(null)].");
		return;
	}
	// Free section list
	section_list = ((book_data*)book)->section_list;
	g_list_free_full(section_list, free_section);

	// Free history list
	history_list = ((book_data*)book)->history_list;
	g_list_free_full(history_list, free_history);

	// Free view data
	g_free(((book_data*)book)->view);

	sn_trace("Freeing notebook [%#lx:%s].",
		(ulong)book, ((book_data*)book)->name);

	// Free book
	g_free(book);

	return;
} // Free book
