#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TTreeReaderValue.h"
#include "TH1F.h"
#include "TCanvas.h"
#include <iostream>
#include <cmath>

void anarecojets() {
    // Open the ROOT file
    TFile *file = TFile::Open("/data/snsingh/aQGC_Gridpacks/ssWW/allrootfiles/T2Int/run01_events.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Unable to open input file!" << std::endl;
        return;
    }

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
    TTreeReaderValue<int> jetSize(reader, "Jet_size"); // Read jet multiplicity

    // Create histograms for kinematic variables and jet multiplicity
    TH1F *hist_pt = new TH1F("Jet_PT", "Jet PT;PT [GeV];Events", 100, 0, 500);
    TH1F *hist_eta = new TH1F("Jet_Eta", "Jet Eta;Eta;Events", 100, -5, 5);
    TH1F *hist_phi = new TH1F("Jet_Phi", "Jet Phi;Phi [rad];Events", 100, -3.14, 3.14);
    TH1F *hist_mass = new TH1F("Jet_Mass", "Jet Mass;Mass [GeV];Events", 100, 0, 200);
    TH1F *hist_multiplicity = new TH1F("Jet_Multiplicity", "Jet Multiplicity;Number of Jets;Events", 20, 0, 20);

    int eventCounter = 0;

    // Loop over all events
    while (reader.Next()) {
        // Fill jet multiplicity histogram
        hist_multiplicity->Fill(*jetSize);

        for (int i = 0; i < pt.GetSize(); i++) {
            // Fill histograms for kinematic variables
            hist_pt->Fill(pt[i]);
            hist_eta->Fill(eta[i]);
            hist_phi->Fill(phi[i]);
            hist_mass->Fill(mass[i]);
        }

        eventCounter++;
    }

    // Create a canvas for plotting
    TCanvas *canvas = new TCanvas("canvas", "Jet Kinematics", 800, 600);

    // Open a PDF file for multi-page output
    canvas->Print("Jet_Kinematics.pdf[");

    // Plot each histogram on a separate page
    hist_pt->SetLineColor(kBlue);
    hist_pt->Draw();
    canvas->Print("Jet_Kinematics.pdf");

    hist_eta->SetLineColor(kRed);
    hist_eta->Draw();
    canvas->Print("Jet_Kinematics.pdf");

    hist_phi->SetLineColor(kGreen);
    hist_phi->Draw();
    canvas->Print("Jet_Kinematics.pdf");

    hist_mass->SetLineColor(kMagenta);
    hist_mass->Draw();
    canvas->Print("Jet_Kinematics.pdf");

    hist_multiplicity->SetLineColor(kCyan);
    hist_multiplicity->Draw();
    canvas->Print("Jet_Kinematics.pdf");

    // Close the PDF file
    canvas->Print("Jet_Kinematics.pdf]");

    // Save histograms to a ROOT file
    TFile *outputFile = TFile::Open("Jet_Kinematics.root", "RECREATE");
    hist_pt->Write();
    hist_eta->Write();
    hist_phi->Write();
    hist_mass->Write();
    hist_multiplicity->Write();
    outputFile->Close();

    // Clean up
    delete hist_pt;
    delete hist_eta;
    delete hist_phi;
    delete hist_mass;
    delete hist_multiplicity;
    delete canvas;
    file->Close();
    delete file;

    std::cout << "Histograms saved to 'Jet_Kinematics.root' and 'Jet_Kinematics.pdf'." << std::endl;
}
