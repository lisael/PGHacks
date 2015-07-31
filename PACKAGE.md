Debian GNU/Linux
================

On a freshly installed Debian GNU/Linux Jessie:

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
```

You should get an installable tarball (see `INSTALL.md`)
