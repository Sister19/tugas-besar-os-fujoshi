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
  splashScreen();

  shell();
  // KERNEL STAY
}
void mkdirHandler(char* cwd, char* path, unsigned int pathsize, unsigned int cwdsize) {


  // write(&metadata, &return_code);
}
void byteprint(byte toprint) {
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

int getinodecwd(char* cwd) {
  struct node_filesystem node_fs_1;
  struct node_filesystem node_fs_2;
  struct node_entry nebuff;
  char* str1;
  int it;
  int i;
  int depth;
  int startslash;
  int endslash;
  int prevparidx;
  bool found = false;
  it = 0;
  startslash = 0;
  endslash = 0;
  depth = 0;
  prevparidx = FS_NODE_P_IDX_ROOT;
  while (cwd[it] != '\0') {
    if (cwd[it] == '/') {
      if (depth == 0 && cwd[it+1] == '\0') {
        return FS_NODE_P_IDX_ROOT;
      } else if (depth == 0) {
        startslash = it;
      } else {
        endslash = it;
        readSector(&node_fs_1, FS_NODE_SECTOR_NUMBER);
        readSector(&node_fs_2, FS_NODE_SECTOR_NUMBER + 1);
        memcpy(&str1, &cwd[startslash+1], endslash-startslash); // string is target

        found = false;
        for (i = 0; i < 64; i++) {
           memcpy(&nebuff, &node_fs_1.nodes[i], sizeof(struct node_entry));
            if (strcmp(nebuff.name, str1) && nebuff.parent_node_index == prevparidx && nebuff.sector_entry_index == FS_NODE_S_IDX_FOLDER) {
              prevparidx = i;
              found = true;
              break;
            }
        }

        if (!found) {
          for (i = 0; i < 64; i++) {
            memcpy(&nebuff, &node_fs_2.nodes[i], sizeof(struct node_entry));
            if (strcmp(nebuff.name, str1) && nebuff.parent_node_index == prevparidx && nebuff.sector_entry_index == FS_NODE_S_IDX_FOLDER) {
              prevparidx = i + 64;
              found = true;
              break;
            }
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

void lsHandler(char* cwd) {
  struct node_filesystem node_fs_1;
  struct node_filesystem node_fs_2;
  struct node_entry nebuff;
  int cwd_id;
  int i;
  i = 0;
  cwd_id = getinodecwd(cwd);
  // folder search
  for (i = 0; i < 64; i++) {
    memcpy(&nebuff, &node_fs_1.nodes[i], sizeof(struct node_entry));
    if (nebuff.parent_node_index == cwd_id && nebuff.sector_entry_index == FS_NODE_S_IDX_FOLDER) {
      interrupt(0x21, 0x0, nebuff.name, 0, 0); endl;
    }
  }
    for (i = 0; i < 64; i++) {
    memcpy(&nebuff, &node_fs_2.nodes[i], sizeof(struct node_entry));
    if (nebuff.parent_node_index == cwd_id && nebuff.sector_entry_index == FS_NODE_S_IDX_FOLDER) {
      interrupt(0x21, 0x0, nebuff.name, 0, 0); endl;
    }
  }
  //file search
}
void shell() {
  struct file_metadata metadata;
  enum fs_retcode return_code;
  struct node_filesystem node_fs_1;
  struct node_filesystem node_fs_2;
  struct node_entry nebuff;
  int cwd_id;
  int i;
  char buf[64];
  char name[64];
  char cwd[64];
  char args1[64];
  char args2[64];
  int splocstart;
  int splocend;


  interrupt(0x21, 0x0, "Masukkan Nama: ", 0, 0);
  interrupt(0x21, 0x1, buf, 0, 0);
  strcpy(&name, buf);
  strcpy(&cwd, "/");
  interrupt(0x21, 0x0, "Halo ", 0, 0);
  interrupt(0x21, 0x0, buf, 0, 0);
  interrupt(0x21, 0x0, ", Selamat Datang di FujOShi, Enjoy your Stay!", 0, 0);
  endl; endl;
  while (true) {
    interrupt(0x21, 0x0, "FujOShi@", 0, 0);
    interrupt(0x21, 0x0, name, 0, 0);
    interrupt(0x21, 0x0, ":", 0, 0);
    interrupt(0x21, 0x0, cwd, 0, 0);
    interrupt(0x21, 0x0, "$ ", 0, 0);
    interrupt(0x21, 0x1, buf, 0, 0);
    interrupt(0x21, 0x0, buf, 0, 0);
    endl;
    splocstart = 0;
    splocend = 0;
    while (buf[splocend] != ' ' && buf[splocend] != '\0') splocend++;
    memcpy(&args1, buf, splocend);
    args1[splocend] = '\0';
    if (strcmp("cd", args1)) {
    } else if (strcmp("ls", args1)) {
        i = 0;
        cwd_id = getinodecwd(cwd);
        // folder search
        for (i = 0; i < 64; i++) {
          memcpy(&nebuff, &node_fs_1.nodes[i], sizeof(struct node_entry));
          if (nebuff.sector_entry_index == FS_NODE_S_IDX_FOLDER) {
            interrupt(0x21, 0x0, nebuff.name, 0, 0); endl;
          }
        }
    for (i = 0; i < 64; i++) {
    memcpy(&nebuff, &node_fs_2.nodes[i], sizeof(struct node_entry));
    if (nebuff.parent_node_index == cwd_id && nebuff.sector_entry_index == FS_NODE_S_IDX_FOLDER) {
      interrupt(0x21, 0x0, nebuff.name, 0, 0); endl;
    }
  }
    } else if (strcmp("mv", buf)) {

    } else if (strcmp("mkdir", args1)) {
      splocstart = splocend+1;
      if (splocstart == 6) {byteprint(splocstart); endl;}
      splocend = splocstart;
      while (buf[splocend] != ' ' && buf[splocend] != '\0') splocend++;
      memcpy(&args1, &buf[splocstart], splocend);
      args1[splocend] = '\0';
      printString(name); endl;
      printString(args1); endl; printString(cwd); endl;
      // PROCESS
      metadata.filesize = 0; 
      byteprint(metadata.filesize); endl;
      // parent searching
      strcpy(&metadata.node_name, args1);
      metadata.parent_index = getinodecwd(cwd);
      if (metadata.parent_index == FS_NODE_P_IDX_ROOT) {
        printString("succeeded"); endl;
      }
      write(&metadata, &return_code);
      /* 
      enum fs_retcode {
    FS_UNKNOWN_ERROR    = -1,
    FS_SUCCESS          = 0,
    FS_R_NODE_NOT_FOUND = 1,
    FS_R_TYPE_IS_FOLDER = 2,

    FS_W_FILE_ALREADY_EXIST   = 3,
    FS_W_NOT_ENOUGH_STORAGE   = 4,
    FS_W_MAXIMUM_NODE_ENTRY   = 5,
    FS_W_MAXIMUM_SECTOR_ENTRY = 6,
    FS_W_INVALID_FOLDER       = 7
};
      */
      if (return_code == FS_SUCCESS) {
        printString("succeeded in creating folder"); endl;
      } else if (return_code == FS_W_FILE_ALREADY_EXIST) {
        printString("failed in creating folder, file already exist"); endl;
      } else if (return_code == FS_W_NOT_ENOUGH_STORAGE) {
        printString("failed in creating folder, not enough storage"); endl;
      } else if (return_code == FS_W_MAXIMUM_NODE_ENTRY) {
        printString("failed in creating folder, maximum node entry"); endl;
      } else if (return_code == FS_W_MAXIMUM_SECTOR_ENTRY) {
        printString("failed in creating folder, maximum sector entry"); endl;
      } else if (return_code == FS_W_INVALID_FOLDER) {
        printString("failed in creating folder, invalid folder"); endl;
      } else {
        printString("failed in creating folder, unknown error"); endl;
      }
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

void splashScreen() {
  char* str = "FujOShi System";
	interrupt(0x10, 0x200, pagenum, 0, 0x222);
	interrupt(0x21, 0x0, str, 0, 0);
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
  // Tambahkan tipe data yang dibutuhkan
  int i;
  int j;
  byte sectorref;
  struct sector_entry fragment;
  struct node_entry nebuff;

  // Masukkan filesystem dari storage ke memori buffer

  // 1. Cari node dengan nama dan lokasi yang sama pada filesystem.
  readSector(&node_fs_buffer, FS_NODE_SECTOR_NUMBER); // first 32 entries
  for (i = 0; i < 64; i++) {
    memcpy(&nebuff, &node_fs_buffer.nodes[i], sizeof(struct node_entry));
    if (strcmp(nebuff.name, metadata->node_name) && nebuff.parent_node_index == metadata->parent_index) {
        sectorref = nebuff.sector_entry_index;
        if (sectorref == FS_NODE_S_IDX_FOLDER) {
          *return_code = FS_R_TYPE_IS_FOLDER;
        } else {
            readSector(&sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);
            memcpy(&fragment, &sector_fs_buffer.sector_list[sectorref], sizeof(struct sector_entry));
            j = 0;
            metadata->filesize = 0;
            while (fragment.sector_numbers[j] != 0x0) {
              readSector(metadata->buffer + j*512, fragment.sector_numbers[j]);
              metadata->filesize += 512;
              j++;
            }
            *return_code = FS_SUCCESS;
        }
        return;
      }
  }
  readSector(&node_fs_buffer, FS_NODE_SECTOR_NUMBER+1);
  for (i = 0; i < 64; i++) {
        memcpy(&nebuff, &node_fs_buffer.nodes[i], sizeof(struct node_entry));
    if (strcmp(nebuff.name, metadata->node_name) && nebuff.parent_node_index == metadata->parent_index) {
      sectorref = nebuff.sector_entry_index;
      if (sectorref == FS_NODE_S_IDX_FOLDER) {
        *return_code = FS_R_TYPE_IS_FOLDER;
      } else {
          readSector(&sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);
          memcpy(&fragment, &sector_fs_buffer.sector_list[sectorref], sizeof(struct sector_entry));
          j = 0;
          metadata->filesize = 0;
          while (fragment.sector_numbers[j] != 0x0) {
            readSector(metadata->buffer + j*512, fragment.sector_numbers[j]);
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
  //    Jika ditemukan node yang cocok, lanjutkan ke langkah ke-2.
  //    Jika tidak ditemukan kecocokan, tuliskan retcode FS_R_NODE_NOT_FOUND
  //    dan keluar.  

  // 2. Cek tipe node yang ditemukan
  //    Jika tipe node adalah file, lakukan proses pembacaan.
  //    Jika tipe node adalah folder, tuliskan retcode FS_R_TYPE_IS_FOLDER
  //    dan keluar.

  // Pembacaan
  // 1. memcpy() entry sector sesuai dengan byte S
  // 2. Lakukan iterasi proses berikut, i = 0..15
  // 3. Baca byte entry sector untuk mendapatkan sector number partisi file
  // 4. Jika byte bernilai 0, selesaikan iterasi
  // 5. Jika byte valid, lakukan readSector() 
  //    dan masukkan kedalam buffer yang disediakan pada metadata
  // 6. Lompat ke iterasi selanjutnya hingga iterasi selesai
  // 7. Tulis retcode FS_SUCCESS dan ganti filesize 
        // pada akhir proses pembacaan.
}



void write(struct file_metadata *metadata, enum fs_retcode *return_code) {
  struct node_filesystem   node_fs_buffer;
  struct sector_filesystem sector_fs_buffer;
  struct map_filesystem    map_fs_buffer;
  // Tambahkan tipe data yang dibutuhkan
  struct node_entry nebuff;
  struct sector_entry sebuff;
  int head;
  int nodeid;
  int sectorid;
  int i;
  int j;
  int pNodeID;
  int emptySector;
  int filesizeinsector;
  byte pb;
  byte sb;
  bool found;

  if (div(metadata->filesize, 512) * 512 == metadata->filesize) {
    filesizeinsector = div(metadata->filesize, 512);
  } else {
    filesizeinsector = div(metadata->filesize, 512) + 1;
  }
  // Masukkan filesystem dari storage ke memori

  readSector(&node_fs_buffer, FS_NODE_SECTOR_NUMBER);
  readSector(&sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);
  readSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);
  // 1. Cari node dengan nama dan lokasi parent yang sama pada node.
  //    Jika tidak ditemukan kecocokan, lakukan proses ke-2.
  //    Jika ditemukan node yang cocok, tuliskan retcode 
  //    FS_W_FILE_ALREADY_EXIST dan keluar. 

  for (i = 0; i < 64; i++) {
    memcpy(&nebuff, &node_fs_buffer.nodes[i], sizeof(struct node_entry));
    if (strcmp(nebuff.name, metadata->node_name) && nebuff.parent_node_index == metadata->parent_index) {
      *return_code = FS_W_FILE_ALREADY_EXIST;
      return;
    }
  }
  printString("got past there"); endl;
  readSector(&node_fs_buffer, FS_NODE_SECTOR_NUMBER+1);
  for (i = 0; i < 64; i++) {
    memcpy(&nebuff, &node_fs_buffer.nodes[i], sizeof(struct node_entry));
    if (strcmp(nebuff.name, metadata->node_name) && nebuff.parent_node_index == metadata->parent_index) {
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
      nodeid = i + 64;
      found = true;
      break;
    }
  }
  if (!found) {
    readSector(&node_fs_buffer, FS_NODE_SECTOR_NUMBER);
    for (i = 0; i < 64; i++) {
      memcpy(&nebuff, &node_fs_buffer.nodes[i], sizeof(struct node_entry));
      if (nebuff.parent_node_index == 0x0) {
        nodeid = i;
        found = true;
        break;
      }
    }
  }

  if (!found) {
    *return_code = FS_W_MAXIMUM_NODE_ENTRY;
    return;
  }
  // 3. Cek dan pastikan entry node pada indeks P adalah folder.
  //    Jika pada indeks tersebut adalah file atau entri kosong,
  //    Tuliskan retcode FS_W_INVALID_FOLDER dan keluar.
  if (pNodeID <= 63) {
    readSector(&node_fs_buffer, FS_NODE_SECTOR_NUMBER);
    memcpy(&nebuff, &node_fs_buffer.nodes[pNodeID], sizeof(struct node_entry));
    if (nebuff.sector_entry_index != 0x0 && nebuff.sector_entry_index != FS_NODE_S_IDX_FOLDER) {
      *return_code = FS_W_INVALID_FOLDER;
      return;
    }
  } else {
    readSector(&node_fs_buffer, FS_NODE_SECTOR_NUMBER+1);
    memcpy(&nebuff, &node_fs_buffer.nodes[pNodeID-64], sizeof(struct node_entry));
    if (nebuff.sector_entry_index != 0x0 && nebuff.sector_entry_index != FS_NODE_S_IDX_FOLDER) {
      *return_code = FS_W_INVALID_FOLDER;
      return;
    }
  }
  // 4. Dengan informasi metadata filesize, hitung sektor-sektor 
  //    yang masih kosong pada filesystem map. Setiap byte map mewakili 
  //    satu sektor sehingga setiap byte mewakili 512 bytes pada storage.
  //    Jika empty space tidak memenuhi, tuliskan retcode
  //    FS_W_NOT_ENOUGH_STORAGE dan keluar.
  //    Jika ukuran filesize melebihi 8192 bytes, tuliskan retcode
  //    FS_W_NOT_ENOUGH_STORAGE dan keluar.
  //    Jika tersedia empty space, lanjutkan langkah ke-5.
  if (metadata->filesize > (unsigned int)8192) {
    byteprint(metadata->filesize); endl;
    printString("GOT HERE BITCH"); endl;
    *return_code = FS_W_NOT_ENOUGH_STORAGE;
    return;
  }

  emptySector = 0;
  for (i = 0; i < 512; i++) {
    if (map_fs_buffer.is_filled[i] == 0x0) {
      emptySector+=1;
    }
  }

  if (emptySector < filesizeinsector) {
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
      sectorid = i;
      found = true;
      break;
    }
  }

  if (!found && metadata->filesize > 0) {
    *return_code = FS_W_MAXIMUM_SECTOR_ENTRY;
    return;
  }
  // Penulisan
  // 1. Tuliskan metadata nama dan byte P ke node pada memori buffer
  // 2. Jika menulis folder, tuliskan byte S dengan nilai
  //    FS_NODE_S_IDX_FOLDER dan lompat ke langkah ke-8 
  // 3. Jika menulis file, tuliskan juga byte S sesuai indeks sector

  if (metadata->filesize == 0) {
    pb = metadata->parent_index;
    sb = FS_NODE_S_IDX_FOLDER;
  } else {
    pb = metadata->parent_index;
    sb = sectorid;
  }
  printString("Here: "); printString(metadata->node_name); endl;
  if (nodeid <= 63) {
    readSector(&node_fs_buffer, FS_NODE_SECTOR_NUMBER);
    memcpy(&node_fs_buffer.nodes[nodeid], pb, 1);
    memcpy(&node_fs_buffer.nodes[nodeid] + 1, sb, 1);
    memcpy(&node_fs_buffer.nodes[nodeid] + 2, metadata->node_name, 14);
  } else {
    readSector(&node_fs_buffer, FS_NODE_SECTOR_NUMBER+1);
    memcpy(&node_fs_buffer.nodes[nodeid-64], pb, 1);
    memcpy(&node_fs_buffer.nodes[nodeid-64] + 1, sb, 1);
    memcpy(&node_fs_buffer.nodes[nodeid-64] + 2, metadata->node_name, 14);
  }
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
    memcpy(&sebuff, &sector_fs_buffer.sector_list[sectorid], sizeof(struct sector_entry));
    while (filesizeinsector > 0 && i < 256) {
      if (!map_fs_buffer.is_filled[i]) {
        writeSector(metadata->buffer[j*512], i);
        sebuff.sector_numbers[j] = i;
        j++;
        map_fs_buffer.is_filled[i] = true;
        filesizeinsector -= 1;
      }
      i++;
    }
    memcpy(&sector_fs_buffer.sector_list[sectorid], &sebuff, sizeof(struct sector_entry));
  }

  // 8. Lakukan penulisan seluruh filesystem (map, node, sector) ke storage
  //    menggunakan writeSector() pada sektor yang sesuai
  writeSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);
  writeSector(&node_fs_buffer, FS_NODE_SECTOR_NUMBER);
  writeSector(&sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);
  // 9. Kembalikan retcode FS_SUCCESS
  *return_code = FS_SUCCESS;
  return;
}

