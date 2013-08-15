#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>

typedef struct {
  int absolute, recurse, force;
  char *source, *target;
  struct stat sstat, tstat;
} Arguments;

Arguments args;

void print_help() {
  fprintf(stdout, "Usage: cps -afr SOURCE TARGET\n");
  fprintf(stdout, "%*s\t Use absolute paths instead of relative\n",7,"-a");
  fprintf(stdout, "%*s\t Force create links and directories if already present\n",7,"-f");
  fprintf(stdout, "%*s\t Recurse sub-directories\n",7,"-r");
  fprintf(stdout, "%*s\t Source file/directory\n",7,"SOURCE");
  fprintf(stdout, "%*s\t Target linkname\n",7,"TARGET");
}
int print_error(int exit_code, char message[256], ... ) {
  va_list ap;
  va_start(ap, message);
  int help=va_arg(ap,int);
  va_end(ap);
  fprintf(stderr, "ERROR: %s\n\n", message);
  if(help) print_help();
  exit(exit_code);
}
int make_tree(char *source, char *target) {
  DIR *dp;
  struct dirent *ep;
  struct stat st;
  char buf[PATH_MAX], tbuf[PATH_MAX];
  if(stat(source, &st) != 0) {
    sprintf(buf, "%s: %s", strerror(errno),source);
    print_error(errno, buf, 0);
  }
  if(S_ISDIR(st.st_mode)) {
    //mkdir(target, st.st_mode);
    printf("DIR: %s -> %s\n", source, target);
    dp = opendir(source);
    if(dp != NULL) {
      while(ep = readdir(dp)) {
        if(!strcmp(ep->d_name, ".")) continue;
        if(!strcmp(ep->d_name, "..")) continue;
        snprintf(buf, sizeof(source)+sizeof(ep->d_name), "%s/%s", source, ep->d_name);
        snprintf(tbuf, sizeof(target)+sizeof(ep->d_name), "%s/%s", target, ep->d_name);
        make_tree(buf, tbuf);
      }
      (void) closedir(dp);
    } else {
      print_error(errno, strerror(errno));
    }
  }
  if(S_ISREG(st.st_mode)){
    printf("LNK: %s -> %s\n", source, target);
    //snprintf(buf, sizeof(target)+sizeof(source), "%s/%s", source,target);
  }
}

int main(int argc, char **argv) {
  int c;

  while( (c = getopt(argc, argv, "arf")) != -1) {
    switch(c) {
    case 'a':
      args.absolute = 1;
      break;
    case 'f':
      args.force = 1;
      break;
    case 'r':
      args.recurse = 1;
      break;
    default:
      print_help();
      abort();
    }
  }
  //Get Source Directory or filename
  if(optind < argc) args.source = argv[optind++];
  else print_error(1,"Source Directory/filename not specified");
  //Get Target Directory Name
  if(optind < argc) args.target = argv[optind++];
  else print_error(2,"Target linkname not specified");
  //Ignore all other arguments after source and target

  //Check if SOURCE exists
  if(stat(args.source, &(args.sstat)) != 0) print_error(errno, "No such file/directory specified as SOURCE");

  //make sure TARGET is not present already
  if(stat(args.target, &(args.tstat)) != 0) {
    make_tree(args.source, args.target);
  } else {
    //If force is enabled, recreate link
    if(args.force) make_tree(args.source, args.target);
    else print_error(EEXIST, "TARGET already exists. Use -f to override.");
  }

  exit(0);
}
