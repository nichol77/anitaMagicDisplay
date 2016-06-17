//////////////////////////////////////////////////////////////////////////////
/////  AnitaGpsCanvasMaker.h        ANITA GPS Canvas Maker                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////                                                                    ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ANITAGPSCANVASMAKER_H
#define ANITAGPSCANVASMAKER_H

//Includes
#include <iostream>
#include <TObject.h>
#include <TMath.h>
#include <TVector3.h>

#include "AnitaConventions.h"
#include "MagicDisplayConventions.h"

class Adu5Pat;
class Adu5Sat;
class Adu5Vtg;
class G12Pos;
class G12Sat;
class GpsGga;

class TPad;

#define MAX_GPS_TIME_POINTS 200

//!  The GPS canvas maker.
/*!
  This is where all the heavy lifting regarding the GPS plots is done.
*/
class AnitaGpsCanvasMaker 
{
 public:
   AnitaGpsCanvasMaker(); ///<Constructor
   ~AnitaGpsCanvasMaker(); ///<Destructor
   
   //Instance generator
   static AnitaGpsCanvasMaker*  Instance(); ///<Instance generator
   TPad *getGpsMainCanvas(Adu5Sat *adu5SatAPtr, Adu5Sat *adu5SatBPtr, G12Sat *g12SatPtr, Adu5Pat *adu5PatAPtr, Adu5Pat *adu5PatBPtr, G12Pos *g12PosPtr, TPad *useCan=0); ///<Returns the main GPS canvas
   TPad *getGpsInfoCanvas(Adu5Sat *adu5SatAPtr, Adu5Sat *adu5SatBPtr, G12Sat *g12SatPtr, Adu5Pat *adu5PatAPtr, Adu5Pat *adu5PatBPtr, G12Pos *g12PosPtr,TPad *useCan); ///<Returns the GPS info canvas, the canvas can be provided in <i>useCan</i>
   
   
   void setGpsDisplayOpt(MagicDisplayGpsDisplay::MagicDisplayGpsDisplay_t displayOpt) { fDisplayOpt=displayOpt;} ///< Sets the GPS display option
   MagicDisplayGpsDisplay::MagicDisplayGpsDisplay_t getGpsDisplayOpt() {return fDisplayOpt;} ///< Gets the GPS display option

 protected:
   static AnitaGpsCanvasMaker *fgInstance; ///< Protect against multiple instances.
   MagicDisplayGpsDisplay::MagicDisplayGpsDisplay_t fDisplayOpt;
   void getSatelliteView(Adu5Sat *adu5SatAPtr, Adu5Sat *adu5SatBPtr, G12Sat *g12SatPtr, TPad *plotPad);
   void addSatellitesToTimeArray(Adu5Sat *adu5SatAPtr, Adu5Sat *adu5SatBPtr, G12Sat *g12SatPtr);
   void getMapView(Adu5Pat *adu5PatPtr, TPad *plotPad);
   void getXYFromLatLong(double latitude, double longitude,
			 double &x, double &y);
   
   void getRelXYFromLatLong(float latitude, float longitude,
			    float &x, float &y);   

   Double_t fG12SatTimeArray[MAX_GPS_TIME_POINTS];
   Double_t fG12SatNumArray[MAX_GPS_TIME_POINTS];
   Double_t fAdu5ASatTimeArray[MAX_GPS_TIME_POINTS];
   Double_t fAdu5ASatNumArray[4][MAX_GPS_TIME_POINTS];
   Double_t fAdu5BSatTimeArray[MAX_GPS_TIME_POINTS];
   Double_t fAdu5BSatNumArray[4][MAX_GPS_TIME_POINTS];
   Int_t fNumG12Sat;
   Int_t fNumAdu5ASat;
   Int_t fNumAdu5BSat;
   Int_t fReloadMap;
};


#endif //ANITAGPSCANVASMAKER_H
