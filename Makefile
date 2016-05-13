#
#  Simple Notebook
#  Makefile
#
#  Copyright 2016 Roy Richardson <rocketx86@users.github.com>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#


TARGET   = notebook

CC       = gcc
CFLAGS   = -Wall -g -std=gnu11

LD       = gcc
LFLAGS   =

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin
RESDIR   = res
DOCDIR   = doc
TESTDIR  = test

BINDEST  = /usr/local/bin/simple-notebook
RESDEST  = /usr/local/share/pixmaps/simple-notebook

DEPENDS  := $(wildcard $(SRCDIR)/*.h)
SOURCES  := $(wildcard $(SRCDIR)/*.c)
PIXSRC   := $(wildcard $(RESDIR)/*.xpm)
PNGSRC   := $(wildcard $(RESDIR)/*.png)

OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
PIXFILZ  := $(PIXSRC:$(RESDIR)/%.xpm=$(RESDEST)/%.xpm)
PNGFILZ  := $(PNGSRC:$(RESDIR)/%.png=$(RESDEST)/%.png)

mkdir    = mkdir -p
rmd      = rm -rf
rm       = rm -f
cpd      = cp -Rf
cp       = cp -f


all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(mkdir) $(BINDIR)
	@$(LD) $(LFLAGS) -o $@ $(OBJECTS) `pkg-config --libs gtk+-2.0`
	@objdump -xdS $@ > $@.disasm
	@echo "Linking '$@' complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c $(DEPENDS)
	@$(mkdir) $(OBJDIR)
	@$(CC) $(CFLAGS) -c $< -o $@ `pkg-config --cflags gtk+-2.0`
	@echo "Compiled '$<' successfully!"

.PHONEY: clean
clean:
	@$(rmd) $(OBJDIR)
	@$(mkdir) $(OBJDIR)
	@$(rmd) $(BINDIR)
	@$(mkdir) $(BINDIR)
	@$(rmd) $(TESTDIR)
	@$(mkdir) $(TESTDIR)
	@$(cpd) $(DOCDIR)/* $(TESTDIR)
	@echo "Notebook cleaned!"

.PHONEY: user
user:
	@$(mkdir) $(HOME)/.simple-notebook
	@$(cpd) $(DOCDIR)/* $(HOME)/.simple-notebook
	@$(cp) ./simple-notebook.desktop $(HOME)/Desktop
	@echo "User initialized!"

.PHONEY: install
install:
	@$(mkdir) $(BINDEST)
	@$(cp) $(BINDIR)/$(TARGET) $(BINDEST)
	@$(mkdir) $(RESDEST)
	@$(cp) $(PIXSRC) $(RESDEST)
	@$(cp) $(PNGSRC) $(RESDEST)
	@echo "Notebook installed!"

.PHONEY: uninstall
uninstall:
	@$(rm) $(PIXFILZ) $(PNGFILZ)
	@$(rmd) $(RESDEST)
	@$(rm) $(BINDEST)/$(TARGET)
	@$(rmd) $(BINDEST)
	@echo "Notebook uninstalled!"
