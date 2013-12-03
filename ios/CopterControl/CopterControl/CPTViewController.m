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

static NSString * const CPTUserDefaultsHighScoreKey = @"HighScore";

@interface CPTViewController ()
@property (nonatomic, strong, readonly) CPTBluetoothManager *bluetoothManager;
@property (nonatomic, strong, readonly) NSMutableData *buffer;
@property (nonatomic, assign) NSUInteger score;
@property (nonatomic, assign) NSUInteger highScore;

@property (nonatomic, weak) IBOutlet UILabel *scoreLabel;
@property (nonatomic, weak) IBOutlet UILabel *highScoreLabel;
@property (nonatomic, weak) IBOutlet UIView *scoreHeaderView;
@property (nonatomic, weak) IBOutlet UIView *highScoreHeaderView;
@property (nonatomic, weak) IBOutlet UIButton *playPauseButton;

- (IBAction)buttonDown:(id)sender;
- (IBAction)buttonUp:(id)sender;
- (IBAction)playPause:(id)sender;
@end

@implementation CPTViewController
@synthesize bluetoothManager = _bluetoothManager;

#pragma mark - Initialization

- (void)commonInitForCPTViewController
{
	_buffer = [NSMutableData data];
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
	if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
		[self commonInitForCPTViewController];
	}
	return self;
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
	if ((self = [super initWithCoder:aDecoder])) {
		[self commonInitForCPTViewController];
	}
	return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	self.view.backgroundColor = [UIColor colorWithWhite:0.f alpha:0.5f];
	self.score = 0;
	self.highScore = [[NSUserDefaults.standardUserDefaults objectForKey:CPTUserDefaultsHighScoreKey] unsignedIntegerValue];

	// Defer to next iteration of the run loop.
	dispatch_async(dispatch_get_main_queue(), ^{
		[self scanBluetoothDevices];
	});
}

#pragma mark - Bluetooth

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


// ======== Specification ========
// (From bt_transmitter.h)
//
// This specification defines the communication protocol used by copter to
// exchange data between the game and an external controller device. The
// following commands are implemented:
//
// 1) RECEIVE: Button Press Down
//    Byte sequence: 0x01 0x01
//
// 2) RECEIVE: Button Press Up
//    Byte sequence: 0x01 0x00
//
// 3) RECEIVE: Toggle play/pause
//    Byte sequence: 0x02
//
// 4) SEND: Game reset signal.
//    Byte sequence: 0x03
//
// 3) SEND: Update score.
//    Byte sequence: 0x04 <32 bit integer>
//
- (void)handleReceivedData:(NSData *)data
{
	[self.buffer appendData:data];
	
	unsigned char byte;
	const NSRange byteRange = NSMakeRange(0, 1);
	[self.buffer getBytes:&byte range:byteRange];
	if (byte == 0x04) {
		const int totalLength = sizeof(uint32_t) + 1;
		if (self.buffer.length >= totalLength) {
			self.score = [self readUInt32FromBuffer];
		}
	} else if (byte == 0x03) {
		self.playPauseButton.selected = NO;
		self.score = 0;
		[self removeBytesInRangeFromBuffer:byteRange];
	} else {
		// Clear out the entire buffer because we have some garbage data.
		// This probably isn't the best way to handle this...
		[self.buffer setData:[NSData data]];
	}
}

- (uint32_t)readUInt32FromBuffer
{
	const size_t size = sizeof(uint32_t);
	uint32_t score;
	[self.buffer getBytes:&score range:NSMakeRange(1, size)];
	[self removeBytesInRangeFromBuffer:NSMakeRange(0, size + 1)];
	return score;
}

- (void)removeBytesInRangeFromBuffer:(NSRange )range
{
	[self.buffer replaceBytesInRange:range withBytes:NULL length:0];
}

#pragma mark - Accessors

- (CPTBluetoothManager *)bluetoothManager
{
	if (_bluetoothManager == nil) {
		_bluetoothManager = [CPTBluetoothManager new];
	}
	return _bluetoothManager;
}

- (void)setScore:(NSUInteger)score
{
	_score = score;
	self.scoreLabel.text = @(score).stringValue;
}

- (void)setHighScore:(NSUInteger)highScore
{
	_highScore = highScore;
	self.highScoreLabel.text = @(highScore).stringValue;
	[NSUserDefaults.standardUserDefaults setObject:@(highScore) forKey:CPTUserDefaultsHighScoreKey];
}

#pragma mark - Actions

- (IBAction)buttonDown:(id)sender
{
	const unsigned char bytes[] = {0x01, 0x01};
	[self.bluetoothManager writeBytes:bytes length:2];
}

- (IBAction)buttonUp:(id)sender
{
	const unsigned char bytes[] = {0x01, 0x00};
	[self.bluetoothManager writeBytes:bytes length:2];
}

- (IBAction)playPause:(UIButton *)sender
{
	sender.selected = !sender.selected;
	const unsigned char bytes[] = {0x02};
	[self.bluetoothManager writeBytes:bytes length:1];
}

@end
