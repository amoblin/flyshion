#include "fx_include.h"

FxProxy *fx_proxy_new(FxMain *fxmain)
{
	FxProxy *fxproxy = (FxProxy*)malloc(sizeof(FxProxy));

	DEBUG_FOOTPRINT();

	memset(fxproxy , 0 , sizeof(FxProxy));
	fxproxy->fxmain = fxmain;
	return fxproxy;
}

void fx_proxy_initialize(FxProxy *fxproxy)
{
	GtkWidget *vbox = NULL;
	GtkWidget *action_area = NULL;
	GtkWidget *hostLabel = NULL;
	GtkWidget *portLabel = NULL;
	GtkWidget *userLabel = NULL;
	GtkWidget *passLabel = NULL;

	DEBUG_FOOTPRINT();

	fxproxy->dialog = gtk_dialog_new();

}

void fx_proxy_on_ok_clicked(GtkWidget *widget , gpointer data)
{

}

void fx_proxy_on_cancel_clicked(GtkWidget *widget , gpointer data)
{

}
