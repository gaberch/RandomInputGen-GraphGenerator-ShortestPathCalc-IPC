#include <csignal>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>





int main (int argc, char **argv) {
//use getopt(argc, argv....)
// int main(){
   std::vector <std::string> cli_cmds = {"","","",""};
   std::string strcmd ;
   std::vector <int> cli_commands = {10,5,5,20};
   int cmd;

   while((cmd = getopt (argc, argv, "s:n:l:c:")) != -1){
      char *end;
      switch (cmd)
      {
         case 's':
            cli_cmds[0] = "-s";
            strcmd = optarg;
            cli_commands[0] = strtol(strcmd.c_str(),&end,10);
            if (cli_commands[0] < 2){
               std::cerr << "Error: -s command must have a value >=2\n";
               exit (EXIT_FAILURE);
            }
            break;
         case 'n':
            cli_cmds[1] = "-n";
            strcmd = optarg;
            cli_commands[1] = strtol(strcmd.c_str(),&end,10);
            if (cli_commands[1] < 1){
               std::cerr << "Error: -n command must have a value >=1\n";
               exit (EXIT_FAILURE);
            }
            break;
         case 'l':
            cli_cmds[2] = "-l";
            strcmd = optarg;
            cli_commands[2] = strtol(strcmd.c_str(),&end,10);
            if (cli_commands[2] < 5){
               std::cerr << "Error: -l command must have a value >=5\n";
               exit (EXIT_FAILURE);
            }
            break;
         case 'c':
            cli_cmds[3] = "-c";
            strcmd = optarg;
            cli_commands[3] = strtol(strcmd.c_str(),&end,10);
            if (cli_commands[3] < 1){
               std::cerr << "Error: -c command must have a value >=1\n";
               exit (EXIT_FAILURE);
            }
            break;

      }
   }







   std::vector <pid_t> kids;

   //Create a pipe
   int stdout_rgen_to_a1[2];        // pipe 1
   int stdout_a1a3_to_a2[2];        // pipe 2 (multiple writers, one reader)


   pipe(stdout_rgen_to_a1);
   pipe(stdout_a1a3_to_a2);


   pid_t child_pid;
   child_pid = fork();
   // Create child for rgen

   if (child_pid == 0){
      // Child process - rgen - created from a3
      dup2(stdout_rgen_to_a1[1], STDOUT_FILENO);
      close(stdout_rgen_to_a1[0]);
      close(stdout_rgen_to_a1[1]);
      close(stdout_a1a3_to_a2[0]);
      close(stdout_a1a3_to_a2[1]);



      std::vector <std::string> commands;
      for (unsigned i = 0; i < cli_cmds.size(); i++){
         if (!cli_cmds[i].empty()){
            commands.push_back(cli_cmds[i]);
            commands.push_back(std::to_string(cli_commands[i]));
         }
      }

      char* args[commands.size() + 2];
      args[0] = (char*)"rgen";
      for (unsigned i = 0; i < commands.size(); i++)
         {args[i+1] = (char*)commands[i].c_str();}
      args[commands.size() + 1] = nullptr;
      execv ("./rgen", args);
      perror("Error: Could not execute rgen");
      return 0;

   }

   else if (child_pid < 0) {
      std::cerr << "Error: Could not fork for rgen\n";
      return 1;
   }

   kids.push_back(child_pid);

   child_pid = fork();
   // create child for a1 process
   if (child_pid == 0){
      // Child process - a1 - created from a3

      dup2(stdout_rgen_to_a1[0], STDIN_FILENO);          // read from rgen
      dup2(stdout_a1a3_to_a2[1], STDOUT_FILENO);         // write to a2
      close(stdout_rgen_to_a1[0]);
      close(stdout_rgen_to_a1[1]);
      close(stdout_a1a3_to_a2[0]);
      close(stdout_a1a3_to_a2[1]);

      char* args[2];
      args[0] = (char*)"ece650-a1.py";
      args[1] = nullptr;
      execv ("./ece650-a1.py", args);
      perror("Error: Could not execute ece650-a1.py");
      return 1;

   }
   else if (child_pid < 0) {
      std::cerr << "Error: Could not fork for a1\n";
      return 1;
   }

   kids.push_back(child_pid);


   child_pid = fork();
   // created child for a2 process
   if (child_pid == 0) {
      // Child process - a2 - created from a3


      dup2(stdout_a1a3_to_a2[0], STDIN_FILENO);          // read from a1 and a3
      close(stdout_rgen_to_a1[0]);
      close(stdout_rgen_to_a1[1]);
      close(stdout_a1a3_to_a2[0]);
      close(stdout_a1a3_to_a2[1]);

      char* args[2];
      args[0] = (char*)"ece650-a2";
      args[1] = nullptr;
      execv ("./ece650-a2", args);
      perror("Error: Could not execute ece650-a2");
      return 1;
   }
   else if (child_pid < 0) {
      std::cerr << "Error: Could not fork for a2\n";
      return 1;
   }

   kids.push_back(child_pid);
   child_pid = 0;

   // for testing purposes only
   // dup2(stdout_rgen_to_a1[0], STDIN_FILENO);




   dup2(stdout_a1a3_to_a2[1], STDOUT_FILENO);
   close(stdout_rgen_to_a1[0]);
   close(stdout_rgen_to_a1[1]);

   close(stdout_a1a3_to_a2[0]);
   close(stdout_a1a3_to_a2[1]);

   while (!std::cin.eof()){
      std::string line;
      std::getline(std::cin, line);
      if (!line.empty())
         {std::cout << line << std::endl;}
   }

   // usleep(4000);

   for (pid_t k : kids) {
      int status;
      kill (k, SIGTERM);
      waitpid(k, &status, 0);
    }


    return 0;
}
