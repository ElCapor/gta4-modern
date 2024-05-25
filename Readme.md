# Reverse Engineering and improving GTA 4 :) IV Definitive Edition

# About
Building this project will result in a DLL file that can be injected into GTAIV using any Injector out there. After the DLL file has been injected, the ingame functions will be replaced by the reversed ones. The game will behave the same. Now if we make any changes to the reversed code, it will take effect in the game. The goal is to keep reversing until we have the entire game reversed to compile a standalone executable.

# Building
`xmake`

# Structure
gta4 -> all the dll code

# Note
im tired of writing sick readmes for stuff i dont finish :skull:

BTW the dll unloads itself when u tell it to

# TODO

[] - Make asserts and more checking for AOBs
[] - write a get near func