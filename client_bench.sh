sudo sysctl -w net.inet.ip.portrange.hifirst=1024
sudo sysctl -w net.inet.ip.portrange.hilast=65535
sudo sysctl -w net.inet.tcp.msl=3000

ulimit -n 65536

wrk -t2 -c16k -d10s -H "Connection: keep-alive" http://localhost:8080/hello