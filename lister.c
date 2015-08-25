/*

Fast linux lister that avoids conventional ls bottlenecks by using system calls directly with large buffer
Inspiration: https://www.olark.com/developers-corner/you-can-list-a-directory-with-8-million-files-but-not-with-ls
Source: http://man7.org/linux/man-pages/man2/getdents.2.html

compile: gcc lister.c -o lister

usage:
./lister [-l | -c] dir

to list to stdout all files:
./lister -l /mydir

to count files:
./lister -c /mydir

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#define _GNU_SOURCE
#include <dirent.h>     /* Defines DT_* constants */
#include <inttypes.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#define handle_error(msg) \
	   do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct linux_dirent {
    long           d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    char           d_name[];
};

// create a big buffer (5MB) default is only 32K

#define BUF_SIZE 1024*1024*5

int main(int argc, char *argv[])
{
    int fd, nread;
    char buf[BUF_SIZE];
    struct linux_dirent *d;
    int bpos;
    char d_type;
    unsigned long count=0;
    char optionList[2] = "-l";
    char optionCount[2] = "-c";
    char userOption[2];

    // get user option
    strcpy(userOption, argv[1]);

    // open the dir
    fd = open(argc > 1 ? argv[2] : ".", O_RDONLY | O_DIRECTORY);

    if (fd == -1)
        handle_error("open");

   // traverse dir by callling SYS_getdents
    for ( ; ; ) {
        nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
        if (nread == -1)
            handle_error("getdents");

        if (nread == 0) {
            if (strcmp(userOption, optionCount) == 0) {printf("%lu\n", count);}
            break;
        }

        // traverse the filled buffer of linux_dirents
        for (bpos = 0; bpos < nread;) {

            // create the struct and file type
            d = (struct linux_dirent *) (buf + bpos);
            d_type = *(buf + bpos + d->d_reclen - 1);

            // if not deleted and a regular file type print name or count
            if (d->d_ino != 0 && d_type == DT_REG) {
                if (strcmp(userOption, optionList) == 0) {
                    printf("%s\n", (char *) d->d_name);
                }
                else {
                    count+=1;
                }
            }
            bpos += d->d_reclen;
        }
    }

    exit(EXIT_SUCCESS);
}
