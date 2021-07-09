#include "microshell.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

int		ft_str_counter(char **argv)
{
	int		i;

	i = 0;
	while (ft_strcmp(argv[i], ";") != 0 && ft_strcmp(argv[i], "|") != 0	&& argv[i] != NULL)
		i++;
	return (i);
}

char	**ft_create_array(char **argv, int n)
{
	int		i;
	char	**array;

	i = 0;
	if (!(array = malloc(sizeof(char *) * (n + 1))))
		return (NULL);
	while (i < n)
	{
		array[i] = argv[i];
		i++;
	}
	array[n] = NULL;
	return (array);
}

t_node	*ft_lst_new(char **array, int id)
{
	t_node	*new_node;

	if (!(new_node = malloc(sizeof(t_node))))
		return (NULL);
	new_node->array = array;
	new_node->id = id;
	new_node->next = NULL;
	return (new_node);
}

t_node	*ft_lst_add_back(t_node **cmds, char **array, int id)
{
	t_node	*tmp;
	t_node	*help;
	t_node	*new;

	tmp = *cmds;
	if (!(new = ft_lst_new(array, id)))
		return (NULL);
	if (!tmp)
		tmp = new;
	else
	{
		help = tmp;
		while (help->next)
			help = help->next;
		help->next = new;
	}
	return (tmp);
}

void	ft_microshell(char **argv, char **envp)
{
	int		i;
	int		n;
	t_node	*cmds;
	char	**array;

	i = 0;
	cmds = NULL;
	while (argv[i] != NULL)
	{
		n = ft_str_counter(&argv[i]);
		if (!(array = ft_create_array(&argv[i], n)))
		{
			// FREE EVERYTHING
		}
		i += n;
		if (!strcmp(argv[i], "|"))
		{
			if (!(ft_lst_add_back(&cmds, array, PIPE)))
			{
				// FREE
			}
		}
		else if (!strcmp(argv[i], ";"))
			ft_execute(cmds);
		if (argv[i])
			i++;
	}
}

int		main(int argc, char **argv, char **envp)
{
	if (argc > 1)
		ft_microshell(&(argv[1]), envp);
	return (0);
}
