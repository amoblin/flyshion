#ifndef FX_DSMS_H
#define FX_DSMS_H

enum{
	PIXBUF_COL ,
	NUMBER_COL ,
	COL_SIZE
};

struct deleteargs{
	FxDSMS *fxdsms;
	GtkTreeIter iter;
};

extern FxConfirm* fx_confirm_new(FxMain *fxmain); 

extern void fx_confirm_initialize(FxConfirm *fxconfirm);

extern FxDSMS* fx_dsms_new(FxMain *fxmain);

extern void fx_dsms_initialize(FxDSMS *fxdsms);

#endif
