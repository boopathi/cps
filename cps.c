#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

// Flag set by --verbose
static int verbose_flag;

int main(int argc, char **argv) {
  int c;
  while(1) {
    static struct option long_options[] = {
      {"verbose", no_argument, &verbose_flag, 1},
      {"brief", no_argument, &verbose_flag, 0},
      //Other options
      {"absolute", no_argument, 0, 'a'},
      {"recurse", no_argument, 0, 'r'},
      {"force", no_argument, 0, 'f'},
      { 0,0,0,0 }
    };
    int option_index = 0;
    c = getopt_long(argc, argv, "arf", long_options, &option_index);
    if(c == -1) break;
    switch(c) {
    case 0:
      if(long_options[option_index].flag != 0) break;
      printf("option %s", long_options[option_index].name );
      if(optarg) printf(" with arg %s", optarg);
      printf("\n");
      break;
    case 'a':
      puts("option a \n");
      break;
    case 'f':
      puts("option f \n");
      break;
    case 'r':
      puts("option recursive \n");
      break;
    case '?':
      //getopt_long already printed an error message
      break;
    default:
      abort();
    }
  }
  if(verbose_flag) puts("Verbose flag is set");
  if(optind < argc) {
    printf("No arguments ");
    while(optind < argc) printf("%s ", argv[optind++]);
    putchar('\n');
  }
  exit(0);
}
