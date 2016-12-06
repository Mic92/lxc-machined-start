# lxc-machined-start

```
$ machinectl status
MACHINE CLASS     SERVICE
adminer container lxc

1 machines listed.
$ systemctl -M adminer status php-fpm.socket
● php-fpm.socket
   Loaded: loaded (/etc/systemd/system/php-fpm.socket; enabled; vendor preset: disabled)
   Active: active (listening) since Tue 2016-12-06 12:47:25 UTC; 18min ago
   Listen: [::]:9000 (Stream)
```
