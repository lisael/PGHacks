From git source
===============

Please follow `/PACKAGE.md` instructions

From source tarball (recommended)
=================================

Pick latest release X.Y from https://github.com/lisael/PGHacks/releases

then run :

```
tar -xvzf pghx-X.Y.tar.gz 
cd pghx-X.Y
./configure
make 
make check
sudo make install
```

This installs the libs in `/usr/local/lib` and headers in `/usr/local/include`.
You may add `--prefix=/somewhere/else` to `./configure` command (BTW `/usr/local`
is fine as it doesn't break your distro and is usually searched by build tools)
