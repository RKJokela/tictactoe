# TicTacToe by Randy Jokela
# Makefile for Visual C++ nmake tool

TARGET=TicTacToe.exe
OBJ=main.obj
LIBS=user32.lib advapi32.lib lib\pdcurses.lib
CFLAGS=/c /nologo /Iinclude

all: $(TARGET)

$(TARGET): $(OBJ)
	link /nologo /out:$(TARGET) $(OBJ) $(LIBS)
	
.cpp.obj:
	cl $(CFLAGS) $<

clean: 
	del *.obj *.exe

rebuild: clean all
