#pragma once
#include <exception>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

//	0. start ::= assignment ; start_opt
//	1. start_opt ::= start | ε
//	2. assignment ::= struct = expression
//	3.1 struct ::= ID struct’
//	3.2 struct’ ::= . struct | ε
//	4.1 expression ::= term expression’
//	4.2 expresstion’ ::= + expression | ε
//	5. term ::= struct | INT
//
//	; = ID . + INT

#define SEMICOLON 256 // ;
#define EQUAL 257     // =
#define ID 258
#define DOT 259  // .
#define PLUS 260 // +
#define INT 261

std::string symbol_to_string(int s);

int scan();
int scan_wrapper(std::string s = "");

////

class struct_storage {
  int *value;
  std::string name; // хранится полная строка имени
  std::vector<std::string> name_parts;
  size_t nestingLevel; // уровень вложенности. Если данный элемент родительский,
                       // то 0

  void default_init(const std::string name) {
    this->name = name;
    this->name_parts = std::vector<std::string>();
    std::istringstream iss(name);

    std::string token;
    while (std::getline(iss, token, '.')) {
      this->name_parts.push_back(token);
    }
    this->nestingLevel = this->name_parts.size() - 1;
  }

public:
  size_t get_nesting_level() { return this->nestingLevel; }
  bool is_root() { return this->nestingLevel == 0; }

  void set(const int &value) {
    if (this->value == nullptr)
      this->value = new int;
    *this->value = value;
    std::cout << this->name << "=" << *this->value << "\n";
  }

  int get() {
    if (this->value == nullptr)
      throw std::runtime_error("Переменной не присвоено значение! " +
                               this->name + "\n");
    return *this->value;
  }

  struct_storage(const std::string name_, int *value_ = nullptr)
      : value(value_) {
    default_init(name_);
  };

  struct_storage(const std::string name_, const int value_) {
    this->value = new int;
    *this->value = value_;
    default_init(name_);
  };
};

class HashTable {
  std::map<std::string, struct_storage *> ht;

public:
  void print() {
    std::cout << "\n\nResult:\n";
    for (const auto &[key, value] : this->ht) {
      std::cout << "ID: " << key << ", Value: " << value->get() << std::endl;
    }
  }

  struct_storage *get_or_create(std::string name) {
    std::cout << "get_or_create(" << name << ")\n";
    if (this->ht.find(name) == this->ht.end()) {
      ht[name] = new struct_storage(name);
    }
    return ht[name];
  }

  struct_storage *get(std::string name) {
    if (this->ht.find(name) == this->ht.end()) {
      throw std::runtime_error(
          "Попытка сослаться на необъявленную переменную/структуру:" + name +
          "\n");
    }
    return ht[name];
  }

  HashTable() { ht = std::map<std::string, struct_storage *>(); }
};

void start();
void start_opt();
void assignment();
std::string struct_grammar();
std::string struct__grammar();
int expression();
int expression__();
int term();
