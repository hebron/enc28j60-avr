**enc28j60-avr** is a set of tools created to operate the Microchip **ENC28J60** (Stand-Alone Ethernet Controller with SPI Interface) using any SPI-capable **AVR microcontroller**. If your microcontroller doesn't feature a dedicated SPI module, you may even bit bang it. The libraries are intended to be compiled/archived on a Linux system using avr-gcc, avr-binutils and [AVR Libc](http://www.nongnu.org/avr-libc/). It will also work on Windows using [WinAVR](http://winavr.sourceforge.net/).

With **enc28j60-avr** libraries, including network capabilities in an embedded, microcontroller hardware project becomes **easy**. The ENC28J60 chip alone is a bit tricky to work with, but dozens of [prebuilt modules](http://www.ebay.ca/sch/i.html?&_nkw=enc28j60&_sacat=See-All-Categories) may be ordered from the Internet.

The project, a set of 4 libraries, is currently under active development (i.e. not finished!). Here is the dependency diagram of **enc28j60-avr** constituents (an arrow from A to B meaning "A depends on B"):

![http://0x3b.org/enc28j60-avr/img/enc28j60_avr_blocks.png](http://0x3b.org/enc28j60-avr/img/enc28j60_avr_blocks.png)

As you can see, **libenc28j60d** and **libbanepro** are independant: they only depend on AVR Libc, the C standard library for AVR C projets, which is not part of this project.

Here is a more complete description of each library.

## libenc28j60d: driver ##

This basic set of functions is at the core of ENC28J60 control. It implements the communication layer between an AVR microcontroller and the actual ENC28J60 chip. The following commands, as described in the [datasheet](http://ww1.microchip.com/downloads/en/devicedoc/39662c.pdf) (section 4.2), are included in **libenc28j60d**:

  * Read Control Register
  * Read Buffer Memory
  * Write Control Register
  * Write Buffer Memory
  * Bit Field Set
  * Bit Field Clear
  * System Reset

All you need to do is provide the library with SPI read and write functions, since not all AVR microcontrollers interface their SPI module the same way.

## libenc28j60fw: framework ##

**libenc28j60fw** is used to perform higher level functions with the chip such as:

  * read/write from/to an 8-bit/16-bit control register
  * read/write from/to a PHY register
  * bitfield set/clear on a control register
  * set device's MAC address
  * set RX/TX buffers sizes
  * check for new packet or if transmission is done
  * perform an easy initialization
  * handle reception/transmission prologues/epilogues
  * configure interrupts

The library needs and uses **libenc28j60d** for underlying low-level operations.

## libbanepro: basic network protocols ##

**libbanepro** consists mostly of analysis and formatting functions. It currently partially supports the following network protocols:

  * Ethernet II and 802.3: known EtherTypes are IPv4, ICMP and ARP
  * IPv4: known types are TCP and UDP
  * ICMP: known types are Echo Reply and Echo Request
  * ARP: reply and request are supported
  * UDP

This library does not need **libenc28j60d** or **libenc28j60fw**. It may thus be used in any other AVR project using other network components. Its only purpose is to analyse a buffer and to store information about it in structures. The structures can also be filled by user and formatted to a buffer.

Internet checksums (mostly for ICMP and UDP) can be computed by the library.

**libbanepro** does not support TCP right now.

## libipkats: IP stack ##

This IP stack depends on the three other libraries because it acts as a bridge between them. It handles lots of actions automatically (ARP reply/request/table, ICMP Echo (ping) Reply, etc.). Give it an UDP callback, call its main function when a new packet interrupt rises and you're pretty done!

That's right — we said **easy**.

More information coming soon!