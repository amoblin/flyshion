//
//  AIFetionPlugin.m
//  AdiumFetionPlugin
//
//  Created by Evan JIANG on 09-3-14.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "AIFetionPlugin.h"
#import "ESFetionService.h"

#import "fetion.h"

@implementation AIFetionPlugin
- (void)installPlugin
{
	setResourcePath([[[NSBundle bundleForClass:[self class]] resourcePath] UTF8String]);
	purple_init_openfetion_plugin();
	[ESFetionService registerService];
}

- (NSString *)pluginAuthor
{
	return @"Evan JIANG <firstfan@gmail.com>";
}

-(NSString *)pluginVersion
{
	return @"0.60";
}

-(NSString *)pluginDescription
{
	return @"China Mobile Fetion Instant Messenger";
}
@end
