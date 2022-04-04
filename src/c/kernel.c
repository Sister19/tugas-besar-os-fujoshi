// Kode kernel
// PENTING : FUNGSI PERTAMA YANG DIDEFINISIKAN ADALAH main(),
//   cek spesifikasi untuk informasi lebih lanjut

// TODO : Tambahkan implementasi kode C
#define endl interrupt(0x21, 0x0, "\r\n", 0, 0)
#define sp interrupt(0x21, 0x0, " ", 0, 0)
#include "header/kernel.h"
#include "header/filesystem.h"
int pagenum = 0;
int rowt = 25;
int colt = 80;



int main() {
  // INITIALIZER
  fillKernelMap();
  makeInterrupt21();
  clearScreen();

  shell();
  // KERNEL STAY
}

void uintprint(unsigned int toprint) {
  printString("0x");
  while (toprint >= 0) {
    byte lb = toprint & 0xF;
    switch (lb) {
      case 0x0:
        printString("0");
        break;
      case 0x1:
        printString("1");
        break;
      case 0x2:
        printString("2");
        break;
      case 0x3:
        printString("3");
        break;
      case 0x4: 
        printString("4");
        break;
      case 0x5:
        printString("5");
        break;
      case 0x6:
        printString("6");
        break;
      case 0x7:
        printString("7");
        break;
      case 0x8:
        printString("8");
        break;
      case 0x9: 
        printString("9");
        break;
      case 0xA:
        printString("A");
        break;
      case 0xB:
        printString("B");
        break;
      case 0xC:
        printString("C");
        break;
      case 0xD: 
        printString("D");
        break;
      case 0xE:
        printString("E");
        break;
      case 0xF:
        printString("F");
        break;
      default:
        printString("wentwrong");
    }
    toprint = toprint >> 4;
    if (toprint == 0) {
      break;
    }
  }
}

// int getinodecwd(char* cwd) {
//   printString(cwd); endl;
//   return 1;
// }

void respcode(enum fs_retcode return_code) {
  if (return_code == FS_SUCCESS) {
    printString("SUC"); endl;
  } else if (return_code == FS_W_FILE_ALREADY_EXIST) {
    printString("FAEX"); endl;
  } else if (return_code == FS_W_NOT_ENOUGH_STORAGE) {
    printString("NES"); endl;
  } else if (return_code == FS_W_MAXIMUM_NODE_ENTRY) {
    printString("MXN"); endl;
  } else if (return_code == FS_W_MAXIMUM_SECTOR_ENTRY) {
    printString("MXS"); endl;
  } else if (return_code == FS_W_INVALID_FOLDER) {
    printString("INV"); endl;
  } else {
    printString("UKN"); endl;
  }

  // write(&metadata, &return_code);
}

int getinodecwd(char* cwd) {
  struct node_filesystem node_fs;
  struct node_entry nebuff;
  char* str1; int it; int i; int depth; int startslash; int endslash; int prevparidx;
  bool found = false;

  it = 0; startslash = 0; endslash = 0; depth = 0; prevparidx = FS_NODE_P_IDX_ROOT;
  readSector(&node_fs, FS_NODE_SECTOR_NUMBER);
  readSector(&node_fs.nodes[32], FS_NODE_SECTOR_NUMBER+1);

  while (cwd[it] != '\0') {
    if (cwd[it] == '/') {
      if (depth == 0 && cwd[it+1] == '\0') {
        return FS_NODE_P_IDX_ROOT;
      } else if (depth == 0) {
        startslash = it;
      } else {
        endslash = it;
        memcpy(&str1, &cwd[startslash+1], endslash-startslash); // string is target
        for (i = 0; i < 64; i++) {
           memcpy(&nebuff, &node_fs.nodes[i], sizeof(struct node_entry));
            if (strcmp(nebuff.name, str1) && nebuff.parent_node_index == prevparidx && nebuff.sector_entry_index == FS_NODE_S_IDX_FOLDER) {
              prevparidx = i;
              break;
            }
        }
        startslash = it;
      }
      depth++;
    }
    it++;
  }
  return prevparidx;
}

