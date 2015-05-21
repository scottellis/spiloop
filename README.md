## spiloop

Simple SPI loopback test using spidev.

    Usage: spiloop [-s <speed>] [-d <device>] [-c <count>] [-i <iterations>] [-vh]
      -s <speed>      Set SPI bus speed in Hz
      -d <device>     Device, defaults to /dev/spidev1.0
      -c <count>      Number of bytes to transfer, default is 32, max is 4096
      -i <iterations> Number of times to repeat, default is 1, 0 means forever
      -v              Be verbose
      -h              Show this help

    Use Ctrl-C to stop.

