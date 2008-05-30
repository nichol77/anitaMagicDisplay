//////////////////////////////////////////////////////////////////////////////
/////  MagicDisplayConventions.h        Magic Display Conventions        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A handy file full of enumerations and the like identifying     /////
/////     the conventions we use in Magic Display                        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////


#include "MagicDisplayConventions.h"


int getNiceColour(int index)
{
    if(index>10) return index;
    Int_t niceColours[11]={kAzure-2,kSpring-1,kRed+1,kMagenta+1,kCyan+1,kOrange+2
,kGreen-2,12,40,20,41};
    return niceColours[index];
}
