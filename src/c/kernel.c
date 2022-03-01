// Kode kernel
// PENTING : FUNGSI PERTAMA YANG DIDEFINISIKAN ADALAH main(),
//   cek spesifikasi untuk informasi lebih lanjut

// TODO : Tambahkan implementasi kode C
#define endl printString("\r\n")
#define sp printString(" ")
#include "header/kernel.h"
int pagenum = 0;
int rowt = 25;
int colt = 80;


int main() {
  // INITIALIZER
  char buf[128];
  makeInterrupt21();

  clearScreen();
  splashScreen();
  printString("Masukkan Nama: ");
  readString(buf);
  printString("Halo ");
  printString(buf);
  printString(", Selamat Datang di FujOShi, Enjoy your Stay!");
  endl;

  // KERNEL STAY
  while (true);
}

void handleInterrupt21(int AX, int BX, int CX, int DX) {
    
}

void splashScreen() {
  interrupt(0x10, 0x200, pagenum, 0, 0x222);
  printString("FujOShi System");
  interrupt(0x10, 0x200, pagenum, 0, 0x400);
}

void printString(char* string) {
  int i = 0;
  while (string[i] != '\0') { // 0x0 == '\0'
    interrupt(0x10, 0xE00+string[i], pagenum, 0, 0);
    i++;
  }
}

void readString(char* toread) {
    int count = 0;
    char c;
    while (true) {
        c = interrupt(0x16, 0, 0, 0, 0);
        if (c == '\r') {
            interrupt(0x10, 0xE00+'\r', pagenum, 0, 0);
            interrupt(0x10, 0xE00+'\n', pagenum, 0, 0);
            toread[count] = '\0';
            break;
        } else if (c == '\b') {
            if (count > 0) {
                toread[count] = '\0';
                interrupt(0x10, 0xE00+'\b', pagenum, 0, 0);
                interrupt(0x10, 0xE00+' ', pagenum, 0, 0);
                interrupt(0x10, 0xE00+'\b', pagenum, 0, 0);
                count--;
            }
        } else {
            interrupt(0x10, 0xE00+c, pagenum, 0, 0);
            toread[count] = c;
            count++;
        }
    }
}

void clearScreen() {
    int i = 0;
    int j = 0;
    for (i = 0; i < rowt; i++) {
      for (j = 0; j < colt; j++) {
        int where = colt*i+j;
        putInMemory(0xB000, 0x8000 + 2*where, 0x0);
        putInMemory(0xB000, 0x8001 + 2*where, 0xF);
      }
    }
    interrupt(0x10, 0x200, pagenum, 0, 0);
}

