int violinplot() {
    TCanvas *c1 = new TCanvas("c1","c1",600,800);
    c1->Divide(1,2);
    c1->cd(1);
    Int_t nx(6), ny(40);
    Double_t xmin(0.0), xmax(+6.0), ymin(0.0), ymax(+4.0);
    TH2F* hviolin = new TH2F("hviolin", "Option VIOLIN example", nx, xmin, xmax, ny, ymin, ymax);
    TF1 f1("f1", "gaus", +0,0 +4.0);
    Double_t x,y;
    for (Int_t iBin=1; iBin<hviolin->GetNbinsX(); ++iBin) {
        Double_t xc = hviolin->GetXaxis()->GetBinCenter(iBin);
        f1.SetParameters(1, 2.0+TMath::Sin(1.0+xc), 0.2+0.1*(xc-xmin)/xmax);
        for(Int_t i=0; i<10000; ++i){
            x = xc;
            y = f1.GetRandom();
            hviolin->Fill(x, y);
        }
    }
    hviolin->SetFillColor(kGray);
    //hviolin->SetLineColor(kGray);
    hviolin->SetMarkerStyle(20);
    hviolin->SetMarkerSize(0.5);
    hviolin->Draw("VIOLIN");
    gPad->Update();
    
    c1->cd(2);
    hviolin->Draw("candle(13001300)");
    return 0;
}
