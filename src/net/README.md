To compile the jiffy, you should do the following commands.

```shell
sudo cp -r include/jiffy /usr/local/include/
sudo cp -r include/thrift /usr/local/include/
sudo cp lib/libjiffy_client.a /usr/local/lib/
sudo cp lib/libthrift.a /usr/local/lib/ 
sudo cp lib/libthriftnb.a /usr/local/lib/
```

and you should change the `/etc/ld.so.conf` to add `/usr/local/lib` in it.

then execute the `sudo ldconfig` to update.