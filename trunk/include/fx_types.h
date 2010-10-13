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
#ifndef FX_TYPES_H
#define FX_TYPES_H
#ifdef HAVE_LIBNOTIFY
 #include <libnotify/notify.h>
#endif

#define WINDOW_WIDTH 280
#define WINDOW_HEIGHT 500

typedef enum
{
	CHAT_DIALOG_FOCUSED = 1 ,
	CHAT_DIALOG_NOT_FOCUSED = 2
} ChatDialogFocusedType;

typedef struct
{
	GThread* loginThread;
	long loginFuncId;
	long cancelFuncId;
	Proxy *proxy;
	/* login widget start */
	GtkWidget* fixed;
	GtkWidget* username;
	GtkWidget* password;
	GtkWidget* loginbutton;
	GtkWidget* userlabel;
	GtkWidget* passlabel;
	GtkWidget* statecombo;
	GtkWidget* remember;
	GtkWidget* proxyBtn;
	GtkWidget* proxyLabel;
	GtkListStore* store;
	GtkTreeModel* model;
	/* logining panel */
	GtkWidget* fixed1;
	GtkWidget* image;
	GtkWidget* label;
	GtkWidget* errlabel;
} FxLogin;

typedef struct
{
	GtkWidget* topbox;
	GtkWidget* portrait;
	GtkWidget* portraitbox;
	GtkWidget* name_label;
	GtkWidget* state_label;
	GtkWidget* impre_box;
	GtkWidget* impre_label;
	GtkWidget* impre_entry;
	GtkWidget* state_button;
	GtkWidget* presence_menu;
	GtkWidget* state_img;
	char oldimpression[1024];
} FxHead;

typedef struct
{
	GtkWidget* toolbar;
} FxBottom;

typedef struct
{
	GtkWidget *treeView;
	GtkWidget *noPgLabel;
	GtkWidget *pgTreeView;
	GtkWidget *searchbox;
	GtkTreeStore *treeModel;
	GtkWidget *scrollWindow;
	GtkWidget *pgScrollWindow;
	GtkWidget *noLabelScrollWindow;
} FxTree;

typedef struct
{
	FxList* clist;				 /*  chat window list							 */
	FxList* mlist; 				 /*  unread message list						 */
	FxList* slist;				 /*  sip list wicth is now listening			 */
	FxList* tlist;				 /*  time out args list							 */
	FxList* shlist;				 /*	 share content window list		   			 */
	FxList* pglist;
	FetionHistory* history;
#ifdef HAVE_LIBNOTIFY
	NotifyNotification *notify;
#endif
	int iconConnectId;			 /*  status icon connect id						 */

	FxLogin* loginPanel;		 /*  login panel								 */
	FxHead* headPanel;			 /*  head panel									 */
	FxTree* mainPanel;			 /*  tree panel include treeview				 */
	FxBottom* bottomPanel;		 /*  bottom panel include some functional buttons*/

	GtkWidget* window;			 /*  main window 								 */
	GtkStatusIcon* trayIcon;	 /*  status icon								 */
	GtkWidget* mainbox;			 /*	 main layout box							 */
	
	User* user;
} FxMain;

typedef struct
{
	FxLogin *fxlogin;
	GtkWidget *dialog;
	GtkWidget *enableBtn;
	GtkWidget *hostEntry;
	GtkWidget *portEntry;
	GtkWidget *userEntry;
	GtkWidget *passEntry;
	GtkWidget *errorLabel;
} FxProxy;

typedef struct
{
	GtkWidget* dialog;
	GtkWidget* add_label;
	GtkWidget* add_entry;
	GtkWidget* ok_button;
	GtkWidget* cancel_button;
	FxMain* fxmain;
} FxAddGroup;

typedef struct
{
	FxMain* fxmain;
	char userid[16];
	GtkWidget* dialog;
	GtkWidget* image ;
	GtkWidget* sid_label ;
	GtkWidget* sid_entry ;
	GtkWidget* gender_label ;
	GtkWidget* gender_entry ;
	GtkWidget* mno_label ;
	GtkWidget* mno_entry ;
	GtkWidget* nick_label ;
	GtkWidget* nick_entry ;
	GtkWidget* impre_label ;
	GtkWidget* impre_entry ;
	GtkWidget* local_label ;
	GtkWidget* local_entry ;
	GtkWidget* nation_label ;
	GtkWidget* nation_entry ;
	GtkWidget* province_label ;
	GtkWidget* province_entry ;
	GtkWidget* city_label ;
	GtkWidget* city_entry ;
	GtkWidget* finish_button ;
} FxProfile;

