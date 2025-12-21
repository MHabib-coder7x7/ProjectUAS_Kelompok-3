#include <stdio.h>

struct Barang {
    int kode;
    char nama[50];
    int harga;
    int stok;
};

int sama(char a[], char b[]) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i])
            return 0;
        i++;
    }
    return a[i] == b[i];
}

int main() {
    FILE *file;
    struct Barang data[200];
    int jumlah = 0;
    int menu;
    char cari[50];

    file = fopen("database_supermarket-UAS.txt", "r");
    if (file == NULL) {
        printf("File database_supermarket-UAS.txt tidak ditemukan\n");
        return 1;
    }

    while (fscanf(file, "%d %s %d %d",
                  &data[jumlah].kode,
                  data[jumlah].nama,
                  &data[jumlah].harga,
                  &data[jumlah].stok) == 4) {
        jumlah++;
    }
    fclose(file);

    do {
        printf("\n=== CEK DATABASE BARANG SUPERMARKET ===\n");
        printf("1. Tampilkan Semua Barang\n");
        printf("2. Cari Barang (Kode / Nama Persis)\n");
        printf("3. Keluar\n");
        printf("Pilih menu: ");
        scanf("%d", &menu);

        if (menu == 1) {
            printf("\nKode   Nama Barang                 Harga   Stok\n");
            printf("------------------------------------------------\n");
            for (int i = 0; i < jumlah; i++) {
                printf("%-6d %-25s %-7d %d\n",
                       data[i].kode,
                       data[i].nama,
                       data[i].harga,
                       data[i].stok);
            }
        }

        else if (menu == 2) {
            printf("Masukkan kode atau nama barang: ");
            scanf("%s", cari);

            int ketemu = 0;

            for (int i = 0; i < jumlah; i++) {
                if (data[i].kode == atoi(cari) || sama(data[i].nama, cari)) {
                    printf("\nBarang Ditemukan:\n");
                    printf("Kode  : %d\n", data[i].kode);
                    printf("Nama  : %s\n", data[i].nama);
                    printf("Harga : %d\n", data[i].harga);
                    printf("Stok  : %d\n", data[i].stok);
                    ketemu = 1;
                }
            }

            if (!ketemu) {
                printf("Barang tidak ditemukan\n");
            }
        }

    } while (menu != 3);

    printf("Program selesai\n");
    return 0;
}