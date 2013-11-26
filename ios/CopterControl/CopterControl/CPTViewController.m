//
//  CPTViewController.m
//  CopterControl
//
//  Created by Indragie Karunaratne on 11/26/2013.
//  Copyright (c) 2013 Indragie Karunaratne. All rights reserved.
//

#import "CPTViewController.h"

#define CPTViewBackgroundColor	[UIColor colorWithRed:0.14 green:0.14 blue:0.16 alpha:1.0]
#define CPTViewScoreFont		[UIFont fontWithName:@"DBLCDTempBlack" size:24.f]
#define CPTViewScoreTextColor	[UIColor colorWithRed:0.27 green:1.00 blue:0.08 alpha:1.0]
#define CPTViewHiScoreTextColor	[UIColor colorWithRed:0.24 green:0.58 blue:0.88 alpha:1.0]

@interface CPTViewController ()
@property (nonatomic, weak) IBOutlet UILabel *scoreLabel;
@property (nonatomic, weak) IBOutlet UILabel *highScoreLabel;
- (IBAction)buttonDown:(id)sender;
- (IBAction)buttonUp:(id)sender;
- (IBAction)playPause:(id)sender;
@end

@implementation CPTViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
	self.view.backgroundColor = CPTViewBackgroundColor;
	self.scoreLabel.font = CPTViewScoreFont;
	self.highScoreLabel.font = CPTViewScoreFont;
	self.scoreLabel.textColor = CPTViewScoreTextColor;
	self.highScoreLabel.textColor = CPTViewHiScoreTextColor;
	
	self.scoreLabel.text = @"1026";
	self.highScoreLabel.text = @"11230";
}

#pragma mark - Actions

- (IBAction)buttonDown:(id)sender
{
	NSLog(@"Button down");
}

- (IBAction)buttonUp:(id)sender
{
	NSLog(@"Button up");
}

- (IBAction)playPause:(UIButton *)sender
{
	sender.selected = !sender.selected;
	NSLog(@"Play pause");
}

@end
