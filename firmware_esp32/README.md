# ESP32 Firmware
## Build
TODO: add build and flash info using esp-idf
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

### Hidlink Protocol
#### Frame pattern
``header`` ``command`` ``len`` ``data`` ``checksum``
* ``header``
  * Packet start marker
  * Length: 1 byte
  * Fixed ``0x3E`` for request
  * Fixed ``0x3C`` for reponse
* ``command``
  * Command code
  * Length: 1 byte
  * See available codes below
* ``len``
  * Length of ``data`` field in bytes
  * Length: 1 byte
* ``data``
  * Data field
  * Length: Value of ``len`` bytes
* ``checksum``
  * Two's complement of the 8 bit truncated sum of all previous fields
  * Suming all bytes of all fields to ``checksum`` value must result in ``0x00`` for a valid packet

#### Commands
##### 0x01 Get HIDLINK Status
* Request: ``0x3E`` ``0x01`` ``0x00`` ``0xC1``
* Response: ``0x3C`` ``0x01`` ``0x01`` ``status`` ``checksum``
  * ``status``
    * ``0x01`` IDLE
    * ``0x02`` SCANNING
    * ``0X03`` CONNECTED
##### 0x02 Start HID peripheral scan
* Request: ``0x3E`` ``0x02`` ``0x00`` ``0xC0``
* Response: ``0x3C`` ``0x02`` ``0x01`` ``ack`` ``checksum``
  * ``ack``
    * ``0x06`` SUCCESS
    * ``0x15`` FAIL
##### 0x03 Stop HID peripheral scan
* Request: ``0x3E`` ``0x03`` ``0x00`` ``0xBF``
* Response: ``0x3C`` ``0x03`` ``0x01`` ``ack`` ``checksum``
  * ``ack``
    * ``0x06`` SUCCESS
    * ``0x15`` FAIL
##### 0x04 Attach to HID peripheral
* Request: ``0x3E`` ``0x04`` ``0x01`` ``index`` ``checksum``
  * ``index``: Index of HID peripheral as in scan responses (starts in 1)
* Response: ``0x3C`` ``0x04`` ``0x01`` ``ack`` ``checksum``
  * ``ack``
    * ``0x06`` SUCCESS
    * ``0x15`` FAIL
##### 0x05 HID peripheral scan data
* No request, responses comes after command ``0x02`` whenever a HID peripheral is scanned
* Response: ``0x3C`` ``0x05`` ``len`` ``index`` ``address`` ``name`` ``checksum``
  * ``index``: Index of device of available HID peripherals to be used in command 0x04 (1 byte)
  * ``len``: Data field lend in bytes (1 byte)
  * ``address``: HID peripheral MAC address (6 bytes)
  * ``name``: HID peripheral name (``len`` - 6 bytes)
 
## Bluetooth Classic (BR/EDR)
The Bluetooth Classic interface is used to interact to HID peripherals. It is used first to scan deviced around. Once one device is choosen by the mobile app, ``hidlink`` will keep connected to it.
