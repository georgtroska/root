void treecandle() {
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
	}
	
	t->Draw("a:b:c","","candle");
	
}
