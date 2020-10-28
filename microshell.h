/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/28 16:47:49 by mondrew           #+#    #+#             */
/*   Updated: 2020/10/28 19:46:00 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
