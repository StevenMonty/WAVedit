# WAVedit
#### A simple .WAV sound file editor


This program is capable of performing basic manipulaitons to a .WAV sound file. It works by first confirming that the specified file 
is a genuine .WAV file by reading the file into memory and using the WAVheader struct to confirm that the header information is 
accurate to real .WAV file. This ensures that another file type such as a jpg or pdf with a .wav in the filename wont pass through the 
`confirmFileType` method. If the file is a genuine wav file, then the following operations are available: Displaying the file information, 
reverseing the audio, and changing the sample rate. These are executed through command line arguments. 

```
Usage: 
./wavedit [FILE.wav]             : Display the files properties
./wavedit [FILE.wav] -reverse    : Reverse the audio
./wavedit [FILE.wav] -rate [INT] : Change the sample rate
```
