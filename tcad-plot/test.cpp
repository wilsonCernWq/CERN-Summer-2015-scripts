/* Header files for c++ I/O */
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

/**
 * \brief Import .csv data from file as single TGraph
 * \param Name data file name
 */
TGraph* getTGraph(const char* name) {
  /* Axis definitions */
  vector<double> depth;  //< Y from the front contact inside the dipole 
  vector<double> field; //< The Y component of E field in each point

  /* Inputs parsing */
  string line;
  ifstream file (name);
  if (file.is_open()) {
    while (getline (file, line)) {
      if (isdigit(line[0]) || line[0] == '-') {
	size_t pos = line.find(',');
	string str1 = line.substr(0,  pos);
	string str2 = line.substr(pos + 1);
	double num1 = atof(str1.c_str());
	double num2 = atof(str2.c_str());
	depth.push_back(num1);
	field.push_back(num2);
      }
    }
    file.close();
  } else { cout << "Unable to open file"; }

  /* Draw graph */
  TGraph* gr = new TGraph(depth.size(), &depth[0], &field[0]);
  return gr;
}

/**
 * \brief Testing draw exercise
 */
void test(const char* l_prefix, 
	  const char* f_prefix,
	  int file_start,
	  int sta, int end, int stp) 
{
  /* Load CLIC style file */
  gROOT->ProcessLine(".L ./CLICdpStyle.C");
  // CLICdpStyle();

  /* Setup Canavas */
  TCanvas *c = new TCanvas("canvas", "Multiple Plot Test",800,400);

  /* Multiple Plots */
  TMultiGraph *mg = new TMultiGraph();
  mg->SetTitle("Multiple APL graphs");

  // setup stacks
  int n = (end - sta) / stp + 1;

  TGraph** gs = new TGraph*[n];
  string*  ns = new string [n]; 

  stringstream lbuffer; //< legend buffer
  stringstream fbuffer; //< filename buffer

  // read single graphs
  for (int i = 0; i < n; ++i) {
    // clean up buffers
    lbuffer.str("");
    fbuffer.str("");
    
    // setup prefix
    lbuffer << string(l_prefix);
    fbuffer << string(f_prefix);

    // plot name
    lbuffer << i * stp + sta;
    ns[i] = lbuffer.str();

    // plot filename
    fbuffer << i + file_start << ".csv";
    gs[i] = getTGraph(fbuffer.str().c_str());

    // plot format
    mg->Add(gs[i]);
    gs[i]->SetTitle(ns[i].c_str());
    // -- setup color variation
    gs[i]->SetMarkerColor(i+2);
    gs[i]->SetLineColor(i+2);
    // -- setup line styles
    gs[i]->SetLineWidth(3);
    gs[i]->SetFillStyle(0); /* line background */
  }

  /* Drawing */
  mg->Draw("ALP");

  // Setup overall title and units
  c->Update();
  
  // -- Setup legend
  TLegend* leg = c->BuildLegend();
  leg->SetFillStyle(0);
  leg->SetX1(leg->GetX1()+0.25);
  leg->SetY1(leg->GetY1()-0.2);

  // -- Setup axis labels and units
  mg->GetXaxis()->SetTitle("depth (um)");
  mg->GetYaxis()->SetTitle("E_{field} (V/cm)");
  c->Modified();
}
