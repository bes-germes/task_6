#include "parser.h"

int symbol = 0;
int t;
int value;
FILE *current_stream;
std::string str;
HashTable HT;

std::string symbol_to_string(int s) {
  switch (s) {
  case SEMICOLON:
    return ";";
  case EQUAL:
    return "=";
  case ID:
    return "ID";
  case DOT:
    return ".";
  case PLUS:
    return "+";
  case INT:
    return "INT";
  case EOF:
    return "EOF";
  default:
    throw std::runtime_error("Unknown s code:" + std::to_string(s) + "\n");
  }
}

int scan_wrapper(std::string custom) {
  auto result = scan();
  std::cout << "Scan... Result:" << symbol_to_string(result) << "; " << custom
            << "\n";
  return result;
}

int scan() {
  str.clear();
  while (1) {
    t = getc(current_stream);
    if (t == ' ' || t == '\t' || t == '\n' || t == '\r' || t == EOF) {
      if (t == EOF)
        return EOF;
    } else if (t == ';' || t == '=' || t == '.' || t == '+') {
      switch (t) {
      case ';':
        return SEMICOLON;
      case '=':
        return EQUAL;
      case '.':
        return DOT;
      case '+':
        return PLUS;
      };
    } else if (t >= '0' && t <= '9') {
      value = t - '0';
      t = getc(current_stream);
      while (t >= '0' && t <= '9') {
        value = value * 10 + t - '0';
        t = getc(current_stream);
      }
      if (t == ';' || t == '=' || t == '.' || t == '+')
        ungetc(t, current_stream);
      return INT;
    } else {
      str.push_back(t);
      while (1) {
        t = getc(current_stream);
        if ((t == ';' || t == '=' || t == '.' || t == '+') ||
            (t == ' ' || t == '\t' || t == '\n' || t == '\r' || t == EOF)) {
          if (t == ';' || t == '=' || t == '.' || t == '+') {
            ungetc(t, current_stream);
          }
          break;
        }
        str.push_back(t);
      }
      std::cout << "Result str:" << str << "\n";
      return ID;
    }
  }
}

//	0. start ::= assignment ; start_opt
//	1. start_opt ::= start | ε
//	2. assignment ::= struct = expression
//	3.1 struct ::= ID struct’
//	3.2 struct’ ::= . struct | ε
//	4.1 expression ::= term expression’
//	4.2 expresstion’ ::= + expression | ε
//	5. term ::= struct | INT

void start() {
  //	0. start ::= assignment ; start_opt
  assignment();

  if (symbol != SEMICOLON)
    throw std::runtime_error(
        "Ожидался ; в start, А получили:" + symbol_to_string(symbol) + "\n");

  symbol = scan_wrapper("rule: start");
  start_opt();
}

void start_opt() {
  //	1. start_opt ::= start | ε
  if (symbol != EOF) {
    start();
  }
}

void assignment() {
  //	2. assignment ::= struct = expression
  auto var = HT.get_or_create(struct_grammar());

  if (symbol != EQUAL)
    throw std::runtime_error("Ожидался = в assignment, А получили:" +
                             symbol_to_string(symbol) + "\n");

  symbol = scan_wrapper("rule: assignment");
  var->set(expression());
}

std::string struct_grammar() {
  //	3.1 struct ::= ID struct’
  if (symbol != ID)
    throw std::runtime_error(
        "Ожидался ID в struct, А получили:" + symbol_to_string(symbol) + "\n");
  std::string name = str;

  symbol = scan_wrapper("rule: struct");
  auto s = struct__grammar();

  if (s.empty()) {
    return name;
  }

  return name + "." + s;
}

std::string struct__grammar() {
  //	3.2 struct’ ::= . struct | ε

  if (symbol != DOT)
    return "";

  symbol = scan_wrapper("rule: struct\'");
  auto result = struct_grammar();

  return result;
}

int expression() {
  //	4.1 expression ::= term expression’
  int left_val = term();

  int right_val = expression__();

  return left_val + right_val;
}

int expression__() {
  //	4.2 expresstion’ ::= + expression | ε
  int result = 0;
  if (symbol != PLUS)
    return result;

  if (symbol != PLUS)
    throw std::runtime_error("Ожидался + в expression\', А получили:" +
                             symbol_to_string(symbol) + "\n");

  symbol = scan_wrapper("rule: expression\'");
  result = expression();

  return result;
}

int term() {
  //	5. term ::= struct | INT
  if (symbol == INT) {
    auto result = value;
    symbol = scan_wrapper("rule: term");
    return result;
  }
  if (symbol == ID) {
    auto result = HT.get(struct_grammar())->get();
    return result;
  }
  throw std::runtime_error("Дошли до конца term:" + symbol_to_string(symbol));
}
