../newlib/configure --host=x86_64-andraos --target=x86_64-andraos --prefix=/opt/andraos
make clean
make
readelf -h libc/sys/andraos/crt0.o