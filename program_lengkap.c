#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <time.h> 

// =================================================================
// 1. STRUKTUR DATA (STRUCT)
// =================================================================
struct Barang {
    int kode;
    char nama[50]; 
    int harga;
    int stok; 
    int qty;        // Digunakan untuk keranjang belanja kasir
};

struct User {
    int id_user;
    char nama_user[30];
    char peran[10]; 
};

// Database User Statis
struct User database_user[] = {
    {101, "Raya", "Admin"},
    {201, "Rayyan", "Kasir"}, 
    {202, "Habib", "Kasir"},
    {301, "Fifah", "Manager"}
};
int total_user = sizeof(database_user) / sizeof(database_user[0]);

char user_aktif[30] = "";
char peran_aktif[10] = ""; 

// =================================================================
// 2. FUNGSI LOGIKA DASAR (SESUAI PROGRAM AWAL)
// =================================================================

int sama(char a[], char b[]) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}

void update_file_barang(struct Barang *list, int count) {
    FILE *file = fopen("database_supermarket-UAS.txt", "w");
    if (file == NULL) return;
    for (int i = 0; i < count; i++) {
        fprintf(file, "%d %s %d %d\n", list[i].kode, list[i].nama, list[i].harga, list[i].stok);
    }
    fclose(file);
}

void catat_log_admin(char *admin, int kode, char *nama, char *tipe, int lama, int baru) {
    FILE *file = fopen("admin_log-UAS.txt", "a");
    if (file == NULL) return;
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char s[64];
    strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", tm);
    fprintf(file, "[%s] Admin: %s | %s (%d) | %s: %d -> %d\n", s, admin, nama, kode, tipe, lama, baru);
    fclose(file);
}

void catat_transaksi(char *kasir, int total, int uang, int kembali, struct Barang *list, int count) {
    FILE *file = fopen("laporan_penjualan-UAS.txt", "a"); 
    if (file == NULL) return;
    time_t t = time(NULL);
    char s[64];
    strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", localtime(&t)); 
    fprintf(file, "=====================================\n");
    fprintf(file, "Waktu: %s | Kasir: %s\n", s, kasir); 
    fprintf(file, "Total: %d | Bayar: %d | Kembali: %d\n", total, uang, kembali);
    for (int i = 0; i < count; i++) {
        if (list[i].qty > 0) fprintf(file, "%d %s x%d = %d\n", list[i].kode, list[i].nama, list[i].qty, list[i].qty * list[i].harga);
    }
    fprintf(file, "=====================================\n\n");
    fclose(file);
}

// =================================================================
// 3. FITUR TAMPILAN & CARI
// =================================================================

void tampilkan_semua_produk(struct Barang *list, int count) {
    printf("\n%-5s | %-20s | %-10s | %-5s\n", "KODE", "NAMA BARANG", "HARGA", "STOK");
    printf("----------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-5d | %-20s | Rp %-7d | %-5d\n", list[i].kode, list[i].nama, list[i].harga, list[i].stok);
    }
}

void fitur_cari(struct Barang *list, int count) {
    char cari[50]; int ketemu = 0;
    printf("\nMasukkan Kode/Nama Barang: "); scanf("%s", cari);
    for (int i = 0; i < count; i++) {
        if (list[i].kode == atoi(cari) || sama(list[i].nama, cari)) {
            printf("HASIL -> Kode: %d | Nama: %-15s | Harga: %d | Stok: %d\n", list[i].kode, list[i].nama, list[i].harga, list[i].stok);
            ketemu = 1;
        }
    }
    if (!ketemu) printf("Barang tidak ditemukan.\n");
}

void laporan_aggregat(struct Barang *list, int count) {
    FILE *file = fopen("laporan_penjualan-UAS.txt", "r");
    if (file == NULL) { printf("Belum ada transaksi.\n"); return; }
    int t_qty[100] = {0}, t_duit[100] = {0}, omzet = 0;
    char line[256], n_item[50]; int k, q, s;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d %s x%d = %d", &k, n_item, &q, &s) == 4) {
            for (int i = 0; i < count; i++) if (list[i].kode == k) { t_qty[i]+=q; t_duit[i]+=s; omzet+=s; }
        }
    }
    fclose(file);
    printf("\n--- LAPORAN AGGREGAT (Omzet: Rp %d) ---\n", omzet);
    for (int i = 0; i < count; i++) if (t_qty[i] > 0) printf("%-15s | Terjual: %d | Total: Rp %d\n", list[i].nama, t_qty[i], t_duit[i]);
}

// =================================================================
// 4. MAIN PROGRAM
// =================================================================

