/*
 * Simple Notebook
 * edit.c
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
 * New edit
 */
edit_data* new_edit(guint32 edit_type, GtkTextIter *edit_start,
				GtkTextIter *edit_end, gchar *edit_text, gboolean same_action)
{
	static guint32 action_id;
	edit_data *edit = NULL;

	// Assign action ID
	if(!same_action || action_id == 0) {
		action_id++;
	}
	// Set edit properties
	edit = g_new0(edit_data, NEW_INSTANCE);
	edit->edit_type = edit_type;
	edit->edit_action = action_id;
	edit->edit_start = gtk_text_iter_get_offset(edit_start);
	edit->edit_end = gtk_text_iter_get_offset(edit_end);
	edit->edit_text = edit_text;
	edit->edit_length = strlen(edit_text);

	return edit;
} // New edit

/*
 * Add edit
 */
edit_data* add_edit(edit_data *edit)
{
	GList *edit_item = NULL;
	book_data *book = NULL;
	section_data *section = NULL;
	entry_data *entry = NULL;
	edit_data *temp_edit = NULL;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();
	section = get_current_section_or_return_with_warning();
	entry = get_current_entry_or_return_with_warning();

	// Remove edit items after current edit
	edit_item = g_list_find(entry->edit_list, entry->curr_edit);
	if(edit_item == NULL) {
		sn_trace("Remove all edit items in %s.", __func__);
		g_list_free_full(entry->edit_list, free_edit);
		entry->edit_list = NULL;
	}
	if(edit_item != NULL) {
		edit_item = edit_item->next;
		while(edit_item != NULL) {
			temp_edit = edit_item->data;
			edit_item = edit_item->next;
			sn_trace("Remove edit item [%i:%s] in %s.",
				temp_edit->edit_action, temp_edit->edit_text, __func__);
			entry->edit_list = g_list_remove(entry->edit_list, temp_edit);
			free_edit(temp_edit);
			temp_edit = NULL;
		}
	}
	// Add edit item and make current
	entry->edit_list = g_list_append(entry->edit_list, edit);
	entry->curr_edit = edit;

	#if __SN_TRACE_ON__
	if(edit->edit_type == edit_insert) {
		sn_trace("Add insert edit item [%i:%s] in %s.",
			edit->edit_action, edit->edit_text, __func__);
	} else {
		sn_trace("Add delete edit item [%i:%s] in %s.",
			edit->edit_action, edit->edit_text, __func__);
	}
	#endif

	return edit;
} // Add edit

edit_data* get_current_edit()
{
	book_data *book = NULL;
	section_data *section = NULL;
	entry_data *entry = NULL;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();
	section = get_current_section_or_return_with_warning();
	entry = get_current_entry_or_return_with_warning();

	return entry->curr_edit;
} // Get current edit

/*
 * Undo text
 */
gboolean undo_text()
{
	GList *edit_item = NULL;
	GtkTextView *text_view = NULL;
	GtkTextBuffer *text_buffer = NULL;
	GtkTextIter text_start, text_end;
	book_data *book = NULL;
	section_data *section = NULL;
	entry_data *entry = NULL;
	edit_data *edit = NULL;
	guint32 action = 0;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();
	section = get_current_section_or_return_with_warning();
	entry = get_current_entry_or_return_with_warning();

	// Get text buffer
	text_view = get_text_view(book);
	text_buffer = gtk_text_view_get_buffer(text_view);

	// Get current edit
	edit = entry->curr_edit;

	if(edit != NULL) {

		action = edit->edit_action;

		// Get text iters from edit offsets
		gtk_text_buffer_get_iter_at_offset(text_buffer,
				&text_start, edit->edit_start);
		gtk_text_buffer_get_iter_at_offset(text_buffer,
				&text_end, edit->edit_end);

		switch(edit->edit_type) {

		// Undo insert
		case edit_insert:
			sn_trace("Undo insert edit item [%i:%s] in %s.",
				edit->edit_action, edit->edit_text, __func__);
			gtk_text_buffer_select_range(text_buffer, &text_start, &text_end);
			gtk_text_buffer_delete_selection(text_buffer, TRUE, TRUE);
			gtk_text_buffer_set_modified(text_buffer, TRUE);
			break;

		// Undo delete
		case edit_delete:
			sn_trace("Undo delete edit item [%i:%s] in %s.",
				edit->edit_action, edit->edit_text, __func__);
			gtk_text_buffer_place_cursor(text_buffer, &text_start);
			gtk_text_buffer_insert_at_cursor(text_buffer,
					edit->edit_text, edit->edit_length);
			gtk_text_buffer_set_modified(text_buffer, TRUE);
			break;
		}

		// Prev edit
		edit_item = g_list_find(entry->edit_list, edit);
		edit_item = edit_item->prev;
		if(edit_item) {
			entry->curr_edit = edit_item->data;
		} else {
			entry->curr_edit = NULL;
		}
	}

	// Is this edit part of the same action?
	edit = entry->curr_edit;
	if(edit != NULL) {
		if(edit->edit_action == action) {
			undo_text();
		}
	}
	return TRUE;
} // Undo text

