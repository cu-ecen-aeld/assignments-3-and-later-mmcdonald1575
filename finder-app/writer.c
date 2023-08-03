#include <stdio.h>
#include <syslog.h>

int main(int argc, char **argv) {
    FILE* writefile = NULL;

    openlog(NULL, 0, LOG_USER);

    if (argc != 3) {
        syslog(LOG_ERR, "%s requires 2 arguments", argv[0]);
        goto cleanup_error;
    }

    writefile = fopen(argv[1], "wb");
    if (!writefile) {
        syslog(LOG_ERR, "Could not open the write file: %s", argv[1]);
        goto cleanup_error;
    }

    syslog(LOG_DEBUG, "Writing %s to %s", argv[2], argv[1]);
    if (fprintf(writefile, "%s", argv[2]) < 1) {
        syslog(LOG_ERR, "Could not write %s to file: %s", argv[2], argv[1]);
        goto cleanup_error;
    }

    if (fclose(writefile) != 0) {
        syslog(LOG_ERR, "Could not close the write file: %s", argv[1]);
        writefile = NULL;
        goto cleanup_error;
    }

    closelog();
    return 0;

cleanup_error:
    if (writefile) {
        fclose(writefile);
    }
    closelog();
    return 1;
}

