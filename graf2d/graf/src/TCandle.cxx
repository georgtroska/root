// @(#)root/graf:$Id$
// Author: Georg Troska 2016/04/14

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include <stdlib.h>
#include <iostream>

#include "TROOT.h"
#include "TCandle.h"
#include "TClass.h"


#include "TMath.h"
#include "TPad.h"
#include "TRandom2.h"


ClassImp(TCandle)

const Int_t kNMAX = 2000;

/** \class TCandle
\ingroup BasicGraphics

One candle of a box-plot
*/

////////////////////////////////////////////////////////////////////////////////
/// TCandle default constructor.

TCandle::TCandle() : 
	fIsCalculated(0), 
	fIsRaw(0), 
	fPosCandleAxis(0.), 
	fCandleWidth(1.0), 
	fMean(0.), 
	fMedian(0.), 
	fBoxUp(0.), 
	fBoxDown(0.), 
	fWhiskerUp(0.), 
	fWhiskerDown(0.), 
	fNDatapoints(0),
	fDismiss(0) {
 
}

////////////////////////////////////////////////////////////////////////////////
/// TCandle raw data constructor.
/*
TCandle::TCandle(const Double_t candlePos, const Double_t candleWidth, const Int_t n, const Double_t * points)
	:TObject(), TAttLine(), TAttFill(), TAttMarker(),
	fIsCalculated(0), 
	fIsRaw(1), 
	fPosCandleAxis(candlePos), 
	fCandleWidth(candleWidth), 
	fNDatapoints(n), 
	fDatapoints(points),
	fDismiss(0) {
	//Preliminary values only, need to be calculated before paint
   fMean = 0;
   fMedian = 0;
   fBoxUp = 0;
   fBoxDown = 0;
   fWhiskerUp = 0;
   fWhiskerDown = 0; 
}
*/
////////////////////////////////////////////////////////////////////////////////
/// TCandle TH1 data constructor.
TCandle::TCandle(const Double_t candlePos, const Double_t candleWidth, TH1D *proj)
	: TObject(), TAttLine(), TAttFill(), TAttMarker(),
	fIsCalculated(0), 
	fIsRaw(0), 
	fPosCandleAxis(candlePos), 
	fCandleWidth(candleWidth), 
	fNDatapoints(0), 
	fDatapoints(0),
	fProj(proj),
	fDismiss(0) {
	//Preliminary values only, need to be calculated before paint
	fMean = 0;
	fMedian = 0;
	fBoxUp = 0;
	fBoxDown = 0;
	fWhiskerUp = 0;
	fWhiskerDown = 0; 
	fNDatapoints = 0;

	fOption = kNoOption;
   
}

////////////////////////////////////////////////////////////////////////////////
/// TCandle default destructor.

TCandle::~TCandle() {
	
}

////////////////////////////////////////////////////////////////////////////////
/// TCandle copy constructor.

TCandle:: TCandle(const TCandle &candle) 
	: TObject(), TAttLine(), TAttFill(), TAttMarker(),
	fIsCalculated(0), 
	fIsRaw(0), 
	fPosCandleAxis(0.), 
	fCandleWidth(1.0), 
	fMean(0.), 
	fMedian(0.), 
	fBoxUp(0.), 
	fBoxDown(0.), 
	fWhiskerUp(0.), 
	fWhiskerDown(0.), 
	fNDatapoints(0),
	fDismiss(0) {
	((TCandle&)candle).Copy(*this);
}

////////////////////////////////////////////////////////////////////////////////
/// Copy this line to line.

