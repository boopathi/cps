#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc, char **argv) {
  int c, index;
  typedef struct {
    int absolute, recurse, force;
    char *source, *target;
  } Arguments;
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
      abort();
    }
  }
  //Get Source Directory or filename
  if(optind < argc) args.source = argv[optind++];
  else abort();
  //Get Target Directory Name
  //Ignore all other arguments after source and target
  if(optind < argc) args.target = argv[optind++];
  else abort();

  printf("%s %s\n", args.source, args.target);

  exit(0);
}
