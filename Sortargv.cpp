#include "Sortargv.hpp"

#include <string.h>

#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

//this function can change $var in user input to it's key value.
std::string transfer_input(std::string input, std::map<std::string, std::string> map) {
  std::size_t i = 0;
  while (i < input.length()) {
    if (input[i] == '$' && i != input.length() - 1 &&
        (isalnum(input[i + 1]) || input[i + 1] == '_')) {
      // start searching when encounter a $(must followed by letters,_,numbers)
      std::size_t j = i;
      std::string choose;
      std::size_t last;
      while (j < input.length()) {
        std::string temp = input.substr(i + 1, j - i);
        if (map.find(temp) != map.end()) {
          choose = temp;
          last =
              j;  // every time a new var can be found in map, update it to string choose.
        }
        j++;
        if (!(isalnum(input[j]) || input[j] == '_')) {
          break;  // if encounter a invalid letter, break
        }
      }
      if (choose.length() != 0) {
        input.erase(i, last - i + 1);
        input.insert(i, map[choose]);
      }
    }
    i++;
  }
  return input;
}

/*  Strat here   
the parse string function consists of two parts:
   map_flag and parseline_help */

/* The function map_flag is the first level of parseline function can detect backslashes 
and escaped quotation. blackslah will be erased and the literal quotation mark will be 
flaged and saved in the map */
std::map<std::size_t, std::size_t> map_flag(std::string & str) {
  std::size_t i = 0;
  std::map<std::size_t, std::size_t> map;
  while (i < str.length() - 1) {
    if (str[i] == '\\') {
      if (str[i + 1] == '\\') {
        str.erase(i, 1);
        i++;  // if str[i] is "\",erase it. if str[i+1] is "\", i++. Store the i in map otherwise.
      }
      else {
        str.erase(i, 1);
        map[i] = 1;
      }
    }
    else {
      i++;
    }
  }
  return map;
}

/* The functoin parseline_help is the next level parseline functoin, it can split one string 
into different arguments by the help of map build the map_flag function */
std::vector<std::string> parseline_help(std::string str) {
  std::map<std::size_t, std::size_t> map = map_flag(str);
  std::vector<std::string> ans;
  std::vector<std::string> empty;
  std::size_t i = 0;

  while (i < str.length()) {
    if (str[i] == ' ' &&
        map.find(i) ==
            map.end()) {  // if encount a space not flaged by map, just continue.
      i++;
    }
    else if (
        str[i] == '"' &&
        map.find(i) ==
            map.end()) {  // if encount a not flaged ", build a string and start new loop to search
      std::string temp;
      std::size_t check = 1;
      i++;
      while (i < str.length()) {
        if (str[i] == '"' && map.find(i) == map.end()) {
          /*in new loop,when find a " not flaged by map: if " is followed by space(not flaged),save this string to vec
	    if the number of not flaged " is even, report error if is odd */
          check++;
          if ((str[i + 1] == ' ' && map.find(i + 1) == map.end()) ||
              i == str.length() - 1) {
            if (check % 2) {
              perror("Error,unclosed quotation mark at the end");
              return empty;
            }
            else {
              ans.push_back(temp);
              if (i < str.length() - 1) {
                i++;
              }
              break;
            }
          }
          else {
            i++;
          }
        }
        else if (str[i] == '"' && map.find(i) != map.end()) {
          temp.push_back(str[i]);
          i++;  // if encount a flaged ", save it to string.
        }
        else {
          temp.push_back(str[i]);
          i++;
        }
        if (i == str.length()) {
          // if i has reached the end: report error if the number of " is odd, save this string otherwise
          if ((check % 2) == 1) {
            perror("still error, unclosed quotation");
            return empty;
          }
          else {
            ans.push_back(temp);
            i++;
          }
        }
      }
    }

    else {  // if encount sth else(not space, not unflaged ") build a new string and start search
      std::string temp;
      std::size_t check = 0;
      temp.push_back(str[i]);
      i++;
      if (i == str.length()) {
        ans.push_back(temp);
        break;
      }
      while (i < str.length()) {
        int flag_break = 0;
        if (str[i] == '"' && map.find(i) == map.end()) {
          check++;
          i++;  // when encount a not flaged " in this loop, start a new loop.
          while (i < str.length()) {
            if (str[i] == '"' && map.find(i) == map.end()) {
              check++;
              /* every time encount a not flaged " in new loop": if number of not flaged " is odd, perror. 
              otherwise, break and return a flag. In the outside loop, loop will break as while if flag is detected.*/
              if ((str[i + 1] == ' ' && map.find(i + 1) == map.end()) ||
                  i == str.length() - 1) {
                if (check % 2) {
                  perror("Error,unclosed quotation mark at the end");
                  return empty;
                }
                else {
                  ans.push_back(temp);
                  if (i < str.length() - 1) {
                    i++;
                  }
                  flag_break = 1;  // set the flag means a new argument has been found!
                  break;
                }
              }
              else {
                i++;
                break;
              }
            }
            else if (str[i] == '"' && map.find(i) != map.end()) {
              temp.push_back(str[i]);
              i++;
            }
            else {
              temp.push_back(str[i]);
              i++;
            }
            if (i == str.length()) {
              perror("still error, unclosde quotation");
              return empty;
            }
          }
        }
        else if (str[i] == ' ' && map.find(i) == map.end()) {
          ans.push_back(temp);
          i++;
          break;  // if encount a not flaged space, push this string to vecotr.
        }
        else {
          temp.push_back(str[i]);
          i++;
        }
        if (flag_break == 1) {
          break;  // to detect flag
        }
        if (i == str.length()) {
          ans.push_back(temp);
        }
      }
    }
  }
  return ans;
}

