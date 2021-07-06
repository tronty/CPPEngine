                                        Touch


Description
Windows version of the Linux tools "touch" to change times of files (full program = 5.5KB).

Usage
touch.exe [-hxamcDsvq] [-r ref_file|-t time|-d date_time] -- <file_1> [file_N]
  -h : Show this help
  -x : Modify creation time
  -a : Modify access time
  -m : Modify modification time
  -c : Do not create file if not found
  -r : Use a file or directory as a reference time source
  -D : If target doesn't exist, create a directory
  -s : Browse sub-directories
  -v : Output the result of every file processed
  -q : Silent mode, no error is displayed (disable '-v')
  -t : Use time [[CC]YY]MMDDhhmm[.SS] instead of today
  -d : Use date_time [[YYYY-]MM-DD][Thh:mm[:ss]] instead of today
  -- : Finish options, followed by the file list
The options '-r', '-d' and '-t' can not be used simultaneously.
By default, all times will be modified (creation, access and modification).

Examples
  touch.exe -sv -d 2011-04-23          -- ".\folder\*.txt"
  touch.exe -cv -d           T10:55:00 -- *.dat
  touch.exe -xv -d 2011-04-23T10:55    -- test2.txt test3.txt

Screen capture
    http://myc01.free.fr/touch/touch.png

Download
touch.exe v1.21 (5.5 KB)
    http://myc01.free.fr/touch/touch.exe

Sources :
touch.c (21.8 KB)
    http://myc01.free.fr/touch/touch.c
(View in HTML)
    http://myc01.free.fr/touch/touch.c.html

Changelog
1.21 - 2011-04-28
  * fix decode date/time
  * add touch folder with *.*
  * Combination of the options '-s' '-v' : '-sv'
1.20 - 2011-04-26
  * argument from 'GetCommandLine' / 'CommandLineToArgv'
  * change option '-d' to '-D'
  * add option '-d date_time'
1.10 - 2011-04-24
  * add options '-v' verbose, '-s' scan-subfolder, '-q' silent mode
1.00 - 2011-04-20
  * initial version with options '-hxamcrdt -- file_n ...'

License
    CC0 1.0 Universal (CC0 1.0)
    Public Domain Dedication
    http://creativecommons.org/publicdomain/zero/1.0/
    