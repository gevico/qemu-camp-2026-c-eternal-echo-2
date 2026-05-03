#include "mywc.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  const char* filepath = "./text.txt";

  process_file(filepath);
  return 0;
}
