#ifndef H_SORTARGV
#define H_SORTARGV

#include <stdio.h>
#include <string.h>

#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

class Command {
 private:
  std::string input;
  std::vector<std::string> vec_cmd;
  std::vector<char *> commandname;
  std::vector<char **> argv;
  std::size_t num_cmd;
  std::size_t num_input;

 public:
  // default constructor
  Command() : num_cmd(0), num_input(0) {}
  // constructor
  Command(std::string input, char * path, std::map<std::string, std::string> map);
  // copy constructor
  Command(const Command & rhs);
  // assignment operator
  Command & operator=(const Command & rhs);
  // destructor
  ~Command();
  // In set command, find the value to be setted
  std::string find_set_value();
  std::string get_cmdname() { return vec_cmd[0]; }
  std::string get_input_ori() { return input; }
  std::vector<char *> get_cmd() { return commandname; }
  std::vector<char **> get_argv_h() { return argv; }
  std::vector<std::string> get_input() { return vec_cmd; }
  std::size_t get_numcmd() { return num_cmd; }
  std::size_t get_numinput() { return num_input; }
  // to check if user input exit command
  bool is_exit();
  // to check if user input cd command
  bool is_cd();
  // to check if user input set command
  bool is_set();
  // to check if user input exportt command
  bool is_export();
  // to check if is var valid in set command
  bool is_var_valid();
  // to check if user input rev command
  bool is_rev();
};
#endif
