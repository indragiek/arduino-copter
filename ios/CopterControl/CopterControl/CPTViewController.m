//
//  CPTViewController.m
//  CopterControl
//
//  Created by Indragie Karunaratne on 11/26/2013.
//  Copyright (c) 2013 Indragie Karunaratne. All rights reserved.
//

#import "CPTViewController.h"
#import "CPTBluetoothManager.h"
#import "SVProgressHUD.h"

@interface CPTViewController ()
@property (nonatomic, strong, readonly) CPTBluetoothManager *bluetoothManager;
@property (nonatomic, weak) IBOutlet UILabel *scoreLabel;
@property (nonatomic, weak) IBOutlet UILabel *highScoreLabel;
@property (nonatomic, weak) IBOutlet UIView *scoreHeaderView;
@property (nonatomic, weak) IBOutlet UIView *highScoreHeaderView;

@property (nonatomic, assign) NSUInteger score;
@property (nonatomic, assign) NSUInteger highScore;

- (IBAction)buttonDown:(id)sender;
- (IBAction)buttonUp:(id)sender;
- (IBAction)playPause:(id)sender;
@end

@implementation CPTViewController
@synthesize bluetoothManager = _bluetoothManager;

- (void)viewDidLoad
{
    [super viewDidLoad];
	self.view.backgroundColor = [UIColor colorWithWhite:0.f alpha:0.5f];

	// Defer to next iteration of the run loop.
	dispatch_async(dispatch_get_main_queue(), ^{
		[self scanBluetoothDevices];
	});
}

- (void)scanBluetoothDevices
{
	[SVProgressHUD showWithStatus:@"Scanning for Devices" maskType:SVProgressHUDMaskTypeGradient];
	NSError *error = nil;
	[self.bluetoothManager scanPeripheralsWithDiscoveryHandler:^(CBPeripheral *peripheral, NSDictionary *advertisementData, NSNumber *RSSI) {
		[self.bluetoothManager stopScan];
		[self.bluetoothManager connectToPeripheral:peripheral connectionStateHandler:^(CPTBluetoothPeripheralConnectionState state, NSError *error) {
			[self handleConnectionState:state error:error];
		} dataHandler:^(NSData *data) {
			[self handleReceivedData:data];
		}];
	} error:&error];
	if (error) {
		[SVProgressHUD showErrorWithStatus:error.localizedDescription];
	}
}

- (void)handleConnectionState:(CPTBluetoothPeripheralConnectionState)state error:(NSError *)error
{
	switch (state) {
		case CPTBluetoothPeripheralConnectionStateConnected:
			[SVProgressHUD showSuccessWithStatus:@"Connected to Device"];
			break;
		case CPTBluetoothPeripheralConnectionStateConnecting:
			[SVProgressHUD showWithStatus:@"Connecting to Device" maskType:SVProgressHUDMaskTypeGradient];
			break;
		case CPTBluetoothPeripheralConnectionStateConnectionFailure:
		case CPTBluetoothPeripheralConnectionStateDisconnected:
		{
			[SVProgressHUD showErrorWithStatus:error.localizedDescription];
			double delayInSeconds = 1.0; // SVProgressHUD docs define the duration
			dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayInSeconds * NSEC_PER_SEC));
			dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
				[self scanBluetoothDevices];
			});
			break;
		}
		default:
			break;
	}
}

- (void)handleReceivedData:(NSData *)data
{
	uint8_t byte;
	[data getBytes:&byte length:1];
	NSLog(@"%d", byte);
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

#pragma mark - Accessors

- (CPTBluetoothManager *)bluetoothManager
{
	if (_bluetoothManager == nil) {
		_bluetoothManager = [CPTBluetoothManager new];
	}
	return _bluetoothManager;
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
