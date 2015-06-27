On a freshly installed debian GNU/Linux Jessie:

```
apt-get update
apt-get install build-essential automake autoconf gnu-standards libtool gettext autoconf-archive check git libpq-dev
git clone https://github.com/lisael/PGHacks.git
cd PGHacks
autoreconf --install
./configure
make
make check
make distcheck
cd ..
mv PGHacks/pghx-0.1.tar.gz .
tar -xvzf pghx-0.1.tar.gz 
cd pghx-0.1
./configure
make 
make check
make install
```