void TCandle::Copy(TObject &obj) const {
	TObject::Copy(obj);
	TAttLine::Copy(((TCandle&)obj));
	TAttFill::Copy(((TCandle&)obj));
	TAttMarker::Copy(((TCandle&)obj));

	((TCandle&)obj).fIsCalculated = fIsCalculated;
	((TCandle&)obj).fIsRaw = fIsRaw;
	((TCandle&)obj).fPosCandleAxis = fPosCandleAxis;
	((TCandle&)obj).fCandleWidth = fCandleWidth;
	((TCandle&)obj).fMean = fMean;
	((TCandle&)obj).fMedian = fMedian;
	((TCandle&)obj).fBoxUp = fBoxUp;
	((TCandle&)obj).fBoxDown = fBoxDown;
	((TCandle&)obj).fWhiskerUp = fWhiskerUp;
	((TCandle&)obj).fWhiskerDown = fWhiskerDown;
	((TCandle&)obj).fNDatapoints = fNDatapoints;
	((TCandle&)obj).fDatapoints = fDatapoints;
	((TCandle&)obj).fProj = fProj;
	((TCandle&)obj).fDismiss = fDismiss;
}

////////////////////////////////////////////////////////////////////////////////
/// Parsing of the option-string. Can't use Option_t here, because it is const
/// char. The option-string will be empty at the end (by-reference).

int TCandle::ParseOption(char * opt) {
	fOption = kNoOption;
	char *l;
	//char opt[128];
	//Int_t nch = strlen(optin);
    //strlcpy(opt,optin,128);
	
	l = strstr(opt,"CANDLE");
	if (l) {
		
		//Hoption.Scat = 0;
		//Hoption.Candle = 1; // bit 0 is always on!

		const CandleOption fallbackCandle = (CandleOption)(kBox + kMedianLine + kMeanCircle + kWhiskerAll + kAnchor);

		char direction = ' ';
		char preset = ' ';

		if (l[6] >= 'A' && l[6] <= 'Z') direction = l[6];
		if (l[6] >= '1' && l[6] <= '9') preset = l[6];
		if (l[7] >= 'A' && l[7] <= 'Z' && preset != ' ') direction = l[7];
		if (l[7] >= '1' && l[7] <= '9' && direction != ' ') preset = l[7];

		if (direction == 'X' || direction == 'V') { /* nothing */ } 
		if (direction == 'Y' || direction == 'H') { fOption = (CandleOption)(fOption + kHorizontal); } 
		if (preset == '1') //Standard candle using old candle-definition
			fOption = (CandleOption)(fOption + fallbackCandle);
		if (preset == '2') //New standard candle with better whisker definition + outlier
			fOption = (CandleOption)(fOption + kBox + kMeanLine + kMedianLine + kWhisker15 + kAnchor + kPointsOutliers);
		if (preset == '3')  //Like candle2 but with a mean as a circle
			fOption = (CandleOption)(fOption + kBox + kMeanCircle + kMedianLine + kWhisker15 + kAnchor + kPointsOutliers);
		if (preset == '4')  //Like candle3 but showing the uncertainty of the median as well
			fOption = (CandleOption)(fOption + kBox + kMeanCircle + kMedianNotched + kWhisker15 + kAnchor + kPointsOutliers);
		if (preset == '5')  //Like candle2 but showing all datapoints
			fOption = (CandleOption)(fOption + kBox + kMeanLine + kMedianLine + kWhisker15 + kAnchor + kPointsAll);
		if (preset == '6')  //Like candle2 but showing all datapoints scattered
			fOption = (CandleOption)(fOption + kBox + kMeanCircle + kMedianLine + kWhisker15 + kAnchor + kPointsAllScat);

		
		if (preset != ' ' && direction != ' ')
			strncpy(l,"        ",8);
		else if (preset != ' ' || direction != ' ')
			strncpy(l,"        ",7);
		else
			strncpy(l,"        ",6);
		

		Bool_t useIndivOption = false;

		if (preset == ' ') { // Check if the user wants to set the properties individually
			Int_t n = 0;
			char *brOpen = strstr(opt,"(");
			char *brClose = strstr(opt,")");
			char indivOption[32];
			if (brOpen && brClose) {
				useIndivOption = true;
				strncpy(indivOption, brOpen, brClose-brOpen +1); //Now the string "(....)" including brackets is in this array
				sscanf(indivOption,"(%d)", &fOption);
				strncpy(brOpen,"                ",brClose-brOpen+1); //Cleanup
			
			}
		}
		//Handle option "CANDLE" ,"CANDLEX" or "CANDLEY" to behave like "CANDLEX1" or "CANDLEY1"
		if (!useIndivOption && !fOption ) {
			fOption = fallbackCandle;
		}
	}
	std::cout << "L is: " << l << std::endl;
	return fOption;
	
}




