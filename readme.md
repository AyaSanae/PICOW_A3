## PICOW_A3 -- An embedded desktop decoration

![PICOW_A3](https://s21.ax1x.com/2024/10/16/pANoQLn.jpg)

The PICOW_A3 drives a 1.3-inch OLED display for content display.

For the driving instructions, please refer:[SH1106](https://github.com/AyaSanae/PICO_SH1106)

You can use the[PICOW_A3_DATA_SENDER](https://github.com/AyaSanae/PICOW_A3_DATA_SENDER.git)to display resource usage information such as the current CPU frequency,temperature,GPU frequency,temperature, etc.

For more details, please refer to:[resc_serialize](https://github.com/AyaSanae/PICOW_A3/blob/master/include/resource.h#L80)

The PICOW_A3_DATA_SENDER sends data every 2 seconds. 

Once PICOW receives the data, it updates the displayed information on the screen, refreshing a fixed 100 frames each time. For details on how to achieve smooth transitions in displayed data, see:[DynamicRendering](https://github.com/AyaSanae/PICOW_A3/blob/master/ui/ui.c#L187)


If you want to send data to PICOW via Bluetooth yourself, please refer to the following sections:
[DATA_TYPE](https://github.com/AyaSanae/PICOW_A3/blob/master/bt/spp.h#L6),
[DATA_TYPE_ADD_QUEUE](https://github.com/AyaSanae/PICOW_A3/blob/master/bt/spp.c#L146),
[PACK_DECODE](https://github.com/AyaSanae/PICOW_A3/blob/master/PICOW_A3.c#L63).

You can determine the type of data being sent by recognizing the first byte of the RFCOMM data packet. For example, if packet[0] == 0x00, it will be identified as RESOURCE_MASG_PAC. Please refer to the aforementioned [DATA_TYPE]. The data will be placed in a thread-safe queue since the SPP service runs on CORE1.

You should edit PICO_SDK_PATH and CMAKE_SYSROOT.

### QuickStart
```
git clone https://github.com/AyaSanae/PICOW_A3.git

mkdir build && cd build

cmake ..

make

```
After compiling and obtaining the PICOW_A3.uf2 file, hold down the BOOTSEL button on the Raspberry Pi Pico W while connecting it to your computer. Then, copy the PICOW_A3.uf2 file into the Raspberry Pi Pico W.
