# unet

Networking for fun.

### Requirements

New gcc (c++17).

### Instructions

```bash
# make
# ./scripts/set_up_dev_tun.sh
# ./bin/unet
...
# ping -i tap0 10.0.0.4
^C
# arp -i tap0
Address                  HWtype  HWaddress           Flags Mask            Iface
10.0.0.4                         (incomplete)                              tap0
```