// void lsHandler(char* cwd) {
//   struct node_filesystem node_fs_1;
//   struct node_filesystem node_fs_2;
//   struct node_entry nebuff;
//   int cwd_id;
//   int i;
//   i = 0;
//   cwd_id = getinodecwd(cwd);
//   // folder search
//   for (i = 0; i < 64; i++) {
//     memcpy(&nebuff, &node_fs_1.nodes[i], sizeof(struct node_entry));
//     if (nebuff.parent_node_index == cwd_id && nebuff.sector_entry_index == FS_NODE_S_IDX_FOLDER) {
//       interrupt(0x21, 0x0, nebuff.name, 0, 0); endl;
//     }
//   }
//     for (i = 0; i < 64; i++) {
//     memcpy(&nebuff, &node_fs_2.nodes[i], sizeof(struct node_entry));
//     if (nebuff.parent_node_index == cwd_id && nebuff.sector_entry_index == FS_NODE_S_IDX_FOLDER) {
//       interrupt(0x21, 0x0, nebuff.name, 0, 0); endl;
//     }
//   }
//   //file search
// }
void shell() {
  // struct node_filesystem node_fs_1;
  // struct node_filesystem node_fs_2;
  // struct node_entry nebuff;
  // int cwd_id;
  // int i;
  struct node_filesystem node_fs;
  struct node_entry nebuff;
  struct file_metadata metadata;
  enum fs_retcode return_code;
  char buf[64]; char name[64]; char cwd[64]; char args1[64]; char args2[64];
  struct strwrap arg1wrap; struct strwrap arg2wrap;
  int i; int j; int cwd_id;

  readSector(&node_fs, FS_NODE_SECTOR_NUMBER);
  readSector(&node_fs.nodes[32], FS_NODE_SECTOR_NUMBER+1);
  strcpy(name,  "GG");
  strcpy(cwd,  "/");

  endl; endl;
  // interrupt(0x21, 0x0, "Masukkan Nama: ", 0, 0);
  // interrupt(0x21, 0x1, buf, 0, 0);
  // strcpy(&name, buf);
  // strcpy(&cwd, "/");
  // interrupt(0x21, 0x0, "Halo ", 0, 0);
  // interrupt(0x21, 0x0, buf, 0, 0);
  // interrupt(0x21, 0x0, ", Selamat Datang di FujOShi, Enjoy your Stay!", 0, 0);
  // endl; endl;
  while (true) {
    clear(buf, 64);
    clear(args1, 64);
    clear(args2, 64);
    interrupt(0x21, 0x0, "FujOShi@", 0, 0);
    interrupt(0x21, 0x0, name, 0, 0);
    interrupt(0x21, 0x0, ":", 0, 0);
    interrupt(0x21, 0x0, cwd, 0, 0);
    interrupt(0x21, 0x0, "$ ", 0, 0);
    interrupt(0x21, 0x1, buf, 0, 0);
    i = 0;
    while (buf[i] != ' ' && buf[i] != '\0') i++;
    memcpy(args1, buf, i);
    args1[i] = '\0';
    if (strcmp("cd", args1)) {
    } else if (strcmp("ls", args1)) {
        readSector(&node_fs, FS_NODE_SECTOR_NUMBER);
        readSector(&node_fs.nodes[32], FS_NODE_SECTOR_NUMBER+1);
        cwd_id = getinodecwd(cwd);
        // folder search
        for (i = 0; i < 64; i++) {
          memcpy(&nebuff, &node_fs.nodes[i], sizeof(struct node_entry));
          if (nebuff.parent_node_index == cwd_id && nebuff.sector_entry_index == FS_NODE_S_IDX_FOLDER) {
            interrupt(0x21, 0x0, "/", 0, 0);
            interrupt(0x21, 0x0, nebuff.name, 0, 0); 
            interrupt(0x21, 0x0, "/", 0, 0); endl;
          }
        }
        for (i = 0; i < 64; i++) {
          memcpy(&nebuff, &node_fs.nodes[i], sizeof(struct node_entry));
          if (nebuff.parent_node_index == cwd_id && nebuff.sector_entry_index != FS_NODE_S_IDX_FOLDER) {
            interrupt(0x21, 0x0, nebuff.name, 0, 0); endl;
          }
        }
        // file search
    } else if (strcmp("mv", buf)) {
    } else if (strcmp("mkdir", args1)) {
      strcpy(args2, buf+i+1);
      strcpy(metadata.node_name, args2);
      metadata.parent_index = getinodecwd(cwd);
      metadata.filesize = 0;
      write(&metadata, &return_code);
      respcode(return_code);
      // PROCESS END
    } else if (strcmp("cat", buf)) {
      
    } else if (strcmp("cp", buf)) {
      
    } else if (strcmp("mv", buf)) {
      
    }
  }
}

