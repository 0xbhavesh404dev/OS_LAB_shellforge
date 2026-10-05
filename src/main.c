#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
int main(void) {
char *line = NULL;
size_t len = 0;
char *args[64];
while (1) {
printf("shellforge$ ");
fflush(stdout);
if (getline(&line, &len, stdin) == -1) break;
line[strcspn(line, "\n")] = '\0';
int i = 0;
char *token = strtok(line, " \t");
while (token != NULL && i < 63)
 {
  args[i++] = token;
  token = strtok(NULL, " \t");
}
args[i] = NULL;
if (i == 0) continue;
if (strcmp(args[0], "exit") == 0) break;
//WEEK 5 
if (strcmp(args[0], "cd") == 0) {
if (args[1] == NULL) {
perror("shellforge: missing path parameter\n");
} else {
if (chdir(args[1]) != 0) {
perror("Directory change failed");
}
}
continue; // CRUCIAL: Skip cloning layout completely!
}
// end of week5 
pid_t pid = fork();
if (pid == 0) 
{
char *command_args[64];
int command_arg_count = 0;
for (int j = 0; j < i; j++) {
if (strcmp(args[j], "<") == 0 || strcmp(args[j], ">") == 0 ||
    strcmp(args[j], ">>") == 0) {
if (j + 1 >= i) {
fprintf(stderr, "shellforge: missing redirection file\n");
_exit(1);
}
int fd;
if (strcmp(args[j], "<") == 0) {
fd = open(args[++j], O_RDONLY);
if (fd < 0) {
perror("shellforge: input redirection");
_exit(1);
}
if (dup2(fd, STDIN_FILENO) < 0) {
perror("shellforge: dup2");
close(fd);
_exit(1);
}
} else {
int flags = O_WRONLY | O_CREAT;
flags |= strcmp(args[j], ">>") == 0 ? O_APPEND : O_TRUNC;
fd = open(args[++j], flags, 0644);
if (fd < 0) {
perror("shellforge: output redirection");
_exit(1);
}
if (dup2(fd, STDOUT_FILENO) < 0) {
perror("shellforge: dup2");
close(fd);
_exit(1);
}
}
close(fd);
} else {
command_args[command_arg_count++] = args[j];
}
}
command_args[command_arg_count] = NULL;
execvp(command_args[0], command_args);
perror("Execution error");
_exit(1);
} else if (pid < 0) {
perror("shellforge: fork");
} else {
waitpid(pid, NULL, 0);
}
}
free(line);
return 0;
}
