//
//  CPTBluetoothManager.m
//  CopterControl
//
//  Created by Indragie Karunaratne on 11/28/2013.
//  Copyright (c) 2013 Indragie Karunaratne. All rights reserved.
//

#import "CPTBluetoothManager.h"

static NSString * const RBL_SERVICE_UUID = @"713D0000-503E-4C75-BA94-3148F18D941E";
static NSString * const RBL_CHAR_TX_UUID = @"713D0002-503E-4C75-BA94-3148F18D941E";
static NSString * const RBL_CHAR_RX_UUID = @"713D0003-503E-4C75-BA94-3148F18D941E";

static NSString * const CPTHandlerDiscoveryKey = @"discovery";
static NSString * const CPTBluetoothErrorDomain = @"CPTBluetoothErrorDomain";

@interface CPTBluetoothManager () <CBCentralManagerDelegate, CBPeripheralDelegate>
@property (nonatomic, strong, readonly) NSMutableDictionary *blocks;
@property (nonatomic, strong, readonly) NSMutableArray *powerOnBlockQueue;
@property (nonatomic, strong, readonly) CBCentralManager *manager;
@property (nonatomic, readonly) dispatch_queue_t delegateQueue;

@property (nonatomic, strong, readwrite) CBPeripheral *activePeripheral;
@property (nonatomic, strong) CBCharacteristic *RXCharacteristic;
@property (nonatomic, strong) CBCharacteristic *TXCharacteristic;
@end

@implementation CPTBluetoothManager

- (id)init
{
	if ((self = [super init])) {
		_blocks = [NSMutableDictionary dictionary];
		_delegateQueue = dispatch_queue_create("com.indragie.CPTBluetoothManager", DISPATCH_QUEUE_SERIAL);
		_manager = [[CBCentralManager alloc] initWithDelegate:self queue:self.delegateQueue];
	}
	return self;
}

- (void)scanPeripheralsWithDiscoveryHandler:(CPTBluetoothDiscoveryBlock)discovery error:(NSError **)error
{
	NSError *managerError = nil;
	BOOL stateCheck = [self checkCentralManagerStateWithError:&managerError];
	if (managerError) {
		if (error) *error = managerError;
		return;
	} else if (stateCheck == NO) {
		__weak __typeof__(self) weakSelf = self;
		[self queuePowerOnBlock:^{
			__typeof__(self) strongSelf = weakSelf;
			[strongSelf scanPeripheralsWithDiscoveryHandler:discovery error:error];
		}];
	} else {
		if (discovery) {
			[self setBlock:discovery forKey:CPTHandlerDiscoveryKey];
		}
		CBUUID *UUID = [CBUUID UUIDWithString:RBL_SERVICE_UUID];
		[self.manager scanForPeripheralsWithServices:@[UUID] options:nil];
	}
}

- (void)stopScan
{
	[self.manager stopScan];
}

- (void)connectToPeripheral:(CBPeripheral *)peripheral
	 connectionStateHandler:(CPTBluetoothConnectionBlock)connection
				dataHandler:(CPTBluetoothDataBlock)data
{
	self.activePeripheral = peripheral;
	self.activePeripheral.delegate = self;
	if (connection) {
		[self setConnectionBlock:connection forPeripheral:peripheral];
	}
	if (data) {
		[self setDataBlock:data forPeripheral:peripheral];
	}
	[self.manager connectPeripheral:peripheral options:@{CBConnectPeripheralOptionNotifyOnConnectionKey : @YES,
		  CBConnectPeripheralOptionNotifyOnDisconnectionKey : @YES}];
	[self handleState:CPTBluetoothPeripheralConnectionStateConnecting forPeripheral:peripheral error:nil];
}

- (void)write:(NSData *)data
{
	[self.activePeripheral writeValue:data forCharacteristic:self.RXCharacteristic type:CBCharacteristicWriteWithoutResponse];
}

- (BOOL)checkCentralManagerStateWithError:(NSError **)error
{
	CBCentralManagerState state = self.manager.state;
	NSString *errorText = nil;
	if (state == CBCentralManagerStateUnsupported) {
		errorText = @"Bluetooth is unsupported on this device.";
	} else if (state == CBCentralManagerStateUnauthorized) {
		errorText = @"This application is unauthorized to access Bluetooth functionality";
	}
	if (errorText) {
		if (error) *error = [self errorWithDescription:errorText];
		return NO;
	}
	return YES;
}

#pragma mark - Error Handling

- (NSError *)errorWithDescription:(NSString *)description
{
	return [NSError errorWithDomain:CPTBluetoothErrorDomain
							   code:0
						   userInfo:@{NSLocalizedDescriptionKey : description ?: @""}];
}

#pragma mark - CBCentralManagerDelegate

- (void)centralManagerDidUpdateState:(CBCentralManager *)central
{
	switch (central.state) {
		case CBCentralManagerStatePoweredOn: {
			for (void (^block)() in self.powerOnBlockQueue) {
				block();
			}
			[self.powerOnBlockQueue removeAllObjects];
			break;
		}
		// TODO: Handle more states?
		default:
			break;
	}
}

- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI
{
	CPTBluetoothDiscoveryBlock block = [self blockForKey:CPTHandlerDiscoveryKey];
	if (block) {
		dispatch_async(dispatch_get_main_queue(), ^{
			block(peripheral, advertisementData, RSSI);
		});
	}
}

- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral
{
	[peripheral discoverServices:@[[CBUUID UUIDWithString:RBL_SERVICE_UUID]]];
}

- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
	[self handleState:CPTBluetoothPeripheralConnectionStateDisconnected forPeripheral:peripheral error:error];
}

- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
	[self handleState:CPTBluetoothPeripheralConnectionStateConnectionFailure forPeripheral:peripheral error:error];
}

- (void)handleState:(CPTBluetoothPeripheralConnectionState)state forPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
	CPTBluetoothConnectionBlock block = [self connectionBlockForPeripheral:peripheral];
	if (block) {
		dispatch_async(dispatch_get_main_queue(), ^{
			block(state, error);
		});
	}
	if (state == CPTBluetoothPeripheralConnectionStateConnectionFailure || state == CPTBluetoothPeripheralConnectionStateDisconnected) {
		[self.manager cancelPeripheralConnection:peripheral];
		[self removeBlocksForPeripheral:peripheral];
		self.RXCharacteristic = nil;
		self.TXCharacteristic = nil;
		self.activePeripheral = nil;
	}
}

#pragma mark - CBPeripheralDelegate

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error
{
	if (error) {
		[self handleState:CPTBluetoothPeripheralConnectionStateConnectionFailure forPeripheral:peripheral error:error];
	} else {
		CBService *service = peripheral.services.lastObject;
		[peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:RBL_CHAR_TX_UUID], [CBUUID UUIDWithString:RBL_CHAR_RX_UUID]] forService:service];
	}
}

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error
{
	CPTBluetoothPeripheralConnectionState state = CPTBluetoothPeripheralConnectionStateConnectionFailure;
	if (error == nil) {
		for (CBCharacteristic *characteristic in service.characteristics) {
			CBUUID *UUID = characteristic.UUID;
			if ([UUID isEqual:[CBUUID UUIDWithString:RBL_CHAR_TX_UUID]]) {
				self.TXCharacteristic = characteristic;
			} else if ([UUID isEqual:[CBUUID UUIDWithString:RBL_CHAR_RX_UUID]]) {
				self.RXCharacteristic = characteristic;
			}
		}
		if (self.TXCharacteristic != nil && self.RXCharacteristic != nil) {
			[peripheral setNotifyValue:YES forCharacteristic:self.TXCharacteristic];
			state = CPTBluetoothPeripheralConnectionStateConnected;
		}
	}
	[self handleState:state forPeripheral:peripheral error:error];
}

- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error
{
	if (characteristic == self.TXCharacteristic && error == nil) {
		CPTBluetoothDataBlock block = [self dataBlockForPeripheral:peripheral];
		if (block) {
			dispatch_async(dispatch_get_main_queue(), ^{
				block(characteristic.value);
			});
		}
	}
}

#pragma mark - Blocks

- (NSString *)dataBlockKeyForPeripheral:(CBPeripheral *)peripheral
{
	return [NSString stringWithFormat:@"data-%@", peripheral.identifier.UUIDString];
}

- (NSString *)connectionBlockKeyForPeripheral:(CBPeripheral *)peripheral
{
	return [NSString stringWithFormat:@"connection-%@", peripheral.identifier.UUIDString];
}

- (CPTBluetoothConnectionBlock)connectionBlockForPeripheral:(CBPeripheral *)peripheral
{
	return [self blockForKey:[self connectionBlockKeyForPeripheral:peripheral]];
}

- (void)setConnectionBlock:(CPTBluetoothConnectionBlock)block forPeripheral:(CBPeripheral *)peripheral
{
	[self setBlock:block forKey:[self connectionBlockKeyForPeripheral:peripheral]];
}

- (CPTBluetoothDataBlock)dataBlockForPeripheral:(CBPeripheral *)peripheral
{
	return [self blockForKey:[self dataBlockKeyForPeripheral:peripheral]];
}

- (void)setDataBlock:(CPTBluetoothDataBlock)block forPeripheral:(CBPeripheral *)peripheral
{
	[self setBlock:block forKey:[self dataBlockKeyForPeripheral:peripheral]];
}

- (void)removeBlocksForPeripheral:(CBPeripheral *)peripheral
{
	[self removeBlockForKey:[self connectionBlockKeyForPeripheral:peripheral]];
	[self removeBlockForKey:[self dataBlockKeyForPeripheral:peripheral]];
}

- (void)queuePowerOnBlock:(void(^)())block
{
	NSParameterAssert(block);
	[self.powerOnBlockQueue addObject:[block copy]];
}

- (void)setBlock:(id)block forKey:(NSString *)key
{
	NSParameterAssert(key);
	NSParameterAssert(block);
	@synchronized(self) {
		_blocks[key] = [block copy];
	}
}

- (id)blockForKey:(NSString *)key
{
	NSParameterAssert(key);
	@synchronized(self) {
		return _blocks[key];
	}
}

- (void)removeBlockForKey:(NSString *)key
{
	NSParameterAssert(key);
	@synchronized(self) {
		[_blocks removeObjectForKey:key];
	}
}

@end
