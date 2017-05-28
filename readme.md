
_Note: Be sure to check out the online version at [ferocious.juddn.com](http://ferocious.juddn.com) ... It also supports [Additional Formats](http://52.62.255.103:1337/fileformats) !!_ 

## Synopsis


Ferocious File Converter is a high-quality audio file-format conversion tool, which utilitizes the resampling capabilities of its companion program, [Resampler](https://github.com/jniemann66/ReSampler "Resampler").

It provides a convenient and easy-to-use graphical interface for converting audio files between a number of formats.

![Ferocious File Converter screenshot](https://github.com/jniemann66/ferocious/blob/master/screenshot.jpg)


## Description of Code

Ferocious File Converter was originally developed using Qt 5.5 on Windows, but also runs just fine on Linux. It has been compiled on Qt versions up to 5.8 and gcc versions up to 5.4.0.
Since Qt is cross-platform, it should be possible to run ferocious on any platform targeted by Qt.

## Motivation

Having written a command-line sample rate converter, I thought the next logical step was to write a user-friendly GUI for it, with a very close coupling between the features of the GUI and commandline.

## Installation

The binaries are included in two zip files in this repository, one for 32-bit and the other for 64-bit. Each of the zip files contains a recent build of resampler.exe for convenience. Download the zip file of your choice (note: the only difference is the version of resampler.exe and associated dlls; one is 32-bit, the other is 64-bit), and unpack the entire folder structure to somewhere on your PC. Then, simply run ferocious.exe. 

All of the relevant dlls and other dependencies are included in the distribution. (*If you have any problems with a particular dependency not being included, then I would like to know about it.)* 

## Usage

##### Minimum effort: #####

- Run the program. 
- Choose an input file (or files)
- Hit "Convert" button 

converter will automatically create an output filename, based on input filename, in the same path as input file.

##### Typical usage: #####
- Run the Program
- Choose an input file (or files)
- Choose (or type the name of) an output file
- (optional) Select output bit-format (sub format) from drop-down list
- (optional) Select "Normalize" and choose a normalization amount between 0.0 and 1.0 (with 1.0 representing maximum possible volume)
- (optional) Select "Double Precision" to force calculations to be done using 64-bit double precision arithmetic

converter will automatically infer the output file format (and subformat) based on the file extension you choose for the output file. The converter's output messages are always displayed in the "Converter Output" box.

Note: when you choose a new output file type, ferocious will run resampler.exe with the --listformats <extension> command to retrieve a list of valid sub-formats and automatically populate the bit format dropdown box with the valid sub-formats for the chosen file type.

##### Selection of multiple files: #####
When opening files for conversion, multiple files may be selected (by holding down 'ctrl' while clicking on filenames in the open files dialog, or entering ctrl+A to select all files).

In multi-file mode, the output filenames are automatically generated according to the rules you set in "Output File Options"

Wildcards (using '*') in filenames (but not directory paths) are also recognized. For details on wildcard processing rules, please refer to [this document](https://github.com/jniemann66/ferocious/blob/master/Ferocious-Wildcard-specs.pdf "this document") 


## Menu Items

Note: all items configured in the Options menu are *persistent* (ie they will be remembered next time you run the program) unless otherwise indicated.

**Options/Converter Location ...**   Use this to specify the location of the **resampler.exe** converter

**Options/Output File Options ...** This allows you to control the settings that govern the generation of automatically-generated output file names:

**Append this suffix To Output File name:**

When enabled, this will add the text you specify to the automatically-generated output filename. For example, you could add something like "Converted-to-44k" to distinguish the output file from the original.

**Use this output directory:**

When enabled, the automatically-generated output filename will use the directory path you specify.

**Default to same file type as Input File**

Don't change the file type

**Default to this File Type:**

Make the format of the automatically-generated output file be the format you specify.

**Options/Compression Levels/flac ...**

Allows you to set the *compression level* to be used when saving files in the flac format. The level setting is an integer between 0 and 8 inclusive, and corresponds to the [compression levels](https://xiph.org/flac/documentation_tools_flac.html#encoding_options "compression levels") of the official [flac command-line tool ](https://xiph.org/flac/index.html "FLAC command-line tool") from [xiph.org](https://xiph.org/ "xiph.org")

**Options/Compression Levels/Ogg Vorbis ...**

Allows you to set the *quality level* to be used when saving files in the ogg vorbis format. The quality level corresponds to the quality level used for the official ogg vorbis command-line tool, and ranges from -1.0 to 10.0. Non-integer values are allowed.

**Options/Lowpass filter** allows you to select the filter characteristics for the anti-aliasing filter.
- **Relaxed:** wide transition band, late (higher) transition frequency - has good frequency response, low ringing, but may allow a small amount of aliasing.
- **Standard:** wide transition band, earlier (lower) transition frequency - no aliasing, low ringing, but frequency response suffers a little (drops a few dB around 20k for 44.1k sample rate).
- **Steep:** narrow transition band, late (higher) transition frequency - no aliasing, good frequency response, but potentially more ringing.

*note: for downsampling to 44.1k, Steep or Relaxed is recommended (although standard is also fine).
For lower rates (eg 22.05), use steep. For higher target rates, use standard.*

**Options/Advanced Dither Options**
- **Noise Shaping:** Select Standard or Flat TPDF.
- **RNG Seed:** allows you to set a fixed seed for the Random Number Generator used for dithering.

**Options/Enable clipping protection**

When clipping protection is enabled, the converter will repeat the conversion process with an adjusted (ie decreased) gain level whenever it detects clipping in the initial conversion. This will ensure that there will be no clipping on the second pass. This will add to the total conversion time, but it is nevertheless strongly recommended. With clipping protection switched off, the converter will still warn you when clipping has occured, but will not attempt to fix it. 

Note that in the conversion process, all signal levels are represented internally as floating point numbers within the range +/- 1.0 (regardless of the file formats involved). Whenever the signal peak exceeds +/- 1.0 it is considered clipping. If clipping occurs, the peak level is remembered, and the gain for the second pass is adjusted down by an amount corresponding to how far the signal peak exceeded +/- 1.0 by.

If normalization is activated, then clipping protection will ensure that the signal peak always aquals the normalization factor (instead of the default +/- 1.0)

Deactivating clipping protection actually sends the --noClippingProtection switch to the resampler.exe converter (the default bahaviour in resampler.exe is to have clipping protection on).

The main cause of potential clipping during sample rate conversion is overshoot effects from the FIR filter when a sharp transient is present in the input signal. (Unfortunately, this is an inevitable consequence of using digital filters, and although the effect can be reduced somewhat through good filter design, it can never be completely eliminated)    

**Options/Enable Multi-Threading** uses the **--mt** option in ReSampler to enable multi-threading. This usually results in a significant speed improvement on multi-CPU systems. !

**Options/Enable tooltips** - Allows you to switch off the tooltips after you have become familiar with the controls, or switch them back on again as desired.

**Options/Theme** - Allows a *theme* to be selected, which affects the appearance of the user interface. Themes are stored in .qss files, which are based on css, but use some non-standard properties which are specific to Qt. 
Themes are potentially highly customisable. Some sample themes are included in the distribution.

**Help/about ...**

Display the version of Ferocious and the version of resampler.

**Help/about Qt ...**

Display information about Qt, the Toolkit used for developing Ferocious.

## Explanation of controls and options

**Bit Format:** specify the bit depth / output subformat of the output file. The items available in the dropdown will vary dynamically, depending on the output file type you specify

**Normalize:** set the maximum peak level of the output file to be what you specify (1.0 is maximum)

**Dither:** Activates dithering. Specify amount of bits of dither to add.

**Autoblank** (only active when dithering is activated). Causes dither to be switched-off whenever digital silence is encountered in the source material.

**Converter Type: Linear Phase, or Minimum Phase**

A whole book could be written about this, but to keep it short:

Linear phase:

- Symmetrical impulse response
- No phase distortion
- Higher Latency (delay)
- Creates Pre- and Post- ringing

Minimum phase: 

- Asymmetrical impulse response
- Introduces phase distortion
- Minimal latency (delay)
- Creates Post-ringing only

The "standard" (ie more common) type of Finite Impulse Response filter used in applications such as this is the Linear Phase filter. If you are in doubt about which one to use, then it is advisable to stay with linear phase.

Here are some actual examples of impulse responses, showing the difference between Linear Phase and Minimum Phase. These are the the impulse responses for resampling from 96kHz down to 44.1kHz:

![Linear Phase vs Minimum Phase](https://github.com/jniemann66/ferocious/blob/master/LinearPhaseVsMinPhase.JPG)

**Double-Precision** Causes all processing calculations to be performed in 64-bit double-precision. (By default, 32-bit single precision is used). Please note that double-precision may decrease processing speed slightly.




