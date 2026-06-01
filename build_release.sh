echo "Building epicswatch"

gcc -ggdb -std=c99 -D_POSIX_C_SOURCE=200112L -Wall -Wpedantic -Wextra -I$EPICS_BASE/include -I$EPICS_BASE/include/os/Linux -I$EPICS_BASE/include/compiler/gcc -I./include -L$EPICS_BASE/lib/linux-x86_64 main.c src/* -o epicswatch -lca -lCom -lm

echo "Setting up .deb" 

mkdir -p epicswatchApp/usr/bin
cp epicswatch epicswatchApp/usr/bin

echo "Building .deb"

dpkg-deb --build ./epicswatchApp/
dpkg-sig --sign builder -k C7C9FDC720377FB2D2BDAE5EBF045B067189F0A8 epicswatchApp.deb
mv epicswatchApp.deb epicswatch_amd64.deb
