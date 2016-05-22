void treecandle() {
	gStyle->SetOptStat(0);
	TCanvas *c1 = new TCanvas();
	c1->Divide(1,2);
	TH2I *h1 = new TH2I("h1","a:b:c",3,1,4,100,-10,10);
	TTree *t = new TTree("tree","tree");
	float a, b, c;
	t->Branch("a", &a);
	t->Branch("b", &b);
	t->Branch("c", &c);
	
	TRandom *rand = new TRandom();
	for (int i = 0; i < 10000; i++) {
		a = rand->Gaus(0,1);
		b = rand->Gaus(0,2);
		c = a + b;
		t->Fill();
		h1->Fill(1,a);
		h1->Fill(2,b);
		h1->Fill(3,c);
	}
	c1->cd(1);
	t->Draw("a:b:c","","candle");
	c1->cd(2);
	h1->Draw("candle2");
	h1->GetXaxis()->SetBinLabel(1,"a");
	h1->GetXaxis()->SetBinLabel(2,"b");
	h1->GetXaxis()->SetBinLabel(3,"c");
	
	
}
