#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TLorentzVector.h"
#include <iostream>
#include <cmath>
#include <string>

const std::string& inputOperator = "T2Quad";
const std::string& inputFilePath = "/data/snsingh/aQGC_Gridpacks/ssWW/allrootfiles/"+inputOperator+"/run01_events.root";

void anarecojj() {
    // Open the ROOT file
    TFile *file = TFile::Open(inputFilePath.c_str());
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Unable to open input file '" << inputFilePath << "'!" << std::endl;
        return;
    }

    std::string outputName = inputOperator + "_recojj.root"; // Create output name
    std::cout << "Output file will be named: " << outputName << std::endl;

    // Get the Delphes tree
    TTree *tree = dynamic_cast<TTree*>(file->Get("Delphes"));
    if (!tree) {
        std::cerr << "Error: TTree 'Delphes' not found in the file!" << std::endl;
        file->Close();
        return;
    }

    // Set up TTreeReader
    TTreeReader reader(tree);
    TTreeReaderArray<float> pt(reader, "Jet.PT");
    TTreeReaderArray<float> eta(reader, "Jet.Eta");
    TTreeReaderArray<float> phi(reader, "Jet.Phi");
    TTreeReaderArray<float> mass(reader, "Jet.Mass");
    TTreeReaderValue<int> jetSize(reader, "Jet_size");

    // Create output ROOT file and TTree for storing NTuples
    TFile *outputFile = new TFile(outputName.c_str(), "RECREATE");
    TTree *outputTree = new TTree("JetKinematics", "Jet Kinematics Tree");

    // Variables to store in the tree
    float jet1_pt, jet1_eta, jet1_phi, jet1_mass;
    float jet2_pt, jet2_eta, jet2_phi, jet2_mass;
    float deltaR = -999.0;
    float mjj = -999.0; // Combined invariant mass of the two jets

    // Branches for the output tree
    outputTree->Branch("Jet1_PT", &jet1_pt, "Jet1_PT/F");
    outputTree->Branch("Jet1_Eta", &jet1_eta, "Jet1_Eta/F");
    outputTree->Branch("Jet1_Phi", &jet1_phi, "Jet1_Phi/F");
    outputTree->Branch("Jet1_Mass", &jet1_mass, "Jet1_Mass/F");
    outputTree->Branch("Jet2_PT", &jet2_pt, "Jet2_PT/F");
    outputTree->Branch("Jet2_Eta", &jet2_eta, "Jet2_Eta/F");
    outputTree->Branch("Jet2_Phi", &jet2_phi, "Jet2_Phi/F");
    outputTree->Branch("Jet2_Mass", &jet2_mass, "Jet2_Mass/F");
    outputTree->Branch("DeltaR", &deltaR, "DeltaR/F");
    outputTree->Branch("Mjj", &mjj, "Mjj/F");

    // Loop over all events
    while (reader.Next()) {
        if (*jetSize != 2) continue; // Process only events with exactly 2 jets

        // Process jets for kinematic variables, ΔR, and mjj calculation
        TLorentzVector jet1, jet2;
        for (int i = 0; i < pt.GetSize(); i++) {
            TLorentzVector jet;
            jet.SetPtEtaPhiM(pt[i], eta[i], phi[i], mass[i]);

            // Store the first two jets
            if (i == 0) {
                jet1 = jet;
                jet1_pt = pt[i];
                jet1_eta = eta[i];
                jet1_phi = phi[i];
                jet1_mass = mass[i];
            } else if (i == 1) {
                jet2 = jet;
                jet2_pt = pt[i];
                jet2_eta = eta[i];
                jet2_phi = phi[i];
                jet2_mass = mass[i];
                break; // Only need the first two jets
            }
        }

        // Calculate ΔR and mjj
        if (jet1.Pt() > 0 && jet2.Pt() > 0) {
            deltaR = jet1.DeltaR(jet2);
            mjj = (jet1 + jet2).M(); // Combined invariant mass
        } else {
            deltaR = -999.0; // Invalid ΔR
            mjj = -999.0;    // Invalid mjj
        }

        // Fill the NTuple
        outputTree->Fill();
    }

    // Write and close the output file
    outputTree->Write();
    outputFile->Close();

    // Clean up
    file->Close();
    delete file;

    std::cout << "NTuples saved to '" << outputName << "'." << std::endl;
}
