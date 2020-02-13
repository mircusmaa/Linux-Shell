#include "Shell.hpp"
// the function can get a map where store the environ
std::map<std::string, std::string> get_map(char ** environ) {
  std::map<std::string, std::string> map;
  int i = 0;
  while (environ[i] != NULL) {
    char * temp = environ[i];
    std::string key;
    std::string value;
    int j = 0;
    while (temp[j] != '\0') {  // to search in char** environ
      key.push_back(temp[j]);
      j++;
      if (temp[j] == '=') {  // = is a sign to find value
        j++;
        while (temp[j] != '\0') {
          value.push_back(temp[j]);
          j++;
        }
      }
    }
    map[key] = value;
    i++;
  }
  return map;
}
// do what cd should do.
void do_cd(std::vector<std::string> str, std::size_t size) {
  int cd = 0;
  if (size == 1) {  // go to /home if user input cd
    std::string parent_directory("/home");
    cd = chdir(parent_directory.c_str());
  }
  else if (size == 2) {
    cd = chdir(str[1].c_str());
  }
  else {
    perror("cd error: more than one directory");
  }
  if (cd < 0) {
    perror("wrong directory");
  }
}
// the function do export!
void Shell::do_export(std::vector<std::string> vec) {
  if (map.find(vec[1]) == map.end()) {  //export when var has already setted
    std::cout << "variable is not yet setted" << std::endl;
  }
  else {
    setenv(vec[1].c_str(), map[vec[1]].c_str(), 1);
  }
}
// Constructor
Shell::Shell() {
  map = get_map(environ);
}
// Intial of env, set ECE551PATH to env.
void Shell::env_initial() {
  char * path = getenv("PATH");
  setenv("ECE551PATH", path, 1);
}
// the main function of execute.
void Shell::execute() {
  pid_t cpid;
  pid_t w;
  int wstatus;
  env_initial();
  while (true) {
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) == 0) {
      perror("getcwd error");
      continue;  // the get the current directory
    };
    char * path = getenv("ECE551PATH");
    std::cout << "ffosh: " << cwd << " $";
    std::string input_temp;
    std::getline(std::cin, input_temp);
    if (std::cin.eof()) {
      break;  // break when user input eof
    }
    if (input_temp.length() == 0) {
      continue;  // when input nothing, do nothing
    }
    if (input_temp.find_first_not_of(" ") == std::string::npos) {
      perror("please input invalid input!");
      continue;  // when user input many space(just space), perror and continue.
    }
    Command command(input_temp, path, map);  //build the class command
    std::string input = command.get_input_ori();
    std::size_t size = command.get_numcmd();
    std::size_t num_input = command.get_numinput();
    std::vector<std::string> vec_input = command.get_input();
    std::vector<char *> vec_cmd = command.get_cmd();
    std::vector<char **> vec_argv = command.get_argv_h();

    if (num_input == 0) {
      continue;  // continue if user input nothing
    }
    else if (command.is_exit()) {
      break;  // exit when user input exit
    }
    else if (command.is_cd()) {
      do_cd(vec_input, num_input);
    }
    else if (command.is_set()) {
      if (num_input <= 1) {
        std::cout << "please input var and value to set" << std::endl;
      }
      else if (num_input == 2) {
        std::cout << "please input value to set" << std::endl;
      }
      else if (
          command.is_var_valid()) {  // set can only be done when there are two arguments
        std::string value = command.find_set_value();
        map[vec_input[1]] = value;
      }
      else {
        std::cout << "variable name is not valid" << std::endl;
      }
    }
    else if (command.is_export() || command.is_rev()) {
      if (num_input == 1) {
        std::cout << "please input var" << std::endl;
        continue;
      }
      else if (num_input > 2) {
        std::cout << "too much variable" << std::endl;
        continue;
      }
      if (command.is_rev()) {  // do rev
        if (map.find(vec_input[1]) != map.end()) {
          std::string temp = map[vec_input[1]];
          reverse(temp.begin(), temp.end());
          map[vec_input[1]] = temp;
        }
        else {
          std::cout << "var to rev is not yet setted" << std::endl;
        }
      }
      else {
        do_export(vec_input);  // do export
      }
    }
    else {
      cpid = fork();
      if (cpid == 0) {
        for (std::size_t i = 0; i < size; i++) {
          execve(vec_cmd[i], vec_argv[i], environ);  // execve all possibilties
        }
        std::cout << "Command " << command.get_cmdname() << " not found" << std::endl;
        exit(EXIT_FAILURE);  // perror if commandname is not found
      }
      else if (cpid == -1) {
        perror("fork fail");
      }
      else {
        do {
          w = waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED);
          if (w == -1) {
            perror("waitpid fail");
          }
          if (WIFEXITED(wstatus)) {
            if (WEXITSTATUS(wstatus) == 0) {
              std::cout << "Program was successful" << std::endl;
            }
            else {
              std::cout << "Program fail with code" << WEXITSTATUS(wstatus) << std::endl;
            }
          }
          if (WIFSIGNALED(wstatus)) {
            std::cout << "Terminated by signal " << WTERMSIG(wstatus) << std::endl;
          }
        } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
      }
    }
  }
}
