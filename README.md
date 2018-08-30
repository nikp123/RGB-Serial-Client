## What's this?

This is a project that tries it's best to make lighting your room easier and fun.

This is the client part of it. (aka. you need to setup the [Arduino part](https://github.com/nikp123/RGB-Serial-Arduino) first)


## What will I need for this?

 - A computer (running Linux, macOS or Windows) with bluetooth capabilities
 - A micro USB cable (to be able to communicate to the thing or power it in case of bluetooth)
 - For compilation: cmake, pkg-config, gtk+3 development libraries and a C compiler

## Getting started

### Windows

For Windows users, they can just download the release from [here](https://github.com/nikp123/RGB-Serial-Client/releases).

Plug the device in. Or in case of bluetooth connect to it.

Extract the 7z file and open RGBserial.exe.

Find out the "Outgoing" COM port of the device.

Select it from the dropdown list and hit connect.

Now you can control the device from here on.


### Others (UNIX-like OS-es)

You would need to compile it. So first install the dependencies.

Connect to the device (using USB or bluetooth).

Then open up a shell/terminal. And proceed like this:
```
$ cd $this_repository
$ mkdir build
$ cd build
$ cmake ..
$ make -j$(nproc)
$ ./RGBSerial (to open it)
```

Once you did that, you must find out what serial device does the arduino belong to.

It's usually ``tty.DEVICE\_NAME`` on macOS.
Or on Linux bluetooth it's usually ``rfcommX``.
Or on Linux but USB it's usually ``ttyUSBX``.

Select it from the dropdown list and connect to it.

Now you can control it.


## Contribute

Contributions of code and ideas are very welcome.

Just make sure that you follow the code style if you are contributing code.


### Code style

 - Use tabs.
 - Don't use some overbloated library.
 - Don't remove stuff unless necesarry.
 - Don't reinvent the wheel.
 - Don't reimagine the format.
 - Don't do something stupid.


## License

Refer to the [LICENSE file](/LICENSE) in this repository.

