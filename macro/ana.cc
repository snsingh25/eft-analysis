#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLorentzVector.h"
#include <iostream>
#include <cmath>

void ana() {
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

    // Debug variables
    int eventCounter = 0;  // Count processed events
    int particleCounter = 0;  // Count processed particles

    // Loop over all events
    while (reader.Next()) {
        eventCounter++;
        std::cout << "Processing event " << eventCounter << "..." << std::endl;

        for (int i = 0; i < pt.GetSize(); i++) {
            particleCounter++;
            // std::cout << "Particle " << particleCounter
            //           << " - PT: " << pt[i]
            //           << ", Eta: " << eta[i]
            //           << ", Phi: " << phi[i]
            //           << ", Mass: " << mass[i] << std::endl;

            // Fill histograms
            hist_pt->Fill(pt[i]);
            hist_eta->Fill(eta[i]);
            hist_phi->Fill(phi[i]);
            hist_mass->Fill(mass[i]);
        }
    }

    // Create a canvas to draw the histograms
    TCanvas *canvas = new TCanvas("canvas", "Particle Kinematics", 1200, 800);
    canvas->Divide(2, 2);

    // Draw histograms
    canvas->cd(1);
    hist_pt->SetLineColor(kBlue);
    hist_pt->Draw();

    canvas->cd(2);
    hist_eta->SetLineColor(kRed);
    hist_eta->Draw();

    canvas->cd(3);
    hist_phi->SetLineColor(kGreen);
    hist_phi->Draw();

    canvas->cd(4);
    hist_mass->SetLineColor(kMagenta);
    hist_mass->Draw();

    // Save the canvas as a PDF
    canvas->SaveAs("Particle_Kinematics_Debug.pdf");

    // Clean up
    delete hist_pt;
    delete hist_eta;
    delete hist_phi;
    delete hist_mass;
    delete canvas;
    file->Close();
    delete file;

    std::cout << "Histograms saved as 'Particle_Kinematics_Debug.pdf'." << std::endl;
}