////////////////////////////////////////////////////////////////////////////////
/// Execute action corresponding to one event.


void TCandle::ExecuteEvent(Int_t event, Int_t px, Int_t py) {
  
}


////////////////////////////////////////////////////////////////////////////////
/// List this TCandle with its attributes.
#if 0
void TCandle::ls(Option_t *) const {
   TROOT::IndentLevel();
   printf("%s  Pos=%f WhiskerUp=%f BoxUp=%f Median=%f Mean=%f BoxDown=%f, WhiskerDown=%f\n",IsA()->GetName(),fPosCandleAxis, fWhiskerUp, fBoxUp, fMedian, fMean, fBoxDown, fWhiskerDown);
}
#endif
////////////////////////////////////////////////////////////////////////////////
/// Paint this line with its current attributes.

void TCandle::Paint(Option_t *opt)
{
	std::cout << "in Paint of TCandle!" << std::endl;
	std::cout << "fOption is: " << fOption <<std::endl;
	/*
	//Overwrite existing options with these
	if (opt) {
		ParseOption(opt);
		Calculate();
	}
	//If something was changed before, we need to recalculate some values
	if (!fIsCalculated) Calculate();
	*/
	
	// Save the attributes as they were set originally
	Style_t saveFill   = GetFillStyle();
	Style_t saveColor  = GetFillColor();
	Style_t saveLine   = GetLineStyle();
	Style_t saveWidth  = GetLineWidth();
	Style_t saveMarker = GetMarkerStyle();

	// Determining the quantiles
	Double_t *prob = new Double_t[5];
	prob[0]=1E-15; prob[1]=0.25; prob[2]=0.5; prob[3]=0.75; prob[4]=1-1E-15;
	Double_t *quantiles = new Double_t[5];
	quantiles[0]=0.; quantiles[1]=0.; quantiles[2] = 0.; quantiles[3] = 0.; quantiles[4] = 0.;

	
	fProj->GetQuantiles(5, quantiles, prob);
	Double_t iqr = quantiles[3]-quantiles[1];

   // Check if the quantiles are valid, seems the under- and overflow is taken
   // into account as well, we need to ignore this!
   if (quantiles[0] >= quantiles[4]) return;
   if (quantiles[1] >= quantiles[3]) return;

   // Definition of the candle in the standard case
   Double_t boxHigh = quantiles[3];
   Double_t boxLow = quantiles[1];
   Double_t whiskerUpper = quantiles[4]; //Standard case
   Double_t whiskerLower = quantiles[0]; //Standard case
   Double_t dimLeft = fPosCandleAxis-0.5*fCandleWidth;
   Double_t dimRight = fPosCandleAxis+0.5*fCandleWidth;
   Double_t median = quantiles[2];
   Double_t mean = fProj->GetMean();
   Double_t medianErr = 1.57*iqr/sqrt(fProj->GetEntries());

	
   //fH->SetMarkerColor(fH->GetLineColor());
   TAttLine::Modify();
   TAttFill::Modify();
   TAttMarker::Modify();
   


   Bool_t swapXY = IsOption(kHorizontal); //((foption.Candle >> 1) & 0x01);
   Bool_t doLogY = false; //(!(swapXY) && Hoption.Logy) || (swapXY && Hoption.Logx);
   Bool_t doLogX = false; //(!(swapXY) && Hoption.Logx) || (swapXY && Hoption.Logy);

   //if (((Hoption.Candle >> 2) & 0x03) == 0x02) { // Improved whisker definition, with 1.5*iqr
   if (IsOption(kWhisker15)) {
      int bin = fProj->FindBin(boxLow-1.5*iqr);
      // extending only to the lowest data value within this range
      while (fProj->GetBinContent(bin) == 0 && bin <= fProj->GetNbinsX()) bin++;
      whiskerLower = fProj->GetBinCenter(bin);

      bin = fProj->FindBin(boxHigh+1.5*iqr);
      while (fProj->GetBinContent(bin) == 0 && bin >= 1) bin--;
      whiskerUpper = fProj->GetBinCenter(bin);
   }

   //if (((Hoption.Candle >> 13) & 0x03) == 0x01) { // Draw a simple box
   if (IsOption(kBox)) {
     // if (((Hoption.Candle >> 4) & 0x03) == 0x02) { // Check if we have to draw a box with notches
     if (IsOption(kMedianNotched)) {
         Double_t x[] = {dimLeft,  dimLeft, dimLeft+fCandleWidth/3., dimLeft, dimLeft, dimRight,
                         dimRight, dimRight-fCandleWidth/3., dimRight, dimRight, dimLeft};
         Double_t y[] = {boxLow, median-medianErr, median, median+medianErr, boxHigh, boxHigh,
                         median+medianErr, median, median-medianErr, boxLow, boxLow};
         PaintLBox(11, x, y, swapXY, kFALSE);
      } else { // draw a simple box
         Double_t x[] = {dimLeft, dimLeft, dimRight, dimRight, dimLeft};
         Double_t y[] = {boxLow,  boxHigh, boxHigh,  boxLow,   boxLow};
         PaintLBox(5, x, y, swapXY, kFALSE);
      }
   } //else if (((Hoption.Candle >> 13) & 0x03) == 0x02) { // Draw a filled box
   else if (IsOption(kBoxFilled)) {
      //if (((Hoption.Candle >> 4) & 0x03) == 0x02) { // Check if we have to draw a box with notches
      if (IsOption(kMedianNotched)) {
         Double_t x[] = {dimLeft,  dimLeft, dimLeft+fCandleWidth/3., dimLeft, dimLeft, dimRight,
                         dimRight, dimRight-fCandleWidth/3., dimRight, dimRight, dimLeft};
         Double_t y[] = {boxLow, median-medianErr, median, median+medianErr, boxHigh, boxHigh,
                         median+medianErr, median, median-medianErr, boxLow, boxLow};
         PaintLBox(11, x, y, swapXY, kTRUE);
      } else { // draw a simple box
         Double_t x[] = {dimLeft, dimLeft, dimRight, dimRight, dimLeft};
         Double_t y[] = {boxLow,  boxHigh, boxHigh,  boxLow,   boxLow};
         PaintLBox(5, x, y, swapXY, kTRUE);
      }
   }

   //if (((Hoption.Candle >> 11) & 0x03) == 0x01) { // Draw the anchor line
   if (IsOption(kAnchor)) {
      PaintLine(dimLeft, whiskerUpper, dimRight, whiskerUpper, swapXY);
      PaintLine(dimLeft, whiskerLower, dimRight, whiskerLower, swapXY);
   }

   //if (((Hoption.Candle >> 2) & 0x03) == 0x01) { // Whiskers are dashed
   if (IsOption(kWhiskerAll)) {
      SetLineStyle(2);
      TAttLine::Modify();
      PaintLine(fPosCandleAxis, whiskerUpper, fPosCandleAxis, boxHigh, swapXY);
      PaintLine(fPosCandleAxis, boxLow, fPosCandleAxis, whiskerLower, swapXY);
      SetLineStyle(saveLine);
      TAttLine::Modify();
   } //else if (((Hoption.Candle >> 2) & 0x03) == 0x02) { // Whiskers without dashing, better whisker definition (done above)
   else if (IsOption(kWhisker15)) {
      PaintLine(fPosCandleAxis, whiskerUpper, fPosCandleAxis, boxHigh, swapXY);
      PaintLine(fPosCandleAxis, boxLow, fPosCandleAxis, whiskerLower, swapXY);
   }

   //if ((( Hoption.Candle >> 4) & 0x03) == 0x01 ) { // Paint median as a line
   if (IsOption(kMedianLine)) {
      PaintLine(dimLeft, median, dimRight, median, swapXY);
   } //else if ((( Hoption.Candle >> 4) & 0x03) == 0x02 ) { // Paint median as a line (using notches, median line is shorter)
   else if (IsOption(kMedianNotched)) {
      PaintLine(dimLeft+fCandleWidth/3, median, dimRight-fCandleWidth/3., median, swapXY);
   } //else if ((( Hoption.Candle >> 4) & 0x03) == 0x03 ) { // Paint median circle
   else if (IsOption(kMedianCircle)) {
      Double_t myMedianX[1], myMedianY[1];
      if (!swapXY) {
         myMedianX[0] = fPosCandleAxis;
         myMedianY[0] = median;
      } else {
         myMedianX[0] = median;
         myMedianY[0] = fPosCandleAxis;
      }

      Bool_t isValid = true;
      if (doLogX) {
         if (myMedianX[0] > 0) myMedianX[0] = TMath::Log10(myMedianX[0]); else isValid = false;
      }
      if (doLogY) {
         if (myMedianY[0] > 0) myMedianY[0] = TMath::Log10(myMedianY[0]); else isValid = false;
      }

      SetMarkerStyle(24);
      TAttMarker::Modify();

      if (isValid) gPad->PaintPolyMarker(1,myMedianX,myMedianY); // A circle for the median

      SetMarkerStyle(saveMarker);
      TAttMarker::Modify();
  
   }

  // if ((( Hoption.Candle >> 6) & 0x03) == 0x03 ) { // Paint mean as a circle
  if (IsOption(kMeanCircle)) {
      Double_t myMeanX[1], myMeanY[1];
      if (!swapXY) {
         myMeanX[0] = fPosCandleAxis;
         myMeanY[0] = mean;
      } else {
         myMeanX[0] = mean;
         myMeanY[0] = fPosCandleAxis;
      }

      Bool_t isValid = true;
      if (doLogX) {
         if (myMeanX[0] > 0) myMeanX[0] = TMath::Log10(myMeanX[0]); else isValid = false;
      }
      if (doLogY) {
         if (myMeanY[0] > 0) myMeanY[0] = TMath::Log10(myMeanY[0]); else isValid = false;
      }

      SetMarkerStyle(24);
      TAttMarker::Modify();

      if (isValid) gPad->PaintPolyMarker(1,myMeanX,myMeanY); // A circle for the mean

      SetMarkerStyle(saveMarker);
      TAttMarker::Modify();
      
   } //else if ((( Hoption.Candle >> 6) & 0x03) == 0x01 ) { // Paint mean as a dashed line
	else if (IsOption(kMeanLine)) {
      SetLineStyle(2);
      TAttLine::Modify();
      
      PaintLine(dimLeft, mean, dimRight, mean, swapXY);
      SetLineStyle(saveLine);
      TAttLine::Modify();

   }

   //if (((Hoption.Candle >> 11) & 0x03) == 0x01) { //Draw standard anchor
   if (IsOption(kAnchor)) {
      PaintLine(dimLeft, whiskerLower, dimRight, whiskerLower, swapXY); // the lower anchor line
      PaintLine(dimLeft, whiskerUpper, dimRight, whiskerUpper, swapXY); // the upper anchor line
   }


   // This is a bit complex. All values here are handled as outliers. Usually
   // only the datapoints outside the whiskers are shown.
   // One can show them in one row as crosses, or scattered randomly. If activated
   // all datapoint are shown in the same way
   TRandom2 random;
   //if ((Hoption.Candle >> 8) & 0x01) { //Draw outliers
   if (GetOption(5) > 0) {
	   std::cout << "Yes, draw the outliers!" << std::endl;
      const int maxOutliers = kNMAX; // Max outliers per candle
      Double_t outliersX[maxOutliers];
      Double_t outliersY[maxOutliers];
      Double_t myScale = 1.;
      if (fProj->GetEntries() > maxOutliers/2) myScale = fProj->GetEntries()/(maxOutliers/2.);
      int nOutliers = 0;
      for (int bin = 0; bin < fProj->GetNbinsX(); bin++) {
         // Either show them only outside the whiskers, or all of them
         //if (proj->GetBinContent(bin) > 0 && (proj->GetBinCenter(bin) < whiskerLower || proj->GetBinCenter(bin) > whiskerUpper || ((Hoption.Candle >> 9) & 0x01)) ) {
         if (fProj->GetBinContent(bin) > 0 && (fProj->GetBinCenter(bin) < whiskerLower || fProj->GetBinCenter(bin) > whiskerUpper || (GetOption(5) > 1)) ) {
         Double_t scaledBinContent = fProj->GetBinContent(bin)/myScale;
         if (scaledBinContent >0 && scaledBinContent < 1) scaledBinContent = 1; //Outliers have a typical bincontent between 0 and 1, when scaling they would disappear
            for (int j=0; j < (int)scaledBinContent; j++) {
               if (nOutliers > maxOutliers) break;
               //if ((Hoption.Candle >> 10) & 0x01) { //Draw outliers and "all" values scattered
               if (IsOption(kPointsAllScat)) {
                  outliersX[nOutliers] = fPosCandleAxis - fCandleWidth/2. + fCandleWidth*random.Rndm();
                  outliersY[nOutliers] = fProj->GetBinLowEdge(bin) + fProj->GetBinWidth(bin)*random.Rndm();
               } else { //Draw them in the "candle line"
                  outliersX[nOutliers] = fPosCandleAxis;
                  if ((int)scaledBinContent == 1) //If there is only one datapoint available put it in the middle of the bin
                     outliersY[nOutliers] = fProj->GetBinCenter(bin);
                  else //If there is more than one datapoint scatter it along the bin, otherwise all marker would be (invisibly) stacked on top of each other
                     outliersY[nOutliers] = fProj->GetBinLowEdge(bin) + fProj->GetBinWidth(bin)*random.Rndm();
               }
               if (swapXY) {
                  //Swap X and Y
                  Double_t keepCurrently;
                  keepCurrently = outliersX[nOutliers];
                  outliersX[nOutliers] = outliersY[nOutliers];
                  outliersY[nOutliers] = keepCurrently;
               }
               // Continue means, that nOutliers is not increased, so that value will not be shown
               if (doLogX) {
                  if (outliersX[nOutliers] > 0) outliersX[nOutliers] = TMath::Log10(outliersX[nOutliers]); else continue;
               }
               if (doLogY) {
                  if (outliersY[nOutliers] > 0) outliersY[nOutliers] = TMath::Log10(outliersY[nOutliers]); else continue;
               }
               nOutliers++;
            }
         }
         if (nOutliers > maxOutliers) { //Should never happen, due to myScale!!!
            Error ("PaintCandlePlot","Not possible to draw all outliers.");
            break;
         }
      }
      //fH->SetMarkerColor(fH->GetLineColor());
     //fH->TAttMarker::Modify();
      
      //if ((Hoption.Candle >> 10) & 0x01) { //Draw outliers and "all" values scattered
	  if (IsOption(kPointsAllScat)) {
         SetMarkerStyle(0);
      } else {
         SetMarkerStyle(5);
      }
      TAttMarker::Modify();
      gPad->PaintPolyMarker(nOutliers,outliersX, outliersY);
   }
/*
   // Set everything back to original
   fH->SetFillStyle(saveFill);
   fH->SetFillColor(saveColor);
   fH->SetLineStyle(saveLine);
   fH->SetMarkerStyle(saveMarker);
   fH->SetLineWidth(saveWidth);
   fH->TAttFill::Modify();
   fH->TAttLine::Modify();
   fH->TAttMarker::Modify();
   * */
	
}


