/*
 * gc_main.c  —  Kişi 5: Garbage Collection + main() döngüsü
 *
 * GÖREV ÖZETI:
 *   1. garbageCollection(): Silinmiş (erişilemeyen) satırları textbuffer'dan
 *      temizle, geçerli satırları başa kaydır.
 *   2. run_editor(): Kullanıcının komut girdiği ana döngü.
 *   3. main(): Programı başlat, run_editor()'ı çağır, kapat.
 */

#include "editor.h"

/* ============================================================
 * GLOBAL TANIMLAR (extern'ler editor.h'de bildirildi)
 * ============================================================ */
struct node textbuffer[MAX_LINES];
int head    = -1;   /* Henüz hiç satır yok */
int tail    = -1;
int freeIdx =  0;   /* Buffer'ın başı boş */
int op_count=  0;   /* İşlem sayacı        */
char current_file[256] = {0};


/* ============================================================
 * GARBAGE COLLECTION
 * ============================================================
 *
 * NEDEN GEREKLİ?
 *   delete_line() bir satırı gerçekten silmez; sadece o satıra
 *   giden linkleri koparır. Dolayısıyla textbuffer[] içinde
 *   "ölü" (erişilemeyen) hücreler birikir. Buffer dolunca yeni
 *   satır ekleyemeyiz — GC bu ölü hücreleri temizler.
 *
 * NASIL ÇALIŞIR? (Adım adım)
 *   Adım 1 : Geçici bir dizi (temp[]) oluştur.
 *   Adım 2 : head'den başlayarak link zincirini izle,
 *            geçerli satırları sırayla temp[]'e kopyala.
 *   Adım 3 : temp[]'i textbuffer[]'a geri yaz.
 *   Adım 4 : head, tail, freeIdx ve tüm next/prev linklerini
 *            yeni konumlara göre güncelle.
 *
 * DÖNÜŞ DEĞERİ:
 *   Temizlenen (geri kazanılan) satır sayısı.
 * ============================================================ */
int garbageCollection(void)
{
    /* Geçici depolama — sadece geçerli satırları tutacak */
    struct node temp[MAX_LINES];
    int         valid_count = 0;   /* Kaç geçerli satır bulduk */
    int         freed       = 0;   /* Kaç satır geri kazanıldı */

    /* ---- Adım 1: Geçerli satırları link zincirini izleyerek topla ---- */
    int cur = head;
    while (cur != -1) {
        temp[valid_count] = textbuffer[cur];   /* içeriği kopyala */
        valid_count++;
        cur = textbuffer[cur].next;
    }

    /* Kaç satır geri kazanıldı = eskiden kullanılan alan - şimdi geçerli olan */
    freed = freeIdx - valid_count;
    if (freed < 0) freed = 0;

    /* ---- Adım 2: textbuffer[]'ı temizle ---- */
    for (int i = 0; i < MAX_LINES; i++) {
        memset(textbuffer[i].statement, 0, MAX_CHARS);
        textbuffer[i].next = -1;
        textbuffer[i].prev = -1;
    }

    /* ---- Adım 3: Geçerli satırları sıfırdan başlayarak geri yaz ---- */
    for (int i = 0; i < valid_count; i++) {
        strncpy(textbuffer[i].statement,
                temp[i].statement,
                MAX_CHARS - 1);
        textbuffer[i].statement[MAX_CHARS - 1] = '\0';

        /* prev/next bağlantılarını yeni konumlarına göre kur */
        textbuffer[i].prev = (i > 0)               ? (i - 1) : -1;
        textbuffer[i].next = (i < valid_count - 1) ? (i + 1) : -1;
    }

    /* ---- Adım 4: Global işaretçileri güncelle ---- */
    if (valid_count == 0) {
        head    = -1;
        tail    = -1;
        freeIdx =  0;
    } else {
        head    = 0;
        tail    = valid_count - 1;
        freeIdx = valid_count;   /* Boş alan buradan başlıyor */
    }

    /* İşlem sayacını sıfırla (GC yeni döngü başlıyor) */
    op_count = 0;

    return freed;
}


/* ============================================================
 * YARDIMCI: GC gerekli mi kontrol et, gerekiyorsa çalıştır
 * Bu fonksiyonu insert/delete her çağrıldığında çağırırız.
 * ============================================================ */
static void maybe_gc(void)
{
    /* Buffer tamamen dolduysa VEYA 10 işlem gerçekleştiyse GC başlat */
    if (freeIdx >= MAX_LINES || op_count >= GC_THRESHOLD) {
        int freed = garbageCollection();

        /* Kullanıcıya bilgi ver (ncurses penceresi açıksa) */
        if (freed > 0) {
            mvprintw(LINES - 2, 0,
                     "[ Otomatik GC: %d satır geri kazanıldı ]   ",
                     freed);
            refresh();
            napms(1200);   /* 1.2 saniye göster */
        }
    }
}


