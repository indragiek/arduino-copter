//
//  CPTBluetoothManager.h
//  CopterControl
//
//  Created by Indragie Karunaratne on 11/28/2013.
//  Copyright (c) 2013 Indragie Karunaratne. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>

typedef NS_ENUM(NSInteger, CPTBluetoothPeripheralConnectionState) {
	CPTBluetoothPeripheralConnectionStateDisconnected = 0,
	CPTBluetoothPeripheralConnectionStateConnecting,
	CPTBluetoothPeripheralConnectionStateConnected,
	CPTBluetoothPeripheralConnectionStateConnectionFailure
};

typedef void (^CPTBluetoothDiscoveryBlock)(CBPeripheral *peripheral, NSDictionary *advertisementData, NSNumber *RSSI);
typedef void (^CPTBluetoothConnectionBlock)(CPTBluetoothPeripheralConnectionState state, NSError *error);
typedef void (^CPTBluetoothDataBlock)(NSData *data);

@interface CPTBluetoothManager : NSObject
@property (nonatomic, strong, readonly) CBPeripheral *activePeripheral;
- (void)scanPeripheralsWithDiscoveryHandler:(CPTBluetoothDiscoveryBlock)discovery error:(NSError **)error;
- (void)stopScan;
- (void)connectToPeripheral:(CBPeripheral *)peripheral
	 connectionStateHandler:(CPTBluetoothConnectionBlock)connection
				dataHandler:(CPTBluetoothDataBlock)data;
- (void)write:(NSData *)data;
@end
