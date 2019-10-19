# UWPOpenGLESApp
UWP OpenGL ES (ANGLE)

This is an Universal Windows Platform Project of Visual Studio 2019 that supports OpenGL ES (2/3/3.x) powered by ANGLE. At the moment, ANGLE template is only available on Visual Studio 15.

# <18/10/2019> Notes on EGL OpenGL ES 
To download opengl es, download the Emulator from here: https://developer.arm.com/tools-and-software/graphics-and-gaming/opengl-es-emulator
Bug: Shader Compiler not specified -> Missing openglessl/*.dll file, copy entire openglessl folder and paste it into Debug folder

# <19/10/2019> It's still SDL2 that makes everything easier. I'm all the way back to SDL2.
	
	UWP can run OpenGLES but how can I load files to the platform? Maybe I can, but
	then I would have to write the code for loading data from UWP seperately.
	
	For Win32 project without using SDL2, things work fine with the window supported by
	Win32. but no File loading mechanism is provided when shifting to Android. where ifstream
	may not work. Same problem for UWP, I have to write the code for loading data particularly
	for win32 and eventually i would have to implement stuff again in SDL2 for android.
	
	Moreover, today I figured out that, SDL2 can run OpenGLES on Windows too, which I had given
	upon last year. And come to the conclusion that SDL2 does not support Emulator for OpenGLES. Such
	a wrong thought. 

	->Conclusion: I will use SDL2 for Win32 and Android. they both have the same main() function.
	I can simply bring the current Win32 project into android project and it works.
	For UWP, In the future, If I want it. I would have to find a way to integrate SDL2 into UWP.
	or else, I would have to write my own file loading functions and many more other stuffs
	that are provided straight forward by SDL2: Sound, network, true font type,etc.
	-> SDL2 is the best!!!!
	

