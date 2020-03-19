# photon-hue
A quick and dirty hack to get circadian lighting with Philips Hue

Previous attempts to use off-the-shelf software proved to have issues with scene detection resulting in lights desyncing from the circadian schedule spontaneously. As I don't use scenes at home, I wrote a dirty hack to forcefully set the colour temperature and Luminosity values regardless of if there is a scene or not to avoid any issues with the lights de-syncing.

**Note: I made this OSS in case it will be helpful to someone. Feature requests and bug reports will be rejected/closed unless you submit a PR to add/fix it yourself.**

## Installing

First we build and install it to the system (make sure you have cmake and a C++17 capable compiler installed):

```
$ git clone --recurse-submodules https://github.com/EmilyBjoerk/photon-hue.git
$ mkdir photon-hue/build
$ cd photon-hue/build
$ cmake ..
$ sudo make install
```

Then we need to configure it with an API key (username) from the hub and optionally specifiy the MAC address of the bridge if there are several bridges. Do this by running the application once, and note the MAC and user name outputted:

```
$ photon-hue
```

Now fill in the user name from the above (and optionally the MAC):

```
$ sudo -e /usr/local/bin/photon-hue-service
```

Finally, make `photon-hue` start automatically on boot:

```
$ sudo systemctl enable photon-hue
$ sudo systemctl start photon-hue
$ sudo systemctl status photon-hue
```

To view the logs you can use `journaltcl`:
```
$ sudo journalctl -fu photon-hue.service
```
