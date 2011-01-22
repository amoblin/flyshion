//
//  ESPurpleFetionAccountViewController.m
//  AdiumFetionPlugin
//
//  Created by Evan JIANG on 09-9-5.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "ESPurpleFetionAccountViewController.h"
#import <Adium/AIAccount.h>
#import "ESPurpleFetionAccount.h"

@implementation ESPurpleFetionAccountViewController
- (NSString *)nibName{
    return @"ESPurpleFetionAccountView";
}

- (void)configureForAccount:(AIAccount *)inAccount
{
	[super configureForAccount:inAccount];
	
	[checkBox_useMobileOnline setState:[[account preferenceForKey:KEY_FETION_USE_MOBILE_ONLINE 
															  group:GROUP_ACCOUNT_STATUS] boolValue]];
}

- (void)saveConfiguration
{
	[account setPreference:[NSNumber numberWithBool:[checkBox_useMobileOnline state]] 
					forKey:KEY_FETION_USE_MOBILE_ONLINE group:GROUP_ACCOUNT_STATUS];
	
	[super saveConfiguration];
}
@end
