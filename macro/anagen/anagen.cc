//******************************
// Generator Level Particle Information.
//******************************

#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TH1F.h"
#include "TCanvas.h"
#include <iostream>
#include <cmath>

void anagen() {
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
    TTreeReaderArray<float> pt(reader, "Particle.PT");
    TTreeReaderArray<float> eta(reader, "Particle.Eta");
    TTreeReaderArray<float> phi(reader, "Particle.Phi");
    TTreeReaderArray<float> mass(reader, "Particle.Mass");

    // Create histograms for kinematic variables
    TH1F *hist_pt = new TH1F("Particle_PT", "Particle PT;PT [GeV];Events", 100, 0, 500);
    TH1F *hist_eta = new TH1F("Particle_Eta", "Particle Eta;Eta;Events", 100, -5, 5);
    TH1F *hist_phi = new TH1F("Particle_Phi", "Particle Phi;Phi [rad];Events", 100, -3.14, 3.14);
    TH1F *hist_mass = new TH1F("Particle_Mass", "Particle Mass;Mass [GeV];Events", 100, 0, 200);

    // Loop over all events
    while (reader.Next()) {
        for (int i = 0; i < pt.GetSize(); i++) {
            // Fill histograms
            hist_pt->Fill(pt[i]);
            hist_eta->Fill(eta[i]);
            hist_phi->Fill(phi[i]);
            hist_mass->Fill(mass[i]);
        }
    }

    // Create a canvas for plotting
    TCanvas *canvas = new TCanvas("canvas", "Particle Kinematics", 800, 600);

    // Open a PDF file for multi-page output
    canvas->Print("Particle_Kinematics.pdf[");

    // Plot each histogram on a separate page
    hist_pt->SetLineColor(kBlue);
    hist_pt->Draw();
    canvas->Print("Particle_Kinematics.pdf");

    hist_eta->SetLineColor(kRed);
    hist_eta->Draw();
    canvas->Print("Particle_Kinematics.pdf");

    hist_phi->SetLineColor(kGreen);
    hist_phi->Draw();
    canvas->Print("Particle_Kinematics.pdf");

    hist_mass->SetLineColor(kMagenta);
    hist_mass->Draw();
    canvas->Print("Particle_Kinematics.pdf");

    // Close the PDF file
    canvas->Print("Particle_Kinematics.pdf]");

    // Save histograms to a ROOT file
    TFile *outputFile = TFile::Open("Particle_Kinematics.root", "RECREATE");
    hist_pt->Write();
    hist_eta->Write();
    hist_phi->Write();
    hist_mass->Write();
    outputFile->Close();

    // Clean up
    delete hist_pt;
    delete hist_eta;
    delete hist_phi;
    delete hist_mass;
    delete canvas;
    file->Close();
    delete file;

    std::cout << "Histograms saved to 'Particle_Kinematics.root' and 'Particle_Kinematics.pdf'." << std::endl;
}
