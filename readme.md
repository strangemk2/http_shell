# HTTP SHELL
A remote shell using http protocol.

### Structure
    +------------+           +------------+           +------------+
    |            |  POST     |            |   GET     |            |
    |   Remote   | =======>  |   Http     | =======>  |   Local    |
    |   server   |  GET      |   file     |   POST    |   client   |
    |            | <=======  |   server   | <=======  |            |
    |            |           |            |           |            |
    +------------+           +------------+           +------------+
* Remote server: Server which we want to execute command on it.
* File server: Server which relay data using restful interface.
* Local client: Send actual command to file server.

Data send to file server will be compressed in gzip, encrypted with aes-128 cbc and encoded to base64.

### Inside
File server only identify GET and POST http request, a sample implementation is in *perl_mock/hss.pl*

Remote server is written in c++ with some c libraries. To make sure binary is small enough and platform independant.

A seperate script *hs_setting1.py* is used for manipulator magic tail in *hs* binary. To change settings without recompile.

Local client is also written in Perl as a sample implementation, *perl_mock/hsc.pl*.

### Build
```
git clone https://github.com/strangemk2/http_shell.git
git submodule update --init
make
```
