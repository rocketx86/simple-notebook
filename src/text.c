/*
 * Simple Notebook
 * text.c
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

static gchar current_buffer_name[MAX_PATH_LEN];

static gboolean insert_return(void);
static gboolean insert_backspace(void);
static gboolean insert_unicode(GdkEventKey *event);

/*
 * On text view focus
 */
gboolean on_text_view_focus(GtkWidget *widget, GdkEvent *event,
					gpointer user_data)
{
	add_history();
	return FALSE;
} // On text view focus

/*
 * Get auto name
 */
gchar* get_auto_entry_name(GtkTextView *text_view)
{
	GtkTextBuffer *text_buffer = NULL;
	GtkTextIter start, end;
	gchar *text = NULL;
	gchar *ptr = NULL;

	text_buffer = gtk_text_view_get_buffer(text_view);

	gtk_text_buffer_get_start_iter(text_buffer, &start);
	gtk_text_buffer_get_iter_at_offset(text_buffer, &end, MAX_NAME_LEN-5);
	text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);

	ptr = strstr(text, "\n");
	if(ptr != NULL)
		*ptr = 0;
	ptr = strstr(text, "\r");
	if(ptr != NULL)
		*ptr = 0;

	return text;
} // Get auto entry name

/*
 * String delete characters
 */
gchar* strdelchr(gchar *dst, const gchar *src, const gchar *del)
{
	guint i, j, k;

	for(i = k = 0; src[i] != 0; i++, k++) {
		dst[k] = src[i];
		for(j = 0; del[j] != 0; j++) {
			if(dst[k] == del[j]) {
				dst[k--] = 0;
				break;
			}
		}
	}
	return dst;
} // String delete characters


/*
 * Select all text
 * Signal handler for "activate" select all
 */
gboolean select_all_text()
{
	GtkTextView *text_view = NULL;
	GtkTextBuffer *text_buffer = NULL;
	GtkTextIter text_start, text_end;
	book_data *book = NULL;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	// Get text view
	text_view = get_text_view(book);

	// Select all text
	text_buffer = gtk_text_view_get_buffer(text_view);
	gtk_text_buffer_get_bounds(text_buffer, &text_start, & text_end);
	gtk_text_buffer_select_range(text_buffer, &text_start, & text_end);

	return TRUE;
} // Select all text

/*
 * Cut text
 * Signal handler for "activate" cut text
 */
gboolean cut_text()
{
	GtkTextView *text_view = NULL;
	GtkTextBuffer *text_buffer = NULL;
	GtkClipboard *clipboard = NULL;
	book_data *book = NULL;
	GtkTextIter text_start, text_end;
	gchar* text = NULL;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	// Get text view
	text_view = get_text_view(book);

	// Get selected text
	text_buffer = gtk_text_view_get_buffer(text_view);
	gtk_text_buffer_get_selection_bounds(text_buffer, &text_start, &text_end);
	text = gtk_text_buffer_get_text(text_buffer, &text_start, &text_end, FALSE);

	// Create edit
	add_edit(new_edit(edit_delete, &text_start, &text_end, text, FALSE));

	// Cut to clipboard
	clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	gtk_text_buffer_cut_clipboard(text_buffer, clipboard, TRUE);
	gtk_text_buffer_set_modified(text_buffer, TRUE);

	return TRUE;
} // Cut text

/*
 * Copy text
 * Signal handler for "activate" copy text
 */
gboolean copy_text()
{
	GtkTextView *text_view = NULL;
	GtkTextBuffer *text_buffer = NULL;
	GtkClipboard *clipboard = NULL;
	book_data *book = NULL;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	// Get text view
	text_view = get_text_view(book);

	// Copy to clipboard
	text_buffer = gtk_text_view_get_buffer(text_view);
	clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	gtk_text_buffer_copy_clipboard(text_buffer, clipboard);

	return TRUE;
} // Copy text

/*
 * Paste text
 * Signal handler for "activate" paste text
 */
