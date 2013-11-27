//
//  CPTViewController.m
//  CopterControl
//
//  Created by Indragie Karunaratne on 11/26/2013.
//  Copyright (c) 2013 Indragie Karunaratne. All rights reserved.
//

#import "CPTViewController.h"
#import "SVProgressHUD.h"

@interface CPTViewController ()
@property (nonatomic, weak) IBOutlet UILabel *scoreLabel;
@property (nonatomic, weak) IBOutlet UILabel *highScoreLabel;
@property (nonatomic, weak) IBOutlet UIView *scoreHeaderView;
@property (nonatomic, weak) IBOutlet UIView *highScoreHeaderView;
- (IBAction)buttonDown:(id)sender;
- (IBAction)buttonUp:(id)sender;
- (IBAction)playPause:(id)sender;
@end

@implementation CPTViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
	self.view.backgroundColor = [UIColor colorWithWhite:0.f alpha:0.5f];
	self.scoreLabel.text = @"6026";
	self.highScoreLabel.text = @"11230";

	// Defer to next iteration of the run loop.
	dispatch_async(dispatch_get_main_queue(), ^{
		[self scanBluetoothDevices];
	});
}

- (void)scanBluetoothDevices
{
	[SVProgressHUD showWithStatus:@"Scanning for Devices" maskType:SVProgressHUDMaskTypeGradient];
	double delayInSeconds = 2.0;
	dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayInSeconds * NSEC_PER_SEC));
	dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
		//[SVProgressHUD dismiss];
		//[self showBluetoothDeviceNotFoundAlert];
		[SVProgressHUD showSuccessWithStatus:@"Found device"];
	});
}

- (void)showBluetoothDeviceNotFoundAlert
{
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"No devices found."
													message:@"Check that the Arduino board and BLE shield are powered on."
												   delegate:self
										  cancelButtonTitle:@"Rescan"
										  otherButtonTitles:nil];
	[alert show];
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	[self scanBluetoothDevices];
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
