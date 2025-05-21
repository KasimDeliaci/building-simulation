# 🏗️ Apartman-Simülasyonu

## 2024-2025 İşletim Sistemleri Dönem Projesi

Bu proje, çok katlı bir apartman binasının inşa sürecini çoklu process ve thread'ler kullanarak eş zamanlı olarak simüle etmektedir.  
İnşa işlemleri sırasında vinç ve asansör gibi sınırlı kaynakların paylaşımı POSIX semafor ve mutex yapılarıyla kontrol edilmektedir.  
Projeye ayrıca bir görselleştirme sistemi entegre edilmiştir. Bu sistem ile inşaat süreci web tarayıcısı üzerinden 3D olarak adım adım izlenebilmektedir.

---

## 📁 Proje Dizini ve Yapısı

Proje aşağıdaki klasör yapısına sahiptir:

```
apartman-sim/
├── src/      → C kaynak ve header dosyaları (main, floor, resource, util)
├── web/      → Web tabanlı görselleştirme (HTML + JS + CSS)
├── scripts/  → Yardımcı scriptler (log2csv.py gibi)
├── build/    → Derleme çıktıları (otomatik oluşturulur)
├── Makefile  → Derleme için make dosyası
├── .gitignore → Git için gereksiz dosya filtreleme
└── README.md → Dökümantasyon
```

---

## 🔧 Derleme ve Çalıştırma (Linux ve macOS)

### Gerekli Kurulumlar

Bu projeyi derlemek ve çalıştırmak için aşağıdaki yazılımlar gereklidir:

- **GCC** veya **Clang** (C17 uyumlu bir derleyici)
- **make** (build yönetimi için)
- **Python 3** (log dosyasını işlemek için)

macOS kullanıyorsanız `xcode-select --install` komutu ile gerekli araçları yükleyebilirsiniz.  
Ubuntu veya WSL için:

```bash
sudo apt update
sudo apt install build-essential python3
```

### Projeyi Derlemek

Proje kök dizinine giderek terminalde aşağıdaki komutu çalıştırabilirsiniz:

```bash
make
```

🛠️ *Not*: Makefile, macOS ve Linux platformlarını otomatik olarak algılar.
clang ve gcc arasında fark varsa uygun şekilde -Wno-unused veya -pthread gibi parametrelerle desteklenir.

Derleme sonucunda build/sim adlı çalıştırılabilir dosya oluşacaktır.

### Simülasyonu Çalıştırmak ve Kayıt Etmek

Simülasyon çıktısını hem terminalde görmek hem de log dosyasına kaydetmek için:

```bash
./build/sim | tee run.log
```

Bu komut, tüm terminal çıktısını run.log adlı bir dosyada da saklar.

---

## ⛓️ Simülasyon Kuralları ve Kısıtlamalar

Bu bölüm, projenin en kritik bölümüdür. Aşağıda her işlem adımının sıralaması, kaynak kısıtları ve birbirine olan bağımlılıkları detaylı bir şekilde açıklanmıştır.

### 1. Katlar Arası Kısıtlar (Process Seviyesi)

- Bir katın taşıyıcı iskeleti ve dış duvarları tamamlanmadan, bir üst katın iskeleti başlamaz.
- Ancak aynı anda hem bir alt katın iç işlemleri hem de bir üst katın dış iskelet işlemleri yürütülebilir.

### 2. Daireler Arası Kısıtlar (Thread Seviyesi)

- Aynı kat üzerindeki 4 daire eş zamanlı olarak çalışabilir.
- Ancak elektrik-su hattı ve final kontrol işlemleri tek tek ve sırayla yapılmak zorundadır.
- Diğer işlemler (sıva, boya, zemin, mobilya) paralel şekilde çalışabilir.

### 3. Kaynak Kullanımı Kısıtları

| Aşama | Kaynak | Kısıt Türü |
|-------|--------|------------|
| Taşıyıcı iskelet + dış duvar | Vinç | Aynı anda sadece 1 iş yapılabilir |
| Fayans / Parke döşeme | Asansör | Aynı anda maksimum 3 iş yapılabilir |
| Mobilya taşıma | Asansör | Fayans / parke sonrası, aynı anda maksimum 3 iş yapılabilir |

### 4. Adım Sıralamaları ve Bağımlılıklar

Aşağıdaki işlemler belirli bir sıraya sahiptir ve bir adım tamamlanmadan sonraki başlatılamaz:

| Önce Yapılması Gereken | Sonra Başlayabilecek Adım | Açıklama |
|------------------------|---------------------------|----------|
| Elektrik-Su döşeme | Sıva | Altyapı tamamlanmadan iç işler başlayamaz |
| Sıva | Boya | Boya için düzgün yüzey gerekir |
| Boya | Fayans / Parke | Görsel tutarlılık için duvarlar önce bitirilir |
| Fayans / Parke | Mobilya yerleştirme | Mobilya zemine zarar vermemesi için sonra taşınır |
| Mobilya yerleştirme | Final kontrol | Daire tamamen hazır hale geldikten sonra kontrol yapılır |

---

## 🔄 Log Dosyasını CSV'ye Dönüştürme

Terminal çıktısı ANSI renk kodları içerdiği için doğrudan okunabilir değildir.
run.log dosyasını temizleyip web/run.csv dosyasına dönüştürmek için şu komutu kullanabilirsiniz:

```bash
python3 scripts/log2csv.py run.log web/run.csv
```

Bu işlem sonucunda, görselleştirme için uygun bir run.csv dosyası üretilecektir.

---

## 🌐 Web Görselleştirmeyi Başlatmak

1. Web sunucusunu başlatın:
   ```bash
   cd web
   python3 -m http.server 8000
   ```

2. Tarayıcıdan açın:
   ```
   http://localhost:8000
   ```

Web arayüzü şu özellikleri sağlar:
- Sol panel: Tüm binanın küp küp inşa edilişi
- Sağ panel: Aynı anda yapılmakta olan 4 işin bağımsız olarak gösterimi
- Alt bilgi: "Vinçte: ..." ve "Asansörde: ..." gibi detaylar

---

## 🎨 Görselleştirme Renk Kodları

| Aşama Türü | Renk | Açıklama |
|------------|------|----------|
| Taşıyıcı iskelet, dış duvar | Cyan | Vinç kullanımı |
| Elektrik-su, final kontrol | Sarı | Kat içi sıralı işlemler |
| Sıva, boya | Mavi | İç dekor işlemleri |
| Fayans, parke, mobilya | Mor | Zemin ve iç yerleşim |
| Tamamlanan adım | Yeşil | İş başarıyla bitmiştir |
| Henüz başlamamış | Gri | Beklemede |
