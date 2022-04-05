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

void respcode(enum fs_retcode return_code) {
  if (return_code == FS_SUCCESS) {
    printString("SUC"); endl;
  } else if (return_code == FS_R_NODE_NOT_FOUND) {
    printString("RNF"); endl;
  } else if (return_code == FS_R_TYPE_IS_FOLDER) {
    printString("RTF"); endl;
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
}

byte getinodecwd(char* cwd) {
  struct node_filesystem node_fs;
  struct node_entry nebuff;
  char str1[64]; int it; int i; int depth; int startslash; int endslash; byte prevparidx;
  bool found = false;
  it = 0; startslash = 0; endslash = 0; depth = 0; prevparidx = 0xFF;
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
        memcpy(str1, &cwd[startslash+1], endslash-startslash-1); // string is target
        str1[endslash-startslash-1] = '\0';
        for (i = 0; i < 64; i++) {
           memcpy(&nebuff, &node_fs.nodes[i], sizeof(struct node_entry));
          //  printString(nebuff.name); sp; printString(str1); endl;
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
  endslash = it;
  memcpy(str1, &cwd[startslash+1], endslash-startslash); // string is target
  str1[endslash-startslash] = '\0';
  for (i = 0; i < 10; i++) {
      memcpy(&nebuff, &node_fs.nodes[i], sizeof(struct node_entry));
      // printString(nebuff.name); sp; printString(str1); endl;
      if (strcmp(nebuff.name, str1) && nebuff.parent_node_index == prevparidx && nebuff.sector_entry_index == FS_NODE_S_IDX_FOLDER) {
        prevparidx = i;
        found = true;
        break;
      }
  }
  if (found) {
    return prevparidx;
  } else {
    return 0x40;
  }
}

char* cdHandler(char* cwd, char* args2) {
  int i; int j; int sp1;
  byte inode;
  char args1[64];
  strcpy(args1, cwd);
  if (strcmp("..", args2)) {
    sp1 = 0;
    i = 0;
    while (cwd[i] != '\0') {
      if (cwd[i] == '/') {
        sp1 = i;
      }
      i++;
    }
    if (sp1 == 0) {
      if (cwd[1] != '\0') {
        strcpy(cwd, "/");
      } else;
    }
    else {
      memcpy(args1, cwd, sp1);
      args1[sp1] = '\0';
      strcpy(cwd, args1);
    }
  } else if (strcmp("/", args2)) {
    strcpy(cwd, "/");
  } else if (args2[0] == '/') {
    strcpy(cwd, args2);
    if (cwd[strlen(cwd)-1] == '/') {
      cwd[strlen(cwd)-1] = '\0';
    }
  } else {
    sp1 = strlen(cwd);
    if (!strcmp(cwd,  "/")) {
      strcpy(&cwd[sp1], "/");
      sp1++;
    }
    strcpy(&cwd[sp1], args2);
    if (cwd[strlen(cwd)-1] == '/') {
      cwd[strlen(cwd)-1] = '\0';
    }
  }
  inode = getinodecwd(cwd);
    // printString(cwd); sp; uintprint(j);endl;
  if (inode == 0x40) {
    interrupt(0x21, 0x0, "DIRNF", 0, 0);
    strcpy(cwd, args1);
  }
  return cwd;
}

void lsHandler(byte* inode) {
  struct node_filesystem node_fs;
  struct node_entry nebuff;
  int cwd_id; int i;

  cwd_id = *inode;
  readSector(&node_fs, FS_NODE_SECTOR_NUMBER);
  readSector(&node_fs.nodes[32], FS_NODE_SECTOR_NUMBER+1);
  
  // folder search
  
  for (i = 0; i < 64; i++) {
    memcpy(&nebuff, &node_fs.nodes[i], sizeof(struct node_entry));
    if (nebuff.parent_node_index == cwd_id && nebuff.sector_entry_index == FS_NODE_S_IDX_FOLDER && nebuff.name[0] != '\0') {
      interrupt(0x21, 0x0, "/", 0, 0);
      interrupt(0x21, 0x0, nebuff.name, 0, 0); endl;
    }
  }
  for (i = 0; i < 64; i++) {
    memcpy(&nebuff, &node_fs.nodes[i], sizeof(struct node_entry));
    if (nebuff.parent_node_index == cwd_id && nebuff.sector_entry_index != FS_NODE_S_IDX_FOLDER && nebuff.name[0] != '\0') {
      interrupt(0x21, 0x0, nebuff.name, 0, 0); endl;
    }
  }
}

void mkdirHandler(char* fname, byte* inode) {
  struct file_metadata metadata;
  enum fs_retcode return_code;
  metadata.parent_index = *inode;
  strcpy(metadata.node_name, fname);
  metadata.filesize = 0;
  write(&metadata, &return_code);
  respcode(return_code);
}

void catHandler(char* fname, byte* inode) {
  struct file_metadata metadata;
  enum fs_retcode return_code;
  metadata.buffer = (byte*)0x2000;
  metadata.parent_index = *inode;
  strcpy(metadata.node_name, fname);
  metadata.filesize = 0;
  read(&metadata, &return_code);
  if (return_code == FS_SUCCESS) {
    interrupt(0x21, 0, metadata.buffer, 0, 0); endl;
  } else {
    interrupt(0x21, 0, "S W W", 0, 0); endl;
    respcode(return_code);
  }

}

void cpHandler(char* src, char* dst, byte* inode) {
  struct file_metadata msrc;
  enum fs_retcode return_code;
  msrc.buffer = (byte*)0x4000;
  strcpy(msrc.node_name, src);
  msrc.parent_index = *inode;
  msrc.filesize = 0;
  // printString("node name: "); printString(msrc.node_name); endl;
  // printString("parent: "); uintprint(msrc.parent_index); endl;
  read(&msrc, &return_code);
  if (return_code == FS_SUCCESS) {
    strcpy(msrc.node_name, dst);
    // printString("node name: "); printString(msrc.node_name); endl;
    // printString("parent: "); uintprint(msrc.parent_index); endl;
    // printString("filesize: "); uintprint(msrc.filesize); endl;
    write(&msrc, &return_code);
    if (return_code == FS_SUCCESS) {
      interrupt(0x21, 0, "FCS", 0, 0); endl;
    } else {
      interrupt(0x21, 0, "W W", 0, 0); endl;
      respcode(return_code);
    }
  } else {
    interrupt(0x21, 0, "W R", 0, 0); endl;
    respcode(return_code);
  }
}

bool strcmpl(char *s1, char *s2) {
  int i = 0;
  if (strlen(s1) == strlen(s2)){
    while (s1[i] != '\0'){
      if (s1[i] != s2[i]){
        // printString("false"); endl;
        return false;
      }
      i++;
    }
    return true;
  } else {
    // printString(s1); sp; printString(s2); endl;
    return false;
  }
}

void shell() {
  // struct node_filesystem node_fs_1;
  // struct node_filesystem node_fs_2;
  // struct node_entry nebuff;
  // int cwd_id;
  // int i;
  char buf[64]; char name[64]; char cwd[64]; char args1[64]; char args2[64];
  int i; int j; byte inode;

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
    if (strcmpl("cd", args1)) {
        strcpy(args2, buf+i+1);
        strcpy(cwd, cdHandler(cwd, args2));
    } else if (strcmpl("ls", args1)) {
        inode = getinodecwd(cwd);
        lsHandler(&inode);  
      // file search
    } else if (strcmpl("mv", args1)) {
    } else if (strcmpl("mkdir", args1)) {
      inode = getinodecwd(cwd);
      strcpy(args2, buf+i+1);
      mkdirHandler(args2, &inode);
      // PROCESS END
    } else if (strcmpl("cat", args1)) {
      inode = getinodecwd(cwd);
      strcpy(args2, buf+i+1);
      catHandler(args2, &inode);
      // j = 0;
      // memcpy(args2, &metadata, 63);
      // args2[63] = '\0';
      // while (args2[0] != '\0') {
      //   interrupt(0x21, 0x0, args2, 0, 0);
      //   j++;
      //   memcpy(args2, (&metadata) + j*64, 63);
      //   args2[63] = '\0';
      // }
    } else if (strcmp("cp", args1)) {
      inode = getinodecwd(cwd);
      j = i+1;
      while (buf[j] != ' ' && buf[j] != '\0') j++;
      memcpy(args1, buf+i+1, j-i-1);
      args1[j-i-1] = '\0';
      strcpy(args2, buf+j+1);
      cpHandler(args1, args2, &inode);
      // clear(&metadata, sizeof(struct file_metadata));
      // metadata.buffer = (byte*)0x2000;
      // metadata.parent_index = getinodecwd(cwd);
      // // GANTI SAMA BUF
      // strcpy(metadata.node_name, "file_idx_63");
      // read(&metadata, &return_code);
      // respcode(return_code);
      // // GANTI SAMA FILE NAME + _2
      // strcpy(metadata.node_name, "file_idx_63_2");
      // write(&metadata, &return_code);
      // respcode(return_code);
    } else {
      interrupt(0x21, 0x0,"Command not Recognized", 0, 0); endl;
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
          printString("INVINT");
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
    if (string[i] == '\n' && string[i-1] != '\r') {
      interrupt(0x10, 0xE00 + '\r', pagenum, 0, 0);
    }
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
        } else if (c == 0x2B) {
            interrupt(0x10, 0xE00+0x5F, pagenum, 0, 0);
            toread[count] = 0x5F;
            count++;
        } else if (c == 0x7E) {
            interrupt(0x10, 0xE00+0x2B, pagenum, 0, 0);
            toread[count] = 0x2B;
            count++;
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
  readSector(&node_fs_buffer.nodes[32], FS_NODE_SECTOR_NUMBER+1);
  readSector(&sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);
  // 1. Cari node dengan nama dan lokasi yang sama pada filesystem.

  metadata->filesize = 0;
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
            // printString("whatsector r: "); sp;
            while (sebuff.sector_numbers[j] != 0x0 && j < 16) {
              // uintprint(sebuff.sector_numbers[j]); sp;
              readSector(metadata->buffer + j*512, sebuff.sector_numbers[j]);
              metadata->filesize += 512;
              j++;
            }
            // endl;
            // printString("mtdt info r"); endl;
            // printString(metadata->node_name); endl;
            // uintprint(metadata->parent_index); endl;
            // uintprint(metadata->filesize); endl;
            // uintprint(i); endl;
            // uintprint(sectorref); endl;
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
    if (strcmp(nebuff.name, metadata->node_name) && nebuff.parent_node_index == metadata->parent_index) {
      if (nebuff.sector_entry_index == FS_NODE_S_IDX_FOLDER && metadata->filesize == 0) {
        *return_code = FS_W_FILE_ALREADY_EXIST;
        return;
      } else if (nebuff.sector_entry_index != FS_NODE_S_IDX_FOLDER && metadata->filesize > 0) {
        *return_code = FS_W_FILE_ALREADY_EXIST;
        return;
      }
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

  if (!found && metadata->filesize == 0) {
    *return_code = FS_W_MAXIMUM_NODE_ENTRY;
    return;
  }
  // 3. Cek dan pastikan entry node pada indeks P adalah folder.
  //    Jika pada indeks tersebut adalah file atau entri kosong,
  //    Tuliskan retcode FS_W_INVALID_FOLDER dan keluar.
  if (metadata->parent_index != 0xFF) {
    memcpy(&nebuff, &node_fs_buffer.nodes[metadata->parent_index], sizeof(struct node_entry));
    if (nebuff.sector_entry_index == 0x0 || nebuff.sector_entry_index != FS_NODE_S_IDX_FOLDER) {
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
    if (sebuff.sector_numbers[0] == 0x0) {
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

  if (metadata->filesize == 0) {
    pb = metadata->parent_index;
    sb = FS_NODE_S_IDX_FOLDER;
  } else {
    pb = metadata->parent_index;
    sb = isect;
  }

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
    // printString(metadata->buffer); endl; printString("gg"); endl;
    // return;
    // printString("whatsector w: "); sp;
    while (fzins > 0 && i < 256) {
      if (!map_fs_buffer.is_filled[i]) {
        // uintprint(i); sp;
        writeSector(&metadata->buffer[j*512], i);
        sebuff.sector_numbers[j] = i;
        j++;
        map_fs_buffer.is_filled[i] = true;
        fzins -= 1;
      }
      i++;
    }
    // endl;
    memcpy(&sector_fs_buffer.sector_list[isect], &sebuff, sizeof(struct sector_entry));
  }

  // // 8. Lakukan penulisan seluruh filesystem (map, node, sector) ke storage
  // //    menggunakan writeSector() pada sektor yang sesuai
  // printString("mtdt info w"); endl;
  // printString(metadata->node_name); endl;
  // uintprint(metadata->parent_index); endl;
  // uintprint(metadata->filesize); endl;
  // uintprint(inode); endl;
  // uintprint(isect); endl;
  writeSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);
  writeSector(&node_fs_buffer, FS_NODE_SECTOR_NUMBER);
  writeSector(&node_fs_buffer.nodes[32], FS_NODE_SECTOR_NUMBER + 1);
  writeSector(&sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);
  // // 9. Kembalikan retcode FS_SUCCESS
  *return_code = FS_SUCCESS;
  return;
}

