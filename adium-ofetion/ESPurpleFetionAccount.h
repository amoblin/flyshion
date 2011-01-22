//
//  ESPurpleFetionAccount.h
//  AdiumFetionPlugin
//
//  Created by Evan JIANG on 09-9-4.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "AdiumLibpurple/CBPurpleAccount.h"

#define AILocalizedString(key, comment) \
AILocalizedStringFromTableInBundle(key, nil, [NSBundle bundleForClass:[self class]], comment)

#define AILocalizedStringFromTable(key, table, comment) \
AILocalizedStringFromTableInBundle(key, table, [NSBundle mainBundle], comment)

#define AILocalizedStringFromTableInBundle(key, table, bundle, comment) \
NSLocalizedStringFromTableInBundle(key, table, bundle, comment)

#define KEY_FETION_USE_MOBILE_ONLINE	@"FETION:USE_MOBILE_ONLINE"

@interface ESPurpleFetionAccount : CBPurpleAccount {

}

@end
