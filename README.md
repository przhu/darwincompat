darwincompat
============

My Darwin compat library for some optional POSIX functions

###System Requirements
* Darwin 11 (Mac OS X 10.7, others may work)

###Current State
* totally UNTESTED
* Currently some time related functions are implemented or just a wrapper. See file `time.h` for the detail.
* `clock_settime` is not implemented, just set `errno` and return.
* Implementation relies that on Intel architecture, mach absolute time unit is the same as nanosecond.
* `timer_*` functions are implemented using libdispatch. `SIGEV_NONE`, `SIGEV_THREAD` are supported but `SIGEV_SIGNAL` is not supported. Since libdispatch does not offer a utility to query the time remaining for a timer to fire, the implementation is rather incomplete.
* many more defects...

###LICENSE
Copyright (c) 2012, PrZhu(przhu@gmx.com)
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
