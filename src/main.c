/*
 * Simple Notebook
 * main.c
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

#define DEFAULT_NOTE_DIR ".simple-notebook"
#define DEFAULT_CONF_DIR ".config/simple-notebook"
#define DEFAULT_RES_DIR "/usr/share/simple-notebook"

const gchar *app_name = "Simple Notebook";
options_data options = {
	.auto_name_entry = TRUE,
	.confirm_delete_book = TRUE,
	.confirm_delete_section = TRUE
};
gchar *note_dir = NULL;
GtkWidget *main_window = NULL;
GtkWidget *main_view = NULL;
GtkWidget *main_menu = NULL;
GtkWidget *main_toolbar = NULL;
GtkWidget *main_statusbar = NULL;
GtkAccelGroup *accel_group = NULL;
guint main_status_id = 0;

static gchar *res_dir = NULL;
static gchar *conf_dir = NULL;

int main(int argc, char *argv[]);
static void read_options(int argc, char *argv[]);
static GtkWidget* create_main_window();
static GdkPixbuf* create_pixbuf(const gchar *filename);

/*
 * Main application
 */
int main(int argc, char *argv[])
{
	sn_trace("Starting [%s].", app_name);

	// Read options
	read_options(argc, argv);

	// Init Gtk framework
	gtk_init(&argc, &argv);
	g_mime_init (0);

	// Create main window
	main_window = create_main_window();

	// Read master index
	if(!(master = read_master(note_dir)))
	{
		master = new_master(note_dir);
		new_book(master, note_dir);
		write_master(master, note_dir);
	}

	// Populate book pages
	populate_books(note_dir);

	// Setup signal handlers
	g_signal_connect(main_view, "switch-page",
		G_CALLBACK(on_book_change), NULL);

	g_signal_connect(main_view, "key-press-event",
		G_CALLBACK(on_book_key_press), NULL);

	g_signal_connect_swapped(main_view, "button-press-event",
		G_CALLBACK(on_main_view_mouse_button_press), bookMenu);

	g_signal_connect(main_window, "delete-event",
		G_CALLBACK(on_main_window_delete), NULL);

	// Show windows
	gtk_widget_show_all(main_window);

	// Run main loop
	gtk_main();

	return 0;
}

/*
 * Read options
 */
static void read_options(int argc, char *argv[])
{
	int option;

	conf_dir = g_malloc0(MAX_PATH_LEN);
	g_snprintf(conf_dir, MAX_PATH_LEN, "%s%s%s", getenv("HOME"),
		G_DIR_SEPARATOR_S, DEFAULT_CONF_DIR);

	note_dir = g_malloc0(MAX_PATH_LEN);
	g_snprintf(note_dir, MAX_PATH_LEN, "%s%s%s", getenv("HOME"),
		G_DIR_SEPARATOR_S, DEFAULT_NOTE_DIR);

	res_dir = g_malloc0(MAX_PATH_LEN);
	g_snprintf(res_dir, MAX_PATH_LEN, "%s", DEFAULT_RES_DIR);

	opterr = 0;
	option = getopt (argc, argv, "c: d: r:");
	while (option != -1) {
		switch (option) {
		case 'c':
			strncpy(conf_dir, optarg, MAX_PATH_LEN-1);
			break;
		case 'd':
			strncpy(note_dir, optarg, MAX_PATH_LEN-1);
			break;
		case 'r':
			strncpy(res_dir, optarg, MAX_PATH_LEN-1);
			break;
		case '?':
			if (optopt == 'd')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint (optopt)) {
				fprintf (stderr, "Unknown option '-%c'.\n", optopt);
			} else {
				fprintf (stderr, "Unknown option character '\\x%x'.\n", optopt);
			}
			/* no break */
		default:
			fprintf(stderr, "Usage: %s [-c ./config/dir] "
				"[-d ./notes/dir] [-r ./res/dir]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
		option = getopt (argc, argv, "c: d: r:");
	}

	sn_trace("Current dir is [%s].", getenv("PWD"));
	sn_trace("Config dir is [%s].", conf_dir);
	sn_trace("Notebook dir is [%s].", note_dir);
	sn_trace("Resource dir is [%s].", res_dir);

	sn_trace("Auto name entry is set to [%d].", options.auto_name_entry);

	return;
}

/*
 * Create main window
 */
static GtkWidget* create_main_window()
{
	GtkWidget *main_box;
	GdkPixbuf *main_icon;
	GtkWidget *main_handle;

	// Create main window
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW (main_window), app_name);
	gtk_window_set_default_size(GTK_WINDOW(main_window), MAIN_WIDTH, MAIN_HEIGHT);
	gtk_widget_set_size_request(main_window, MIN_WIDTH, MIN_HEIGHT);
	gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER);
	gtk_window_set_resizable (GTK_WINDOW(main_window), TRUE);
	gtk_widget_realize(main_window);

	sn_trace("Creating main window [%#lx].", (ulong)main_window);

	// Set icon
	main_icon = create_pixbuf ("notebook.png");
	gtk_window_set_icon(GTK_WINDOW(main_window), main_icon);
	g_object_unref (main_icon);

	// Setup main box
	main_box = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(main_window), main_box);

	// Setup accel group
	accel_group = gtk_accel_group_new();
	gtk_window_add_accel_group(GTK_WINDOW(main_window), accel_group);

	// Create main menu
	main_menu = create_main_menu();
	gtk_box_pack_start(GTK_BOX(main_box), main_menu, FALSE, FALSE, 0);

	// Create toolbar
	main_handle = gtk_handle_box_new();
	gtk_box_pack_start(GTK_BOX(main_box), main_handle, FALSE, FALSE, 0);
	main_toolbar = create_main_toolbar();
	gtk_container_add(GTK_CONTAINER(main_handle), main_toolbar);

	// Create notebook
	main_view = gtk_notebook_new ();
	gtk_container_set_border_width (GTK_CONTAINER (main_view), 0);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(main_view), FALSE);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(main_view), TRUE);
	gtk_box_pack_start(GTK_BOX(main_box), main_view, TRUE, TRUE, 0);

	// Create statusbar
	main_statusbar = gtk_statusbar_new();
	gtk_box_pack_start(GTK_BOX(main_box), main_statusbar, FALSE, FALSE, 0);
	main_status_id = gtk_statusbar_get_context_id
		(GTK_STATUSBAR(main_statusbar),"main-status");

	return main_window;
}

