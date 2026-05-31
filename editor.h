#ifndef EDITOR_H
#define EDITOR_H

#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// ============================================================
// VERİ YAPISI — Tüm ekip bu yapıyı kullanır
// ============================================================

#define MAX_LINES     100   // textbuffer'ın maksimum satır sayısı
#define MAX_CHARS      40   // Her satırda maksimum karakter sayısı
#define MAX_VISIBLE    30   // Ekranda aynı anda gösterilebilecek satır sayısı
#define GC_THRESHOLD   10   // Kaç insert/delete sonrası otomatik GC çalışsın

struct node {
    char statement[MAX_CHARS]; // Satırın içeriği
    int  next;                 // Bir sonraki geçerli satırın textbuffer[] indeksi (-1 = yok)
    int  prev;                 // Bir önceki geçerli satırın textbuffer[] indeksi (-1 = yok)
};

// ============================================================
// GLOBAL DEĞİŞKENLER — Tüm .c dosyaları bu extern'leri kullanır
// ============================================================
extern struct node textbuffer[MAX_LINES];
extern int head;          // İlk geçerli satırın indeksi (-1 = boş)
extern int tail;          // Son geçerli satırın indeksi  (-1 = boş)
extern int freeIdx;       // Buffer'ın sonundaki ilk boş yer
extern int op_count;      // Insert/delete işlem sayacı (GC için)
extern char current_file[256]; // Şu an açık dosyanın adı

// ============================================================
// FONKSİYON PROTOTOGLARI
// ============================================================

// Kişi 1
void edit(char *filename);
void save(void);

// Kişi 2
void init_ui(void);
void close_ui(void);
int  cursorLine(void);
int  cursorChar(void);

// Kişi 3
void insert_line(int index);
void delete_line(int index);

// Kişi 4
void replace_char(int char_index, int line_index);
void print_text(void);

// Kişi 5 (Sen!)
int  garbageCollection(void);
void run_editor(void);

#endif // EDITOR_H
