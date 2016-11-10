Quick & dirty http shell mockup in Perl.
========================================

relay server
------------
hss.pl

Simple http server for save & load file using specific url.
Use standard http get & post methods.

client behind wall
------------
hs.pl

A daemon process for check relay server regularly.
Data is encoded in ase -> base64.
Lines start with "s:" will be settings.
Currently "s:interval" is the only setting, which adjust the interval between check server.

client from user
------------
hsc.pl

Using "hsc.pl cmd" to upload command to relay server.
"hsc.pl" to fetch result.
