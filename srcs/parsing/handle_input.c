/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_input.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trpham <trpham@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 15:19:11 by trpham            #+#    #+#             */
/*   Updated: 2025/06/19 02:48:16 by trpham           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/shell.h"

void	process_valid_line(t_shell *mshell)
{
	mshell->cmd_list = parse_tokens_to_commands(mshell->token_list);
	if (!mshell->cmd_list)
	{
		mshell->exit_code = 0;
		return ;
	}
	mshell->tree = convert_cmd_to_ast(mshell->cmd_list);
	if (!mshell->tree)
	{
		mshell->exit_code = 1;
		return ;
	}
	run_ast_pipeline(mshell);
}

void	run_ast_pipeline(t_shell *mshell)
{
	t_ast	*tree;

	tree = mshell->tree;
	if (!tree)
		return ;
	if (process_heredocs(mshell, tree))
	{
		cleanup_heredoc_tempfiles(tree);
		mshell->exit_code = 130;
		return ;
	}
	execute_ast(tree, mshell);
	cleanup_heredoc_tempfiles(tree);
}

void	cleanup_heredoc_tempfiles(t_ast *tree)
{
	t_redirect	*redir;

	if (!tree)
		return ;
	if (tree->redirects)
	{
		redir = tree->redirects;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC && redir->tmp_file)
			{
				unlink(redir->tmp_file);
				free(redir->tmp_file);
				redir->tmp_file = NULL;
			}
			redir = redir->next;
		}
	}
	cleanup_heredoc_tempfiles(tree->left);
	cleanup_heredoc_tempfiles(tree->right);
}