void handleInterrupt21(int AX, int BX, int CX, int DX) {
	byte AL,AH;
	AL = AX & 0xFF;
	AH = AX >> 8;
  switch (AX) {
      case 0x0:
          printString(BX);
          break;
      case 0x1:
          readString(BX);
          break;
      case 0x2:
          readSector(BX, CX);
          break;
      case 0x3:
          writeSector(BX, CX);
          break;
      case 0x4:
          read(BX, CX);
          break;
      case 0x5:
          write(BX, CX);
          break;
      default:
          printString("Invalid Interrupt");
  }

}

// void splashScreen() {
//   char* str = "FujOShi System";
// 	interrupt(0x10, 0x200, pagenum, 0, 0x222);
// 	interrupt(0x21, 0x0, str, 0, 0);
// 	interrupt(0x10, 0x200, pagenum, 0, 0x400);
// }

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

void readSector(byte *buffer, int sector_number){
    int sector_read_count = 0x01;
    int cylinder, sector;
    int head, drive;

    cylinder = div(sector_number, 36) << 8; // CH
    sector   = mod(sector_number, 18) + 1;  // CL

    head  = mod(div(sector_number, 18), 2) << 8; // DH
    drive = 0x00;                                // DL

    interrupt(
        0x13,                       // Interrupt number
        0x0200 | sector_read_count, // AX
        buffer,                     // BX
        cylinder | sector,          // CX
        head | drive                // DX
    );

}
void writeSector(byte *buffer, int sector_number){
    int sector_read_count = 0x01;
    int cylinder, sector;
    int head, drive;

    cylinder = div(sector_number, 36) << 8; // CH
    sector   = mod(sector_number, 18) + 1;  // CL

    head  = mod(div(sector_number, 18), 2) << 8; // DH
    drive = 0x00;                                // DL

    interrupt(
        0x13,                       // Interrupt number
        0x0300 | sector_read_count, // AX
        buffer,                     // BX
        cylinder | sector,          // CX
        head | drive                // DX
    );
}

void fillKernelMap() {
  struct map_filesystem map_fs_buffer;
  int i;

  readSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);
  // sysreserve
  for (i = 0; i < 16; i++) {
    map_fs_buffer.is_filled[i] = true;
  }
  // sysfile
  for (i = 16; i < 256; i++) {
    map_fs_buffer.is_filled[i] = false;
  }
  // sysres
  for (i = 256; i < 512; i++) {
    map_fs_buffer.is_filled[i] = true;
  }
  writeSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);
}

void read(struct file_metadata *metadata, enum fs_retcode *return_code) {
  struct node_filesystem   node_fs_buffer;
  struct sector_filesystem sector_fs_buffer;
  struct sector_entry sebuff;
  struct node_entry nebuff;
  // // Tambahkan tipe data yang dibutuhkan
  int i; int j;
  byte sectorref;


  // // Masukkan filesystem dari storage ke memori buffer
  readSector(&node_fs_buffer, FS_NODE_SECTOR_NUMBER);
  readSector(&node_fs_buffer.nodes[32], FS_SECTOR_SECTOR_NUMBER+1);
  readSector(&sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);
  // 1. Cari node dengan nama dan lokasi yang sama pada filesystem.
  for (i = 0; i < 64; i++) {
    memcpy(&nebuff, &node_fs_buffer.nodes[i], sizeof(struct node_entry));
    if (strcmp(nebuff.name, metadata->node_name) && nebuff.parent_node_index == metadata->parent_index) {
        sectorref = nebuff.sector_entry_index;
        if (sectorref == FS_NODE_S_IDX_FOLDER) {
          *return_code = FS_R_TYPE_IS_FOLDER;
        } else {
            memcpy(&sebuff, &sector_fs_buffer.sector_list[sectorref], sizeof(struct sector_entry));
            j = 0;
            metadata->filesize = 0;
            while (sebuff.sector_numbers[j] != 0x0) {
              readSector(metadata->buffer[j*512], sebuff.sector_numbers[j]);
              metadata->filesize += 512;
              j++;
            }
            *return_code = FS_SUCCESS;
        }
        return;
      }
  }
  *return_code = FS_R_NODE_NOT_FOUND;
  return;
}



