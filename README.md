## Kelompok IT-04

* Maulida Meirisdiana 05311940000005
* Dava Aditya Jauhar 05311940000030
* Gerry Putra Fresnando 05311940000031

### Soal 1:
Di suatu jurusan, terdapat admin lab baru yang super duper gabut, ia bernama Sin. Sin baru menjadi admin di lab tersebut selama 1 bulan. Selama sebulan tersebut ia bertemu orang-orang hebat di lab tersebut, salah satunya yaitu Sei. Sei dan Sin akhirnya berteman baik. Karena belakangan ini sedang ramai tentang kasus keamanan data, mereka berniat membuat filesystem dengan metode encode yang mutakhir. Berikut adalah filesystem rancangan Sin dan Sei :
	
    NOTE : 
    Semua file yang berada pada direktori harus ter-encode menggunakan Atbash cipher(mirror).
    Misalkan terdapat file bernama kucinglucu123.jpg pada direktori DATA_PENTING
    “AtoZ_folder/DATA_PENTING/kucinglucu123.jpg” → “AtoZ_folder/WZGZ_KVMGRMT/pfxrmtofxf123.jpg”
    Note : filesystem berfungsi normal layaknya linux pada umumnya, Mount source (root) filesystem adalah directory /home/[USER]/Downloads, dalam penamaan file ‘/’ diabaikan,     dan ekstensi tidak perlu di-encode.
    Referensi : https://www.dcode.fr/atbash-cipher

a. Jika sebuah direktori dibuat dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
b. Jika sebuah direktori di-rename dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
c. Apabila direktori yang terenkripsi di-rename menjadi tidak ter-encode, maka isi direktori tersebut akan terdecode.
d. Setiap pembuatan direktori ter-encode (mkdir atau rename) akan tercatat ke sebuah log. Format : /home/[USER]/Downloads/[Nama Direktori] → /home/[USER]/Downloads/AtoZ_[Nama Direktori]
e. Metode encode pada suatu direktori juga berlaku terhadap direktori yang ada di dalamnya.(rekursif)

Penyelesaian:

#### Revisi
    void atbash(char *name) {
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) return;

        char *dot = strrchr(name, '.');
        char *atoz = strstr(name, "AtoZ_");
        int i;
        for (i = atoz - name; i < strlen(name); ++i) {
            if (name[i] == '/') {
                break;
            }
        }

        if (atoz == NULL) {
            i = 0;
        }

        int last = dot ? dot - name: strlen(name);
        for (; i < last; ++i) {
            if ('A' <= name[i] && name[i] <= 'Z') {
                name[i] = 155 - name[i];
            } else if ('a' <= name[i] && name[i] <= 'z') {
                name[i] = 219 - name[i];
            }
        }
    }

* Fungsi `atbash` merupakan sebuah metode enkripsi substitusi sederhana dengan cara membalikkan alfabet sehingga setiap huruf dipetakan ke huruf di posisi yang sama kebalikan dari abjad. Jika terdapat direktori bernama AtoZ_ atau direname menjadi AtoZ_ maka isinya akan terenkripsi.

      void rot13(char *name) {
          if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) return;

          int name_len = strlen(name);
          for (int i = 0; i < name_len; ++i) {
              if ('A' <= name[i] && name[i] <= 'M') {
                  name[i] = 13 + name[i];
              } else if ('N' <= name[i] && name[i] <= 'Z') {
                  name[i] = -13 + name[i];
              } else if ('a' <= name[i] && name[i] <= 'm') {
                  name[i] = 13 + name[i];
              } else if ('n' <= name[i] && name[i] <= 'z') {
                  name[i] = -13 + name[i];
              }
          }

          char *dot = strrchr(name, '.');
          for (int i = (int)(dot - name); i < name_len; ++i) {
              if ('A' <= name[i] && name[i] <= 'M') {
                  name[i] = 13 + name[i];
              } else if ('N' <= name[i] && name[i] <= 'Z') {
                  name[i] = -13 + name[i];
              } else if ('a' <= name[i] && name[i] <= 'm') {
                  name[i] = 13 + name[i];
              } else if ('n' <= name[i] && name[i] <= 'z') {
                  name[i] = -13 + name[i];
              }
          }
      }

* Fungsi `rot13` merupakan enkripsi sederhana yang menggunakan sandi abjad-tunggal dengan pergeseran k=13.

      void vigenere_enc(char *name) {
          char *key = "SISOP";

          if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) return;

          int n = 0;

          char *dot = strrchr(name, '.');
          for (int i = 0; i < (int)(dot - name); ++i) {
              if ('A' <= name[i] && name[i] <= 'Z') {
                  name[i] = 65 + (name[i] + key[n] - 130) % 26;
                  n = (n + 1) % 5;
              } else if ('a' <= name[i] && name[i] <= 'z') {
                  name[i] = 97 + (name[i] + key[n] - 162) % 26;
                  n = (n + 1) % 5;
              }
          }
      }

      void vigenere_dec(char *name) {
          char *key = "SISOP";

          if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) return;

          int n = 0;

          char *dot = strrchr(name, '.');
          for (int i = 0; i < (int)(dot - name); ++i) {
              if ('A' <= name[i] && name[i] <= 'Z') {
                  name[i] = 65 + (name[i] - key[n] + 26) % 26;
                  n = (n + 1) % 5;
              } else if ('a' <= name[i] && name[i] <= 'z') {
                  name[i] = 97 + (name[i] - key[n] - 6) % 26;
                  n = (n + 1) % 5;
              }
          }

      }

