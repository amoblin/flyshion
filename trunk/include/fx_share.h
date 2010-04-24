#ifndef FX_SHARE_H
#define FX_SHARE_H

extern FxShare *fx_share_new(FxMain *fxmain , const char *sipuri);

extern void fx_share_initialize(FxShare *fxshare);

extern FxShare *fx_share_find_by_sipuri(FxList *fxlist , const char *sipuri);

extern void fx_share_start_transfer(FxShare *fxshare);

#endif
