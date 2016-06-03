/// \file
/// \ingroup tutorial_hist
/// Example showing how to combine the various candle plot options.
///
/// \macro_image
/// \macro_code
///
/// \author Georg Troska

void candlehisto()
{
   TCanvas *c1 = new TCanvas("c1","Candle Presets",1200,800);
   c1->Divide(3,2);

   TRandom *rand = new TRandom();
   TH2I *h1 = new TH2I("h1","Sin",18,0,360,100,-1.5,1.5);
   h1->GetXaxis()->SetTitle("Deg");

   float myRand;
   for (int i = 0; i < 360; i+=10) {
      for (int j = 0; j < 100; j++) {
         myRand = rand->Gaus(sin(i*3.14/180),0.2);
         h1->Fill(i,myRand);
      }
   }
   for (int i = 1; i < 7; i++) {
      c1->cd(i);
      char str[16];
      sprintf(str,"violinx%d",i);
      TH2I * myhist = (TH2I*)h1->DrawCopy(str);
      myhist->SetTitle(str);
   }

   TCanvas *c2 = new TCanvas("c2","Candle Individual",1200,800);
   c2->Divide(3,1);
   char myopt[16][8] = {"1000000","2000000","3000000","21","31","30","111","311","301","1111","2321","12111","112111","212111","312111"};
   for (int i = 0; i < 3; i++) {
      c2->cd(i+1);
      char str[16];
      sprintf(str, "candlex(%s)",myopt[i]);
      TH2I * myhist = (TH2I*)h1->DrawCopy(str);
	 myhist->SetFillColor(kYellow);
	 
	 //myhist->SetFillStyle(0);
	//myhist->SetBarWidth(1.5);
      myhist->SetTitle(str);
   }
}
