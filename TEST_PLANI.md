# CSE 232 — 5. Kişi Test Planı
## Garbage Collection & main() Test Senaryoları

---

## 1. garbageCollection() — Birim Testler

### Test 1.1 — Temel GC (Silme Sonrası)
**Amaç:** Silinen satırların GC sonrası temizlenip temizlenmediğini doğrula.

**Adımlar:**
1. `E mytext.txt` → aaa, bbb, ccc, ddd yükle
2. Cursor bbb üzerinde → `D` (bbb sil)
3. `G` komutu ver
4. Buffer'ı kontrol et

**Beklenen:**
- textbuffer[0] = "aaa", textbuffer[1] = "ccc", textbuffer[2] = "ddd"
- head = 0, tail = 2, freeIdx = 3
- bbb'ye erişim YOK

---

### Test 1.2 — Çoklu Silme + GC
**Amaç:** Birden fazla silme sonrası GC'nin doğru çalışıp çalışmadığını test et.

**Adımlar:**
1. `E mytext.txt` → 4 satır yükle
2. bbb ve ccc'yi sil (2x D)
3. `G` komutu
4. Buffer içeriğini yazdır

**Beklenen:**
- Sadece aaa ve ddd kalmalı
- freeIdx = 2

---

### Test 1.3 — Boş Buffer'da GC
**Amaç:** Hiç satır yokken GC çökme yapmamalı.

**Adımlar:**
1. Editörü başlat (dosya açma)
2. Hemen `G` ver

**Beklenen:**
- "0 satır geri kazanıldı" mesajı
- head = -1, tail = -1, freeIdx = 0

---

### Test 1.4 — Tam Dolu Buffer + GC
**Amaç:** 100 satır dolu, hiç silme yok → GC hiçbir şey temizlememeli.

**Adımlar:**
1. 100 satır insert et (ya da 100 satırlık dosya aç)
2. Hiç silme yapma
3. `G` ver

**Beklenen:**
- "0 satır geri kazanıldı"
- Tüm satırlar yerinde

---

## 2. Otomatik GC Testleri

### Test 2.1 — 10 İşlemde Otomatik Tetiklenme
**Amaç:** 10 insert/delete sonrası GC otomatik başlamalı.

**Adımlar:**
1. Dosya aç
2. Arka arkaya 10 insert yap
3. 10. işlemde ekranda "Otomatik GC" mesajı çıkmalı

**Beklenen:**
- op_count 10'a ulaştığında garbageCollection() çağrılır
- op_count sıfırlanır

---

### Test 2.2 — Buffer Dolunca Otomatik GC
**Amaç:** freeIdx >= 100 olunca GC tetiklenmeli.

**Adımlar:**
1. 95 satırlık dosya aç
2. 5 satır daha insert et (buffer doluyor)
3. 1 tane daha insert dene

**Beklenen:**
- GC otomatik çalışır
- Eğer gerçekten silinmiş satır yoksa "yer yok" uyarısı

---

## 3. main() Döngüsü Testleri

### Test 3.1 — E Komutu
**Adımlar:** `E mytext.txt` gir
**Beklenen:** Dosya yüklenir, ekranda satırlar görünür

---

### Test 3.2 — Geçersiz Komut
**Adımlar:** `X` gir (geçersiz)
**Beklenen:** "Bilinmeyen komut" uyarısı, program çökmez

---

### Test 3.3 — Q ile Çıkış
**Adımlar:** `Q` → Onay sorusu → `E` (evet)
**Beklenen:** Program temiz kapanır, terminal normale döner

---

### Test 3.4 — S Komutu
**Adımlar:**
1. Dosya aç, bir satır ekle
2. `S` ver
3. Dosyayı `cat` ile kontrol et

**Beklenen:** Yeni satır dosyada görünür

---

## 4. Entegrasyon Testleri

### Test 4.1 — Proje PDF'indeki Örnek
**Adımlar (tam örnek):**
1. `E mytext.txt` → aaa, bbb, ccc, ddd
2. Cursor ccc'nin üstüne git → `I` → "xxx" yaz
3. Cursor bbb'ye git → `D`
4. Cursor xxx'in "x"'ine git → `R` → "y" yaz
5. `G` ver
6. `P` ile son hali kontrol et

**Beklenen son hâl:**
```
aaa
ccc
xxy
ddd
```
Buffer: textbuffer[0]=aaa, [1]=ccc, [2]=ddd, [3]=xxy → GC sonrası [2]=xxy

---

## 5. Hata Durumu Testleri

| Senaryo | Beklenen Davranış |
|---|---|
| Olmayan dosya aç | Hata mesajı, çökmez |
| Boş buffer'da D | "Silinecek satır yok" uyarısı |
| 100 dolu buffer'da I + GC sonrası hâlâ yer yoksa | "Maksimum satır" uyarısı |
| head = -1 iken P | Boş ekran, çökmez |

---

## 6. Derleme ve Çalıştırma

```bash
# Tüm .c dosyaları hazırsa:
make

# Sadece gc_main.c test etmek için (stub'larla):
gcc -Wall -g -o test gc_main.c stubs.c -lncurses

# Çalıştır:
./texteditor
```

---

## 7. Debug İpuçları

- **Segfault alıyorsan:** `gdb ./texteditor` → `run` → `bt` (backtrace)
- **Sonsuz döngü:** Ctrl+C, GDB ile `info locals` ile next/prev linklerini kontrol et
- **ncurses terminal bozuldu:** `reset` komutunu terminale yaz

---

## 8. Ekibe Teslim Listesi

- [ ] `editor.h` → TÜM EKIBE DAĞIT (ortak header)
- [ ] `gc_main.c` → Garbage collection + main loop
- [ ] `Makefile` → Derleme için
- [ ] `mytext.txt`, `test_code.txt` → Test dosyaları
- [ ] Bu test planı belgesi
