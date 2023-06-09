#include "minishell.h"

int		next_alloc(char *line, int *i)
{
	int		count;
	int		j;
	char	c;

	count = 0;
	j = 0;
	c = ' ';
	while (line[*i + j] && (line[*i + j] != ' ' || c != ' '))
	{
		if (c == ' ' && (line[*i + j] == '\'' || line[*i + j] == '\"'))
			c = line[*i + j++];
		else if (c != ' ' && line[*i + j] == c)
		{
			count += 2;
			c = ' ';
			j++;
		}
		else
			j++;
		if (line[*i + j - 1] == '\\')
			count--;
	}
	return (j - count + 1);
}

t_token *set_token_value(char *line, int *i) {
    t_token *token;
    char c;
    int j;
    
    c = ' ';
    j = 0;
	// if (!(token = (t_token *)malloc(sizeof(t_token)))
	// || !(token->str = (char *)malloc(sizeof(char) * next_alloc(line, i))))
	// 	return (NULL);


	if (!(token = (t_token *)malloc(sizeof(t_token))))
		return (NULL);
	if (!(token->str = (char *)malloc(sizeof(char) * next_alloc(line, i))))
		return (NULL);

    // Parse the line
    while (line[*i] && (line[*i] != ' '|| c != ' ')) {
        if (c == ' ' && (line[*i] == '\'' || line[*i] == '\"')) {
			if (line[*i] == '\'')
				token->type = simple_quote;
            // If current character is a quote and c is a space, set c to the quote and move to the next character
            c = line[(*i)++];
        } else if (c != ' ' && line[*i] == c) {
            // If current character is the closing quote and c is not a space, set c back to a space and move to the next character
            c = ' ';
            (*i)++;
        } else if (line[*i] == '\\' && (*i)++) {
            // If current character is a backslash, skip it and add the next character to the token string
            token->str[j++] = line[(*i)++];
        } else {
            // Otherwise, add the current character to the token string
            token->str[j++] = line[(*i)++];
        }
    }
    token->str[j] = '\0';
    return (token);
}

int delete_token(t_data *data, t_token *curr)
{
	// int status;

	// status = 0;
	if(curr)
	{
		// curr is in the middle of the list, we have a prev and next token to relink to
		if(curr->next && curr->prev)
		{
			curr->prev->next = curr->next;
			curr->next->prev = curr->prev;
		}
		// curr is the first token in the list
		else if(curr->next && curr->prev == NULL)
		{
			curr->next->prev = NULL;
			data->token = curr->next;
		}
		// curr is the last token in the list
		else if(curr->next == NULL && curr->prev)
		{
			curr->prev->next = NULL;
		}
		// curr is the only token in the list (give back prompt)
		else if(curr->next == NULL && curr->prev == NULL)
			data->token = NULL;
		// free_token(curr);
	}
	else
		return (1);
	return (0);
}

void	set_token_type(t_token *token)
{
	// if (ft_strcmp(token->str, "") == 0)
	// 	token->type = EMPTY;
	if (ft_strcmp(token->str, ">") == 0)
		token->type = simple_redir_right;
	else if (ft_strcmp(token->str, "<") == 0)
		token->type = simple_redir_left;
	else if (ft_strcmp(token->str, ">>") == 0)
		token->type = double_redir_right;
	else if (ft_strcmp(token->str, "<<") == 0)
		token->type = double_redir_left;
	else if (ft_strcmp(token->str, "|") == 0)
		token->type = pipeline;
	else if (ft_strchr(token->str, '=') != NULL)
		token->type = new_variable;
	else if (ft_strchr(token->str, '$') != NULL && ft_strchr(token->str, '=') == NULL && token->type != simple_quote && ft_strcmp(token->str, "&?") != 0)
		token->type = variable;
	else if (token->prev == NULL || token->prev->type == pipeline)
		token->type = cmd;
	else if (token->prev->type == double_redir_left)
		token->type = heredoc_delimiter;
	else
		token->type = arg;
}

void	skip_space(const char *str, int *i)
{
	while ((str[*i] == ' ' || str[*i] == '\t') || (str[*i] == '\r' || str[*i] == '\v' || str[*i] == '\f'))
		(*i)++;
}

void init_heredoc_token(t_token *curr, char *line, int start)
{
	curr->type = arg;
	curr->heredoc_EOF = ft_strdup(curr->str);
	curr->str = ft_strjoin(curr->str, line + start);
}

int tokenize(t_data *data, char *line)
{
    t_token *curr;
    t_token *next;
    int i;

    i = 0;
    next = NULL;
    curr = set_token_value(line, &i);
	if (curr == NULL || curr->str == NULL)
	{
		ft_printf("ERROR ALLOC MALLOC");
	}
	curr->prev = NULL;
	set_token_type(curr);
    data->token = curr;
    while(line[i])
    {
        // if(line[i] == ' ')
        // {
        //     i++;
        //     continue;
        // }
		skip_space(line, &i);
        next = set_token_value(line, &i);
        next->prev = curr;
		set_token_type(next);  
        next->next = NULL;
        curr->next = next;
        curr = next;
		if (curr->type == heredoc_delimiter) // begin of heredoc, store the rest of the line including delimiter in next node
		{
			curr->heredoc_EOF = ft_strdup(curr->str);
			curr->str = ft_strjoin(curr->str, line + i);
			curr->type = arg;
			// init_heredoc_token(curr, line, i);
			// free line ?
			break;
		}
    }
	if(ft_strcmp(curr->str, "	") == 0)
	{
		delete_token(data, curr);
		// ft_printf("TOOOOKKEKSDF [%s]", curr->str);
	}
    return (0);
}
