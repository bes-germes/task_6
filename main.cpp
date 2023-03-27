#include "parser.h"
#include <string.h>

extern int symbol;
extern int value;
extern FILE *current_stream;
extern HashTable HT;

void write_string_to_stdin(const std::string &str) {
  current_stream = fmemopen((void *)str.c_str(), str.size(), "r+");
}

int main() {
  std::string input = "a.b=1+2+2;a.cd.efg=a.b+3;a.c=a.b;a.cd =a.cd.efg + 2;";
  write_string_to_stdin(input);
  std::cout << input << "\n\n";

  symbol = scan_wrapper();
  start();
  HT.print();

  if (!(symbol == EOF || input.size() == ftell(current_stream)))
    throw std::runtime_error("Поток не дочитан до конца");

  printf("\n");
  return 0;
}
