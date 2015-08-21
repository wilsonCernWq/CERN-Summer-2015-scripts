#include "FormattedRecord.h"
#include "EventFragmentRecord.h"
#include "HistoNoData2D.h"

#include "storage/storageio.h"
#include "storage/event.h"
#include "storage/hit.h"

#include <stdlib.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <time.h>

#include <TApplication.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <TF1.h>
#include <TFile.h>

// add by Wu Qi for search files
#include <sys/types.h>
#include <dirent.h>

//#define DEBUG /** debug mode */

/**
 * \brief Describe function usages 
 */
void usage() {
  using namespace std;
  cout << "usage: RCEReadout <inDir> <outDir> <detector1Idx> <detector2Idx> ... <detectorNIdx>..." << endl;
  cout << "  To convert files with Allpix output format into Judith input format"<< endl;
}

/**
 * \brief Initialize reference detector
 */
void init_index(std::map<int, int>& m) {
  m[300] = 0;
  m[301] = 1;
  m[302] = 2;
  m[303] = 3;
  m[304] = 4;
  m[305] = 5;
}

/**
 * \brief Main entrance
 */
int main(int argc, char* argv[]) {
  using namespace std;
  typedef std::map<int,int>::iterator mapit;

  // Initialize indices
  std::map<int, int> index_setup;
  init_index(index_setup);  

  // Variables to be extracted
  double PosX, PosY, PosZ;
  int PixX, PixY, Value, chip_idx;;
  int HitInCluster, Timing,NHits;
  int TimeStamp, FrameNumber, TriggerOffset, TriggerInfo, Invalid;

  // ---------------------------------------------------------------------------------
  // P A R S E   I N P U T   A R G U M E N T S
  // ---------------------------------------------------------------------------------
  // Test argument number and parse arguments
  if (argc < 3) {

    cerr << ">>> ERROR: insufficient input arguments" << endl;
    usage(); return -1; 

  } else if (argc >= 3) {

    for (int i = 3; i < argc; ++i) {
      
      int idx = std::atoi( argv[i] ); // Get index from string input

      // Failure test      
      if (idx != 0) { 

	// Update index_setup list
	index_setup[idx] = index_setup.size()-1;

      } else {

	// Parsing error
	std::cerr << ">>> ERROR: Invalid detector index: " << argv[i] << std::endl;
	usage(); return -1;

      }

#ifdef DEBUG
      cout << "***DEBUG*** new detector index: "<< idx << endl;
#endif

    }

  }
  
  // Print detector indices
  for (mapit it = index_setup.begin(); it != index_setup.end(); ++it) {

    std::cout << ">>> -- Detector " << it->first << "\t=> " << it->second << std::endl;

  } 

  // parsing i/o directories
  std::string ifdir = string(argv[1]);
  std::string ofdir = string(argv[2]);

#ifdef DEBUG
  cout << "***DEBUG*** input: " << ifdir << " output: " << ofdir << endl;
#endif

  // ---------------------------------------------------------------------------------
  // S E T U P   S T O R A G E S   A N D   I / O   N A M E S 
  // ---------------------------------------------------------------------------------
  // ROOT storage
  char* ofile = Form("%s.root",ofdir.c_str(),0); // output root file name
  Storage::StorageIO* storage = 0; // setup storage
  unsigned int treeMask = Storage::Flags::TRACKS | Storage::Flags::CLUSTERS;
  storage = new Storage::StorageIO(ofile, Storage::OUTPUT, index_setup.size(), treeMask);
  
#ifdef DEBUG
  cout << "enerything is good till now (line " << __LINE__ << ") " << endl;
#endif

  // ---------------------------------------------------------------------------------
  // T R A V E R S E   I N P U T   D I R E C T O R Y 
  // ---------------------------------------------------------------------------------
  // Get input directory
  DIR *dir = opendir( ifdir.c_str() );

  // Failure test
  if ( dir == NULL ) {
    
    // Directory open failure
    std::cerr << ">>> ERROR: No such directory: "<< ifdir << std::endl;
    return -1;

  }

#ifdef DEBUG
  cout << "enerything is good till now (line " << __LINE__ << ") " << endl;
#endif

  // Read the directory, and pull in every file that doesn't start with '.'
  struct dirent *entry = NULL;
  int event_counter = 0; // Entry counter
  while ((entry = readdir(dir)) != NULL) {

#ifdef DEBUG
      cout << "***DEBUG*** found file: " << entry->d_name << endl;
#endif
    
    // files with '.' prefix are invisiable
    if ( entry->d_name[0] != '.'  ) {
      
#ifdef DEBUG
      cout << "***DEBUG*** enter file: " << entry->d_name << endl;
#endif
      
      // Loop through input files
      char* ifile = Form("%s/%s",ifdir.c_str(),entry->d_name);
      std::ifstream infile(ifile);
      
      // Failure test
      if (infile.good()) {

#ifdef DEBUG
      cout << "***DEBUG*** open file: " << entry->d_name << endl;
#endif

      } else {

	std::cerr << ">>> ERROR: Fail to open file: " << entry->d_name << std::endl;
	return -1;

      }
      ++event_counter;
      
#ifdef DEBUG
      cout << "enerything is good till now (line " << __LINE__ << ") " << endl;
#endif
      
      // Build storage for new event
      Storage::Event* storageEvent = NULL;
      storageEvent = new Storage::Event( index_setup.size() );

      // Parsing single files
      std::string line;
      while ( std::getline(infile, line) ) {
	std::stringstream strbuffer (line);

#ifdef DEBUG
	cout << "***DEBUG*** new line: " << strbuffer.str() << endl;
#endif

	// Check context
	if ( line[0] == '#' ) { // HEAD

	  // Data style from Allpix output
	  // # Start time (string) : Aug 03 12:48:16.000 2015 
	  // # Start time : 1438609696.000 
	  // # Acq time : 0.000000 
	  // # ChipboardID : Chip_300 
	  // # DACs : 5 100 255 127 127 0 405 7 130 128 80 62 128 128 
	  // # Mpx type : 3 
	  // # Timepix clock : 40  
	  // # Eventnr 4 
	  // # RelaxD 2 devs 4 TPX DAQ = 0x110402 = START_HW STOP_HW MPX_PAR_READ COMPRESS=1

#ifdef DEBUG	
	  cout << "***DEBUG*** --->>> reading head information" << endl; 
#endif
	  strbuffer.seekg(1);
	  std::string stime_str, stime_num, stime_acq, schip_idx; 
	  std::getline(strbuffer, stime_str, '#');
	  std::getline(strbuffer, stime_num, '#');
	  std::getline(strbuffer, stime_acq, '#');
	  std::getline(strbuffer, schip_idx, '#');

#ifdef DEBUG
	  cout << "***DEBUG*** " << stime_str << stime_num << stime_acq << schip_idx << endl;
#endif
	  
	  // Parse chip index
	  chip_idx = std::atof(schip_idx.substr(1+schip_idx.find('_')).c_str());

#ifdef DEBUG
	  cout << "***DEBUG*** reading detector: "<< chip_idx << endl;
#endif

	} else { // DATA

#ifdef DEBUG
	  cout << "***DEBUG*** --->>> reading data" << endl; 
#endif

	  // Extract integer data
	  strbuffer >> PixX >> PixY >> Value;
	  
	  // Some data ... ??
	  NHits=0;
	  Timing=0;
	  HitInCluster=1;
	  PosX=0;
	  PosY=0;
	  PosZ=0;

#ifdef DEBUG
	  cout << "***DEBUG*** --->>> data:";
	  cout << PixX << '-';
	  cout << PixY << '-';
	  cout << Value << endl; 
#endif

	  // Make new hit	  
	  Storage::Hit* hit = storageEvent->newHit(index_setup[chip_idx]);
	  hit->setPix((unsigned int)PixX, (unsigned int)PixY);
	  hit->setValue((unsigned int)Value);
	  hit->setTiming(1);

	}

      }

      infile.close();
      // Make stroage
      storageEvent->setTimeStamp(0);
      storageEvent->setFrameNumber(event_counter-1);
      storageEvent->setTriggerOffset(0);
      storageEvent->setTriggerInfo(0);
      storageEvent->setInvalid(false);
      storage->writeEvent(storageEvent);

    }
    
  }

  // Print finishing information
  std::cout << "Program Terminated >>> processed up to file No." << event_counter << " <<<" << std::endl;
  
#ifdef DEBUG
  cout << "enerything is good till now (line " << __LINE__ << ") " << endl;
#endif

  // clean up workspace
  delete storage;

#ifdef DEBUG
  cout << "enerything is good till now (line " << __LINE__ << ") " << endl;
#endif

  return 0;
}
