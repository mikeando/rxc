#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>

#include "clar/clar.h"

int main(int argc, char* argv[]) {

  int res = clar_test(argc,argv);

//  char c;
//  read(0, &c, 1);
  return res;
}

// vim: sw=2 ts=2 expandtab