////////////////////////////////////////////////////////////////////////////////
/// Dump this line with its attributes.
#if 0
void TCandle::Print(Option_t *) const {
   printf("%s  X1=%f Y1=%f X2=%f Y2=%f",IsA()->GetName(),fX1,fY1,fX2,fY2);
   if (GetLineColor() != 1) printf(" Color=%d",GetLineColor());
   if (GetLineStyle() != 1) printf(" Style=%d",GetLineStyle());
   if (GetLineWidth() != 1) printf(" Width=%d",GetLineWidth());
   printf("\n");
}
#endif

////////////////////////////////////////////////////////////////////////////////
/// Save primitive as a C++ statement(s) on output stream out

#if 0
void TCandle::SavePrimitive(std::ostream &out, Option_t * /*= ""*/)
{
   if (gROOT->ClassSaved(TCandle::Class())) {
      out<<"   ";
   } else {
      out<<"   TCandle *";
   }
   
    out<<"candle = new TCandle("<<fX1<<","<<fY1<<","<<fX2<<","<<fY2
      <<");"<<std::endl;

   SaveLineAttributes(out,"candle",1,1,1);

   out<<"   candle->Draw();"<<std::endl;
   
}
#endif


////////////////////////////////////////////////////////////////////////////////
/// Stream an object of class TLine.
/* Do we really need this?????
 * 
void TLine::Streamer(TBuffer &R__b)
{
   if (R__b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
      if (R__v > 1) {
         R__b.ReadClassBuffer(TLine::Class(), this, R__v, R__s, R__c);
         return;
      }
      //====process old versions before automatic schema evolution
      TObject::Streamer(R__b);
      TAttLine::Streamer(R__b);
      Float_t x1,y1,x2,y2;
      R__b >> x1; fX1 = x1;
      R__b >> y1; fY1 = y1;
      R__b >> x2; fX2 = x2;
      R__b >> y2; fY2 = y2;
      //====end of old versions

   } else {
      R__b.WriteClassBuffer(TLine::Class(),this);
   }
}
* */

