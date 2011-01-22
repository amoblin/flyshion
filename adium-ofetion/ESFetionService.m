//
//  ESFetionService.m
//  AdiumFetionPlugin
//
//  Created by Evan JIANG on 09-9-4.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Adium/AISharedAdium.h>
#import <Adium/AIStatusControllerProtocol.h>
#import <AIUtilities/AICharacterSetAdditions.h>
#import <AIUtilities/AIImageAdditions.h>

#import "ESFetionService.h"
#import "ESPurpleFetionAccount.h"
#import "ESPurpleFetionAccountViewController.h"
#import "fetion.h"

@implementation ESFetionService
- (id)init
{
	if ((self = [super init])) {
//		moodTooltip = [[AMPurpleJabberMoodTooltip alloc] init];
//		[adium.interfaceController registerContactListTooltipEntry:moodTooltip secondaryEntry:YES];
	}
	
	return self;
}

- (void)dealloc
{
//	[adium.interfaceController unregisterContactListTooltipEntry:moodTooltip secondaryEntry:YES];
//	[moodTooltip release]; moodTooltip = nil;
	
	[super dealloc];
}

//Account Creation
- (Class)accountClass{
	return [ESPurpleFetionAccount class];
}

- (AIAccountViewController *)accountViewController{
    return [ESPurpleFetionAccountViewController accountViewController];
}

- (DCJoinChatViewController *)joinChatView{
//	return [DCPurpleJabberJoinChatViewController joinChatView];
	return nil;
}

- (BOOL)canCreateGroupChats{
	return YES;
}

//Service Description
- (NSString *)serviceCodeUniqueID{
	return @"libpurple-Fetion";
}
- (NSString *)serviceID{
	return @"Fetion";
}
- (NSString *)serviceClass{
	return @"Fetion";
}
- (NSString *)shortDescription{
	return @"Fetion";
}
- (NSString *)longDescription{
	return @"Fetion";
}



- (NSCharacterSet *)allowedCharacters{
	return [NSCharacterSet decimalDigitCharacterSet];
}


- (NSUInteger)allowedLength{
	return 11;
}

- (BOOL)caseSensitive{
	return NO;
}
- (AIServiceImportance)serviceImportance{
	return AIServicePrimary;
}
- (BOOL)canRegisterNewAccounts{
	return YES;
}
- (NSString *)userNameLabel{
    return AILocalizedString(@"Mobile NO.","");    //Sign-in name
}

- (void)registerStatuses{
	[adium.statusController registerStatus:STATUS_NAME_AVAILABLE
						   withDescription:[adium.statusController localizedDescriptionForCoreStatusName:STATUS_NAME_AVAILABLE]
									ofType:AIAvailableStatusType
								forService:self];
	
	[adium.statusController registerStatus:STATUS_NAME_AWAY
						   withDescription:[adium.statusController localizedDescriptionForCoreStatusName:STATUS_NAME_AWAY]
									ofType:AIAwayStatusType
								forService:self];
	
	[adium.statusController registerStatus:STATUS_NAME_BUSY
						   withDescription:[adium.statusController localizedDescriptionForCoreStatusName:STATUS_NAME_BUSY]
									ofType:AIAvailableStatusType
								forService:self];
	
	[adium.statusController registerStatus:STATUS_NAME_EXTENDED_AWAY
						   withDescription:[adium.statusController localizedDescriptionForCoreStatusName:STATUS_NAME_EXTENDED_AWAY]
									ofType:AIAwayStatusType
								forService:self];
	
	[adium.statusController registerStatus:STATUS_NAME_INVISIBLE
						   withDescription:[adium.statusController localizedDescriptionForCoreStatusName:STATUS_NAME_INVISIBLE]
									ofType:AIInvisibleStatusType
								forService:self];
}

- (NSImage *)defaultServiceIconOfType:(AIServiceIconType)iconType
{
    if ((iconType == AIServiceIconSmall) || (iconType == AIServiceIconList)) {
        return [NSImage imageNamed:@"fetion_small" forClass:[self class]        loadLazily:YES];
    } else {
        return [NSImage imageNamed:@"fetion" forClass:[self class] loadLazily:  YES];
    }
}


@end