/*
 * Simple Notebook
 * master.c
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

const gchar *master_file = "Notebook.idx";
const gchar *file_header = "Notebook 2.0";

/*
 * Global master data structure
 */
master_data *master = NULL;

/*
 * New master index file
 */
master_data* new_master(gchar *note_dir)
{
	FILE *fp = NULL;
	gchar filename[MAX_PATH_LEN];

	// Create directory
	sn_trace("Creating base dir [%s].", note_dir);

	if(mkdir(note_dir, S_IRWXU | S_IRWXG) != 0) {
		sn_warning("Unable to create base dir [%s]", note_dir);
	}

	// Create filename
	g_snprintf(filename, sizeof(filename), "%s%s%s",
		note_dir, G_DIR_SEPARATOR_S, master_file);

	sn_trace("Creating master index [%s].", filename);

	// Open index file
	fp = fopen(filename, "w");
	if (fp != NULL) {

		// Create master
		master = g_new0(master_data, NEW_INSTANCE);
		strcpy(master->name, file_header);

		sn_trace("Creating master [%s].", master->name);

		// Write to file
		fprintf(fp, "%s\r\n", master->name);
		fclose(fp);
		return master;

	} else {
		sn_error("Unable to create master index [%s].", filename);
		return NULL;
	}
	return NULL;
} // New master

/*
 * Read master index file
 */
master_data* read_master(gchar *note_dir)
{
	FILE *fp = NULL;
	gchar filename[MAX_PATH_LEN];
	gchar *buffer = NULL;
	size_t bufsize = MAX_NAME_LEN;
	master_data *master = NULL;
	book_data *book = NULL;
	gboolean open_found = FALSE;
	glong num_read = 0;

	// Create filename
	g_snprintf(filename, sizeof(filename), "%s%s%s",
		note_dir, G_DIR_SEPARATOR_S, master_file);

	sn_trace("Reading master index [%s].", filename);

	// Open master file
	fp = fopen(filename, "r");
	if (fp != NULL) {

		// Read header
		buffer = g_malloc0(bufsize);
		num_read = getline(&buffer, &bufsize, fp);

		if(num_read > 1) {
			buffer[num_read-2] = 0; // Strip CR LF
			if(strcmp(buffer, file_header)) {
				sn_warning("File header mismatch in [%s].", filename);
			}
		}
		// Create master
		master = g_new0(master_data, NEW_INSTANCE);

		// Set master name
		strcpy(master->name, file_header);

		// Read books
		num_read = getline(&buffer, &bufsize, fp);
		while(num_read > 0) {

			// Check for length
			if(num_read > MAX_NAME_LEN-3 || num_read < 3) {
				// Get next line
				num_read = getline(&buffer, &bufsize, fp);
				continue;
			}

			// Create book
			book = g_new0(book_data, NEW_INSTANCE);

			// Is this book marked open?
			if(buffer[num_read-3] == '*') {
				buffer[num_read-3] = 0;
				book->is_open = TRUE;
				master->open_count++;

				// Is this the first found?
				if(open_found == FALSE) {
					master->curr_book = book;
				}
				open_found = TRUE;
			}
			else {
				buffer[num_read-2] = 0;
				book->is_open = FALSE;
			}

			// Update book name
			strncpy(book->name, buffer, MAX_NAME_LEN-5);

			// Update master list
			master->book_list = g_list_append(master->book_list, book);
			master->book_count++;

			// Get next line
			num_read = getline(&buffer, &bufsize, fp);
		}
		g_free(buffer);
		fclose(fp);

		if(!open_found) {
			sn_warning("Open book marker not found in [%s].", filename);
		}

		return master;
	} else {
		sn_warning("Unable to read master index [%s].", filename);
		return NULL;
	}
	return NULL;
} // Read master

/*
 * Write master index file
 */
gboolean write_master(master_data *master, gchar *note_dir)
{
	FILE *fp = NULL;
	gchar filename[MAX_PATH_LEN];
	gchar *buffer = NULL;
	size_t bufsize = MAX_NAME_LEN;
	book_data *book = NULL;
	GList *book_item = NULL;

	// Create filename
	g_snprintf(filename, sizeof(filename), "%s%s%s",
		note_dir, G_DIR_SEPARATOR_S, master_file);

	sn_trace("Writing master index [%s].", filename);

	// Open master index file
	fp = fopen(filename, "w");
	if (fp != NULL) {

		// Write master
		fprintf(fp, "%s\r\n", file_header);

		// Write books
		buffer = g_malloc0(bufsize);

		for(book_item = master->book_list;
		book_item != NULL; book_item = book_item->next) {

			book = book_item->data;
			strcpy(buffer, book->name);

			if(book->is_open) {
				strcat(buffer, "*\r\n");
			} else {
				strcat(buffer, "\r\n");
			}
			fputs(buffer, fp);
		}
		g_free(buffer);
		fclose(fp);
		return TRUE;
	} else {
		sn_error("Unable to write master index [%s].", filename);
		return FALSE;
	}
	return FALSE;
} // Write master

/*
 * Free master
 */
void free_master(master_data *master)
{
	GList *book_list = NULL;

	if(master == NULL) {
		sn_warning0("Freeing master [(null)].");
		return;
	}
	// Free books
	book_list = master->book_list;
	g_list_free_full(book_list, free_book);

	sn_trace("Freeing master [%s].", master->name);

	// Free master
	g_free(master);

	return;
} // Free master