// to find if commandname(path name) has a / in it.
bool commandname_slash(std::string commandname) {
  std::size_t size = commandname.size();
  for (std::size_t i = 0; i < size; i++) {
    if (commandname[i] == '/') {
      return true;
    }
  }
  return false;
}
/* input is a string of pathname, and a vector of arguments. A char ** argv 
will be built*/
char ** get_argv(std::string commandname, std::vector<std::string> command) {
  std::size_t size = command.size();
  char ** argv = new char *[size + 1];
  argv[0] = strdup(commandname.c_str());
  for (std::size_t i = 1; i < size; i++) {
    argv[i] = strdup(command[i].c_str());
  }
  argv[size] = NULL;
  return argv;
}

/* This functipn can output a vector contains char * path(different directories plus path name) 
when path name has no slash. The input is a vector contains all directories and the size of it */
std::vector<char *> get_cmdname_noslash(std::vector<std::string> vec_path,
                                        std::size_t size) {
  std::vector<char *> vec_cmdname_noslash;
  for (std::size_t i = 0; i < size; i++) {
    char * cmdname_noslash = new char[vec_path[i].length() + 1];
    strcpy(cmdname_noslash, vec_path[i].c_str());
    vec_cmdname_noslash.push_back(cmdname_noslash);
  }
  return vec_cmdname_noslash;
}

