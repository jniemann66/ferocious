## Synopsis

Ferocious File Converter is essentially a front-end for the command-line audio sample rate converter [Resampler](https://github.com/jniemann66/ReSampler "Resampler").

It provides a convenient and easy-to-use graphical interface for converting audio files between a number of formats.

![Ferocious File Converter screenshot](https://github.com/jniemann66/ferocious/blob/master/screenshot.jpg)


## Description of Code

Ferocious File Converter was developed using Qt 5.5 on Windows, with the 32-bit minGW compiler version 4.9.2.
Since Qt is cross-platfrom, and resampler.exe is command-line only, it would be quite feasible to port this project to other OSes (hint: fork my project :-) ) 

## Motivation

Having written a command-line sample rate converter, I thought the next logical step was to write a user-friendly GUI for it, with a very close coupling between the features of the GUI and commandline.

## Installation

The binaries are included in two zip files in this repository, one for 32-bit and the other for 64-bit. Each of the zip files contains a recent build of resampler.exe for convenience. Download the zip file of your choice (note: the only difference is the version of resampler.exe and associated dlls; one is 32-bit, the other is 64-bit), and unpack the entire folder structure to somewhere on your PC. Then, simply run ferocious.exe. 

All of the relevant dlls and other dependencies are included in the distribution. (*If you have any problems with a particular dependency not being included, then I would like to know about it.)* 

## Usage

##### Minimum effort: #####

- Run the program. 
- Choose an input file
- Hit "Convert" button 

converter will automatically create an output filename, based on input filename, in the same path as input file.

##### Typical usage: #####
- Run the Program
- Choose an input file
- Choose (or type the name of) an output file
- (optional) Select output bit-format (sub format) from drop-down list
- (optional) Select "Normalize" and choose a normalization amount between 0.0 and 1.0 (with 1.0 representing maximum possible volume)
- (optional) Select "Double Precision" to force calculations to be done using 64-bit double precision arithmetic

converter will automatically infer the output file format (and subformat) based on the file extension you choose for the output file. The converter's output messages are always displayed in the "Converter Output" box.

Note: when you choose a new output file type, ferocious will run resampler.exe with the --listformats <extension> command to retrieve a list of valid sub-formats and automatically populate the bit format dropdown box with the valid sub-formats for the chosen file type. 