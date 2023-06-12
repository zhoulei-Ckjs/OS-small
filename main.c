#include "tools/floppy.h"
#include "tools/fileinfo.h"

int main() {

    //模仿dd命令，写入软盘程序
    char* mbr_filepath = "/root/OS/build/boot/mbr.o";
    char* loader_filepath = "/root/OS/build/boot/loader.o";

    Floppy* floppy = create_floppy();

    Fileinfo* mbr_fileinfo = read_file(mbr_filepath);
    write_bootloader(floppy, mbr_fileinfo);

    Fileinfo* loader_fileinfo = read_file(loader_filepath);
    write_floppy_fileinfo(floppy,  loader_fileinfo, 0, 0, 2);

    create_image("/root/OS/os.img", floppy);

    return 0;
}
