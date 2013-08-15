#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>


typedef struct {
  int absolute, recurse, force;
  char *source, *target;
  struct stat sstat, tstat;
} Arguments;

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

void create_link(Arguments args) {
  if(S_ISREG(args.sstat.st_mode)) {
    //Regular file
    //Just create a softlink
  }
  if(S_ISDIR(args.sstat.st_mode)) printf("Directory");
}

int main(int argc, char **argv) {
  int c, index;
  Arguments args;

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

  //Finally the implementation
  if(stat(args.source, &(args.sstat)) != 0) print_error(errno, "No such file/directory specified as SOURCE");

  //make sure TARGET is not present already
  if(stat(args.target, &(args.tstat)) != 0) {
    create_link(args);
  } else {
    if(args.force) create_link(args);
    else print_error(4,"TARGET already exists. Use -f to override.");
  }

  exit(0);
}
