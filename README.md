# Setup
Git clone this, git clone Google's robotstxt, install Abseil, and run this command:<br>
`g++ Helpers.cpp Main.cpp robotstxt/robots.cc 
  -I robotstxt/ 
  -labsl_strings -labsl_strings_internal -labsl_throw_delegate -lcurl -o tritone`<br>
It should work.
## Copyright
(C) Tritone Communications. All rights reserved.