typedef struct
{
	char userid[16];
	GtkTreeIter iter;
	FxMain* fxmain;

	GtkWidget* dialog;
	GtkWidget* remark_label;
	GtkWidget* remark_entry;
	GtkWidget* ok_button;
	GtkWidget* cancel_button;
} FxEdit;

typedef struct
{
	int groupid;	
	GtkTreeIter iter;
	FxMain* fxmain;

	GtkWidget* dialog;
	GtkWidget* remark_label;
	GtkWidget* remark_entry;
	GtkWidget* ok_button;
	GtkWidget* cancel_button;
} FxGEdit;

typedef struct
{
	FxMain* fxmain;
	GtkWidget* dialog;
	GtkWidget* remark_label;
	GtkWidget* remark_entry;
	GtkWidget* ok_button;
	GtkWidget* cancel_button;
} FxLookup;

typedef struct
{
	FxMain* fxmain;
	Contact* contact;
	GtkWidget* dialog;
	GtkWidget* image ;
	GtkWidget* sid_label ;
	GtkWidget* sid_entry ;
	GtkWidget* gender_label ;
	GtkWidget* gender_entry ;
	GtkWidget* mno_label ;
	GtkWidget* mno_entry ;
	GtkWidget* nick_label ;
	GtkWidget* nick_entry ;
	GtkWidget* impre_label ;
	GtkWidget* impre_entry ;
	GtkWidget* birth_label ;
	GtkWidget* birth_entry ;
	GtkWidget* nation_label ;
	GtkWidget* nation_entry ;
	GtkWidget* province_label ;
	GtkWidget* province_entry ;
	GtkWidget* city_label ;
	GtkWidget* city_entry ;
	GtkWidget* ok_button ;
	GtkWidget* cancel_button ;
} FxLookupres;

typedef struct
{
	FxMain* fxmain;
	Conversation* conv;
	ChatDialogFocusedType hasFocus;
	gboolean sendtophone;
	StateType state;
	int unreadMsgCount;
	/* main widget begin */
	GtkWidget* dialog;
	GtkWidget* headbox;
	GdkPixbuf* headpix;
	GtkWidget* headimage;
	GtkWidget* name_box;
	GtkWidget* name_label;
	GtkWidget* impre_label; 
	GtkWidget* recv_text;
	GtkWidget* send_text;
	GtkWidget* recv_scroll;
	GtkWidget* send_scroll;
	GtkTextMark* mark;
	GtkTextBuffer* send_buffer;
	GtkTextBuffer* recv_buffer;
	GtkTextIter send_iter;
	GtkTextIter recv_iter;
	/* toolbar begin */
	GtkWidget* toolbar;
	GtkWidget* tophone;
	GtkWidget* tocomputer;
	GtkWidget* historybutton;
	GtkWidget* nouge;
	GtkWidget* countLabel;
} FxChat;

typedef struct
{
	FxMain* fxmain;
	Conversation* conv;
	GtkWidget* dialog;
	GtkWidget* headbox;
	GdkPixbuf* headpix;
	GtkWidget* headimage;
	GtkWidget* name_label;
	GtkWidget* impre_label; 
	GtkWidget* recv_text;
	GtkWidget* send_text;
	GtkWidget* recv_scroll;
	GtkWidget* send_scroll;
	GtkTextMark* mark;
	GtkTextBuffer* send_buffer;
	GtkTextBuffer* recv_buffer;
	GtkTextIter send_iter;
	GtkTextIter recv_iter;
} FxMyself;

typedef struct
{
	FxMain* fxmain;
	char userid[16];
	char name[48];
	GtkWidget* dialog;
	GtkWidget* toolbar;
	GtkWidget* textview;
	GtkWidget* daycombo;
	GtkWidget* closebtn;
} FxHistory;

typedef struct
{
	FxMain* fxmain;	
	int chooseCount;
	GtkWidget* dialog;
	/*left*/
	GtkWidget* hbox;
	GtkWidget* tree;
	GtkWidget* selected;
	/*right*/
	GtkWidget* label;
	GtkWidget* close_button;
	GtkWidget* send_button;
	GtkWidget* recv_text;
	GtkWidget* send_text;
	GtkWidget* recv_scroll;
	GtkWidget* send_scroll;
	GtkTextMark* mark;
	GtkTextBuffer* send_buffer;
	GtkTextBuffer* recv_buffer;
	GtkTextIter send_iter;
	GtkTextIter recv_iter;
} FxMany;

typedef struct
{
	FxMain* fxmain;
	FxList* phraselist;
	int phraseid;
	NumberType notype;
	GtkWidget* dialog;
	GtkWidget* fetion_button;
	GtkWidget* mobile_button;
	GtkWidget* fetion_entry;
	GtkWidget* mobile_entry;
	GSList* group;
	GtkWidget* tablebox;
	GtkWidget* group_combo;
	GtkWidget* name_entry;
	GtkWidget* myname_entry;
	GtkWidget* msgbox; 

} FxAddbuddy;

