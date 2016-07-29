/*
 * Simple Notebook
 * common.h
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

#ifndef __SN_COMMON_H__
#define __SN_COMMON_H__

#define GTK_DISABLE_DEPRECATED

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gmime/gmime.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#define MIN_WIDTH 500
#define MIN_HEIGHT 400
#define MAIN_WIDTH 1080
#define MAIN_HEIGHT 700
#define SM_DLG_WIDTH 300
#define SM_DLG_HEIGHT 100

#define MAX_NAME_LEN 256
#define MAX_PATH_LEN 1024

#define MAX_TRIES 1000

#define END_OF_LIST -1
#define NEW_INSTANCE 1
#define ILLEGAL_CHARS "/\\:*?\"><|"

#define SN_TRACE_ON TRUE

#define sn_trace0(msg) \
	do { if (SN_TRACE_ON) g_message("%s:%d: " msg, \
		__FILE__, __LINE__); } while (0)

#define sn_trace(fmt, ...) \
	do { if (SN_TRACE_ON) g_message("%s:%d: " fmt, \
		__FILE__, __LINE__, __VA_ARGS__); } while (0)

#define sn_warning0(msg) \
	do { g_warning("%s:%d: " msg, \
		__FILE__, __LINE__); } while (0)

#define sn_warning(fmt, ...) \
	do { g_warning("%s:%d: " fmt, \
		__FILE__, __LINE__, __VA_ARGS__); } while (0)

#define sn_error0(msg, ...) \
	do { g_error("%s:%d: " msg, \
		__FILE__, __LINE__); } while (0)

#define sn_error(fmt, ...) \
	do { g_error("%s:%d: " fmt, \
		__FILE__, __LINE__, __VA_ARGS__); } while (0)

#include "main.h"
#include "master.h"
#include "book.h"
#include "section.h"
#include "entry.h"
#include "history.h"
#include "text.h"
#include "page.h"
#include "menu.h"
#include "toolbar.h"
#include "edit.h"

#endif //__SN_COMMON_H__
