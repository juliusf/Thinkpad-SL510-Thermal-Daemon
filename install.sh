gcc sl510thermal.c -o sl510thermal
cp sl510thermal /usr/sbin
cp sl510thermal-daemon /etc/rc.d/
/etc/rc.d/sl510thermal-daemon start
