void candledecay()
{
   TCanvas *c1 = new TCanvas("c1","Candle Decay",1200,800);
   c1->Divide(2,1);
   TRandom *rand = new TRandom();
   TH2I *h1 = new TH2I("h1","Decay",1000,0,1000,20,0,20);
   
   float myRand;
   for (int i = 0; i < 17; i++) {
      for (int j = 0; j < 1000000; j++) {
         myRand = rand->Gaus(350+i*5,5+5*i);
         h1->Fill(myRand,i);
      }
   }
   h1->SetBarWidth(4);
   h1->SetFillStyle(0);
   h1->GetYaxis()->SetTitle("time");
   h1->GetXaxis()->SetTitle("probability density"); 
   TH2I *h2 = (TH2I*)h1->Clone("h2");
   h2->SetBarWidth(0.8);
   c1->cd(1);
   h1->Draw("candley(2000000)");
   c1->cd(2);
   h2->DrawCopy("candley2");
}
