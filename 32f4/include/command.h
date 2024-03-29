/*
 * include/command.h
 *
 */
#ifndef _CCD_COMMAND_H_
#define _CCD_COMMAND_H_

//struct user_command_t;

typedef struct user_command {
	const char *name;
	void (*cmdfunc)(int argc, const char **);
	struct user_command *next_cmd;
	const char *helpstr;
} user_command_t;

typedef struct user_subcommand {
	const char *name;
	void (*cmdfunc)(int argc, const char **);
	const char *helpstr;
} user_subcommand_t;

/* General interfaces */
void add_command(user_command_t *cmd);
void execcmd(int, const char **);
void exec_string(char *);
void execsubcmd(user_subcommand_t *, int, const char **);
void print_usage(char *strhead, user_subcommand_t *);
void invalid_cmd(const char *cmd_name, user_subcommand_t *cmds);
int init_builtin_cmds(void);

#endif /* _CCD_COMMAND_H_ */
