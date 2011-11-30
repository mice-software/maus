#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <cstring>
#include <map>
using namespace std;

 struct channel_id{
   channel_id():channel_number(0),fiber_number(0),plane_number(0),station_number(0),tracker_number(0),type("NULL"),tb(0){}
   int channel_number;
   int fiber_number;
   int plane_number;
   int station_number;
   int tracker_number;
   const char type[40];
   TBranch * tb;
 };


void TTreetest(){


  TTree * tree = new TTree("Alex","kool");

  //  vector<channel_id>* m_test=new vector<channel_id>();
  //tree->Branch("channel_id",&m_test,"channel_number/I:fiber_number/I:plane_number/I:station_number/I:tracker_number/I:type[40]/C");


  int i[10] = {1,2,3,4,5,6,7,8,9,10};
  //vector< vector<int>* > * m_vec = new vector<vector<int>* >();
  std::map<int,double>* m_map = new std::map<int,double>();
  //  tree->Branch("vecbranch",m_vec);

  channel_id chid;

  tree->Branch("alexbranch",&chid);

  (*m_map)[0] = 9.8;
  (*m_map)[1] = 19.8;
  (*m_map)[2] = 29.8;
  (*m_map)[3] = 39.8;

  //m_vec.push_back(new vector<int>(i,i+5));
  //m_vec.push_back(new vector<int>(i+5,i+9));


  tree->Fill();


  //  tree->Fill();

  TFile outFile("TEST.root","RECREATE");
  tree->Write();
  outFile.Close();

  delete tree;
  tree=0;



}
