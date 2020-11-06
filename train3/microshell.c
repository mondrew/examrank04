#include "microshell.h"
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h> // test

void	ft_putstr(char *str)
{
	int		i;

	i = 0;
	while (str[i])
		i++;
	write(2, str, i);
}

int		ft_str_counter(char **argv)
{
	int		i;

	i = 0;
	while (argv[i] != NULL && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
		i++;
	return (i);
}

char	**ft_create_array(char **argv, int n)
{
	char	**array;
	int		i;

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

int		ft_count_pipes(t_node *tmp)
{
	int		i;

	i = 0;
	while (tmp && tmp->status == PIPE)
		i++;
	return (i);
}

int		ft_execute_in_parent(char **array)
{
	int		args;
	int		i;

	i = 0;
	while (array[i])
		i++;
	args = i - 1;
	if (args != 1)
	{
		// cd args error
		ft_putstr("error: cd: bad arguments\n");
		return (-1);
	}
	if (chdir(array[1]) == -1)
	{
		// error cd cannot goto path
		ft_putstr("error: cd: cannot change directory to ");
		ft_putstr(array[1]);
		ft_putstr("\n");
		return (-1);
	}
	return (0);
}

int		ft_simple_execute(char **array, char **envp)
{
	pid_t	pid;

	if ((pid = fork()) == -1)
	{
		// fatal (treat in ft_execute)
		return (-1);
	}
	if (pid == 0)
	{
		// Child
		if (execve(array[0], array, envp) == -1)
		{
			// error execve
			ft_putstr("error: cannot execute ");
			ft_putstr(array[0]);
			ft_putstr("\n");
			exit(1);
		}
		exit(0);
	}
	else
	{
		// Parent
		waitpid(pid, NULL, 0);
		return (0);
	}
	return (0);
}

void	ft_lst_clear(t_node *cmds)
{
	t_node	*tmp;

	while (cmds)
	{
		tmp = cmds->next;
		free(cmds->array);
		free(cmds);
		cmds = tmp;
	}
}

t_node	*ft_lst_new(char **array, int status)
{
	t_node	*new_node;

	if (!(new_node = malloc(sizeof(t_node))))
		return (NULL);
	new_node->array = array;
	new_node->status = status;
	new_node->next = NULL;
	return (new_node);
}

t_node	*ft_lst_add_back(t_node **cmds, char **array, int status)
{
	t_node	*tmp;
	t_node	*new_node;
	t_node	*help;

	tmp = *cmds;
	if (!(new_node = ft_lst_new(array, status)))
		return (NULL);
	if (!tmp)
		tmp = new_node;
	else
	{
		help = tmp;
		while (help->next)
			help = help->next;
		help->next = new_node;
	}
	*cmds = tmp;
	return (*cmds);
}

int		ft_execute_with_pipes(t_node *cmds, char **envp, int pipes)
{
	int		i;
	int		pipefd[pipes * 2];
	pid_t	pid;

	i = 2;
	// first pre-pipe command
	if (pipe(pipefd) == -1)
	{
		// syscall error
		return (-1);
	}
	if ((pid = fork()) == -1)
	{
		// syscall error
		return (-1);
	}
	if (pid == 0)
	{
		// Child
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		if (execve(cmds->array[0], cmds->array, envp) == -1)
		{
			// execve error
			ft_putstr("error: cannot execute ");
			ft_putstr(cmds->array[0]);
			ft_putstr("\n");
			exit(1);
		}
		exit(0);
	}
	else
	{
		// Parent
		waitpid(pid, NULL, 0);
		cmds = cmds->next;
	}
	// Other pipes
	if (pipes > 1 && pid != 0)
	{
		if (pipe(pipefd + i) == -1)
		{
			// syscall error
			return (-1);
		}
		if ((pid = fork()) == -1)
		{
			// syscall error
			return (-1);
		}
		if (pid == 0)
		{
			// Child
			close(pipefd[i - 1]);
			dup2(pipefd[i - 2], STDIN_FILENO);
			close(pipefd[i - 2]);

			close(pipefd[i]);
			dup2(pipefd[i + 1], STDOUT_FILENO);
			close(pipefd[i + 1]);

			if (execve(cmds->array[0], cmds->array, envp) == -1)
			{
				// execve error
				ft_putstr("error: cannot execute ");
				ft_putstr(cmds->array[0]);
				ft_putstr("\n");
				exit(1);
			}
			exit(0);
		}
		else
		{
			// Parent
			close(pipefd[i - 2]);
			close(pipefd[i - 1]);

			waitpid(pid, NULL, 0);

			i += 2;
			cmds = cmds->next;
		}
	}
	// Last after-pipe command
	if (pid != 0)
	{
		if ((pid = fork()) == -1)
		{
			// syscall error
			return (-1);
		}
		if (pid == 0)
		{
			// Child
			close(pipefd[i - 1]);
			dup2(pipefd[i - 2], STDIN_FILENO);
			close(pipefd[i - 2]);

			if (execve(cmds->array[0], cmds->array, envp) == -1)
			{
				// execve error
				ft_putstr("error: cannot execute ");
				ft_putstr(cmds->array[0]);
				ft_putstr("\n");
				exit(1);
			}
			exit(0);
		}
		else
		{
			// Parent
			close(pipefd[i - 2]);
			close(pipefd[i - 1]);

			waitpid(pid, NULL, 0);
		}
	}
	return (0);
}

int		ft_execute(t_node **cmds, char **envp)
{
	int		pipes;
	t_node	*tmp;

	tmp = *cmds;
	pipes = ft_count_pipes(tmp);
	if (!pipes)
	{
		if (strcmp(tmp->array[0], "cd") == 0)
		{
			if (ft_execute_in_parent(tmp->array) == -1)
			{
				ft_lst_clear(*cmds);
				*cmds = NULL;
				return (1);
			}
		}
		else
		{
			if (ft_simple_execute(tmp->array, envp) == -1)
			{
				// syscall fatal error
				ft_putstr("error: fatal\n");
				ft_lst_clear(*cmds);
				*cmds = NULL;
				exit(1);
			}
		}
	}
	else
	{
		if (ft_execute_with_pipes(tmp, envp, pipes) == -1)
		{
			// syscall fatal error
			ft_putstr("error: fatal\n");
			ft_lst_clear(*cmds);
			*cmds = NULL;
			exit(1);
		}
	}
	ft_lst_clear(*cmds);
	*cmds = NULL;
	return (0);
}

int		ft_microshell(char **argv, char **envp)
{
	int		i;
	int		n;
	char	**array;
	t_node	*cmds;

	i = 0;
	n = 0;
	array = NULL;
	cmds = NULL;
	while (argv[i] != NULL)
	{
		n = ft_str_counter(&argv[i]);
		if (!(array = ft_create_array(&argv[i], n)))
		{
			ft_lst_clear(cmds);
			return (1);
		}
		i += n;
		// PIPE finished
		if (argv[i] != NULL && strcmp(argv[i], "|") == 0)
		{
			if (!ft_lst_add_back(&cmds, array, PIPE))
			{
				ft_lst_clear(cmds);
				return (1);
			}
		}
		// SEMICOLON finished
		else if (argv[i] == NULL || strcmp(argv[i], ";") == 0)
		{
			if (array[0] == NULL)
				free(array);
			else
			{
				if (!ft_lst_add_back(&cmds, array, END))
				{
					ft_lst_clear(cmds);
					return (1);
				}
				if (ft_execute(&cmds, envp) == -1)
				{
					ft_lst_clear(cmds);
					return (1);
				}
			}
		}
		if (argv[i])
			i++;
	}
	return (0);
}

int		main(int argc, char **argv, char **envp)
{
	if (argc > 1)
		ft_microshell(&argv[1], envp);
	return (0);
}
