#include <sstream>
#include "TH1.h"
#include "TF1.h"
#include "TROOT.h"
#include "TMath.h"
#include "TStyle.h"
#include "TString.h"
#include "TMinuit.h"
#include "TRandom.h"

const Int_t numOfPara = 3;
const Double_t XPIXEL = 27.5; // pixel half size (Assume square pixel)
const std::string para[4] = { "GSigma", "Height", "SWidth", "Center" };

std::string failed_fit;

/**
 * @brief Square impulse function
 * @para m Mean (center) location
 * @para w Full width square function
 */
Double_t squarefun(Double_t x, Double_t m = 0.0, Double_t w = 0.055) {
  Double_t ax = TMath::Abs(x - m);                   //< actual X distance
  Double_t ay = TMath::Sqrt(0.25 * w * w - ax * ax); //< actual Y distance
  if (ax > w * 0.5) {
    return 0.0;
  } else {
    return 1.0;
    //return TMath::Min(1.0, ay / TMath::Min(XPIXEL, 0.5 * w));
  }
}

/**
 * @brief Square impulse convoluted with a Gaussian
 * @para par[0] Width (sigma) of convoluted Gaussian function
 * @para par[1] Height of the square impulse
 * @//para par[2] Width of the square impulse
 * @//para par[3] Square function center
 */
Double_t squaregaufun(Double_t *x, Double_t *par) {
  /** @todo Control constants */
  const Double_t invsq2pi = 0.3989422804014; //< (2 pi)^(-1/2)
  const Double_t np = 1000.0;                //< number of convolution steps
  const Double_t sc =    8.0;                //< convolution extends to +-sc Gaussian sigmas
  
  //< Default arguments
  Double_t* arg[4];
  arg[0] = new Double_t(0.00300);
  arg[1] = new Double_t(0.12690);
  arg[2] = new Double_t(0.05500);
  arg[3] = new Double_t(0.00000);
  for (Int_t i = 0; i < numOfPara; ++i) { 
    delete arg[i];
    arg[i] = &par[i];
  }

  Double_t fsqua;
  Double_t fgaus;

  Double_t xx;         //< integral variable
  Double_t xlow, xupp; //< integral ranges
  Double_t step, sum = 0.0;

  /** @todo Range and step of convolution integral */
  xlow = x[0] - sc * par[0];
  xupp = x[0] + sc * par[0];
  step = (xupp-xlow) / np;

  /** @todo Convolution integral of Landau and Gaussian by sum */
  for (Double_t i = 1.0; i <= np/2; ++i) {
    //< first half
    xx = xlow + (i-.5) * step;
    fsqua = squarefun(xx, arg[3][0], arg[2][0]);
    fgaus = TMath::Gaus(x[0],xx,arg[0][0]);
    sum += fsqua * fgaus;

    //< second half
    xx = xupp - (i-.5) * step;
    fsqua = squarefun(xx, arg[3][0], arg[2][0]);
    fgaus = TMath::Gaus(x[0],xx,arg[0][0]);
    sum += fsqua * fgaus;
  }
  return (arg[1][0] * step * sum * invsq2pi / arg[0][0]);
}

/**
 * @brief Fit the square-gaussian function
 * @para his Histogram to fit
 * @para fitrange[2] Lower and upper boundaries of fit range
 * @para startvalues[n] Reasonable start values for the fit
 * @para parlimitslo[n] Lower parameter limits
 * @para parlimitshi[n] Upper parameter limits 
 * @para fitparams[n] Returns the final fit parameters
 * @para fiterrors[n] Returns the final fit errors
 * @para ChiSqr Returns the chi square
 * @//para NDF Returns ndf (number of degrees of freedom)
 */
void squaregaufit(TH1D* his, Double_t *fitrange, Double_t *startvalues, 
		  Double_t *parlimitslo, Double_t *parlimitshi, 
		  Double_t *fitparams, Double_t *fiterrors,   
		  Double_t *ChiSqr, Double_t *NDF) {

  /** @todo Clean function name */
  Char_t FunName[100]; sprintf(FunName,"Fitfcn_%s",his->GetName());
  auto ffitold = (TF1*)gROOT->GetListOfFunctions()->FindObject(FunName);
  if (ffitold) { delete ffitold; }

  /** @todo Build function */
  auto ffit = new TF1(FunName,squaregaufun,fitrange[0],fitrange[1],numOfPara);
  ffit->SetParameters(startvalues);
  for (Int_t i = 0; i < numOfPara; ++i) {
    ffit->SetParName(i, para[i].c_str());
  }

  /** @todo Set parameter limits */
  for (Int_t i = 0; i < numOfPara; ++i) { 
    ffit->SetParLimits(i, parlimitslo[i], parlimitshi[i]); 
  }
  
  /** @todo Fitting */
  his->Fit(FunName, "REMWW"); // Chi2 fitting
  ffit->GetParameters(fitparams);   //< get fitted parameters
  for (Int_t i = 0; i < numOfPara; ++i) {   //< get fitted parameter errors
    fiterrors[i] = ffit->GetParError(i);
  }
  (*ChiSqr) = ffit->GetChisquare();  //< obtain chi square
  (*NDF   ) = (Double_t)ffit->GetNDF(); //< obtain ndf
}