/* ============================================================
 * ANA EDITÖR DÖNGÜSÜ
 * ============================================================
 *
 * NASIL ÇALIŞIR?
 *   - Ekran ncurses ile başlatılır.
 *   - Sonsuz bir döngüde kullanıcıdan komut beklenir.
 *   - Girilen komuta göre ilgili fonksiyon çağrılır.
 *   - Q komutuyla döngü kırılır ve program kapanır.
 *
 * KOMUTLAR:
 *   E <dosya>   → edit()
 *   P           → print_text()
 *   I           → insert_line( cursorLine() )
 *   D           → delete_line( cursorLine() )
 *   R           → replace_char( cursorChar(), cursorLine() )
 *   S           → save()
 *   G           → garbageCollection()
 *   Q           → çıkış
 * ============================================================ */
void run_editor(void)
{
    char cmd_buf[300];   /* Kullanıcının girdiği komut satırı */
    int  ch;             /* Basılan tuş */

    /* ncurses başlat (kişi 2'nin fonksiyonu) */
    init_ui();

    /* İlk ekranı çiz */
    print_text();

    /* ---------- ANA DÖNGÜ ---------- */
    while (1) {

        /* Komut satırını göster */
        move(LINES - 1, 0);
        clrtoeol();
        printw("> ");
        refresh();

        /* Komut oku (ncurses'in getch yerine satır okuyalım) */
        echo();
        getnstr(cmd_buf, sizeof(cmd_buf) - 1);
        noecho();

        /* Boş komut → yok say */
        if (cmd_buf[0] == '\0') continue;

        /* Büyük harfe çevir (e ve E aynı çalışsın) */
        char cmd = (char)toupper((unsigned char)cmd_buf[0]);

        /* ------ KOMUT AYIRT ------ */
        switch (cmd) {

        /* ---- E: Dosya Aç ---- */
        case 'E': {
            char filename[256] = {0};
            /* "E dosyaadi" formatından dosya adını al */
            if (sscanf(cmd_buf + 1, " %255s", filename) == 1) {
                edit(filename);
                strncpy(current_file, filename, 255);
            } else {
                /* Dosya adı verilmemişse sor */
                move(LINES - 1, 0);
                clrtoeol();
                printw("Dosya adi: ");
                echo();
                getnstr(filename, 255);
                noecho();
                if (filename[0] != '\0') {
                    edit(filename);
                    strncpy(current_file, filename, 255);
                }
            }
            print_text();
            break;
        }

        /* ---- P: Ekrana Yaz ---- */
        case 'P':
            print_text();
            break;

        /* ---- I: Satır Ekle ---- */
        case 'I': {
            int idx = cursorLine();
            insert_line(idx);
            op_count++;          /* İşlem sayacını artır */
            maybe_gc();          /* Gerekirse otomatik GC */
            print_text();
            break;
        }

        /* ---- D: Satır Sil ---- */
        case 'D': {
            int idx = cursorLine();
            delete_line(idx);
            op_count++;
            maybe_gc();
            print_text();
            break;
        }

        /* ---- R: Karakter Değiştir ---- */
        case 'R': {
            int line_idx = cursorLine();
            int char_idx = cursorChar();
            replace_char(char_idx, line_idx);
            print_text();
            break;
        }

        /* ---- S: Kaydet ---- */
        case 'S':
            save();
            move(LINES - 2, 0);
            clrtoeol();
            printw("[ Dosya kaydedildi: %s ]", current_file);
            refresh();
            napms(1000);
            break;

        /* ---- G: Manuel Garbage Collection ---- */
        case 'G': {
            int freed = garbageCollection();
            move(LINES - 2, 0);
            clrtoeol();
            printw("[ GC tamamlandi: %d satir geri kazanildi ]", freed);
            refresh();
            napms(1200);
            print_text();
            break;
        }

        /* ---- Q: Çıkış ---- */
        case 'Q':
            /* Kaydedilmemiş değişiklik uyarısı */
            move(LINES - 1, 0);
            clrtoeol();
            printw("Kaydedilmeden cikmak istiyor musunuz? (E/H): ");
            refresh();
            ch = getch();
            if (ch == 'E' || ch == 'e') {
                close_ui();
                return;   /* Döngüden çık */
            }
            break;

        default:
            move(LINES - 2, 0);
            clrtoeol();
            printw("[ Bilinmeyen komut: '%c' — E/I/D/R/P/S/G/Q ]", cmd);
            refresh();
            napms(900);
            break;
        }
    }
}


/* ============================================================
 * MAIN — Programın Giriş Noktası
 * ============================================================ */
int main(int argc, char *argv[])
{
    /* Eğer komut satırından dosya adı verilmişse hemen aç */
    if (argc == 2) {
        /* editor.h'deki run_editor başlamadan önce edit çağrılamaz
         * (ncurses henüz başlamadı). Sadece dosya adını saklayalım. */
        strncpy(current_file, argv[1], 255);
    }

    /* Ana döngüyü başlat */
    run_editor();

    printf("Cikis yapildi.\n");
    return 0;
}