gboolean paste_text()
{
	GtkTextView *text_view = NULL;
	GtkTextBuffer *text_buffer = NULL;
	GtkClipboard *clipboard = NULL;
	book_data *book = NULL;
	edit_data *edit = NULL;
	GtkTextMark *text_mark = NULL;
	GtkTextIter text_start, text_end;
	gchar *text = NULL;
	gboolean same_action = FALSE;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	// Get text view
	text_view = get_text_view(book);

	// Get text insertion
	text_buffer = gtk_text_view_get_buffer(text_view);
	if(gtk_text_buffer_get_has_selection(text_buffer)) {
		gtk_text_buffer_get_selection_bounds(text_buffer, &text_start, &text_end);
		text = gtk_text_buffer_get_text(text_buffer, &text_start, &text_end, FALSE);

		// Create edit
		add_edit(new_edit(edit_delete, &text_start, &text_end, text, FALSE));
		gtk_text_buffer_delete_selection(text_buffer, TRUE, TRUE);
		same_action = TRUE;
	}
	text_mark = gtk_text_buffer_get_insert(text_buffer);
	gtk_text_buffer_get_iter_at_mark(text_buffer, &text_start, text_mark);

	// Create edit
	edit = add_edit(new_edit(edit_insert, &text_start, &text_start,
		"PASTE", same_action));

	// Paste from clipboard
	gtk_text_buffer_set_modified(text_buffer, TRUE);
	clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	gtk_text_buffer_paste_clipboard(text_buffer, clipboard, NULL, TRUE);

	sn_trace("Start paste edit item [%i:%s] in %s.",
		edit->edit_action, edit->edit_text, __func__);

	return TRUE;
} // Paste text

/*
 * On paste done
 * Signal handler for "paste-done" set in page.c
 */
void on_paste_done(GtkTextBuffer *text_buffer, GtkClipboard *clipboard,
					gpointer user_data)
{
	edit_data *edit = NULL;
	GtkTextIter text_start, text_end;
	GtkTextMark *text_mark = NULL;
	gchar *text = NULL;

	// Get text insertion
	text_mark = gtk_text_buffer_get_insert(text_buffer);
	gtk_text_buffer_get_iter_at_mark(text_buffer, &text_end, text_mark);

	// Get last edit
	edit = get_current_edit();
	if(edit == NULL) return;

	edit->edit_end = gtk_text_iter_get_offset(&text_end);
	gtk_text_buffer_get_iter_at_offset(text_buffer, &text_start, edit->edit_start);

	// Get selected text
	text = gtk_text_buffer_get_text(text_buffer, &text_start, &text_end, FALSE);
	edit->edit_length = strlen(text);
	edit->edit_text = text;

	sn_trace("Finish paste edit item [%i:%s] in %s.",
		edit->edit_action, edit->edit_text, __func__);

	return;
} // On paste done

/*
 * Delete text
 * Signal handler for "activate" delete text
 */
gboolean delete_text()
{
	GtkTextView *text_view = NULL;
	GtkTextBuffer *text_buffer = NULL;
	book_data *book = NULL;
	GtkTextMark *text_mark = NULL;
	GtkTextIter text_start, text_end;
	gchar* text = NULL;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	// Get text view
	text_view = get_text_view(book);

	// Delete text
	text_buffer = gtk_text_view_get_buffer(text_view);
	if(gtk_text_buffer_get_has_selection(text_buffer)) {

		gtk_text_buffer_get_selection_bounds(text_buffer, &text_start, &text_end);
		text = gtk_text_buffer_get_text(text_buffer, &text_start, &text_end, FALSE);

		// Create edit
		add_edit(new_edit(edit_delete, &text_start, &text_end, text, FALSE));

		gtk_text_buffer_delete_selection(text_buffer, TRUE, TRUE);

	} else {

		text_mark = gtk_text_buffer_get_insert(text_buffer);
		gtk_text_buffer_get_iter_at_mark(text_buffer, &text_start, text_mark);
		text_end = text_start;
		gtk_text_iter_forward_char(&text_end);
		if(gtk_text_iter_get_char(&text_end) == '\n') {
			gtk_text_iter_forward_char(&text_end);
		}
		text = gtk_text_buffer_get_text(text_buffer, &text_start, &text_end, FALSE);

		// Create edit
		add_edit(new_edit(edit_delete, &text_start, &text_end, text, FALSE));

		gtk_text_buffer_delete_interactive(text_buffer, &text_start, &text_end, TRUE);

	}
	gtk_text_buffer_set_modified(text_buffer, TRUE);

	return TRUE;
} // Delete text

/*
 * Insert return
 * Key handler for return
 * Called from on_text_key_press in text.c
 */
