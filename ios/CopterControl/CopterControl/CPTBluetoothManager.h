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

/**
 *  Class that handles Bluetooth interfacing with the BLE shield.
 */
@interface CPTBluetoothManager : NSObject
/**
 *  Returns the currently connected Bluetooth peripheral or nil if there is none.
 */
@property (nonatomic, strong, readonly) CBPeripheral *activePeripheral;
/**
 *  Starts a scan for available BLE Shield devices and calls the discovery handler
 *  block for each one.
 *
 *  @param discovery The block to call for each discovered device.
 *  @param error     Pointer to an error to be set if the scan can not start.
 */
- (void)scanPeripheralsWithDiscoveryHandler:(CPTBluetoothDiscoveryBlock)discovery error:(NSError **)error;

/**
 *  Stops a scan previously started using `scanPeripheralsWithDiscoveryHandler:error:`
 */
- (void)stopScan;

/**
 *  Connects to a specified Bluetooth peripheral device.
 *
 *  @param peripheral Bluetooth peripheral to connect to.
 *  @param connection A block handler for changes in connection state.
 *  @param data       A block handler for data received from the device.
 */
- (void)connectToPeripheral:(CBPeripheral *)peripheral
	 connectionStateHandler:(CPTBluetoothConnectionBlock)connection
				dataHandler:(CPTBluetoothDataBlock)data;

/**
 *  Writes data to the active (connected) peripheral.
 *
 *  @param data The data to write.
 */
- (void)write:(NSData *)data;

/**
 *  Writes bytes to the active (connected) peripheral
 *
 *  @param bytes The byte array.
 *  @param len   The length of the byte array.
 */
- (void)writeBytes:(const unsigned char *)bytes length:(NSUInteger)len;
@end
