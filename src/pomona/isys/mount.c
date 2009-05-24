
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "mount.h"

static int mkdirIfNone(char * directory);

static int readFD(int fd, char **buf) {
    char *p;
    size_t size = 4096;
    int s, filesize = 0;

    *buf = calloc(4096, sizeof (char));
    if (*buf == NULL)
        abort();

    do {
        p = &(*buf)[filesize];
        s = read(fd, p, 4096);
        if (s < 0)
            break;

        filesize += s;
        if (s == 0)
           break;

        size += s;
        *buf = realloc(*buf, size);
        if (*buf == NULL)
            abort();
    } while (1);

    if (filesize == 0 && s < 0) {
        free(*buf);
        *buf = NULL;
        return -1;
    }

    return filesize;
}

int doPwMount(char *dev, char *where, char *fs, char *options, char **err) {
    int rc, child, status, pipefd[2];
    char *opts = NULL, *device;
    
    if (mkdirChain(where))
        return MOUNT_ERR_ERRNO;

    if (strstr(fs, "nfs")) {
        if (options) {
            if (asprintf(&opts, "%s,nolock", options) == -1) {
                fprintf(stderr, "%s: %d: %s\n", __func__, __LINE__,
                        strerror(errno));
                fflush(stderr);
                abort();
            }
        } else {
            opts = strdup("nolock");
        }
        device = strdup(dev);
    } else {
        if ((options && strstr(options, "bind") == NULL) &&
            strncmp(dev, "LABEL=", 6) && strncmp(dev, "UUID=", 5) &&
            *dev != '/') {
           if (asprintf(&device, "/dev/%s", dev) == -1) {
               fprintf(stderr, "%s: %d: %s\n", __func__, __LINE__,
                       strerror(errno));
               fflush(stderr);
               abort();
           }
        } else {
           device = strdup(dev);
        }
        if (options)
            opts = strdup(options);
    }

    if (pipe(pipefd))
        return MOUNT_ERR_ERRNO;

    if (!(child = fork())) {
        int fd;

        close(pipefd[0]);

        /* Close stdin entirely, redirect stdout to tty5, and redirect stderr
         * to a pipe so we can put error messages into exceptions.  We'll
         * only use these messages should mount also return an error code.
         */
        fd = open("/dev/tty5", O_RDONLY);
        close(STDIN_FILENO);
        dup2(fd, STDIN_FILENO);
        close(fd);

        fd = open("/dev/tty5", O_WRONLY);
        close(STDOUT_FILENO);
        dup2(fd, STDOUT_FILENO);

        dup2(pipefd[1], STDERR_FILENO);

        if (opts) {
            rc = execl("/bin/mount",
                       "/bin/mount", "-n", "-t", fs, "-o", opts, device, where, NULL);
            exit(1);
        }
        else {
            rc = execl("/bin/mount", "/bin/mount", "-n", "-t", fs, device, where, NULL);
            exit(1);
        }
    }

    close(pipefd[1]);

    if (err != NULL)
        rc = readFD(pipefd[0], err);

    close(pipefd[0]);
    waitpid(child, &status, 0);

    free(opts);
    free(device);
    if (!WIFEXITED(status) || (WIFEXITED(status) && WEXITSTATUS(status)))
       return MOUNT_ERR_OTHER;

    return 0;
}

int mkdirChain(char * origChain) {
    char * chain;
    char * chptr;

    chain = alloca(strlen(origChain) + 1);
    strcpy(chain, origChain);
    chptr = chain;

    while ((chptr = strchr(chptr, '/'))) {
        *chptr = '\0';
        if (mkdirIfNone(chain)) {
            *chptr = '/';
            return MOUNT_ERR_ERRNO;
        }
    
        *chptr = '/';
        chptr++;
    }

    if (mkdirIfNone(chain))
        return MOUNT_ERR_ERRNO;

    return 0;
}

static int mkdirIfNone(char * directory) {
    int rc, mkerr;
    char * chptr;

    /* If the file exists it *better* be a directory -- I'm not going to
       actually check or anything */
    if (!access(directory, X_OK))
        return 0;

    /* if the path is '/' we get ENOFILE not found" from mkdir, rather
       then EEXIST which is weird */
    for (chptr = directory; *chptr; chptr++)
        if (*chptr != '/')
            break;
    if (!*chptr)
        return 0;

    rc = mkdir(directory, 0755);
    mkerr = errno;

    if (!rc || mkerr == EEXIST)
        return 0;

    return MOUNT_ERR_ERRNO;
}
