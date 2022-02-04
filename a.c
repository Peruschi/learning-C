#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SHELL_LINE_BUFFER_SIZE 128
#define SHELL_TOKENS_BUFFER_SIZE 16
#define SHELL_TOKEN_DELIM " \t\r\n\a"

// start and run shell
void shell();
int shell_execute(char **args);
int shell_launch(char **args);

// check function
void check_allocate(const void *block);
void check_getcwd(const char *path);

// reveive and analyz ecommands
char* shell_receive_line();
char** shell_parse_line(char *line);

// builtin commamds in shell
int shell_num_builtin_commands();
int shell_cd(char **args);
int shell_ls(char **args);
int shell_rm(char **args);
int shell_pwd(char **args);
int shell_help(char **args);
int shell_exit(char **args);
int shell_mkdir(char **agrs);
int shell_rmdir(char **args);
int shell_clear(char ** args);
int shell_touch(char **args);
char *builtin_commands[] = {
	"cd",
	"ls",
	"rm",
	"pwd",
	"help",
	"exit",
	"mkdir",
	"rmdir",
	"clear",
	"touch",
};
int (*builtin_function[])(char**) = {
	&shell_cd,
	&shell_ls,
	&shell_rm,
	&shell_pwd,
	&shell_help,
	&shell_exit,
	&shell_mkdir,
	&shell_rmdir,
	&shell_clear,
	&shell_touch,
};

// support and assist uiltin commands
void print_files(char *path);
char* integrate_path(char *path, char ** args);

int main(int argc, char** argv){
        
	printf("\033[2J \033[1;1H");
	shell();
	printf("\033[2J \033[1;1H");

        return EXIT_SUCCESS;
}

void shell(){
	char *line = NULL;
	char **args = NULL;
	int status = 1;

	while (status){
		char curr_dir[80];
		printf("\33[36m%s\33[m$", getcwd(curr_dir, 80));
		line = shell_receive_line();
		args = shell_parse_line(line);
		status = shell_execute(args);
		
		free(line);
		free(args);
		line = NULL;
		args = NULL;
	}
}

char* shell_receive_line(){
	int buffer_size = SHELL_LINE_BUFFER_SIZE;
	char *buffer = malloc(buffer_size * sizeof(char));
	check_allocate(buffer);

	int index = 0, c = 0;
	while (1){
		c = getchar();
		if (c == EOF || c == '\n'){
			buffer[index] = '\0';
			return buffer;
		}
		else {
			buffer[index] = c;
		}
		index++;

		if (buffer_size <= index){
			buffer_size += SHELL_LINE_BUFFER_SIZE;
			buffer = realloc(buffer, buffer_size);
			check_allocate(buffer);
		}
	}
}

char** shell_parse_line(char *line){
	int buffer_size = SHELL_TOKENS_BUFFER_SIZE, index = 0;
	char **tokens = malloc(buffer_size * sizeof(char*));
	check_allocate(tokens);

	char *token = strtok(line, SHELL_TOKEN_DELIM);
	while (token){
		tokens[index] = token;
		index++;
		if (buffer_size <= index){
			buffer_size += SHELL_TOKENS_BUFFER_SIZE;
			tokens = realloc(tokens, buffer_size * sizeof(char*));
			check_allocate(tokens);
		}

		token = strtok(NULL, SHELL_TOKEN_DELIM);
	}

	tokens[index] = NULL;
	return tokens;
}

int shell_execute(char **args){
        if (!args[0]){
                return 1;
        }

        int num_commands = shell_num_builtin_commands();
        for (int i = 0; i < num_commands; i++){
                if (!strcmp(args[0], builtin_commands[i])){
                        return (*builtin_function[i])(args);
                }
        }
        
	return shell_launch(args);
}

int shell_launch(char **args){
	pid_t wpid, pid;
	pid = fork();
	if (!pid){
		if (execvp(args[0], args) == -1){
			perror("shell: ");
		}
		exit(EXIT_FAILURE);
	}
	else if (pid < 0){
		perror("shell: ");
	}
	else {
		int status;
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}

void check_allocate(const void *block){
        if (!block){
                fprintf(stderr, "shell: allocate memory failed!\n");
                exit(EXIT_FAILURE);
        }
}

void check_getcwd(const char *path){
        if (!path){
                fprintf(stderr, "Searching current directory failed!\n");
        }
}

int shell_num_builtin_commands(){
	return sizeof(builtin_commands) / sizeof(char*);
}

int shell_cd(char **args){
	if (!args[1]){
		fprintf(stderr, "shell: excepted argument to \"cd\"\n");
	}
	else if (chdir(args[1])){
		perror("shell");
	}

	return 1;
}

int shell_ls(char **args){
	char path[80] = {0};
	check_getcwd(getcwd(path, 80));
	
	print_files(path);

	return 1;
}

int shell_rm(char ** args){
	if (!args[1]){
		fprintf(stderr, "shell: excepted argument to \"rm\"\n");
		return 1;
	}

	char path[80] = {0};
	check_getcwd(getcwd(path, 80));
	
	int i = 1;
	while (args[i]){
		strcat(path, "/");
		strcat(path, args[i]);
		i++;
	}
	
	if (remove(path)){
		fprintf(stderr, "Remove file failed!\n");
	}

	return 1;
}

int shell_pwd(char **args){
	char path[80] = {0};
	check_getcwd(getcwd(path, 80));
	printf("%s\n", path);

	return 1;
}

int shell_help(char **args){
	printf("<<-- Shell Commands -->>\n");
	printf("Type program names and arguments, and hit enter.\n");
	printf("The following are built in:\n");
	
	int num_commands = shell_num_builtin_commands();
	for (int i = 0; i < num_commands; i++){
		printf("\t%s\n", builtin_commands[i]);
	}
	printf("Use the man commands on other program.\n");

	return 1;
}

int shell_exit(char **agrs){
	return 0;
}

int shell_mkdir(char **args){
	char path[80] = {0};
	check_getcwd(getcwd(path, 80));

	int i = 1;
        while (args[i]){
                strcat(path, "/");
                strcat(path, args[i]);
                i++;
        }

	if (mkdir(path, S_IRWXU)){
		fprintf(stderr, "Create directory failed!\n");
	}

	return 1;
}

int shell_rmdir(char ** args){
        char path[80] = {0};
        check_getcwd(getcwd(path, 80));

        int i = 1;
        while (args[i]){
                strcat(path, "/");
                strcat(path, args[i]);
                i++;
        }
        
	if (rmdir(path)){
                fprintf(stderr, "Remove directiory failed!\n");
        }

        return 1;
}

int shell_clear(char **args){
	printf("\033[2J \033[1;1H");
	return 1;
}

int shell_touch(char ** args){
	char path[80] = {0};
	check_getcwd(getcwd(path, 80));
	
	int i = 1;
	while (args[i]){
		strcat(path, "/");
		strcat(path, args[i]);
		i++;
	}
	
	if (!creat(path, 0777)){
		fprintf(stderr, "Create file failed!\n");
	}

	return 1;
}

void print_files(char *base_path){
	DIR *dir;
	struct dirent *ptr;

	if ((dir = (DIR*)opendir(base_path)) == NULL){
		fprintf(stderr, "Open direct failed!\n");
	}

	while (ptr = readdir(dir)){
		if (!strcmp(ptr->d_name, ".") || !strcmp(ptr->d_name, "..")){
			continue;
		}
		else {
			printf("\33[32m%s\33[m\n", ptr->d_name);
		}
	}
	closedir(dir);
}
