## Kelompok IT-04

* Maulida Meirisdiana 05311940000005
* Dava Aditya Jauhar 05311940000030
* Gerry Putra Fresnando 05311940000031

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
