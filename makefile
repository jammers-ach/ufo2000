##############################################################################
# Compiling ufo2000: make {win32=1} {debug=1}                                #
#                                                                            #
# Define win32=1 when compiling with Mingw32 gcc compiler for windows        #
# Define debug=1 when you want to build debug version of ufo2000             #
#                                                                            #
# Running just make builds the release version of ufo2000 for *nix           #
# (Linux, FreeBSD, ...)                                                      #
#                                                                            #
# The game depends on Allegro (4.0.x) and Expat library, so you need to      #
# install them before running make                                           #
#                                                                            #
# Also it is highly recommended that you have subversion client installed    #
# ???                                                                        #
##############################################################################

CC = g++
LD = g++
UFO_SVNVERSION := ${shell svnversion .}
CFLAGS = -funsigned-char -Wall
CFLAGS += -pipe -DUFO_SVNVERSION=\"$(UFO_SVNVERSION)\"
OBJDIR = obj
NAME = ufo2000
SERVER_NAME = ufo2000-srv

VPATH = src src/jpgalleg

SRCS = bullet.cpp cell.cpp config.cpp connect.cpp dirty.cpp           \
       editor.cpp explo.cpp font.cpp icon.cpp inventory.cpp item.cpp  \
       keys.cpp main.cpp mainmenu.cpp map.cpp map_pathfind.cpp        \
       multiplay.cpp netsock.cpp packet.cpp pck.cpp place.cpp         \
       platoon.cpp soldier.cpp sound.cpp spk.cpp terrapck.cpp         \
       units.cpp video.cpp wind.cpp word.cpp crc32.cpp persist.cpp    \
       jpeg.cpp pfxopen.cpp minimap.cpp about.cpp stats.cpp           \
       server_protocol.cpp server_transport.cpp server_gui.cpp        \
       server_config.cpp

SRCS_SERVER = server_main.cpp server_protocol.cpp \
       server_transport.cpp server_config.cpp

ifdef debug
	CFLAGS += -g
	OBJDIR := ${addsuffix -debug,$(OBJDIR)}
	NAME := ${addsuffix -debug,$(NAME)}
	SERVER_NAME := ${addsuffix -debug,$(SERVER_NAME)}
else
	CFLAGS += -O -mcpu=i686
endif

ifdef win32
	OBJDIR := ${addsuffix -win32,$(OBJDIR)}
	NAME := ${addsuffix .exe,$(NAME)}
	SERVER_NAME := ${addsuffix .exe,$(SERVER_NAME)}
	CFLAGS += -DWIN32
	LIBS = -lexpat -lNL -lalleg -lws2_32
else
	CFLAGS += -DLINUX
	INCLUDES = ${shell allegro-config --cflags}
	CFLAGS += $(INCLUDES)
	LIBS += -lexpat -lNL ${shell allegro-config --libs}
endif

OBJS = $(addprefix $(OBJDIR)/,$(SRCS:.cpp=.o))
DEPS = $(addprefix $(OBJDIR)/,$(SRCS:.cpp=.d))
OBJS_SERVER = $(addprefix $(OBJDIR)/,$(SRCS_SERVER:.cpp=.o))
DEPS_SERVER = $(addprefix $(OBJDIR)/,$(SRCS_SERVER:.cpp=.d))

ifdef win32
	OBJS := $(OBJS) Seccast.o 
ifndef debug
	SUBSYSTEM := -Wl,--subsystem=windows -e _WinMainCRTStartup
	CFLAGS += -s
endif
endif

##############################################################################

all: $(OBJDIR) $(NAME)

server: $(OBJDIR) $(SERVER_NAME)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	$(CC) -MMD $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(LD) $(CFLAGS) -o $@ $^ $(LIBS) $(SUBSYSTEM)

$(SERVER_NAME): $(OBJS_SERVER)
	$(LD) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	$(RM) $(OBJDIR)/*.o
	$(RM) $(OBJDIR)/*.d
	$(RM) $(NAME)

source-zip:
# create zip archive with ufo2000 sources, requires 7-zip archiver
	-$(RM) ufo2000-r$(UFO_SVNVERSION)-src.zip
	svn export . ufo2000-r$(UFO_SVNVERSION)-src
	7z a -tzip -r -mx ufo2000-r$(UFO_SVNVERSION)-src.zip "ufo2000-r$(UFO_SVNVERSION)-src/*"
	svn delete --force ufo2000-r$(UFO_SVNVERSION)-src

source-bz2:
# create tar.bz2 archive with ufo2000 sources (on *nix systems)
	-$(RM) ufo2000-r$(UFO_SVNVERSION)-src.tar.bz2
	svn export . ufo2000-r$(UFO_SVNVERSION)-src
	tar -cjf ufo2000-r$(UFO_SVNVERSION)-src.tar.bz2 ufo2000-r$(UFO_SVNVERSION)-src
	svn delete --force ufo2000-r$(UFO_SVNVERSION)-src

-include $(DEPS)
-include $(DEPS_SERVER)