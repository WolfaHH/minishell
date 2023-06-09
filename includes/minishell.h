
#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/includes/libft.h"
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <string.h>
# include <fcntl.h>
# include <dirent.h>
# include <sys/wait.h>
# include <limits.h>
# include <errno.h>
# include <signal.h>
# include <readline/readline.h>
# include <readline/history.h>

# define MAX_CMD_LEN 256
# define MAX_NUM_ARGS 1024
# define STDIN 0
# define STDOUT 1
# define STDERR 2
# define MAX_INT 2147483647

typedef enum		e_token_type
{
    simple_redir_left,
    simple_redir_right,
    double_redir_left,
    double_redir_right,
    pipeline,
	new_variable,
    variable,
    cmd,
    arg,
	heredoc_delimiter,
	simple_quote
}					t_token_type;

typedef struct	s_token
{
	char			*str;
	char			*heredoc_EOF;
	t_token_type	type;
	struct s_token	*prev;
	struct s_token	*next;
}					t_token;

typedef struct	s_env
{
	char			*key;
	char			*value;
	char			*(*get_joined_env)(struct s_env*);
	struct s_env	*next;
}					t_env;

typedef struct s_data
{
    t_token     	*token;
    t_env       	*env;
    t_env       	*secret_env;
	int				in;
	int				out;
	int				exit;
	int				ret;
}               	t_data;


//	env/
//  init_env.c
//
char	*parse_env_key(char *env_val);
char	*parse_env_value(char *env_val);
int		init_env(t_data *data, char **local_env, int flag);
char	*get_joined_env(t_env *curr_env);

//	env/
//  get_env.c
//
t_env   *get_env_with_key(char *key, t_env *env_list);
t_env   *get_last_env(t_env *env_list);
//	env/
//  set_env.c
//
void    increment_shell_level(t_env *env_list);
int     add_new_env(t_env *env, char *key, char *value);
void    sort_env(char **tab, int env_len);
//	shell_data/
//  init_data.c
//
int 	init_data(t_data *data);

// parsing /
// token.c
int		next_alloc(char *line, int *i);
t_token *set_token_value(char *line, int *i);
void	set_token_type(t_token *token);
int     delete_token(t_data *data, t_token *curr);
int     tokenize(t_data *data, char *line);
void	skip_space(const char *str, int *i);

//	utils/
//  free.c
//
void	free_env(t_env *env);
void	free_all_tokens(t_token *token);
void    free_arr(char **arr);

/*
** BUILTINS
*/
int		ft_echo(char **args);
int		ft_cd(char **args, t_env *env);
int		ft_pwd(void);
int		ft_env(t_env *env);
int		ft_export(t_env *env);
int		ft_unset(char *key, t_data *data);
char	*get_env_name(char *dest, const char *src);
void	sort_env(char **tab, int env_len);
void	print_sorted_env(t_env *env);
int		is_valid_env(const char *env);

void    super_parser(t_data *data);
void    handle_signal(int sig);
void    super_executer(t_data *data);


int add_new_env(t_env *env, char *key, char *value);

#endif