static gboolean insert_return()
{
	GtkTextView *text_view = NULL;
	GtkTextBuffer *text_buffer = NULL;
	GtkTextMark *text_mark = NULL;
	GtkTextIter text_start, text_end;
	book_data *book = NULL;
	gchar *text = NULL;
	gboolean same_action = FALSE;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	// Get text view
	text_view = get_text_view(book);

	// Insert return
	text_buffer = gtk_text_view_get_buffer(text_view);

	if(gtk_text_buffer_get_has_selection(text_buffer)) {
		gtk_text_buffer_get_selection_bounds(text_buffer, &text_start, &text_end);
		text = gtk_text_buffer_get_text(text_buffer, &text_start, &text_end, FALSE);

		// Create edit
		add_edit(new_edit(edit_delete, &text_start, &text_end, text, FALSE));
		gtk_text_buffer_delete_selection(text_buffer, TRUE, TRUE);
		same_action = TRUE;
	}

	gtk_text_buffer_insert_at_cursor(text_buffer, "\r\n", 2);
	gtk_text_buffer_set_modified(text_buffer, TRUE);

	text_mark = gtk_text_buffer_get_insert(text_buffer);
	gtk_text_buffer_get_iter_at_mark(text_buffer, &text_end, text_mark);
	text_start = text_end;
	gtk_text_iter_backward_chars(&text_start, 2);
	text = gtk_text_buffer_get_text(text_buffer, &text_start, &text_end, FALSE);

	// Create edit
	add_edit(new_edit(edit_insert, &text_start, &text_end, text, same_action));

	return TRUE;
} // Insert return

/*
 * Insert backspace
 * Key handler for backspace
 * Called from on_text_key_press in text.c
 */
static gboolean insert_backspace()
{
	GtkTextView *text_view = NULL;
	GtkTextBuffer *text_buffer = NULL;
	GtkTextMark *text_mark = NULL;
	GtkTextIter text_start, text_end;
	book_data *book = NULL;
	gchar *text = NULL;
	gboolean same_action = FALSE;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	// Get text view
	text_view = get_text_view(book);

	// Insert backspace
	text_buffer = gtk_text_view_get_buffer(text_view);

	if(gtk_text_buffer_get_has_selection(text_buffer)) {
		gtk_text_buffer_get_selection_bounds(text_buffer, &text_start, &text_end);
		text = gtk_text_buffer_get_text(text_buffer, &text_start, &text_end, FALSE);

		// Create edit
		add_edit(new_edit(edit_delete, &text_start, &text_end, text, FALSE));
		gtk_text_buffer_delete_selection(text_buffer, TRUE, TRUE);
		same_action = TRUE;
	}
	text_mark = gtk_text_buffer_get_insert(text_buffer);
	gtk_text_buffer_get_iter_at_mark(text_buffer, &text_end, text_mark);
	text_start = text_end;
	gtk_text_iter_backward_char(&text_start);
	if(gtk_text_iter_get_char(&text_start) == '\n') {
		gtk_text_iter_backward_char(&text_start);
	}
	text = gtk_text_buffer_get_text(text_buffer, &text_start, &text_end, FALSE);

	// Create edit
	add_edit(new_edit(edit_delete, &text_start, &text_end, text, same_action));

	gtk_text_buffer_delete_interactive(text_buffer, &text_start, &text_end, TRUE);

	return TRUE;
} // Insert backspace

/*
 * Insert unicode
 * Key handler for most alpha keys
 * Called from on_text_key_press in text.c
 */
static gboolean insert_unicode(GdkEventKey *event)
{
	GtkTextView *text_view = NULL;
	GtkTextBuffer *text_buffer = NULL;
	GtkTextMark *text_mark = NULL;
	GtkTextIter text_start, text_end;
	book_data *book = NULL;
	gchar *text = NULL;
	guint32 unicode = 0L;
	gboolean same_action = FALSE;

	// Assert master exists
	g_assert_nonnull(master);

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	// Get text view
	text_view = get_text_view(book);

	// Insert unicode
	text_buffer = gtk_text_view_get_buffer(text_view);

	if(gtk_text_buffer_get_has_selection(text_buffer)) {
		gtk_text_buffer_get_selection_bounds(text_buffer, &text_start, &text_end);
		text = gtk_text_buffer_get_text(text_buffer, &text_start, &text_end, FALSE);

		// Create edit
		add_edit(new_edit(edit_delete, &text_start, &text_end, text, FALSE));
		gtk_text_buffer_delete_selection(text_buffer, TRUE, TRUE);
		same_action = TRUE;
	}
	text = g_malloc0(2);
	unicode = gdk_keyval_to_unicode(event->keyval);
	g_snprintf(text, 2, "%c", unicode);

	text_mark = gtk_text_buffer_get_insert(text_buffer);

	gtk_text_buffer_insert_at_cursor(text_buffer, text, NEW_INSTANCE);
	gtk_text_buffer_set_modified(text_buffer, TRUE);

	gtk_text_buffer_get_iter_at_mark(text_buffer, &text_end, text_mark);
	text_start = text_end;
	gtk_text_iter_backward_char(&text_start);

	// Create edit
	add_edit(new_edit(edit_insert, &text_start, &text_end, text, same_action));

	return TRUE;
} // Insert unicode