typedef enum
{
	CODE_ERROR ,
	CODE_NOT_ERROR
} ErrorType;

typedef struct
{
	char reason[256];
	char tip[256];
	ErrorType error;
	FxMain* fxmain;
	GtkWidget *dialog;
	GtkWidget *reasonlabel;
	GtkWidget *codeentry;
	GtkWidget *tiplabel;
	GtkWidget *codepic;
	GtkWidget *codebox;
} FxCode;

typedef struct
{
	FxMain *fxmain;
	char sipuri[48];
	char userid[16];
	char desc[48];
	int phraseid;

	GtkWidget *dialog;
	GtkWidget *msgLabel;
	GSList *rdoList;
	GtkWidget *aptButton;
	GtkWidget *rjtButton;
	GtkWidget *lnEntry;
	GtkWidget *agCombo;
} FxApp;

typedef struct
{
	FxMain *fxmain;

	GtkWidget *dialog;
	GtkWidget *notebook;
	GtkWidget *psetting;
	GtkWidget *ssetting;
	/* system setting */
	GtkWidget *apBtn;
	GtkWidget *apEty;
	GtkWidget *ppCb;
	GtkWidget *muteBtn;
	GtkWidget *smallBtn;
	GtkWidget *ctBtn;
	GtkWidget *etBtn;
	GtkWidget *alertBtn;
	GtkWidget *iconBtn;
	/* personal setting */
	GtkWidget* image ;
	GtkWidget* sid_label ;
	GtkWidget* sid_entry ;
	GtkWidget* gender_label ;
	GtkWidget* mno_label ;
	GtkWidget* mno_entry ;
	GtkWidget* nick_label ;
	GtkWidget* nick_entry ;
	GtkWidget* impre_label ;
	GtkWidget* impre_entry ;
	GtkWidget* province_label ;
	GtkWidget* city_label ;
	GtkWidget* city_entry ;
	GtkWidget* email_label;
	GtkWidget* email_entry;
	GtkWidget* job_label;
	GtkWidget* job_entry;
	GtkWidget* gender_combo;
	GtkWidget* province_entry;
} FxSet;

typedef struct
{
	FxChat *fxchat;
	GtkWidget *dialog;
} FxEmotion;

typedef struct
{
	FxMain* fxmain;
	FetionSip* sip;
	Conversation* conv;
} ThreadArgs;

typedef struct
{
	FxMain *fxmain;
	Share *share;
	Contact *contact;

	GtkWidget *dialog;
	GtkWidget *progress;
	GtkWidget *uLabel;
	GtkWidget *iLabel;

} FxShare;

typedef struct
{
	FxMain *fxmain;
	
	GtkWidget *dialog;
	GtkWidget *codeEntry;
} FxConfirm;

typedef struct
{
	FxMain *fxmain;

	GtkWidget *dialog;
	GtkWidget *recvText;
	GtkWidget *sendText;
	GtkWidget *numberEntry;
	GtkWidget *addEventBox;
	GtkWidget *msgLabel;
	GtkWidget *chooseList;
	GtkWidget *countLabel;
	GtkWidget *checkBtn;
	GtkWidget *sigEntry;
} FxDSMS;

typedef struct
{
	FxMain *fxmain;

	GtkWidget *dialog;
	GtkWidget *closeBtn;
	GtkWidget *iconBtn;
	GtkWidget *notalert;
} FxClose;

typedef struct
{
	FxMain *fxmain;

	GtkWidget *window;
	GtkWidget *view;
}FxSearch;

typedef struct
{
	FxMain *fxmain;

	int hasFocus;
	PGGroup *pggroup;
	GtkWidget *window;
	GtkWidget *image;
	GtkWidget *nameLabel;
	GtkWidget *otherLabel;
	GtkWidget *phoneButton;
	GtkWidget *sendView;
	GtkWidget *recvView;
	GtkWidget *toolbar;
	GtkWidget *noticeView;
	GtkWidget *treeView;
} FxPGGroup;

typedef struct
{
	FxMain *fxmain;
	PGGroup *pggroup;
	GtkWidget *window;
	GtkWidget *image;
	GtkWidget *sidEntry;
	GtkWidget *nameEntry;
	GtkWidget *leaderEntry;
	GtkWidget *timeEntry;
	GtkWidget *introView;
	GtkWidget *bulletinView;
} FxPGProfile;

typedef struct
{
	FxMain *fxmain;

	GtkWidget *dialog;
	GtkWidget *label;
	GSList *gslist;
} FxSmsstat;

#endif