// do single plot 
/**
 * @brief 
 * @para disk Disk file to save plot
 * @para fp[4] Final fit parameters
 * @para fe[4] Returns the final fit errors
 * @para fq[2] Fitting quality: chisqr, ndf
 */
void singlefit(TFile* disk, Int_t index,
	       const char* inputd, const char* output,
	       const char* method, const char* branch,
	       Double_t* fp, Double_t* fe, Double_t* fq) {

  using namespace std;
  /** @todo build random number generator */
  auto rand = new TRandom();
  rand->SetSeed();

  /** @todo get file names */
  string root_base = (inputd + ("/Run" + to_string(index)) + '/' +
		      method + "/output_rootfile.root");
  string name_base = (output + ("/Run" + to_string(index)) + '_' +
		      method + '_' + branch + "_fitted");

  /** @todo open root data */
  printf("Opened data\t>>> %s <<< \n", root_base.c_str());
  auto f = new TFile(root_base.c_str());

  /** @todo Draw reference graph */
  auto hNOR = (TH1D*)f->Get(branch);  
  auto c0 = new TCanvas();
  //auto hNOR = (TH1D*)hSNR->DrawNormalized("Q");
  //hNOR->Sumw2();
  //hNOR->Rebin(2);

  /** @todo Fitting histo */
  printf("Fitting...\n");
  Double_t heightU = hNOR->GetMaximum();
  Double_t heightI = heightU - 200;
  Double_t heightL = heightU - 400;
  Double_t fr[2];
  Double_t sv[4], pllo[4], plhi[4]; //< maximum number of parameters are 4

  fr[0] =-0.065;
  fr[1] = 0.065;

  sv[0] = 0.00300; pllo[0] = 0.00100; plhi[0] = 0.00500; 
  sv[1] = heightI; pllo[1] = heightL; plhi[1] = heightU; 
  sv[2] = 0.05500; pllo[2] = 0.00000; plhi[2] = 0.06000; 
  sv[3] = 0.00000; pllo[3] =-0.00050; plhi[3] = 0.00050; 

  //< Get fitted data
  cout << "  GSigma\t" << sv[0] << endl;
  cout << "  Height\t" << sv[1] << endl;
  cout << "  SWidth\t" << sv[2] << endl;
  cout << "  Center\t" << sv[3] << endl;
  squaregaufit(hNOR,fr,sv,pllo,plhi,fp,fe, &fq[0], &fq[1]);

  //< secondary fit if FAILED
  Int_t loop = 0;
  while (string(gMinuit->fCstatu.Data()).compare(0, 6, "FAILED")==0) {
    if (++loop > 50) { failed_fit += to_string(index) + " "; break; }
    printf(">>>> !!!!!! Re-Fit No %d !!!!!! %s ...\n", loop, gMinuit->fCstatu.Data());

    //< generate a random new value with gaussian distribution
    for (Int_t i = 0; i < numOfPara; ++i) {
      sv[i] = rand->Gaus(fp[i], 0.3 * fe[i]);
    }

    for (Int_t i = 0; i < numOfPara; ++i) {
      pllo[i] = sv[i] - 0.6 * fe[i];
      plhi[i] = sv[i] + 0.6 * fe[i];
    }

    cout << ">>>>   GSigma\t" << sv[0] << endl;
    cout << ">>>>   Height\t" << sv[1] << endl;
    cout << ">>>>   SWidth\t" << sv[2] << endl;
    cout << ">>>>   Center\t" << sv[3] << endl;
    squaregaufit(hNOR,fr,sv,pllo,plhi,fp,fe, &fq[0], &fq[1]);
  }

  /** @todo Plotting Histro with Fitted function */
  printf("Fitting done! Plotting results...\n");
  auto c1 = new TCanvas();
  hNOR->SetName ((string(hNOR->GetName() )+ '_' + method + "_Run" +to_string(index)).c_str());
  hNOR->SetTitle((string(hNOR->GetTitle())+ ' ' + method + " Run" +to_string(index)).c_str());
  hNOR->Draw();
  disk->WriteTObject(hNOR);
  c1->SaveAs((name_base + ".pdf").c_str());

  /** @todo clean up */
  c0->Close();
  c1->Close();
  f->Close("R");
}

/**
 * @brief Draw plots for one method
 * @para iidx Starting index
 * @para fidx Ending index
 * @para method
 */
