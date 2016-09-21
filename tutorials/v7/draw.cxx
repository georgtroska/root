/// \file
/// \ingroup tutorial_v7
///
/// \macro_code
///
/// \date 2015-03-22
/// \warning This is part of the ROOT 7 prototype! It will change without notice. It might trigger earthquakes. Feedback is welcome!
/// \author Axel Naumann <axel@cern.ch>

/*************************************************************************
 * Copyright (C) 1995-2015, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "Rtypes.h"

R__LOAD_LIBRARY(libGpad);

#include "ROOT/THist.h"
#include "ROOT/Canvas.h"
#include "ROOT/TDirectory.h"

void draw() {
  using namespace ROOT;

  Experimental::TAxisConfig xaxis("x", 10, 0., 1.);
  Experimental::TAxisConfig yaxis("y", {0., 1., 2., 3.,10.});
  auto pHist = std::make_shared<Experimental::TH2D>(xaxis, yaxis);

  pHist->Fill({0.01, 1.02});
  pHist->Fill({0.54, 3.02});
  pHist->Fill({0.98, 1.02});
  pHist->Fill({1.90, 1.02});
  pHist->Fill({0.75,-0.02});
  Experimental::TDirectory::Heap().Add("hist", pHist);

  auto canvas = Experimental::TCanvas::Create("Canvas Title");
  canvas->Draw(pHist);
}
