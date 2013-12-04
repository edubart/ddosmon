### DDOS Monitor

Program that uses low level linux packet sniffing in incoming network traffic
for monitoring possible network attacks and reacting to them by alerting and
triggering user defined self defence mechanisms.

With a ncurses interface you can monitor network traffic live and watch
recent events. Logs are saved to log folder, any ddos attack detection send
an email to the user.

It can classify following attacks:
  * SYN Flood
  * UDP Flood
  * ICMP Flood

Any other attack with massive amount of traffic or packet would still be detected.

### Building

```
git clone git@github.com:edubart/ddosmon ddosmon
cd ddosmon
mkdir build && cd build
cmake ..
make
```

### Running

```
# optional, I usually run this inside a screen session
screen 

sudo ./build/ddosmon configs/example.lua
```

**NOTE:** Root is needed for sniffing the network adapter packets.


### Scripts

Script called when a known DDOS attack starts or stops:
`./scripts/networkcompromise <compromised/uncomprimised>`

Script called to notificate admins (usually via email):
`./scripts/notificate <subject> <message>`

Script called when one of your servers ip address might be unreachable and you may want to block/unblock it from your main server:
`./scripts/ipblock <block/unblock> <ip>`

### Configurations

You can find and edit these configuration for you needs inside `configs/home.lua`

* interface = "eth0"
* global_traffic_threshold = 900000
* global_packets_threshold = 225000
* ip_traffic_threshold = 500000
* ip_packets_threshold = 125000
* notification_traffic_threshold = 20000
* notification_packets_threshold = 20000
* ipblock_retry_ticks = 5*3600*1000
* notification_command = "./scripts/notificate \"%1%\" \"%2%\" &"
* onblockip_command = "./scripts/ipblock block %1% &"
* onunblockip_command = "./scripts/ipblock unblock %1% &"
* network_uncompromise_ticks = 30
* onnetwork_compromise_command = "./scripts/networkcompromise compromised &"
* onnetwork_uncompromise_command = "./scripts/networkcompromise uncompromised &"
* log="logs/home.log"
* watchedips="configs/example_watchedips.xml"
* notificationsubject="DDOS Monitor on server1 notification"

### Watched IPs

**NOTE:** Don't foger to configure the ips you want to monitor in the `example_watchedips.xml` file.

This program was intended to monitor multiple ip addresses, so you can configure as many you like.
