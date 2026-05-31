# ============================================================
# Makefile — CSE 232 Text Editor
# Kullanım: make          → derle
#           make clean    → .o ve binary'yi sil
#           make run      → derle ve çalıştır
# ============================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -g
LIBS    = -lncurses
TARGET  = texteditor

# Ekipteki herkesin .c dosyası buraya eklenir
SRCS    = gc_main.c      \
          editor_core.c  \
          ui_ncurses.c   \
          insert_delete.c \
          replace_print.c

OBJS    = $(SRCS:.c=.o)

# Varsayılan hedef: derle
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "Derleme basarili! './$(TARGET)' ile calistir."

%.o: %.c editor.h
	$(CC) $(CFLAGS) -c $< -o $@

# Çalıştır
run: all
	./$(TARGET)

# Temizle
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "Temizlendi."

.PHONY: all run clean
