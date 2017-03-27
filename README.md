# Marvellous Anita Graphical Interface & Classy Display (Magic Display)
May 2008 -- Ryan Nichol <r.nichol@ucl.ac.uk>

## Introduction

Okay so it's a somewhat stupid name, but what can you do I only have some many hours in the day to come up with names. 

Q. What is it?

A. It is an event display and plotting program for use with ANITA-III/IV data in ROOT format.

Q. Why should I use it?

A. That's a very good question, maybe you shouldn't. I find it is a useful way to look at the rootified data. I know there a people who don't like the rootified data and that's fine an updated AnitaViewer will do everything this does and a whole lot more.

Q. So what's the point?

A. My preferred way to look at the data is using the ROOT trees rather than the binary files. I find it easier to implement calibrations and unpacking in one place rather than everyone having to do it themselves. This provides people, well right now me, with a (hopefully) really simple way to look at the ROOT data files.

Q. How do I get it?

A. Read on...

## Prerequisites and installation

There are, unfortunately, several pre requisites for installing and running Magic Display.

i) ROOT -- If you don't have it installed get the latest version from http://root.cern.ch

ii) FFTW3 -- The Fastest Fourier Transform in the west. If you don't have libfftw3.so on your machine then you need to install it. Most linux distributions have a package ready to install, otherwise head to http://www.fftw.org/

iii) Pretty much all the ANITA libraries (eventReaderRoot, libRootFftwWrapper, AnitaAnalysisFramework, AnitaAnalysisTools, UCorrelator...). The recommended way to install MagicDisplay is by using the anitaBuildTool
```bash
git clone https://github.com/anitaNeutrino/anitaBuildTool.git
```

## Running

i) Get some ANITA III/IV data. UCL has a web facing copy of the ANITA-3 data here: http://www.hep.ucl.ac.uk/uhen/anita/private/anita3/flight1415/root/
(The usual ANITA username and password are required)

ii) Navigate to the directory where anitaMagicDisplay is installed and edit the runMagicDisplay.C macro to point to where you put the ANITA data

iii) Then, to look at run 352, do
```bash
root macros/runMagicDisplay.C\(352\)
```

## Keyboard shortcuts

| Key          | (Key 2)      | Command                                                                                      |
| ------------ |--------------| ---------------------------------------------------------------------------------------------|
| RIGHT ARROW  |              | Next event																					 |
| LEFT ARROW   |              | Previous event																				 |
| SPACE        |              | Start playing events forwards																 |
| BACKSPACE    |              | Start playing events reversed																 |
| V            |              | VPol waveforms (default view)																 |
| H	           |              | HPol waveforms																				 |
| X	           |              | Both polarizations																			 |
| TAB	       |              | Cycle filters																				 |
| F	           |              | Show/Hide filtering panel																	 |
| 	           | Q or F       | Quit filtering panel															             |
| G 	       |              | Show/Hide event selection panel															     |
| 	           | Q or G       | Quit event selection panel (only works before pressing tab)					                 |
| 	           | TAB          | Cycle through run/eventNumber fields											             |
| 	           | ENTER/RETURN | Go to selected event (and close panel)											             |
| U	           |              | UCorrelator view																			 |
| I	           |              | Interferometry (CrossCorrelator... currently put to shame by all UCorrelator's features)	 |




## Comments or Requests

To the usual address and bank account details
