#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <time.h> 

// =================================================================
// STRUKTUR DATA
// =================================================================

struct Barang {
    int kode;
    char nama[50]; 
    int harga;
    int stok; 
    int qty; 
    int subtotal;
};

struct User {
    int id_user;
    char nama_user[30];
    char peran[10]; 
};

char user_aktif[30] = "";
char peran_aktif[10] = ""; 

struct User database_user[] = {
    {101, "Raya", "Admin"},
    {201, "Rayyan", "Kasir"}, 
    {202, "Habib", "Kasir"},
    {301, "Fifah", "Manager"}
};
int total_user = sizeof(database_user) / sizeof(database_user[0]);

// =================================================================
// FUNGSI HELPER
// =================================================================

// Fungsi pembanding string (Fix: Diperlukan untuk pencarian nama)
int sama(char *a, char *b) {
    return strcasecmp(a, b) == 0;
}

// =================================================================
// FUNGSIONALITAS FILE & LOG
// =================================================================

void update_file_barang(struct Barang *list, int count, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("ERROR: Gagal menyimpan data ke file %s\n", filename);
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "%d %s %d %d\n", list[i].kode, list[i].nama, list[i].harga, list[i].stok);
    }
    
    fclose(file);
    printf("Data barang berhasil diperbarui!\n");
}

void catat_log_admin(char *admin, int kode, char *nama_barang, char *jenis_edit, int nilai_lama, int nilai_baru) {
    FILE *file = fopen("admin_log-UAS.txt", "a");
    if (file == NULL) {
        printf("ERROR: Gagal mencatat log Admin.\n");
        return;
    }

    time_t timer;
    char buffer[26];
    struct tm* tm_info;
    
    time(&timer);
    tm_info = localtime(&timer);
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info); 

    fprintf(file, "[%s] Admin: %s | Barang: %s (%d) | Perubahan %s: %d -> %d\n", 
            buffer, admin, nama_barang, kode, jenis_edit, nilai_lama, nilai_baru);
    
    fclose(file);
}

void catat_transaksi(char *kasir, int total, int uang, int kembalian, struct Barang *list, int count) {
    FILE *file = fopen("laporan_penjualan-UAS.txt", "a"); 
    if (file == NULL) {
        printf("ERROR: Gagal mencatat laporan penjualan.\n");
        return;
    }

    time_t timer;
    char buffer[26];
    struct tm* tm_info;
    
    time(&timer);
    tm_info = localtime(&timer);
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info); 

    fprintf(file, "=====================================\n");
    fprintf(file, "Waktu Transaksi: %s\n", buffer); 
    fprintf(file, "Kasir: %s\n", kasir);
    fprintf(file, "Total Belanja: %d\n", total);
    fprintf(file, "Uang Dibayar: %d\n", uang);
    fprintf(file, "Kembalian: %d\n", kembalian);
    fprintf(file, "--- Detail Barang ---\n");
    
    for (int i = 0; i < count; i++) {
        if (list[i].qty > 0) {
            fprintf(file, "%d %s x%d = %d\n", list[i].kode, list[i].nama, list[i].qty, list[i].qty * list[i].harga);
        }
    }
    fprintf(file, "=====================================\n\n");
    fclose(file);
    printf("Laporan penjualan berhasil dicatat!\n");
}

void generate_kode_barang(struct Barang *list, int count, int *kode_baru) {
    int max = 0;
    for (int i = 0; i < count; i++) {
        if (list[i].kode > max) {
            max = list[i].kode;
        }
    }
    *kode_baru = max + 1;
}

void tambah_barang(struct Barang *list, int *count) {
    struct Barang b;
    generate_kode_barang(list, *count, &b.kode);

    printf("Masukkan Nama Barang : ");
    scanf(" %[^\n]", b.nama);
    printf("Masukkan Harga Barang: ");
    scanf("%d", &b.harga);
    printf("Masukkan Stok Awal   : ");
    scanf("%d", &b.stok);

    b.qty = 0;
    b.subtotal = 0;

    list[*count] = b;
    (*count)++;

    update_file_barang(list, *count, "database_supermarket-UAS.txt");
    catat_log_admin(user_aktif, b.kode, b.nama, "Tambah Barang", 0, b.stok);
    printf("Barang baru berhasil ditambahkan dengan kode: %d\n", b.kode);
}

void tampilkan_semua_produk(struct Barang *list, int count) {
    printf("\n--- DAFTAR STOK HARGA BARANG ALPHAMART ---\n");
    printf("%-6s %-30s %-8s %s\n", "Kode", "Nama Barang", "Harga", "Stok"); 
    printf("----------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-6d %-30s %-8d %d\n", list[i].kode, list[i].nama, list[i].harga, list[i].stok);
    }
    printf("----------------------------------------------------------\n");
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

