# Handmade Hero
Handmade Hero is a ongoing project where I will be following the handmade hero tutorials by Casey Muratori.<br>
<h2>Important notes:</h2>
-This program will NOT work on anything less then Windows XP!<br>
<h2>Notes / Bugs</h2>
-[BUG] Screen flickers when the resolution is not 1280 by 720.<br>
-[NOTE/BUG] On my crappy coding laptop, the sound is awful to listen to.<br>
<h2>Current Version:</h2>
0.0.2, build: 57.<br>
<h2>Current day finished:</h2>
Week 3, Day 13.<br>
<h2>Compiler Options</h2>
If HANDMADE_INTERNAL = 1, it is a dev build and will include dev / debug code. These might not be stable.<br>
If HANDMADE_INTERNAL = 0, it is a build for public release / shipping build. These _should_ be stable.<br>
If HANDMADE_SLOW = 1, the build is not optimized for performance and could perform worse than expected.<br>
If HANDMADE_SLOW = 0, the build _should_ not include slow / performance hindering code.<br>
Public release builds should always be compiled with HANDMADE_INTERNAL and HANDMADE_SLOW set to 0.<br>
<h2>Current Status</h2>
Status: Early Alpha.<br>
Main job: Finishing the prototype platform layer.<br>
Currently working on: Game Memory.<br>
Last completed job: Abstracting XInput to the platform non-specific code.