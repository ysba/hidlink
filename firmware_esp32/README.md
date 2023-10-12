# ESP32 Firmware
## Bluetooth Low Energy (BLE)
The BLE interface is used in ``hidlink`` for configuration using the mobile app.
### Advertisement packet
``02 01 06 08 09 68 69 64 6c 69 6e 6b``
### Device name
``hidlink``
### GATT Server
#### CDTP - Custom Data Transfer Profile: 
  * Service UUID: ``59534241-ef18-4d1f-850e-b7a87878dfa0`` 
  * Data characteristic UUID: ``59534241-ef18-4d1f-850e-b7a87878dfa1``
    * Requests are performed by:
      * Writing to this characteristic
    * Responses are received by:
      * Reading to this characteristic or 
      * Receiving indications from this characteristic
      * Receiving notifications from this characteristic

### ``hidlink`` Protocol

#### Frame pattern


#### Commands


##### 0x01 Get Status

##### 0x02 Scan HID

##### 0x03 Attach to HID

##### 0x04 Detach from HID

## Bluetooth Classic (BR/EDR)
The Bluetooth Classic interface is used to interact to HID peripherals. It is used first to scan deviced around. Once one device is choosen by the mobile app, ``hidlink`` will keep connected to it.
