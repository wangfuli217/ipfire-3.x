#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "setuid.h"

#define BUFFER_SIZE 1024

char command[BUFFER_SIZE]; 

int main(int argc, char *argv[])
{

        if (!(initsetuid()))
                exit(1);

        // Check what command is asked
        if (argc==1)
        {
            fprintf (stderr, "Missing tripwirectrl command!\n");
            return 1;
        }

        if (strcmp(argv[1], "tripwirelog")==0)
        {
        char log; 
            snprintf(command, BUFFER_SIZE-1, "/usr/sbin/twprint -m r --twrfile /var/ipfire/tripwire/report/%s", argv[2]);
            log=safe_system(command);
            printf(command);
            return(log);
        }

        if (strcmp(argv[1], "generatereport")==0)
        {
            safe_system("/usr/sbin/tripwire --check --cfgfile /var/ipfire/tripwire/tw.cfg --polfile /var/ipfire/tripwire/tw.cfg");
            printf(command);
            return 0;
        }

        if (strcmp(argv[1], "updatedatabase")==0)
        {
        char file; 
            file=safe_system("ls -S | tail -1");
            snprintf(command, BUFFER_SIZE-1, "/usr/sbin/tripwire --update --accept-all --cfgfile /var/ipfire/tripwire/tw.cfg --polfile /var/ipfire/tripwire/tw.cfg --local-passphrase %s --twrfile %s", argv[2], file);
            safe_system(command);
            printf(command);
            return 0;
        }

        if (strcmp(argv[1], "keys")==0)
        {
            snprintf(command, BUFFER_SIZE-1, "rm -rf /var/ipfire/tripwire/site.key && /usr/sbin/twadmin --generate-keys --site-keyfile /var/ipfire/tripwire/site.key --site-passphrase %s && chmod 640 /var/ipfire/tripwire/site.key", argv[2]);
            safe_system(command);
            printf(command);
            snprintf(command, BUFFER_SIZE-1, "rm -rf /var/ipfire/tripwire/$(HOSTNAME)-local.key && /usr/sbin/twadmin --generate-keys --local-keyfile /var/ipfire/tripwire/$(HOSTNAME)-local.key --local-passphrase %s && chmod 640 /var/ipfire/tripwire/$(HOSTNAME)-local.key", argv[3]);
            safe_system(command);
            printf(command);
            snprintf(command, BUFFER_SIZE-1, "rm -rf /var/ipfire/tripwire/tw.cfg && /usr/sbin/twadmin --create-cfgfile --cfgfile /var/ipfire/tripwire/tw.cfg --site-keyfile /var/ipfire/tripwire/site.key --site-passphrase %s /var/ipfire/tripwire/twcfg.txt && chmod 640 /var/ipfire/tripwire/tw.cfg", argv[2]);
            safe_system(command);
            printf(command);
            snprintf(command, BUFFER_SIZE-1, "rm -rf /var/ipfire/tripwire/tw.pol && /usr/sbin/twadmin --create-polfile --cfgfile /var/ipfire/tripwire/tw.cfg --site-keyfile /var/ipfire/tripwire/site.key --site-passphrase %s /var/ipfire/tripwire/twpol.txt && chmod 640 /var/ipfire/tripwire/tw.pol", argv[2]);
            safe_system(command);
            printf(command);
            snprintf(command, BUFFER_SIZE-1, "/usr/sbin/tripwire --init --cfgfile /var/ipfire/tripwire/tw.cfg --polfile /var/ipfire/tripwire/tw.cfg --site-passphrase %s", argv[2]);
            safe_system(command);
            printf(command);
            return 0;
        }

        if (strcmp(argv[1], "generatepolicy")==0)
        {
            snprintf(command, BUFFER_SIZE-1, "/usr/sbin/twadmin --generate-polfile --site-keyfile /var/ipfire/tripwire/site.key --site-passphrase %s --polfile /var/ipfire/tripwire/tw.pol --cfgfile /var/ipfire/tripwire/tw.cfg /var/ipfire/tripwire/twpol.txt", argv[2]);
            safe_system(command);
            printf(command);
            snprintf(command, BUFFER_SIZE-1, "/usr/sbin/tripwire --init -c /var/ipfire/tripwire/tw.cfg -p /var/ipfire/tripwire/tw.cfg --site-passphrase %s", argv[2]);
            safe_system(command);
            printf(command);
            return 0;
        }

        if (strcmp(argv[1], "resetpolicy")==0)
        {
            snprintf(command, BUFFER_SIZE-1, "/usr/sbin/twadmin --generate-polfile --site-keyfile /var/ipfire/tripwire/site.key --site-passphrase %s --polfile /var/ipfire/tripwire/tw.pol --cfgfile /var/ipfire/tripwire/tw.cfg /var/ipfire/tripwire/twpol.default", argv[2]);
            safe_system(command);
            printf(command);
            snprintf(command, BUFFER_SIZE-1, "/usr/sbin/tripwire --init -c /var/ipfire/tripwire/tw.cfg -p /var/ipfire/tripwire/tw.cfg --site-passphrase %s", argv[2]);
            safe_system(command);
            printf(command);
            return 0;
        }

        if (strcmp(argv[1], "readconfig")==0)
        {
            safe_system("/bin/chown nobody:nobody /var/ipfire/tripwire/twcfg.txt");
            printf(command);
            return 0;
        }

        if (strcmp(argv[1], "lockconfig")==0)
        {
            safe_system("/bin/chown root:root /var/ipfire/tripwire/twcfg.txt");
            printf(command);
            return 0;
        }
}