void laporan_penjualan(struct Barang *list, int count) {
    FILE *file = fopen("laporan_penjualan-UAS.txt", "r");
    if (file == NULL) {
        printf("ERROR: Laporan belum tersedia.\n");
        return;
    }

    int total_qty_terjual[100] = {0}; 
    int total_pendapatan_per_barang[100] = {0}; 
    int total_pendapatan = 0;
    char line[256];
    char nama_item[50];
    int kode_item, qty_beli, subtotal;
    
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, "Total Belanja:")) {
            int current_total;
            sscanf(line, "Total Belanja: %d", &current_total);
            total_pendapatan += current_total; 
        }
        if (sscanf(line, "%d %s x%d = %d", &kode_item, nama_item, &qty_beli, &subtotal) == 4) {
            for (int i = 0; i < count; i++) {
                if (list[i].kode == kode_item) {
                    total_qty_terjual[i] += qty_beli;
                    total_pendapatan_per_barang[i] += subtotal; 
                    break;
                }
            }
        }
    }
    fclose(file);

    printf("\n========================================================================\n");
    printf("              LAPORAN PENJUALAN PERIODE (AGGREGAT)\n"); 
    printf("========================================================================\n");
    printf("TOTAL KESELURUHAN PENDAPATAN: %d\n", total_pendapatan);
    printf("------------------------------------------------------------------------\n");
    printf("%-6s %-30s %-15s %s\n", "Kode", "Nama Barang", "QTY Terjual", "Total Penjualan");
    printf("------------------------------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        if (total_qty_terjual[i] > 0) {
            printf("%-6d %-30s %-15d %d\n", list[i].kode, list[i].nama, total_qty_terjual[i], total_pendapatan_per_barang[i]);
        }
    }
    printf("========================================================================\n");
}

void tampilkan_log_file(const char *filename, const char *title) {
    printf("\n--- %s ---\n", title);
    FILE *log_file = fopen(filename, "r");
    if (log_file == NULL) {
        printf("File %s belum tersedia.\n", filename);
        return;
    }
    char line[256];
    while (fgets(line, sizeof(line), log_file) != NULL) {
        printf("%s", line);
    }
    fclose(log_file);
}

// =================================================================
// FUNGSI LOGIN
// =================================================================

int login_user() {
    int id;
    int max_percobaan = 3;
    printf("\n==================================\n");
    printf("--- SELAMAT DATANG DI ALPHAMART ---\n");
    printf("==================================\n");
    
    for (int i = 0; i < max_percobaan; i++) {
        printf("Masukkan ID Staf Alphamart (%d kesempatan): ", max_percobaan - i);
        if (scanf("%d", &id) != 1) {
            while (getchar() != '\n'); continue;
        }

        for (int j = 0; j < total_user; j++) {
            if (database_user[j].id_user == id) {
                strcpy(user_aktif, database_user[j].nama_user);
                strcpy(peran_aktif, database_user[j].peran);
                printf("\nVerifikasi Berhasil! Selamat Bertugas, %s (%s).\n", user_aktif, peran_aktif);
                return 1; 
            }
        }
        printf("ID Salah!\n");
    }
    return 0; 
}

// =================================================================
// MAIN PROGRAM
// =================================================================

