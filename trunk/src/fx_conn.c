/***************************************************************************
 *   Copyright (C) 2010 by lwp                                             *
 *   levin108@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <fx_include.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netdb.h>

void fx_conn_init(void)
{
	connlst = (struct conn_list*)malloc(sizeof(struct conn_list));
	connlst->conn = NULL;
	connlst->next = connlst->pre = connlst;
}

void fx_conn_append(FetionConnection *conn)
{
	struct conn_list *tmp;
	tmp = (struct conn_list*)malloc(sizeof(struct conn_list));
	tmp->conn = conn;

	connlst->next->pre = tmp;
	tmp->next = connlst->next;
	tmp->pre = connlst;
	connlst->next = tmp;
}

static void fx_conn_remove(struct conn_list *conn)
{
	conn->next->pre = conn->pre;
	conn->pre->next = conn->next;
}

void fx_conn_offline(FxMain *fxmain)
{
	FxHead           *fxhead;
	FxTree           *fxtree;
	User             *user;
	GtkTreeView      *treeview;
	GtkTreeModel     *model;
	GtkTreeIter       rootiter;
	GtkTreeIter       childiter;
	struct conn_list *cur;
	struct conn_list *tmp;
	FxList           *cw_cur;
	FxChat           *fxchat;

	fxhead = fxmain->headPanel;
	user = fxmain->user;

	/* set state image to be offline */
	fx_head_set_state_image(fxmain, P_OFFLINE);
	
	/* set user state to be offline */
	fetion_user_set_st(fxmain->user, P_OFFLINE);

	/* shutdown all the connections */
	for(cur = connlst->next;cur != connlst;){
		tmp = cur;
		cur = cur->next;
		tcp_connection_close(tmp->conn);
		fx_conn_remove(tmp);
		g_free(tmp);
	}

	/* clear the state of all contacts */
	fxtree = fxmain->mainPanel;
	treeview = GTK_TREE_VIEW(fxtree->treeView);
	model = gtk_tree_view_get_model(treeview);

	if(gtk_tree_model_get_iter_root(model, &rootiter)){
		do{
			/* clear group online numbers */
			gtk_tree_store_set(GTK_TREE_STORE(model),
					&rootiter, G_ONLINE_COUNT_COL, 0, -1);

			if(gtk_tree_model_iter_children(model,
						&childiter, &rootiter))
				do{
					/* clear contacts' online state */
					gtk_tree_store_set(GTK_TREE_STORE(model),
							&childiter, B_STATE_COL, P_HIDDEN, -1);
				}while(gtk_tree_model_iter_next(model, &childiter));

		}while(gtk_tree_model_iter_next(model, &rootiter));
	}

	/* notify all the chat window */
	foreach_list(fxmain->clist, cw_cur){
		fxchat = (FxChat*)cw_cur->data;
		fx_chat_add_information(fxchat,
			_("\nConnection has been shutdown\n"));
	}

}

int fx_conn_check_action(FxMain *fxmain)
{
	if(fxmain->user->state == P_OFFLINE){
		fx_util_popup_warning(fxmain,
				_("The action can't be performed in offline state"));
		return 0;
	}
	return 1;
}
