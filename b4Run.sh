for whatever in `ipcs -s | awk '{print $2}'`; do ipcrm -s $whatever; done;
for whatever in `ipcs -q | awk '{print $2}'`; do ipcrm -q $whatever; done;
rm -f *.o *.bak MessageQueueApp
make
./MessageQueueApp.exe server