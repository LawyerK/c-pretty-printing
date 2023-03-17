/* define __DEBUG__ before including log.h for DEBUGLN to print */
#define __DEBUG__

#include "log.h"

void do_debug() { DEBUGLN("debug log here!"); }

void run_example() {
  int array[50];

  PRINTLN("There are %d items in the array %s. The first item is %d.",
          sizeof array / sizeof *array, "array", array[0]);
  do_debug();
}

int main() {
  PRINTLN("Initializing the script...");
  /* initialization code */
  run_example();

  ERRORLN("ERROR!");

  return 0;
}