# RP2040 Firmware
## Build
### Linux
* Install system packages
  ```sh
  sudo pacman -S cmake arm-none-eabi-gcc arm-none-eabi-binutils arm-none-eabi-newlib arm-none-eabi-gdb
  ```
* Get ``pico-sdk`` repository
  ```sh
  cd ~
  git clone https://github.com/raspberrypi/pico-sdk
  cd pico-sdk
  git checkout 1.5.1
  git submodule update --init --recursive
  ```
* Configure ``cmake``
  ```sh
  cd [path_where_hidlink_repo_is]/hidlink/firmware_rp2040
  mkdir build
  cd build
  cmake -DPICO_SDK_PATH=/home/[user_name]/pico-sdk -B build -GNinja
  ```
* Build
  ```sh
  ninja -C build
  ```
  The generated files will be available at the ``build`` dir. The file with ``uf2`` can be loaded directly into the USB drive of the Raspberry Pi Pico board.