* Fungsi `vigenere` adalah metode menyandikan teks alfabet dengan menggunakan deretan sandi Caesar berdasarkan huruf-huruf pada kata kunci.

      void check_encryption(char *path, const char *fpath) {
          printf("check %s %s\n", path, fpath);
          if (strstr(fpath, "/AtoZ_") != NULL) {
              atbash(path);
          } else if (strstr(fpath, "/RX_") != NULL) {
              atbash(path);
              rot13(path);
          }
          printf("enc %s\n", path);
      }

* Fungsi `check_encryption` ini digunakan untuk memeriksa direktori. JIka direktori memiliki awalan `/AtoZ` maka akan digunakan enkripsi `atbash`. Jika direktori berawalan `/RX_` maka akan digunakan enkripsi `rot13` dan `atbash`.

### Soal 4:
Untuk memudahkan dalam memonitor kegiatan pada filesystem mereka Sin dan Sei membuat sebuah log system dengan spesifikasi sebagai berikut.
* Log system yang akan terbentuk bernama “SinSeiFS.log” pada direktori home pengguna (/home/[user]/SinSeiFS.log). Log system ini akan menyimpan daftar perintah system call yang telah dijalankan pada filesystem.
* Karena Sin dan Sei suka kerapian maka log yang dibuat akan dibagi menjadi dua level, yaitu INFO dan WARNING.
* Untuk log level WARNING, digunakan untuk mencatat syscall rmdir dan unlink.
* Sisanya, akan dicatat pada level INFO.
* Format untuk logging yaitu:

  [Level]::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC :: DESC]

Dengan penjelasan format sebagai berikut:
Level : Level logging, dd : 2 digit tanggal, mm : 2 digit bulan, yyyy : 4 digit tahun, HH : 2 digit jam (format 24 Jam),MM : 2 digit menit, SS : 2 digit detik, CMD : System Call yang terpanggil, DESC : informasi dan parameter tambahan

  INFO::28052021-10:00:00:CREATE::/test.txt
  INFO::28052021-10:01:00:RENAME::/test.txt::/rename.txt

Penyelasaian:
    
    static const char *dirpath = "/home/gerry";
    static const char *logpath = "/home/gerry/SinSeiFS.log";
    char log1[1200], *desc;
    
* Pertama, dibuat direktori mount source untuk filesystem pada `/home/gerry/`
* Selanjutnya berkas log system akan terbentuk bernama `SinSeiFS.log` yang akan diletakkan di direktori `/home/gerry/` yang berguna untuk menyimpan daftar perintah system call yang telah dijalankan.

          void makelog(char *level,char *desc)
      {
          FILE *fp = fopen(logpath,"a");
          time_t t = time(NULL);
          struct tm *tm = localtime(&t);

          char waktu[1000];
          strftime(waktu, 100, "%y%m%d-%H:%M:%S", tm);

          sprintf(log1, "%s::%s::%s\n", level, waktu, desc);
          fputs(log1, fp);
          memset(log1, 0, sizeof(log1));

          fclose(fp);
      }
      
 * Pembuatan berkas log system ini dibuat dengan menggunakan fungsi `makelog` yang nantinya akan dipanggil oleh fungsi-fungsi FUSE.
 * Fungsi makelog() adalah fungsi yang menuliskan log dengan format `[LEVEL]::[yy][mm][dd]-[HH]:[MM]:[SS]::[CMD]::[DESC]` di `/home/gerry/SinSeiFS.log`
 
         static struct fuse_operations xmp_oper = {
            .getattr = xmp_getattr,
            .readdir = xmp_readdir,
            .read = xmp_read,
            .mkdir = xmp_mkdir,
            .rmdir = xmp_rmdir,
            .rename = xmp_rename,
            .create = xmp_create,
            .unlink = xmp_unlink,
        };
      
* Di atas merupakan fungsi-fungsi yang digunakan pada FUSE.
* Log level WARNING, merupakan pencatatan log untuk syscall `rmdir` dan `unlink`. Sisanya, akan dicatat dengan level INFO.

      int main(int argc, char *argv[])
      {
        umask(0);
        return fuse_main(argc, argv, &xmp_oper, NULL);
      }

### Dokumentasi
1. Mount ke dir tes berisi file dari downloads
![image](https://user-images.githubusercontent.com/73152464/121812277-74648100-cc91-11eb-8c2b-027531951277.png)

2. Membuat dir AtoZ_halo dan didalamnya membuat dir test1 dan test2 yang ter encode menjadi gvhg1 dan gvhg2
![image](https://user-images.githubusercontent.com/73152464/121812340-aa096a00-cc91-11eb-91e2-0af6eaf050d8.png)
![image](https://user-images.githubusercontent.com/73152464/121812343-ad045a80-cc91-11eb-80d3-5979e2cfebd1.png)

3. Merename nama folder hai menjadi AtoZ_hai dan merename kembali menjadi hai
![image](https://user-images.githubusercontent.com/73152464/121812369-c4dbde80-cc91-11eb-89e4-8186139a74f7.png)
![image](https://user-images.githubusercontent.com/73152464/121812376-cad1bf80-cc91-11eb-8f6a-47e607fbeeb6.png)

4. Mencoba menghapus dir
![image](https://user-images.githubusercontent.com/73152464/121812478-12f0e200-cc92-11eb-8027-48089882316b.png)

5. Log
![image](https://user-images.githubusercontent.com/73152464/121812492-2308c180-cc92-11eb-8f48-41aabb7c969c.png)

