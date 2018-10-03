#include "DriftTubeUnpack.h"
#include "FairRunOnline.h"
#include "PixelUnpack.h"
#include "RPCUnpack.h"
#include "ScalerUnpack.h"
#include "ShipTdcSource.h"
#include "TROOT.h"
#include "TString.h"
#include "boost/program_options.hpp"
#include "FairLogger.h"

int main(int argc, char **argv)
{
   std::string infile, outfile;
   try {
      int run_number = 0;
      bool charm = false;
      /** Define and parse the program options
       */
      namespace po = boost::program_options;
      po::options_description desc("Options");
      desc.add_options()
         ("infile,f", po::value<std::string>(&infile)->required(), "Input file (can be on EOS)")
         ("outfile,o", po::value<std::string>(&outfile)->required(), "Output file")
         ("run,n", po::value<int>(&run_number), "Run number")
         ("charm", po::bool_switch(&charm), "Unpack charm data (default: muon flux)");

      po::variables_map vm;
      try {
         po::store(po::parse_command_line(argc, argv, desc),
                   vm); // can throw

         po::notify(vm); // throws on error, so do after help in case
                         // there are any problems
      } catch (po::error &e) {
         LOG(ERROR) << e.what() << FairLogger::endl;
         LOG(ERROR) << desc << FairLogger::endl;
         return 1;
      }

      gROOT->SetBatch(true);
      auto source = new ShipTdcSource(infile.data());

      source->AddUnpacker(new DriftTubeUnpack());
      source->AddUnpacker(new RPCUnpack());
      source->AddUnpacker(new ScalerUnpack());
      if (charm) {
         // TODO merge into single unpacker?
         source->AddUnpacker(new PixelUnpack(0x0800));
         source->AddUnpacker(new PixelUnpack(0x0801));
         source->AddUnpacker(new PixelUnpack(0x0802));
      }

      // Create online run ---------------------------------------------------------
      auto run = new FairRunOnline(source);
      run->SetOutputFile(outfile.data());
      run->SetAutoFinish(true);
      run->SetRunId(run_number);

      // Initialize ----------------------------------------------------------------
      run->Init();

      // Run -----------------------------------------------------------------------
      run->Run(-1, 0); // run over entire file for negative argument.

   } catch (std::exception &e) {
      LOG(ERROR) << "Unhandled Exception reached the top of main: " << e.what() << ", application will now exit" << FairLogger::endl;
      return 2;
   }

   return 0;
}
