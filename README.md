## spiloop

Simple SPI loopback test using spidev.

    Usage: spiloop [-s <speed>] [-d <device>] [-c <count>] [-i <iterations>] [-vh]
      -s <speed>      Set SPI bus speed in Hz
      -d <device>     Device, defaults to /dev/spidev1.0
      -b <bytes>      Number of bytes per transfer, default 32, min 1, max 4096
      -i <iterations> Number of times to repeat, default is 1, 0 means forever
      -v              Be verbose
      -h              Show this help

    Use Ctrl-C to stop.

