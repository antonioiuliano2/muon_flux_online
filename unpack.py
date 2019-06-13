#!/usr/bin/env python2
import argparse
import ROOT

# Fix https://root-forum.cern.ch/t/pyroot-hijacks-help/15207 :
ROOT.PyConfig.IgnoreCommandLineOptions = True


def main():
    source = ROOT.ShipTdcSource(args.input)

    unpackers = [
        ROOT.DriftTubeUnpack(args.charm),
        ROOT.RPCUnpack(),
        ROOT.ScalerUnpack(),
    ]
    if args.charm:
        unpackers += [
            ROOT.PixelUnpack(0x0800),
            ROOT.PixelUnpack(0x0801),
            ROOT.PixelUnpack(0x0802),
            ROOT.SciFiUnpack(0x0900),
        ]

    for unpacker in unpackers:
        source.AddUnpacker(unpacker)

    run = ROOT.FairRunOnline(source)
    run.SetOutputFile(args.output)
    run.SetAutoFinish(True)
    run.SetRunId(args.run)

    run.Init()

    run.Run(-1, 0)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-f", "--input", required=True, help="Input file (can be on EOS)"
    )
    parser.add_argument("-o", "--output", required=True, help="Output file")
    parser.add_argument("-n", "--run", default=0, type=int, help="Run number")
    parser.add_argument(
        "--charm", action="store_true", help="Unpack charm data (default: muon flux)"
    )
    args = parser.parse_args()
    ROOT.gROOT.SetBatch(True)
    main()