void write(struct file_metadata *metadata, enum fs_retcode *return_code) {
  struct node_filesystem   node_fs_buffer;
  struct sector_filesystem sector_fs_buffer;
  struct map_filesystem    map_fs_buffer;
  // Tambahkan tipe data yang dibutuhkan
  struct node_entry nebuff; struct sector_entry sebuff;
  int i; int j; //loopers
  int fzins; int inode; int isect; int emsect;// multipurpose numbers
  byte pb; byte sb; // multipurpose byte
  bool found;

  // Masukkan filesystem dari storage ke memori
  readSector(&node_fs_buffer, FS_NODE_SECTOR_NUMBER);
  readSector(&node_fs_buffer.nodes[32], FS_NODE_SECTOR_NUMBER+1);
  readSector(&sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);
  readSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);
  if (metadata->filesize == 0) {
    pb = metadata->parent_index;
    sb = FS_NODE_S_IDX_FOLDER;
  } else {
    pb = metadata->parent_index;
    sb = isect;
  }
  if (div(metadata->filesize, 512) * 512 == metadata->filesize) {
    fzins = div(metadata->filesize, 512);
  } else {
    fzins = div(metadata->filesize, 512) + 1;
  }
  // 1. Cari node dengan nama dan lokasi parent yang sama pada node.
  //    Jika tidak ditemukan kecocokan, lakukan proses ke-2.
  //    Jika ditemukan node yang cocok, tuliskan retcode 
  //    FS_W_FILE_ALREADY_EXIST dan keluar. 
  for (i = 0; i < 64; i++) {
    memcpy(&nebuff, &node_fs_buffer.nodes[i], sizeof(struct node_entry));
    if (strcmp(nebuff.name, metadata->node_name) && nebuff.parent_node_index == pb && nebuff.sector_entry_index == sb) {
      *return_code = FS_W_FILE_ALREADY_EXIST;
      return;
    }
  }
  // 2. Cari entri kosong pada filesystem node dan simpan indeks.
  //    Jika ada entry kosong, simpan indeks untuk penulisan.
  //    Jika tidak ada entry kosong, tuliskan FS_W_MAXIMUM_NODE_ENTRY
  //    dan keluar.
  found = false;
  for (i = 0; i < 64; i++) {
    memcpy(&nebuff, &node_fs_buffer.nodes[i], sizeof(struct node_entry));
    if (nebuff.parent_node_index == 0x0) {
      inode = i;
      found = true;
      break;
    }
  }

  if (!found) {
    *return_code = FS_W_MAXIMUM_NODE_ENTRY;
    return;
  }
  // 3. Cek dan pastikan entry node pada indeks P adalah folder.
  //    Jika pada indeks tersebut adalah file atau entri kosong,
  //    Tuliskan retcode FS_W_INVALID_FOLDER dan keluar.
  memcpy(&nebuff, &node_fs_buffer.nodes[metadata->parent_index], sizeof(struct node_entry));
  if (nebuff.sector_entry_index == 0x0 || nebuff.sector_entry_index == FS_NODE_S_IDX_FOLDER) {
    *return_code = FS_W_INVALID_FOLDER;
    return;
  }
  // 4. Dengan informasi metadata filesize, hitung sektor-sektor 
  //    yang masih kosong pada filesystem map. Setiap byte map mewakili 
  //    satu sektor sehingga setiap byte mewakili 512 bytes pada storage.
  //    Jika empty space tidak memenuhi, tuliskan retcode
  //    FS_W_NOT_ENOUGH_STORAGE dan keluar.
  //    Jika ukuran filesize melebihi 8192 bytes, tuliskan retcode
  //    FS_W_NOT_ENOUGH_STORAGE dan keluar.
  //    Jika tersedia empty space, lanjutkan langkah ke-5.
  if (metadata->filesize > 8192) {
    *return_code = FS_W_NOT_ENOUGH_STORAGE;
    return;
  }

  emsect = 0;
  for (i = 0; i < 512; i++) {
    if (map_fs_buffer.is_filled[i] == 0x0) {
      emsect+=1;
    }
  }

  if (emsect < fzins) {
    *return_code = FS_W_NOT_ENOUGH_STORAGE;
    return;
  }
  // 5. Cek pada filesystem sector apakah terdapat entry yang masih kosong.
  //    Jika ada entry kosong dan akan menulis file, simpan indeks untuk 
  //    penulisan.
  //    Jika tidak ada entry kosong dan akan menulis file, tuliskan
  //    FS_W_MAXIMUM_SECTOR_ENTRY dan keluar.
  //    Selain kondisi diatas, lanjutkan ke proses penulisan.
  found = false;
  for (i = 0; i < 32; i++) {
    memcpy(&sebuff, &sector_fs_buffer.sector_list[i], sizeof(struct sector_entry));
    if (&sebuff == 0x0) {
      isect = i;
      found = true;
      break;
    }
  }

  if (!found && metadata->filesize > 0) {
    *return_code = FS_W_MAXIMUM_SECTOR_ENTRY;
    return;
  }
  // // Penulisan
  // // 1. Tuliskan metadata nama dan byte P ke node pada memori buffer
  // // 2. Jika menulis folder, tuliskan byte S dengan nilai
  // //    FS_NODE_S_IDX_FOLDER dan lompat ke langkah ke-8 
  // // 3. Jika menulis file, tuliskan juga byte S sesuai indeks sector


  strcpy(nebuff.name, metadata->node_name);
  nebuff.parent_node_index = pb;
  nebuff.sector_entry_index = sb;
  memcpy(&node_fs_buffer.nodes[inode], &nebuff, sizeof(struct node_entry));

  // printString("Here: "); printString(metadata->node_name); endl;
  // if (nodeid <= 63) {

  if (metadata->filesize > 0) {
    // 4. Persiapkan variabel j = 0 untuk iterator entry sector yang kosong
    // 5. Persiapkan variabel buffer untuk entry sector kosong
    // 6. Lakukan iterasi berikut dengan kondisi perulangan 
          // (penulisan belum selesai && i = 0..255)
        //    1. Cek apakah map[i] telah terisi atau tidak
    //    2. Jika terisi, lanjutkan ke iterasi selanjutnya / continue
    //    3. Tandai map[i] terisi
    //    4. Ubah byte ke-j buffer entri sector dengan i
    //    5. Tambah nilai j dengan 1
    //    6. Lakukan writeSector() dengan file pointer buffer pada metadata 
    //       dan sektor tujuan i
    //    7. Jika ukuran file yang telah tertulis lebih besar atau sama dengan
    //       filesize pada metadata, penulisan selesai
    
    // 7. Lakukan update dengan memcpy() buffer entri sector dengan 
    //    buffer filesystem sector

    i = 16; // sector reserved for fsys
    j = 0;
    memcpy(&sebuff, &sector_fs_buffer.sector_list[isect], sizeof(struct sector_entry));
    while (fzins > 0 && i < 256) {
      if (!map_fs_buffer.is_filled[i]) {
        writeSector(metadata->buffer[j*512], i);
        sebuff.sector_numbers[j] = i;
        j++;
        map_fs_buffer.is_filled[i] = true;
        fzins -= 1;
      }
      i++;
    }
    memcpy(&sector_fs_buffer.sector_list[isect], &sebuff, sizeof(struct sector_entry));
  }

  // // 8. Lakukan penulisan seluruh filesystem (map, node, sector) ke storage
  // //    menggunakan writeSector() pada sektor yang sesuai
  writeSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);
  writeSector(&node_fs_buffer, FS_NODE_SECTOR_NUMBER);
  writeSector(&node_fs_buffer.nodes[32], FS_NODE_SECTOR_NUMBER + 1);
  writeSector(&sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);
  // // 9. Kembalikan retcode FS_SUCCESS
  *return_code = FS_SUCCESS;
  return;
}