///////////////////////////////////////////////////////////////////////////////
/// Getter for one option from fOption

int TCandle::GetOption(const int pos) {
	return (fOption/(int)TMath::Power(10,pos))%10;
}

///////////////////////////////////////////////////////////////////////////////
/// Return true is this option is activated in fOption
bool TCandle::IsOption(CandleOption opt) {
	int myOpt = 9;
	int pos = 0;
	for (pos = 0; pos < 7; pos++) {
		if (myOpt > opt) break;
		else myOpt *=10;
	} 
	myOpt /= 9;
	int thisOpt = GetOption(pos);
	
	return ((thisOpt * myOpt) == opt);
	
	
	
	
}

void TCandle::PaintLBox(Int_t nPoints, Double_t *x, Double_t *y, Bool_t swapXY, Bool_t fill)
{
   Bool_t doLogY = false; //(!(swapXY) && Hoption.Logy) || (swapXY && Hoption.Logx);
   Bool_t doLogX = false; //(!(swapXY) && Hoption.Logx) || (swapXY && Hoption.Logy);
   if (doLogY) {
      for (int i=0; i<nPoints; i++) {
         if (y[i] > 0) y[i] = TMath::Log10(y[i]);
         else return;
      }
   }
   if (doLogX) {
      for (int i=0; i<nPoints; i++) {
         if (x[i] > 0) x[i] = TMath::Log10(x[i]);
         else return;
      }
   }
   if (!swapXY) {
      if (fill) gPad->PaintFillArea(nPoints, x, y);
      
      gPad->PaintPolyLine(nPoints, x, y);
   } else {
      if (fill) gPad->PaintFillArea(nPoints, y, x);
      gPad->PaintPolyLine(nPoints, y, x);
   }
}

void TCandle::PaintLine(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Bool_t swapXY)
{
   Bool_t doLogY = false; //(!(swapXY) && Hoption.Logy) || (swapXY && Hoption.Logx);
   Bool_t doLogX = false; //(!(swapXY) && Hoption.Logx) || (swapXY && Hoption.Logy);
   if (doLogY) {
      if (y1 > 0) y1 = TMath::Log10(y1); else return;
      if (y2 > 0) y2 = TMath::Log10(y2); else return;
   }
   if (doLogX) {
      if (x1 > 0) x1 = TMath::Log10(x1); else return;
      if (x2 > 0) x2 = TMath::Log10(x2); else return;
   }
   if (!swapXY) {
      gPad->PaintLine(x1, y1, x2, y2);
   } else {
      gPad->PaintLine(y1, x1, y2, x2);
   }
}