int main() {
    struct Barang list[100]; 
    int count = 0;
    FILE *file = fopen("database_supermarket-UAS.txt", "r"); 

    if (file != NULL) {
        while (fscanf(file, "%d %s %d %d", &list[count].kode, list[count].nama, &list[count].harga, &list[count].stok) == 4) {
            list[count].qty = 0;
            list[count].subtotal = 0;
            count++;
        }
        fclose(file);
    }
    
    while (1) {
        if (!login_user()) break; 

        while (1) {
            int pilih;
            printf("\n[%s - %s] Menu Alphamart:\n", peran_aktif, user_aktif);
            
            if (strcmp(peran_aktif, "Kasir") == 0) {
                printf("1. Lihat Daftar Harga Barang\n"); 
                printf("2. Mulai Transaksi Baru\n");
                printf("3. Selesai Bertugas (Logout)\n");
            } 
            else if (strcmp(peran_aktif, "Admin") == 0) {
                printf("1. Tampilkan Semua Produk\n");
                printf("2. Cari & Edit Barang\n"); 
                printf("3. Tambah Barang Baru\n");  
                printf("4. Lihat Log Admin\n"); 
                printf("5. Logout\n"); 
            }
            else if (strcmp(peran_aktif, "Manager") == 0) {
                printf("1. Lihat Daftar Stok Barang\n");
                printf("2. Data Penjualan (Struk Rinci)\n");
                printf("3. Laporan Penjualan (Aggregat)\n");
                printf("4. Laporan Log Aktivitas Admin\n");
                printf("5. Selesai Bertugas (Logout)\n");
            }
            
            printf("Pilihan: ");
            scanf("%d", &pilih);

            // LOGIKA LOGOUT
            if ((strcmp(peran_aktif, "Kasir") == 0 && pilih == 3) ||
                ((strcmp(peran_aktif, "Admin") == 0 || strcmp(peran_aktif, "Manager") == 0) && pilih == 5)) {
                printf("Terima kasih, %s. Logout berhasil.\n", user_aktif);
                break; 
            }

            // FITUR 1: LIHAT DAFTAR (Semua Peran)
            if (pilih == 1) {
                tampilkan_semua_produk(list, count);
            } 
            
            // FITUR 2: TRANSAKSI (Kasir) / EDIT (Admin) / DATA PENJUALAN (Manager)
            else if (pilih == 2) {
                if (strcmp(peran_aktif, "Kasir") == 0) {
                    int kode, qty, total = 0;
                    while (1) {
                        printf("Masukkan Kode Barang (0 untuk selesai): "); scanf("%d", &kode);
                        if (kode == 0) break;
                        int found = -1;
                        for (int i = 0; i < count; i++) { if (list[i].kode == kode) found = i; }
                        if (found == -1) { printf("Kode tidak ditemukan!\n"); continue; }
                        printf("Kuantitas %s (Stok: %d): ", list[found].nama, list[found].stok); scanf("%d", &qty);
                        if (list[found].stok >= qty) {
                            list[found].qty += qty; total += list[found].harga * qty;
                        } else printf("Stok tidak cukup!\n");
                    }
                    if (total > 0) {
                        int uang; printf("Total: %d. Masukkan Uang: ", total); scanf("%d", &uang);
                        if (uang >= total) {
                            int kembali = uang - total;
                            // --- PRINT STRUK LAYAR ---
                            printf("\n========== STRUK BELANJA ==========\n");
                            for(int i=0; i<count; i++) if(list[i].qty > 0) printf("%-15s x%d  = %d\n", list[i].nama, list[i].qty, list[i].qty * list[i].harga);
                            printf("-----------------------------------\nTOTAL     : %d\nBAYAR     : %d\nKEMBALIAN : %d\n===================================\n", total, uang, kembali);
                            
                            catat_transaksi(user_aktif, total, uang, kembali, list, count);
                            for (int i = 0; i < count; i++) { if (list[i].qty > 0) list[i].stok -= list[i].qty; list[i].qty = 0; }
                            update_file_barang(list, count, "database_supermarket-UAS.txt");
                        } else printf("Uang kurang!\n");
                    }
                } 
                else if (strcmp(peran_aktif, "Admin") == 0) {
                    char in_edit[50]; printf("Masukkan Nama/Kode Barang: "); scanf("%s", in_edit);
                    int idx = -1;
                    for(int i=0; i<count; i++) {
                        if(list[i].kode == atoi(in_edit) || sama(list[i].nama, in_edit)) idx = i;
                    }
                    if(idx != -1) {
                        printf("Ditemukan: %s | Harga: %d | Stok: %d\n", list[idx].nama, list[idx].harga, list[idx].stok);
                        printf("1. Ubah Harga, 2. Ubah Stok, 3. Batal: "); int sub; scanf("%d", &sub);
                        if(sub == 1) {
                            int hb; printf("Harga Baru: "); scanf("%d", &hb);
                            catat_log_admin(user_aktif, list[idx].kode, list[idx].nama, "Harga", list[idx].harga, hb);
                            list[idx].harga = hb;
                        } else if(sub == 2) {
                            int sb; printf("Stok Baru: "); scanf("%d", &sb);
                            catat_log_admin(user_aktif, list[idx].kode, list[idx].nama, "Stok", list[idx].stok, sb);
                            list[idx].stok = sb;
                        }
                        update_file_barang(list, count, "database_supermarket-UAS.txt");
                    } else printf("Barang tidak ada.\n");
                }
                else if (strcmp(peran_aktif, "Manager") == 0) {
                    tampilkan_log_file("laporan_penjualan-UAS.txt", "DATA PENJUALAN (STRUK RINCI)");
                }
            }
            
            // FITUR 3: TAMBAH (Admin) / LAPORAN (Manager)
            else if (pilih == 3) {
                if (strcmp(peran_aktif, "Admin") == 0) tambah_barang(list, &count);
                else if (strcmp(peran_aktif, "Manager") == 0) laporan_penjualan(list, count);
            }
            
            // FITUR 4: LOG ADMIN (Admin & Manager)
            else if (pilih == 4) {
                tampilkan_log_file("admin_log-UAS.txt", "LOG AKTIVITAS ADMIN");
            }
        }
    }
    return 0;
}
  