/*
 * On text key press
 * Signal handler for "key-press-event"
 */
gboolean on_text_key_press(GtkTextView *text_view,
					GdkEventKey *event, gpointer data)
{
	GdkModifierType modifiers;
	book_data *book = NULL;

	// Get currently selected
	book = get_current_book_or_return_with_warning();

	// Get default modifiers
	modifiers = gtk_accelerator_get_default_mod_mask();

	switch (event->keyval) {
	case GDK_KEY_Return:
		/* no break */
	case GDK_KEY_KP_Enter:
		if((event->state & modifiers) & GDK_SHIFT_MASK) {
			sn_trace("Shift+Enter in %s", __func__);
			write_current_entry();
			gtk_widget_grab_focus(GTK_WIDGET(get_entry_view(book)));
			return TRUE;
		} else {
			insert_return();
			return TRUE;
		}
	case GDK_KEY_s:
		if((event->state & modifiers) & GDK_CONTROL_MASK) {
			sn_trace("Control+s in %s", __func__);
			write_current_entry();
			return TRUE;
		} else {
			insert_unicode(event);
			return TRUE;
		}
	case GDK_KEY_a:
		if((event->state & modifiers) & GDK_CONTROL_MASK) {
			sn_trace("Control+a in %s", __func__);
			select_all_text();
			return TRUE;
		} else {
			insert_unicode(event);
			return TRUE;
		}
	case GDK_KEY_BackSpace:
		insert_backspace();
		return TRUE;

	case GDK_KEY_space ... GDK_KEY_at:
	case GDK_KEY_A ... GDK_KEY_quoteleft:
	case GDK_KEY_b ... GDK_KEY_r:
	case GDK_KEY_t ... GDK_KEY_asciitilde:
	case GDK_KEY_KP_Space ... GDK_KEY_KP_Tab:
	case GDK_KEY_KP_F1 ... GDK_KEY_KP_9:
		insert_unicode(event);
		return TRUE;

	default:
		sn_trace("Key [%u] in %s", event->keyval, __func__);
		return FALSE;
	}
	return TRUE;
} // On text key press

/*
 * Set text view focus
 */
void set_text_view_focus(GtkTextView *text_view)
{
	GtkTextBuffer *text_buffer = NULL;
	GtkTextIter text_iter;

	text_buffer = gtk_text_view_get_buffer(text_view);
	gtk_text_buffer_get_iter_at_offset(text_buffer, &text_iter, 0);
	gtk_text_buffer_place_cursor(text_buffer, &text_iter);
	gtk_widget_grab_focus(GTK_WIDGET(text_view));

	return;
} // Set text view focus

/*
 * Read text
 * Called from populate_entries in entry.c
 * Called from on_entry_change in entry.c
 */