/*
 * Redo text
 */
gboolean redo_text()
{
	GList *edit_item = NULL;
	GtkTextView *text_view = NULL;
	GtkTextBuffer *text_buffer = NULL;
	GtkTextIter text_start, text_end;
	book_data *book = NULL;
	section_data *section = NULL;
	entry_data *entry = NULL;
	edit_data *edit = NULL;
	guint32 action = 0;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();
	section = get_current_section_or_return_with_warning();
	entry = get_current_entry_or_return_with_warning();

	// Get text buffer
	text_view = get_text_view(book);
	text_buffer = gtk_text_view_get_buffer(text_view);

	// Get prev edit
	edit_item = g_list_find(entry->edit_list, entry->curr_edit);
	if(edit_item == NULL) {
		edit_item = g_list_first(entry->edit_list);
		if(edit_item == NULL) {
			return FALSE;
		}
	} else {
		edit_item = edit_item->next;
		if(edit_item == NULL) {
			return FALSE;
		}
	}
	edit = edit_item->data;

	if(edit != NULL) {

		action = edit->edit_action;

		// Get text iters from edit offsets
		gtk_text_buffer_get_iter_at_offset(text_buffer,
				&text_start, edit->edit_start);
		gtk_text_buffer_get_iter_at_offset(text_buffer,
				&text_end, edit->edit_end);

		switch(edit->edit_type) {

		// Redo insert
		case edit_insert:
			sn_trace("Redo insert edit item [%i:%s] in %s.",
				edit->edit_action, edit->edit_text, __func__);
			gtk_text_buffer_place_cursor(text_buffer, &text_start);
			gtk_text_buffer_insert_at_cursor(text_buffer,
					edit->edit_text, edit->edit_length);
			gtk_text_buffer_set_modified(text_buffer, TRUE);
			break;

		// Redo delete
		case edit_delete:
			sn_trace("Redo delete edit item [%i:%s] in %s.",
				edit->edit_action, edit->edit_text, __func__);
			gtk_text_buffer_select_range(text_buffer, &text_start, &text_end);
			gtk_text_buffer_get_selection_bounds(text_buffer, &text_start, &text_end);
			gtk_text_buffer_delete_selection(text_buffer, TRUE, TRUE);
			gtk_text_buffer_set_modified(text_buffer, TRUE);
			break;
		}

		// Set current edit
		entry->curr_edit = edit;
	}

	// Is this edit part of the same action?
	edit_item = g_list_find(entry->edit_list, edit);
	edit_item = edit_item->next;
	if(edit_item) {
		edit = edit_item->data;
		if(edit != NULL) {
			if(edit->edit_action == action) {
				redo_text();
			}
		}
	}

	return TRUE;
} // Redo text

/*
 * Copy file
 */
gint copy_file(const gchar *to, const gchar *from)
{
	gint fd_to, fd_from;
	gchar buf[4096];
	ssize_t nread;
	gint saved_errno;

	fd_from = open(from, O_RDONLY);
	if(fd_from < 0)
		return -1;

	fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
	if(fd_to < 0)
		goto out_error;

	while(nread = read(fd_from, buf, sizeof buf), nread > 0) {
		char *out_ptr = buf;
		ssize_t nwritten;

		do {
			nwritten = write(fd_to, out_ptr, nread);

			if(nwritten >= 0) {
				nread -= nwritten;
				out_ptr += nwritten;
			} else if(errno != EINTR) {
				goto out_error;
			}
		} while(nread > 0);
	}

	if(nread == 0) {
		if(close(fd_to) < 0) {
			fd_to = -1;
			goto out_error;
		}
		close(fd_from);

		/* Success! */
		return 0;
	}

out_error:
	saved_errno = errno;

	close(fd_from);
	if(fd_to >= 0)
		close(fd_to);

	errno = saved_errno;
	return -1;
}

/*
 * Free edit
 */
void free_edit(gpointer edit)
{
	if(edit == NULL) {
		sn_warning0("Freeing edit [(null)].");
		return;
	}

	sn_trace("Freeing text [%#lx:%s].",
		((ulong)((edit_data*)edit)->edit_text),
		((edit_data*)edit)->edit_text);

	g_free(((edit_data*)edit)->edit_text);

	sn_trace("Freeing edit [%#lx:%i].",
		(ulong)edit, ((edit_data*)edit)->edit_action);

	// Free edit
	g_free(edit);

	return;
} // Free edit
