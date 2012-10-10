#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
 
#include <sys/inotify.h>
 
void printhelp(FILE *out, char *app) {
    fprintf(out, "Usage: %s <command> <dirs to watch>\n", app);
}

int main(int argc, char **argv) {
    int fdnotify = -1;
    int i;
 
    fdnotify = inotify_init();
    if (fdnotify < 0) {
        fprintf(stderr, "inotify_init failed: %s\n", strerror(errno));
        exit(-1);
    }
 
    if (argc < 3) {
        fprintf(stderr, argv[0]);
        exit(-1);
    }
 
    for(i = 2;i<argc;i++) {
        int wd = inotify_add_watch(fdnotify, argv[i], IN_MODIFY);
        fprintf(stdout, "watch descriptor %d\n", wd);
        fflush(stdout);

        if (wd < 0) {
            fprintf(stderr, "inotify_add_watch failed: %s\n", strerror(errno));
        }
    }
 
    while(1) {
        char buffer[4096];
        struct inotify_event *event = NULL;
        int exec = 0;
        uint32_t cookie = 0;
 
        int len = read(fdnotify, buffer, sizeof(buffer));
        if (len < 0) {
            fprintf(stderr, "read: %s\n", strerror(errno));
            exit(-1);
        }
 
        event = (struct inotify_event *) buffer;
        while(event != NULL) {
            if ( (event->mask & IN_MODIFY) ) {
                cookie = event->cookie;
                printf("File Modified: %s\n", event->name);
                printf("File Cookie: %d\n", event->cookie);
                exec = 1;
            } else {
                printf("Unknown Mask 0x%d\n", event->mask);
                printf("Mask 0x%d\n",IN_MODIFY);
            }
                 
            /* Move to next struct */  
            len -= sizeof(*event) + event->len;
            if (len > 0)
                event = ((void *) event) + sizeof(event) + event->len;
            else
                event = NULL;
        }
     
        if (exec)   
            system(argv[1]);
    }
 
    return 0;
}
