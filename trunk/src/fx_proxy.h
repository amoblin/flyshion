#ifndef FX_PROXY_H
#define FX_PROXY_H

extern FxProxy *fx_proxy_new(FxMain *fxmain);

extern void fx_proxy_initialize(FxProxy *fxproxy);

extern void fx_proxy_on_ok_clicked(GtkWidget *widget , gpointer data);

extern void fx_proxy_on_cancel_clicked(GtkWidget *widget , gpointer data);
#endif