int main() {
    struct Barang list[100]; int count = 0;
    FILE *f = fopen("database_supermarket-UAS.txt", "r");
    if (!f) { printf("Gagal buka database!\n"); return 1; }
    while (fscanf(f, "%d %s %d %d", &list[count].kode, list[count].nama, &list[count].harga, &list[count].stok) == 4) {
        list[count].qty = 0; count++;
    }
    fclose(f);

    while (1) {
        int id_log; printf("\nLOGIN STAF - Masukkan ID: ");
        if (scanf("%d", &id_log) != 1) { while(getchar()!='\n'); continue; }
        int user_found = 0;
        for (int i = 0; i < total_user; i++) {
            if (database_user[i].id_user == id_log) {
                strcpy(user_aktif, database_user[i].nama_user);
                strcpy(peran_aktif, database_user[i].peran);
                user_found = 1; break;
            }
        }
        if (!user_found) { printf("ID Tidak Terdaftar!\n"); continue; }

        while (1) {
            printf("\n--- MENU %s [%s] ---\n", peran_aktif, user_aktif);
            int pilih;
            
            // KASIR
            if (strcmp(peran_aktif, "Kasir") == 0) {
                printf("1. Transaksi\n2. Cari Barang\n3. Logout\nPilih: "); scanf("%d", &pilih);
                if (pilih == 3) break;
                else if (pilih == 2) fitur_cari(list, count);
                else if (pilih == 1) {
                    int k_tr, q_tr, t_bl = 0;
                    while(1) {
                        printf("Kode Barang (0 = Bayar): "); scanf("%d", &k_tr);
                        if (k_tr == 0) break;
                        for(int i=0; i<count; i++) if(list[i].kode == k_tr) {
                            printf("Jumlah %s: ", list[i].nama); scanf("%d", &q_tr);
                            if(list[i].stok >= q_tr) { list[i].qty += q_tr; t_bl += (list[i].harga * q_tr); }
                            else printf("Stok Kurang!\n");
                        }
                    }
                    if(t_bl > 0) {
                        int byr; printf("Total: %d. Bayar: ", t_bl); scanf("%d", &byr);
                        if(byr >= t_bl) {
                            int kembalian = byr - t_bl;
                            catat_transaksi(user_aktif, t_bl, byr, kembalian, list, count);
                            printf("\n========== STRUK BELANJA ==========\n");
                            for(int i=0; i<count; i++) if(list[i].qty > 0) printf("%-15s x%d  Rp %d\n", list[i].nama, list[i].qty, list[i].qty*list[i].harga);
                            printf("-----------------------------------\nTOTAL: %d | BAYAR: %d | KEMBALI: %d\n", t_bl, byr, kembalian);
                            for(int i=0; i<count; i++) { list[i].stok -= list[i].qty; list[i].qty = 0; }
                            update_file_barang(list, count);
                        }
                    }
                }
            } 
            // ADMIN
            else if (strcmp(peran_aktif, "Admin") == 0) {
                printf("1. Tampilkan Semua Produk\n2. Cari & Edit Barang \n3. Tambah Barang Baru\n4. Lihat Log Admin\n5. Logout\nPilih: ");
                scanf("%d", &pilih);
                if (pilih == 5) break;
                if (pilih == 1) tampilkan_semua_produk(list, count);
                if (pilih == 4) { FILE *l=fopen("admin_log-UAS.txt","r"); if(l){char c;while((c=fgetc(l))!=EOF)putchar(c);fclose(l);} }
                if (pilih == 3) {
                    int mx = 0; for(int i=0; i<count; i++) if(list[i].kode > mx) mx = list[i].kode;
                    list[count].kode = mx + 1;
                    printf("Nama Barang: "); scanf(" %[^\n]", list[count].nama);
                    printf("Harga: "); scanf("%d", &list[count].harga);
                    printf("Stok: "); scanf("%d", &list[count].stok);
                    catat_log_admin(user_aktif, list[count].kode, list[count].nama, "Barang Baru", 0, list[count].stok);
                    count++; update_file_barang(list, count); printf("Barang Berhasil Ditambah!\n");
                }
                // --- MODE KONTINU EDIT ---
                if (pilih == 2) {
                    char in_edit[50];
                    while(1) {
                        printf("\n[MODE EDIT] Masukkan Kode atau Nama Barang (-1 untuk Selesai): ");
                        scanf("%s", in_edit);
                        if (strcmp(in_edit, "-1") == 0) break;

                        int idx = -1;
                        for(int i=0; i<count; i++) {
                            if(list[i].kode == atoi(in_edit) || sama(list[i].nama, in_edit)) {
                                idx = i; break;
                            }
                        }

                        if(idx != -1) {
                            printf("Ditemukan: %s | Harga: %d | Stok: %d\n", list[idx].nama, list[idx].harga, list[idx].stok);
                            printf("Pilih yang ingin diubah (1:Harga, 2:Stok, 3:Batal): ");
                            int sub; scanf("%d", &sub);
                            if(sub == 1) {
                                int hb; printf("Harga Baru: "); scanf("%d", &hb);
                                catat_log_admin(user_aktif, list[idx].kode, list[idx].nama, "Harga", list[idx].harga, hb);
                                list[idx].harga = hb;
                            } else if(sub == 2) {
                                int sb; printf("Stok Baru: "); scanf("%d", &sb);
                                catat_log_admin(user_aktif, list[idx].kode, list[idx].nama, "Stok", list[idx].stok, sb);
                                list[idx].stok = sb;
                            }
                            update_file_barang(list, count);
                            printf("Berhasil diperbarui!\n");
                        } else {
                            printf("Barang tidak ditemukan. Coba lagi.\n");
                        }
                    }
                }
            }
            // MANAGER
            else if (strcmp(peran_aktif, "Manager") == 0) {
                printf("1. Semua Produk\n2. Laporan Omzet\n3. Log Admin\n4. Cari\n5. Logout\nPilih: "); scanf("%d", &pilih);
                if (pilih == 5) break;
                if (pilih == 1) tampilkan_semua_produk(list, count);
                if (pilih == 2) laporan_aggregat(list, count);
                if (pilih == 3) { FILE *l=fopen("admin_log-UAS.txt","r"); if(l){char c;while((c=fgetc(l))!=EOF)putchar(c);fclose(l);} }
                if (pilih == 4) fitur_cari(list, count);
            }
        }
    }
    return 0;
}
