#include <stdio.h>

// For example, &etext gives the address of the end of the program text / start
// of initialized data
extern char etext, edata, end;

int main(int argc, char *argv[]) {
  printf("&etext: %p\n", &etext);
  printf("&edata: %p\n", &edata);
  printf("&end  : %p\n", &end);
  return 0;
}
