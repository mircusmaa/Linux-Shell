#ifndef H_SHELL
#define H_SHELL
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

#include "Sortargv.hpp"

class Shell {
 private:
  std::map<std::string, std::string> map;  // the map store the environ

 public:
  //constructor
  Shell();
  // the execute function
  void execute();
  // the function will do export command
  void do_export(std::vector<std::string> vec);
  // the function can initial environ
  void env_initial();
};
#endif