gboolean read_text(book_data *book, entry_data *entry)
{
	GtkTextView *text_view = NULL;
	GtkTextBuffer *text_buffer = NULL;
	section_data *section = NULL;
	gchar filename[MAX_PATH_LEN];
	gchar *buffer = NULL;
	size_t bufsize = 0;
	FILE *fp = NULL;

	// Check for valid entry
	if (entry == NULL) {

		// Get text view
		text_view = get_text_view(book);

		// Set text buffer
		text_buffer = gtk_text_view_get_buffer(text_view);
		gtk_text_buffer_set_text(text_buffer, "", -1);
		gtk_text_buffer_set_modified(text_buffer, FALSE);

		// Set current buffer name
		strcpy(current_buffer_name, "");

		//Get entry section
		section = book->curr_section;

		// Update ststus bar
		if(section != NULL) {

			// Create filename
			g_snprintf(filename, sizeof(filename),
				"%s%s%s",
				book->name, G_DIR_SEPARATOR_S,
				section->name);

			gtk_statusbar_pop(GTK_STATUSBAR(main_statusbar), main_status_id);
			gtk_statusbar_push(GTK_STATUSBAR(main_statusbar),
				main_status_id, filename);
		} else {

			gtk_statusbar_pop(GTK_STATUSBAR(main_statusbar), main_status_id);
			gtk_statusbar_push(GTK_STATUSBAR(main_statusbar),
				main_status_id, book->name);
		}
		return FALSE;
	}

	//Get entry section
	section = entry->parent_section;

	// Create filename
	g_snprintf(filename, sizeof(filename),
		"%s%s%s%s%s%s%s.txt",
		note_dir, G_DIR_SEPARATOR_S,
		book->name, G_DIR_SEPARATOR_S,
		section->name, G_DIR_SEPARATOR_S,
		entry->name);

	// Check current buffer name
	if(strcmp(current_buffer_name, filename) == 0) {
		sn_trace("! Reading [%s/%s] not necessary.",
			section->name, entry->name);
		return TRUE;
	}

	// Open text file
	fp = fopen(filename, "r");
	if (fp != NULL) {

		// Determine buffer size
		fseek(fp, 0L, SEEK_END);
		bufsize = ftell(fp);

		// Allocate buffer
		buffer = g_malloc(bufsize + 1);

		// Read file into buffer
		fseek(fp, 0L, SEEK_SET);
		fread(buffer, sizeof(char), bufsize, fp);

		// Terminate buffer
		buffer[bufsize] = 0;

		// Get text view
		text_view = get_text_view(book);

		// Set text buffer in view
		text_buffer = gtk_text_view_get_buffer(text_view);
		gtk_text_buffer_set_text(text_buffer, buffer, -1);
		gtk_text_buffer_set_modified(text_buffer, FALSE);

		// Set current buffer name
		strcpy(current_buffer_name, filename);

		free(buffer);
		fclose(fp);

		// Create status
		g_snprintf(filename, sizeof(filename),
			" %s%s%s%s%s ",
			book->name, G_DIR_SEPARATOR_S,
			section->name, G_DIR_SEPARATOR_S,
			entry->name);

		// Update status bar
		gtk_statusbar_pop(GTK_STATUSBAR(main_statusbar),main_status_id);
		gtk_statusbar_push(GTK_STATUSBAR(main_statusbar),main_status_id, filename);

		return TRUE;
	} else {
		sn_warning("Unable to read text file [%s].", filename);
		return FALSE;
	}
	return FALSE;
} // Read text

/*
 * Write text
 * Called from write_current_entry in entry.c
 */
gboolean write_text(book_data *book, entry_data *entry)
{
	FILE *fp = NULL;
	gchar filename[MAX_PATH_LEN];
	gchar *text = NULL;
	GtkTextView *text_view = NULL;
	GtkTextBuffer *text_buffer = NULL;
	GtkTextIter start, end;
	section_data *section = NULL;
	GMimeStream *stream = NULL;
	GMimeStream *fstream = NULL;
	GMimeFilter *filter = NULL;

	// Check for valid book
	if (book == NULL) {
		sn_warning0("Invalid book writing text.");
		return FALSE;
	}

	// Check for valid entry
	if (entry == NULL) {
		sn_warning0("Invalid entry writing text.");
		return FALSE;
	}

	// Get entry section
	section = entry->parent_section;
	if (section == NULL) {
		sn_warning0("Invalid section writing text.");
		return FALSE;
	}

	// Create filename
	g_snprintf(filename, sizeof(filename),
		"%s%s%s%s%s%s%s.txt",
		note_dir, G_DIR_SEPARATOR_S,
		book->name, G_DIR_SEPARATOR_S,
		section->name, G_DIR_SEPARATOR_S,
		entry->name);

	// Get text buffer
	text_view = get_text_view(book);
	text_buffer = gtk_text_view_get_buffer(text_view);

	// Check current buffer name
	if(strcmp(current_buffer_name, filename) == 0) {
		if(gtk_text_buffer_get_modified(text_buffer) == FALSE) {
			sn_trace("! Writing [%s/%s] not necessary.",
				section->name, entry->name);
			return TRUE;
		}
	}

	// Open text file
	fp = fopen(filename, "w");
	if (fp != NULL) {
		
		// Get text from buffer
		gtk_text_buffer_set_modified(text_buffer, FALSE);
		gtk_text_buffer_get_start_iter(text_buffer, &start);
		gtk_text_buffer_get_end_iter(text_buffer, &end);
		text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);

		// Create filtered file stream
		stream = g_mime_stream_file_new(fp);
		fstream = g_mime_stream_filter_new(stream);
		filter = g_mime_filter_crlf_new(TRUE, FALSE);
		g_mime_stream_filter_add((GMimeStreamFilter*)fstream, filter);
		g_object_unref(filter);

		// Write to stream
		g_mime_stream_write_string(fstream, text);
		g_mime_stream_flush(fstream);
		g_mime_stream_write_string(stream, "\00");
		g_mime_stream_flush(stream);

		// Free stream
		g_object_unref(fstream);
		
		g_free(text);
		fclose(fp);
		return TRUE;
	} else {
		sn_warning("Unable to write text file [%s].", filename);
		return FALSE;
	}

	return FALSE;
} // Write text
