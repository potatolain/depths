# Frequently Asked Questions

This section tries to address common questions and answers for new developers.

If you have a question that isn't addressed here, feel free to oepn a Github issue, or reach out on
Twitter/somewhere else.

TODO: Link chapter references before, once chapters are a little more solidified.

#### Can I play my game on a real NES?

Absolutely! This engine produces roms that are completely compatible with the NES - they are not limited
to just emulators. It's actually very easy with the right hardware.

There's a whole chapter about this in section 4! 

#### Can I use this for Game Jams/Coding competitions?

As far as I'm concerned, definitely! I plan to use this for a Ludum Dare at some point myself to see
how it fares.

If the contest allows "base code", or "engine code" or something similar, I believe this qualifies.

This does include some basic music and sprites and levels - if you are required to create
all of your content as part of the Jam, you should be sure to remove this content before the end
of the contest.

Here is a list of all non-engine content to be aware of: 

1. Music and sound effects in the famitracker files in the `sound/` directory.
2. The Tiled maps in the `levels/` directory.
3. Nesst graphics in the `graphics/` directory

#### My game doesn't have the change that I expected. What do I do?

This shouldn't really ever happen, but well, sometimes it does! This may be due to a mistake in the engine, or
due to new code that has been added that doesn't line up with what the `makefile` expects. There are a few things
we can try...

##### 1. The "dangit" fix: Make sure all of your files are saved

Hey, we all forget to hit `ctrl+s` sometimes!

##### 2. Clean up generated files, and rebuild from scratch

This is the equivalent to turning it off and on again. In your cygwin terminal in the base of `nes-starter-kit`,
run `make clean`, then build it again. This will get rid of all temporary files on your system, forcing us to
re-generate all files. If you find yourself doing this often, you may want to open a support ticket with
`nes-starter-kit` on GitHub so we can fix this in the project itself.

##### 3. Change something else, and see if that works

When all else fails, this can help find problems, or just help verify that the system is working. Make another change
that should break the build (a syntax error for example) and prove that breaks it. If it doesn't, the file you are
editing may not be included in the build, or an older version might be included in the build.

Next, try removing this error, and make a change to code that will be extremely obvious if it happens. One option is
to add an infinite loop, such as `while(1) {}` to the area in question. If the game crashes, you got to that point. 
If this works, you know where you are in code execution. If not, try to move this thing earlier in execution until it
behaves as expected.

Another option is to comment out part of the code that does something obvious, and see if that thing keeps happening.

This part is always painful, but just try to keep probing at things until you learn something about the problem. You
also might want to try reaching out to someone and talking through the problem (even if they're not familiar with
`nes-starter-kit`) to see if it helps you come to a solution. I hope this helps, and don't be afraid to reach out for
help!

#### What is all the `CODE_BANK` / `banked_call()` stuff about?

The NES has very limited space for code, and for graphics/etc. It does not know how to use more than 32 
kilobytes of this data at once. This may not make much sense, since many games are 512kb or 
larger! Our game actually uses 128kb of space by default. How is this possible?

The short answer is, we separate the code into smaller 16k sections, and switch out which ones the
NES knows about at once. (This is known as ROM banking) The `CODE_BANK(number)` syntax tells the compiler
which of these 16k sections the code belongs in. The `banked_call` method runs code that is in one of
these sections.

This is explained in greater detail in the 4th section, in a chapter titled "Understanding and adjusting 
the size of your game."

#### What does `Warning: Memory area overflow` mean?

In short, it means you tried to fit too much data/code into the area given. Check out the chapter titled
"Understanding and adjusting the size of your game" in section 4. 