/* The help function to get char**argv (a vector contains all of them). This function will use
function get_argv  */
std::vector<char **> get_argv_noslash(std::vector<std::string> vec_path,
                                      std::vector<std::string> vec_command,
                                      std::size_t size) {
  std::vector<char **> vec_argv_noslash;
  for (std::size_t i = 0; i < size; i++) {
    char ** argv_slash = get_argv(
        vec_path[i],
        vec_command);  // different directories will be put into function get_argv
    vec_argv_noslash.push_back(argv_slash);
  }
  return vec_argv_noslash;
}
// This function can help split a path into different directories and save them into a vector.
std::vector<std::string> parseline_PATH(char * path) {
  std::vector<std::string> vec;
  int i = 0;
  while (path[i] != '\0') {
    std::string temp;
    while (path[i] != '\0') {
      temp.push_back(path[i]);
      i++;
      if (path[i] == ':') {
        vec.push_back(temp);
        i++;
        break;
      }
      if (path[i] == '\0') {
        vec.push_back(temp);
        break;
      }
    }
  }
  return vec;
}
// This function can put the directory and the command(path name) together
std::vector<std::string> path_plus_command(std::vector<std::string> path,
                                           std::string command) {
  std::size_t size = path.size();
  for (std::size_t i = 0; i < size; i++) {
    path[i].append("/");
    path[i].append(command);
  }
  return path;
}
// This is the help function to get a vector contains: directory+path name
std::vector<std::string> parseline_PATH_help(char * path, std::string commandname) {
  std::vector<std::string> vec1 = parseline_PATH(path);
  std::vector<std::string> vec2 = path_plus_command(vec1, commandname);
  return vec2;
}
// The constructor of class
Command::Command(std::string str, char * path, std::map<std::string, std::string> map) {
  input = transfer_input(str, map);
  vec_cmd = parseline_help(input);
  num_input = vec_cmd.size();
  if (num_input != 0) {                   // if user input is invalid
    if (commandname_slash(vec_cmd[0])) {  //when path name has slash
      char * cmd_slash = new char[vec_cmd[0].length() + 1];
      strcpy(cmd_slash, vec_cmd[0].c_str());
      commandname.push_back(cmd_slash);
      num_cmd = 1;
      char ** argv_slash = get_argv(vec_cmd[0], vec_cmd);
      argv.push_back(argv_slash);
    }
    else {  // when path name has no slash
      std::vector<std::string> vec_path = parseline_PATH_help(path, vec_cmd[0]);
      std::size_t size = vec_path.size();
      commandname = get_cmdname_noslash(vec_path, size);
      argv = get_argv_noslash(vec_path, vec_cmd, size);
      num_cmd = commandname.size();
    }
  }
  else {  // if user input is not valid, we set everyhing to 0 and empty.
    num_cmd = 0;
    std::vector<char *> commandname;
    std::vector<char **> argv;
  }
}
// Assignment constructor
Command & Command::operator=(const Command & rhs) {
  Command * temp = new Command(rhs);
  std::swap(this->input, temp->input);
  std::swap(this->vec_cmd, temp->vec_cmd);
  std::swap(this->commandname, temp->commandname);
  std::swap(this->argv, temp->argv);
  std::swap(this->num_cmd, temp->num_cmd);
  std::swap(this->num_input, temp->num_input);
  return *this;
}
// Copy constructor
Command::Command(const Command & rhs) {
  Command * temp = new Command(rhs);
  std::swap(input, temp->input);
  std::swap(vec_cmd, temp->vec_cmd);
  std::swap(commandname, temp->commandname);
  std::swap(argv, temp->argv);
  std::swap(num_cmd, temp->num_cmd);
  std::swap(num_input, temp->num_input);
}
// to detect if user wanna exit
bool Command::is_exit() {
  std::string temp("exit");
  if (vec_cmd[0].compare(temp) == 0) {
    return true;
  }
  return false;
}
// Destructor
Command::~Command() {
  for (std::size_t i = 0; i < argv.size(); i++) {
    for (std::size_t j = 0; j < num_input; j++) {
      free(argv[i][j]);
    }
    delete[] argv[i];
  }
  for (std::size_t z = 0; z < commandname.size(); z++) {
    delete[] commandname[z];
  }
}
// to detect if user input cd
bool Command::is_cd() {
  std::string temp("cd");
  if (vec_cmd[0].compare(temp) == 0) {
    return true;
  }
  return false;
}
// to detect if user input set
bool Command::is_set() {
  std::string temp("set");
  if (vec_cmd[0].compare(temp) == 0) {
    return true;
  }
  return false;
}
// to detect if user input export
bool Command::is_export() {
  std::string temp("export");
  if (vec_cmd[0].compare(temp) == 0) {
    return true;
  }
  return false;
}
// to detect if user input rev
bool Command::is_rev() {
  std::string temp("rev");
  if (vec_cmd[0].compare(temp) == 0) {
    return true;
  }
  return false;
}
// is var user want to set is valid
bool Command::is_var_valid() {
  std::size_t i = 0;
  while (i < vec_cmd[1].length()) {
    if (!(isalnum(vec_cmd[1][i]) || vec_cmd[1][i] == '_')) {
      return false;
    }
    i++;
  }
  return true;
}
// function to find the value of var the user wanna set
std::string Command::find_set_value() {
  std::size_t found_cmd0 = input.find(vec_cmd[0]);
  std::size_t l_endofcmd0 = found_cmd0 + vec_cmd[0].length();
  std::string temp_fromcmd1 = input.substr(l_endofcmd0);
  std::size_t found_cmd1 = temp_fromcmd1.find(vec_cmd[1]);
  std::size_t l_endofcmd1 = found_cmd1 + vec_cmd[1].length();
  std::string temp_fromcmd2 = input.substr(l_endofcmd0 + l_endofcmd1);
  std::size_t found = temp_fromcmd2.find_first_not_of(" ");
  std::size_t index_var = l_endofcmd0 + l_endofcmd1 + found;
  std::string ans = input.substr(index_var);
  return ans;
}
