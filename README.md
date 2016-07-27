# Simple Notebook

## Introduction

Simple Notebook is a note taking application that makes it easy for you to organize and reference your personal information. Simple Notebook is designed to be as easy to use as your old paper notebook. It's simple software for managing all kinds of notes, lists, research, contacts, and more.

Simple Notebook is designed to take notes quickly and has full keyboard command support. Simple Notebook enables you to create entries quickly and then jump back to the previous note exactly where you left off. It's ideal for the creative authoring process.

Here are the key features:

* Clean and simple user interface
* Full keyboard command control
* Efficient section and entry organization
* Unlimited number of notebooks, sections or entries
* Quick history and bookmark navigation
* Search for sections and entries
* Find and replace text in entries
* Print notebook, sections and entries

Simple Notebook allows you to organize your notes into multiple notebooks, where each notebook can be used to organize multiple sections, and each section can be used to organize multiple entries. The open notebooks are displayed in the tab bar along the top of the work area. The active notebook's sections are displayed in the list on the upper left and the entries within the selected section are displayed in the list on the lower left. The text of the selected entry is displayed in the text pane on the right. Sections and entries can be copied and moved across open notebooks and sections.

Simple Notebook data files are portable because they are plain text files. Your notes can easily be opened by any other software product capable of processing text files. The notebooks and sections are stored as file folders, and entries are stored as text files. The order of sections and entries is maintained in a separate index file in each notebook folder. There is no information stored in the section folders or the text files themselves. All changes to your notebook are saved automatically to disk so there is little chance of your changes being lost.

## Installation

**Source**  
Download the source code for project and create the sub-directory simple-notebook.
```
	$ git clone https://github.com/rocketx86/simple-notebook
```

**Dependencies**  
Install build dependencies from your GNU/Linux distribution's repository.
```
	$ sudo apt install libgtk2.0-dev
	$ sudo apt install libgmime-2.6-dev
```

**Build**  
Change to sub-directory, clean test data, and compile and link the application. 
```
	$ cd simple-notebook  
	$ make clean  
	$ make all  
```

**Test**  
Run the application with local test notebook and resources.
```
	$ bin/simple-notebook -d ./test -r ./res
```

**Install**  
Install the application into /usr/bin and resources into /usr/share.
```
	$ sudo make install
```

**Init User**  
Create default user notebook in $(HOME)/.simple-notebook and desktop launcher.
```
	$ make user
```

**Run**  
Run from the installation directory opening default user notebook.
```
	$ /usr/bin/simple-notebook
```
Run from the installation directory opening custom notebook.
```
	$ /usr/bin/simple-notebook -d /path/to/notebook
```

**Uninstall**  
Uninstall the application from /usr/bin and /usr/share.
```
	$ sudo make uninstall
```
_Does not remove user notebook or desktop launcher._

## License

Simple Notebook is distributed as free software under the GNU General Public License. The software is written in the C programming language using the GNU C toolchain and the GNU GLIB and GTK+ 2.0 libraries. It should run well on any GNU/Linux based OS like Debian. You can redistribute it and/or modify this software under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
