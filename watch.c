#include <sys/inotify.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "globals.h"

static
void displayInotifyEvent(struct inotify_event *ev)
{
    printf("    wd =%2d; ", ev->wd);
    if (ev->cookie > 0)
        printf("cookie =%4d; ", ev->cookie);

    printf("mask = ");
    if (ev->mask & IN_ACCESS)        printf("IN_ACCESS ");
    if (ev->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
    if (ev->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (ev->mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
    if (ev->mask & IN_CREATE)        printf("IN_CREATE ");
    if (ev->mask & IN_DELETE)        printf("IN_DELETE ");
    if (ev->mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
    if (ev->mask & IN_IGNORED)       printf("IN_IGNORED ");
    if (ev->mask & IN_ISDIR)         printf("IN_ISDIR ");
    if (ev->mask & IN_MODIFY)        printf("IN_MODIFY ");
    if (ev->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
    if (ev->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
    if (ev->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
    if (ev->mask & IN_MOVE)          printf("IN_MOVE ");
    if (ev->mask & IN_OPEN)          printf("IN_OPEN ");
    if (ev->mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW ");
    if (ev->mask & IN_UNMOUNT)       printf("IN_UNMOUNT ");
    printf("\n");

    if (ev->len > 0)
        printf("        name = %s\n", ev->name);
}

int create_file_watch(char *path, int flags) {
    int notify_fd = inotify_init1(flags);
    if (notify_fd == -1)
        err_exit("inotify_init failed");

    //     int wd = inotify_add_watch(notify_fd, path, IN_ALL_EVENTS);
    int wd = inotify_add_watch(notify_fd, path, IN_CLOSE_WRITE | IN_MOVE);

    if (wd == -1)
        err_exit("problem with inotify_add_watch");

    return notify_fd;
}

int ends_on(const char *str, const char *pattern)
{
    /// asdfjklö.frag
    ///        | <- strlen - patlen
    int pat_len = strlen(pattern);
    int str_len = strlen(str);

    if (pat_len > str_len)
        return 0;

    int offset = str_len - pat_len;
    str += offset;

    while( *str && *pattern && *str++ == *pattern++ ) {}

    return *str == *pattern;
}

int should_reload(int notify_fd)
{
    struct inotify_event *event;
    int frag_files_changed = 0;
    const int buflen = 10 * (sizeof(struct inotify_event) + NAME_MAX + 1);
    char buf[buflen] __attribute__ ((aligned(8)));

    ssize_t bytes_read = read(notify_fd, buf, buflen);

    if (bytes_read < 0)
        return 0;

    if (bytes_read == 0)
        err_exit("read() from inotify fd returned %ld!", bytes_read);

    printf("Read %ld bytes from inotify fd\n", bytes_read);

    for (char *p = buf; p < buf + bytes_read; ) {
        printf(".");
        fflush(stdout);
        struct inotify_event *event = (struct inotify_event *) p;

        if ( event->len > 0 && ends_on(event->name, ".frag") )
            frag_files_changed++;
//         displayInotifyEvent(event);

        p += sizeof(struct inotify_event) + event->len;
    }

    printf("frag_files_changed %d", frag_files_changed);
    return frag_files_changed;
}

int
_main(int argc, char *argv[])
{

    const int buflen = 10 * (sizeof(struct inotify_event) + NAME_MAX + 1);
    char buf[buflen] __attribute__ ((aligned(8)));

    struct inotify_event *event;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        err_exit("%s pathname\n", argv[0]);

    printf("watching %s\n", argv[1]);
    int notify_fd = create_file_watch(argv[1], 0 /* flags: 0 = blocking*/ );

    while(1) {
        printf("\nWaiting for events...\n");

//         ssize_t bytes_read = read(notify_fd, buf, buflen);
//         if (bytes_read <= 0)
//             err_exit("read() from inotify fd returned %ld!", bytes_read);
//         printf("Read %ld bytes from inotify fd\n", bytes_read);
//         for (char *p = buf; p < buf + bytes_read; ) {
//             event = (struct inotify_event *) p;
//             displayInotifyEvent(event);
//             p += sizeof(struct inotify_event) + event->len;
//         }

        if ( should_reload(notify_fd) )
            printf("  --- yes we should reload --- \n");
        else
            printf("  --- no we should not reload --- \n");
    }

    exit(EXIT_SUCCESS);
}
