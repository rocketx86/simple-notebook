/*
 * Simple Notebook
 * main.h
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

#ifndef __SN_MAIN_H__
#define __SN_MAIN_H__

typedef struct _options_data {
	gboolean auto_name_entry;
	gboolean confirm_delete_book;
	gboolean confirm_delete_section;
} options_data;

extern options_data options;
extern const gchar *app_name;
extern gchar *note_dir;
extern GtkWidget *main_window;
extern GtkWidget *main_view;
extern GtkWidget *main_menu;
extern GtkWidget *main_toolbar;
extern GtkWidget *main_statusbar;
extern guint main_status_id;
extern GtkAccelGroup *accel_group;

extern GtkWidget* create_icon_widget(const gchar *filename);
extern gboolean on_main_window_delete(GtkWidget *widget,
					GdkEvent *event, gpointer data);
extern gboolean on_main_view_mouse_button_press(GtkWidget *menu,
					GdkEvent *event, gpointer data);

#endif //__SN_MAIN_H__
