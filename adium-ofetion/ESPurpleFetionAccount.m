//
//  ESPurpleFetionAccount.m
//  AdiumFetionPlugin
//
//  Created by Evan JIANG on 09-9-4.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "ESPurpleFetionAccount.h"

@implementation ESPurpleFetionAccount
- (const char*)protocolPlugin
{
	return "prpl-fetion";
}

- (NSSet *)supportedPropertyKeys
{
	static NSMutableSet *supportedPropertyKeys = nil;
	
	if (!supportedPropertyKeys) {
		supportedPropertyKeys = [[NSMutableSet alloc] initWithObjects:
								 @"AvailableMessage",
								 @"Invisible",
								 nil];
		[supportedPropertyKeys unionSet:[super supportedPropertyKeys]];
	}
	
	return supportedPropertyKeys;
}

- (NSString *)host
{
	return @"nav.fetion.com.cn";
}

- (BOOL)shouldSetAliasesServerside
{
	return YES;
}

- (void)configurePurpleAccount
{
	[super configurePurpleAccount];
	
	BOOL useMobileOnline = [[self preferenceForKey:KEY_FETION_USE_MOBILE_ONLINE group:GROUP_ACCOUNT_STATUS] boolValue];
	purple_account_set_bool(account, "use_mobile_online", useMobileOnline);

	purple_account_set_string(account, "realname", [[self preferenceForKey:KEY_ACCOUNT_DISPLAY_NAME group:GROUP_ACCOUNT_STATUS] UTF8String]);
}
@end
