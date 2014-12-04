Vision3
=======
Compile:
g++ *.cpp $(pkg-config opencv --cflags --libs)
If compile fails, probably due to hardcoded location of opencv headers
=======
Running:
./a.out REGION RESOLUTION SHOW_DERIVS
