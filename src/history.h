/*
 * Simple notebook
 * history.h
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

#ifndef __SN_HISTORY_H__
#define __SN_HISTORY_H__

enum { HISTORY_ITEM = 0, N_HISTORY_COLUMNS };

typedef struct _history_data {
	gchar section_name[MAX_NAME_LEN];
	gchar entry_name[MAX_NAME_LEN];
} history_data;

extern gboolean view_history();
extern gboolean clear_history();
extern gboolean add_history();
extern gboolean remove_history();
extern gboolean remove_section_history();
extern gboolean forward_history();
extern gboolean back_history();
extern gboolean on_history_mouse_button_press(GtkWidget *view,
					GdkEvent *event, gpointer data);
extern void free_history();

#endif //__SN_HISTORY_H__
