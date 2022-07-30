# Description

A C guitar "tuner", but actually more of a monophonic guitar string note/frequency
tracker that can be used for tuning. It uses the default microphone for audio input. 
Customisable are parameters such as sample rate and chunk size: read the entry point code
and main initialisation for these parameters.


# Dependent Libraries

* [fftw3](http://fftw.org) for fast Fourier transform (FFT)
* [portaudio](http://portaudio.com) for audio input


# Algorithm

Samples are processed a chunk of samples at a time, where a chunk of input samples gives a
frequency (Hz) output, which is then converted to a musical note. The following lists the
steps involved in getting this frequency reading from digital audio signal samples read from
an input device.

<s>1. Normalise samples into range -1 to 1 (both ends inclusive). The max sample 
of the samples chunk gets normalised to 1, and the min to -1.</s> (See code and comments in it about 
skipping this step.)
2. Preprocess the normalised samples by running a Hanning window on it.
3. Run a fast Fourier transform on the normalised samples to convert them from time domain
to frequency domain. The input to FFT are the normalised samples and the output are complex numbers.
4. Calculate the magnitude for each complex number output.
5. Run a harmonic product spectrum using the magnitudes in attempt to find the fundamental frequency
of the note and not be tricked by harmonics. This is done since when a guitar string is played it
produces multiple frequencies, a fundamental frequency, the frequency which can be used to get the
played note, and harmonic frequencies, multiples of the fundamental frequency (it's why you see
the A2 note getting picked up and tricked as an A3 near the end of the demo video).
6. Use the index of the max value output of the harmonic product spectrum as the frequency of 
the chunk of samples.


# Demo

Serenade demonstration of the guitar tuner in use.

https://www.youtube.com/watch?v=3CyOm8BIrVE
