#ifndef MICROSHELL_H
# define MICROSHELL_H

# define PIPE	0

typedef struct		s_list
{
	char			*str;
	struct s_list	*next;
}					t_list;

typedef struct		s_node
{
	char			**array;
	int				id;
	struct t_node	*next;
}					t_node;

#endif
