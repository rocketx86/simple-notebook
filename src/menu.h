/*
 * Simple Notebook
 * menu.h
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

#ifndef __SN_MENU_H__
#define __SN_MENU_H__

extern GtkWidget *bookMenu;
extern GtkWidget *sectionMenu;
extern GtkWidget *entryMenu;
extern GtkWidget *textMenu;

extern GtkWidget *bookBookMi;
extern GtkWidget *createBookMi;
extern GtkWidget *renameBookMi;
extern GtkWidget *openBookMi;
extern GtkWidget *closeBookMi;
extern GtkWidget *deleteBookMi;
extern GtkWidget *sep01Mi;
extern GtkWidget *viewHistoryMi;
extern GtkWidget *clearHistoryMi;
extern GtkWidget *viewBookmarksMi;
extern GtkWidget *clearBookmarksMi;
extern GtkWidget *sep02Mi;
extern GtkWidget *searchBookMi;
extern GtkWidget *printBookMi;
extern GtkWidget *sep03Mi;
extern GtkWidget *exitBookMi;

extern GtkWidget *sectionSectionMi;
extern GtkWidget *createSectionMi;
extern GtkWidget *renameSectionMi;
extern GtkWidget *copySectionMi;
extern GtkWidget *moveSectionMi;
extern GtkWidget *cleanTrashMi;
extern GtkWidget *deleteSectionMi;
extern GtkWidget *sep04Mi;
extern GtkWidget *searchSectionMi;
extern GtkWidget *printSectionMi;

extern GtkWidget *entryEntryMi;
extern GtkWidget *createEntryMi;
extern GtkWidget *renameEntryMi;
extern GtkWidget *copyEntryMi;
extern GtkWidget *moveEntryMi;
extern GtkWidget *trashEntryMi;
extern GtkWidget *deleteEntryMi;
extern GtkWidget *sep05Mi;
extern GtkWidget *backHistoryMi;
extern GtkWidget *forwardHistoryMi;
extern GtkWidget *bookmarkEntryMi;
extern GtkWidget *sep06Mi;
extern GtkWidget *searchEntryMi;
extern GtkWidget *printEntryMi;

extern GtkWidget *textTextMi;
extern GtkWidget *replaceTextMi;
extern GtkWidget *undoTextMi;
extern GtkWidget *redoTextMi;
extern GtkWidget *selectTextMi;
extern GtkWidget *cutTextMi;
extern GtkWidget *copyTextMi;
extern GtkWidget *pasteTextMi;
extern GtkWidget *deleteTextMi;

extern GtkWidget* create_main_menu (void);

#endif //__SN_MENU_H__
