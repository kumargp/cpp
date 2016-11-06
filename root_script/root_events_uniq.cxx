//.L scriptname

#include <vector>

Analysis(Char_t FileName[])
{
    TChain* chain = new TChain("SNSNFlux");
    chain->Add(FileName);


    Int_t eventID=0;
    chain->SetBranchAddress("evtID",&eventID);

    vector <Int_t> evtArr;
    vector <Int_t> evtCountArr;
    for (Long64_t i=0; i<chain->GetEntries(); ++i)
    {
        chain->GetEntry(i);
        bool newEvt = true;
        for(Long64_t n=0; n< evtArr.size(); ++n)
        {
            if(evtArr[n] == eventID)
			{
				newEvt = false;
				break; //use same n below
			}
        }
     
        if(newEvt)
        {
            evtArr.push_back(eventID);
			evtCountArr.push_back(1);
        }
        else
		{
			evtCountArr[n]++;
		}
    }
    for(Long64_t n=0; n< evtArr.size(); n++)
    {
	cout << "eventID:num " << evtArr[n] << " "<< evtCountArr[n] << endl;
    }
    cout << "number_of_uniq_events " <<  evtArr.size() << endl;
    cout << "Total events " << chain->GetEntries() << endl;    
}
