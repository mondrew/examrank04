#ifndef MICROSHELL_H
# define MICROSHELL_H

# define PIPE 0
# define END 1
typedef struct		s_node
{
	char			**array;
	int				status;
	struct s_node	*next;
}					t_node;

#endif
