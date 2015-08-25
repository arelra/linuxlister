Fast linux lister that avoids conventional ls bottlenecks by using system calls directly with large buffer

Inspiration: https://www.olark.com/developers-corner/you-can-list-a-directory-with-8-million-files-but-not-with-ls

Source: http://man7.org/linux/man-pages/man2/getdents.2.html

compile:

```gcc lister.c -o lister```

usage:

```./lister [-l | -c] dir```

to list to stdout all files:

```./lister -l /mydir```

to count files:

```./lister -c /mydir```