/*
 * Create pixel buffer from file
 */
static GdkPixbuf* create_pixbuf(const gchar *filename)
{
	GdkPixbuf *pixbuf = NULL;
	GError *error = NULL;
	gchar image_path[MAX_PATH_LEN];

	g_snprintf(image_path, sizeof(image_path), "%s/%s", res_dir, filename);
	pixbuf = gdk_pixbuf_new_from_file(image_path, &error);

	if (!pixbuf) {
		sn_warning("%s", error->message);
		g_error_free(error);
	}

	return pixbuf;
}

/*
 * Create icon widget from file
 */
GtkWidget* create_icon_widget(const gchar *filename)
{
	GtkWidget *iconw = NULL;
	gchar image_path[MAX_PATH_LEN];

	g_snprintf(image_path, sizeof(image_path), "%s/%s", res_dir, filename);
	iconw = gtk_image_new_from_file(image_path);

	return iconw;
}

/*
 * On main view mouse button press
 * Signal handler for mouse clicks on main view
 */
gboolean on_main_view_mouse_button_press(GtkWidget *menu,
					GdkEvent *event, gpointer data)
{
	const gushort LEFT_CLICK = 1;
	const gushort MIDDLE_CLICK = 2;
	const gushort RIGHT_CLICK = 3;

	GtkTextView *text_view = NULL;
	book_data *book = NULL;

	GdkModifierType modifiers;
	modifiers = gtk_accelerator_get_default_mod_mask ();

	gtk_widget_grab_focus(GTK_WIDGET(data));
	if (event->type == GDK_BUTTON_PRESS) {

		GdkEventButton *bevent = (GdkEventButton*) event;

		if (bevent->button == LEFT_CLICK) {
			if((bevent->state & modifiers) == GDK_SHIFT_MASK) {
				sn_trace("Shift+Left Button Click in %s.", __func__);
				bevent->state ^= GDK_SHIFT_MASK;

				// Assert master exists
				g_assert_nonnull(master);

				// Get currently selected
				book = get_current_book_or_return_with_warning();

				// Trigger call to add_history in on_entry_change
				// after new entry selection
				book->add_history = TRUE;

				return FALSE;

			} else if((bevent->state & modifiers) == GDK_CONTROL_MASK) {
				sn_trace("Control+Left Button Click in %s.", __func__);
				bevent->state ^= GDK_CONTROL_MASK;
				return FALSE;
			}
			sn_trace("Left Button Click in %s.", __func__);
			return FALSE;

		} else if (bevent->button == MIDDLE_CLICK) {
			sn_trace("Middle Button Click in %s.", __func__);
			return FALSE;

		} else if (bevent->button == RIGHT_CLICK) {
			sn_trace("Right Button Click in %s.", __func__);

			// Show context popup menu
			gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
				bevent->button, bevent->time);
			return TRUE;
		}
		return FALSE;

	} else if (event->type == GDK_2BUTTON_PRESS) {
		GdkEventButton *bevent = (GdkEventButton*) event;

		if (bevent->button == LEFT_CLICK) {
			sn_trace("Left Double Click in %s.", __func__);

			// Assert master exists
			g_assert_nonnull(master);

			// Get currently selected
			book = get_current_book_or_return_with_warning();

			// Get text view
			text_view = get_text_view(book);

			// Set text view focus
			set_text_view_focus(text_view);

			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

/*
 * On main window delete
 */
gboolean on_main_window_delete(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	GList *book_item = NULL;
	book_data *book = NULL;

	sn_trace("Destroying main window [%#lx].", (ulong)main_window);

	// Write current entry
	write_current_entry();

	// Write books
	for(book_item = master->book_list;
	book_item != NULL; book_item = book_item->next) {
		book = (book_data*)book_item->data;
		write_book(book, note_dir);
	}

	// Write master
	write_master(master, note_dir);

	// Free master
	free_master(master);

	g_free(conf_dir);
	conf_dir = NULL;

	g_free(note_dir);
	note_dir = NULL;

	g_free(res_dir);
	res_dir = NULL;

	gtk_main_quit();
	return FALSE;
}
