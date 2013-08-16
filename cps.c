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
  int absolute, skip, verbose;
  char *source, *target;
  struct stat sstat, tstat;
} Arguments;

Arguments args;

typedef struct {
  char d[1024][2][256];
  int l;
} Subs;

Subs rep;

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
  int isexit=va_arg(ap,int);
  va_end(ap);
  fprintf(stderr, "ERROR: %s\n\n", message);
  if(help) print_help();
  if(isexit) exit(exit_code);
}

char *
str_replace ( const char *string, const char *substr, const char *replacement ){
  char *tok = NULL;
  char *newstr = NULL;
  char *oldstr = NULL;

  /* if either substr or replacement is NULL, duplicate string a let caller handle it */
  if ( substr == NULL || replacement == NULL ) return strdup (string);
  newstr = strdup (string);
  while ( (tok = strstr ( newstr, substr ))){
    oldstr = newstr;
    newstr = malloc ( strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) + 1 );
    /*failed to alloc mem, free old string and return NULL */
    if ( newstr == NULL ){
      free (oldstr);
      return NULL;
    }
    memcpy ( newstr, oldstr, tok - oldstr );
    memcpy ( newstr + (tok - oldstr), replacement, strlen ( replacement ) );
    memcpy ( newstr + (tok - oldstr) + strlen( replacement ), tok + strlen ( substr ), strlen ( oldstr ) - strlen ( substr ) - ( tok - oldstr ) );
    memset ( newstr + strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) , 0, 1 );
    free (oldstr);
  }
  return newstr;
}

char *apply_subs(const char *p) {
  int i;
  char *q[2];
  q[0] = str_replace(p, rep.d[0][0], rep.d[0][1]);
  for(i=1;i<rep.l;i++)
    q[i%2] = str_replace(q[(i+1)%2], rep.d[i][0], rep.d[i][1]);
  return q[(i+1)%2];
}

int make_tree(char *source, char *target) {
  DIR *dp;
  struct dirent *ep;
  struct stat st;
  char buf[PATH_MAX], tbuf[PATH_MAX], *tmp;
  if(stat(source, &st) != 0) {
    sprintf(buf, "%s: %s", strerror(errno),source);
    print_error(errno, buf, 0);
  }
  if(S_ISDIR(st.st_mode)) {
    mkdir(target, st.st_mode);
    if(args.verbose) printf("DIR: %s -> %s\n", source, target);
    dp = opendir(source);
    if(dp != NULL) {
      while(ep = readdir(dp)) {
        if(!strcmp(ep->d_name, ".")) continue;
        if(!strcmp(ep->d_name, "..")) continue;
        snprintf(buf, sizeof(source)+sizeof(ep->d_name), "%s/%s", source, ep->d_name);
        tmp = apply_subs(ep->d_name);
        snprintf(tbuf, sizeof(target)+sizeof(tmp), "%s/%s", target, tmp);
        make_tree(buf, tbuf);
      }
      (void) closedir(dp);
    } else {
      print_error(errno, strerror(errno), 0);
    }
  }
  if(S_ISREG(st.st_mode)){
    if(args.verbose) printf("LNK: %s -> %s\n", source, target);
    //create symlink here
    //snprintf(buf, sizeof(target)+sizeof(source), "%s/%s", source,target);
    if(symlink(source, target) != 0 )
      print_error(errno, strerror(errno), 0, !args.skip);
  }
}

int main(int argc, char **argv) {
  int c,i;
  char *s, *where, *tmp;

  rep.l = 0;
  args.absolute = 0;
  args.skip = 0;
  args.verbose = 0;

  while( (c = getopt(argc, argv, "akvs:")) != -1) {
    switch(c) {
    case 'a':
      args.absolute = 1;
      break;
    case 'k':
      args.skip = 1;
      break;
    case 'v':
      args.verbose = 1;
    case 's':
      optind--;
      tmp = argv[optind++];
      s = malloc(strlen(tmp) + 1);
      for(i=0;i<strlen(tmp);i++) s[i] = tmp[i];
      i=0;
      while(*s != '/' && *s != '\0') rep.d[rep.l][0][i++] = *s++;
      s++; i=0;
      while(*s != '/' && *s != '\0') rep.d[rep.l][1][i++] = *s++;
      rep.l++;
      *s = 0;
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
  if(stat(args.target, &(args.tstat)) != 0)
    make_tree(args.source, args.target);
  else print_error(EEXIST, "TARGET already exists. Use -f to override.");

  return 0;
}