TGraph** methodplots(TFile* disk, const Int_t iidx, const Int_t fidx,
		     const char* inputd, const char* output,
		     const char* method, const char* branch) {


  const Int_t size = (fidx - iidx) + 1;

  /** @todo initialize paratemers */
  Double_t** fp = new Double_t* [numOfPara]; // fit parameters
  Double_t** fe = new Double_t* [numOfPara]; // fit errors
  Double_t** fq = new Double_t* [2]; // fit qualities
  Double_t axis[size];
  for (Int_t i = 0; i < numOfPara; ++i) { fp[i] = new Double_t[size]; fe[i] = new Double_t[size]; }
  for (Int_t i = 0; i < 2; ++i) { fq[i] = new Double_t[size]; }
  for (Int_t i = 0; i < size; ++i) { axis[i] = (150.0 - 20.0) / 40.0 * i + 20.0; }

  /** @todo get parameters */
  Double_t rp[numOfPara]; // raw paratemers
  Double_t re[numOfPara]; // raw errors
  Double_t rq[2]; // raw qualities  
  TCanvas* c = NULL;
  for (Int_t k = 0; k < size; ++k) {
    singlefit(disk, k + iidx, inputd, output, method, branch, rp, re, rq);
    for (Int_t i = 0; i < numOfPara; ++i) { fp[i][k] = rp[i]; fe[i][k] = re[i]; }
    for (Int_t i = 0; i < 2; ++i) { fq[i][k] = rq[i]; }
  }

  /** @todo Setup graph option */
  auto gr = new TGraph* [numOfPara];
  for (Int_t i = 0; i < numOfPara; ++i) {
    gr[i] = new TGraphErrors(size, axis, fp[i], NULL, fe[i]);
    gr[i]->SetTitle(method);
    gr[i]->SetMarkerStyle(21);
    gr[i]->SetLineWidth(1);
    gr[i]->SetFillStyle(0);
  }
  return gr;
}

/**
 * @brief Main entrance
 */
void squaregaus(const char* inputd = "/afs/cern.ch/work/q/qiwu/Data/Simulation_DESY_r50/histo-clic",
		const char* output = ".",
		const char* branch = "resX_s1x1y1") {

  auto f = new TFile ("squaregaus.root","RECREATE");
  gBenchmark->Show("squaregaus");
  
  /** @todo Load CLIC style file */
  gROOT->ProcessLine(".L /afs/cern.ch/work/q/qiwu/Data/root/tools/CLICdpStyle.C");

  auto cas = new TCanvas*[numOfPara];
  auto mgs = new TMultiGraph*[numOfPara];

  //< No difference in different method at this moment so calculate one method only
  const Int_t numOfMethon = 1;
  auto grs = new TGraph**[numOfMethon];
  
  grs[0] = methodplots(f, 0, 40, inputd, output, "DigitalCentroid", branch);
  //grs[1] = methodplots(f, 0, 40, inputd, output, "QWeighted", branch);
  //grs[2] = methodplots(f, 0, 40, inputd, output, "EtaCorrection", branch);

  for (Int_t i = 0; i < numOfPara; ++i) {  
    mgs[i] = new TMultiGraph(("c_" + std::to_string(i)).c_str(), (para[i] + " residual fit plot").c_str());
    for (Int_t j = 0; j < numOfMethon; ++j) {
      mgs[i]->Add(grs[j][i]);
      grs[j][i]->SetMarkerColor(4+j);
      grs[j][i]->SetLineColor(1+j);
    }

    cas[i] = new TCanvas(("c_" + std::to_string(i)).c_str(), (para[i] + " residual fit plot").c_str(), 1000, 600); 
    cas[i]->SetGrid();
    mgs[i]->Draw("ALP");
    cas[i]->Update();

    // -- Setup legend
    TLegend* leg = cas[i]->BuildLegend();
    leg->SetX1(leg->GetX1()+0.20);
    leg->SetX2(leg->GetX2()+0.02);
    leg->SetY1(leg->GetY1()+0.25);
    leg->SetY2(leg->GetY2()+0.10);
    leg->SetTextSize(0.03);
    leg->SetFillStyle(0);
    
    // -- Setup axis labels and units
    mgs[i]->SetName((para[i] + '_' + branch + "_fit").c_str());
    mgs[i]->SetTitle((para[i] + " residual fit plot").c_str());
    mgs[i]->GetXaxis()->SetTitle("Zt(mm)");
    cas[i]->Modified();
    cas[i]->SaveAs(("./" + para[i] + '_' + branch + "_fit.pdf").c_str());
    f->WriteTObject(mgs[i]);
  }
  f->Close("R");
  std::cout << "***************************************************" << std::endl;
  std::cout << "    Failed fit: " << failed_fit << std::endl;
  std::cout << "***************************************************" << std::endl;
  gBenchmark->Show("squaregaus");
}
