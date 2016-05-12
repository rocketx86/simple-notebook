/*
 * Simple Notebook
 * master.h
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

#ifndef __SN_MASTER_H__
#define __SN_MASTER_H__

typedef struct _book_data book_data;

typedef struct _master_data {
	gchar name[MAX_NAME_LEN];
	GList *book_list;
	book_data *curr_book;
	glong book_count;
	glong open_count;
} master_data;

extern master_data *master;
extern const gchar *master_file;
extern const gchar *file_header;

extern master_data* new_master(gchar *note_dir);
extern master_data* read_master(gchar *note_dir);
extern gboolean write_master(master_data *master, gchar *note_dir);
extern void free_master(master_data *master);

#endif //__SN_MASTER_H__
