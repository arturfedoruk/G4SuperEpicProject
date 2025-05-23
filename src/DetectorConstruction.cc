//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//

#include "DetectorConstruction.hh"
//#include "DetectorMessenger.hh"
//#include "TrackerSD.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
//#include "G4SDManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"

#include "G4UserLimits.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4SystemOfUnits.hh"

using namespace SEP;

namespace SEP
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal
G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = nullptr;
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
  delete fStepLimit;
  //delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // Material definition

  G4NistManager* nistManager = G4NistManager::Instance();

  // Air defined using NIST Manager
  nistManager->FindOrBuildMaterial("G4_AIR");

  // Lead defined using NIST Manager
  fTargetMaterial  = nistManager->FindOrBuildMaterial("G4_W");

  // Xenon gas defined using NIST Manager
  fTrackerMaterial = nistManager->FindOrBuildMaterial("G4_He");

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
  G4Material* air  = G4Material::GetMaterial("G4_AIR");

  // Sizes of the principal geometrical components (solids)

  G4double trackerWidth = 20.*cm; // width of the tracker
  G4double trackerRadius = 10.*cm; // radius of the tracker
  G4double targetSide =  5.*cm; // side of the Target

  G4double worldZ = 30.*cm;
  G4double worldXY = 20.*cm;

  // Definitions of Solids, Logical Volumes, Physical Volumes

  // World

  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(worldZ);

  G4cout << "Computed tolerance = "
         << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
         << " mm" << G4endl;

  auto worldS = new G4Box("world",                       // its name
    worldXY / 2, worldXY / 2, worldZ / 2);  // its size
  auto worldLV = new G4LogicalVolume(worldS,             // its solid
    air,                                                 // its material
    "World");                                            // its name

  //  Must place the World Physical volume unrotated at (0,0,0).
  //
  auto worldPV = new G4PVPlacement(nullptr,  // no rotation
    G4ThreeVector(),                         // at (0,0,0)
    worldLV,                                 // its logical volume
    "World",                                 // its name
    nullptr,                                 // its mother  volume
    false,                                   // no boolean operations
    0,                                       // copy number
    fCheckOverlaps);                         // checking overlaps

  // Target

  G4ThreeVector positionTarget = G4ThreeVector(0,0, -10.*cm);

  auto targetS = new G4Box("target", targetSide / 2, targetSide / 2, targetSide / 2);
  fLogicTarget = new G4LogicalVolume(targetS, fTargetMaterial, "Target", nullptr, nullptr, nullptr);
  fTargetPV = new G4PVPlacement(nullptr,  // no rotation
    positionTarget,           // at (x,y,z)
    fLogicTarget,             // its logical volume
    "Target",                 // its name
    worldLV,                  // its mother volume
    false,                    // no boolean operations
    0,                        // copy number
    fCheckOverlaps);          // checking overlaps

  G4cout << "Target is " << targetSide/cm << " cm of "
         << fTargetMaterial->GetName() << G4endl;

  // Tracker

  G4ThreeVector positionTracker = G4ThreeVector(0,0, (worldZ - trackerWidth)/2);

  auto trackerS = new G4Tubs("tracker", 0, trackerRadius, trackerWidth/2, 0. * deg, 360. * deg);
  fLogicTracker = new G4LogicalVolume(trackerS, fTrackerMaterial, "Tracker", nullptr, nullptr, nullptr);
  fTrackerPV = new G4PVPlacement(nullptr,  // no rotation
    positionTracker,          // at (x,y,z)
    fLogicTracker,            // its logical volume
    "Tracker",                // its name
    worldLV,                  // its mother  volume
    false,                    // no boolean operations
    0,                        // copy number
    fCheckOverlaps);          // checking overlaps

  // Visualization attributes

  G4VisAttributes boxVisAtt(G4Colour::White());
  G4VisAttributes trackerVisAtt(G4Colour::Yellow());

  worldLV      ->SetVisAttributes(boxVisAtt);
  fLogicTarget ->SetVisAttributes(boxVisAtt);
  fLogicTracker    ->SetVisAttributes(trackerVisAtt);


  // Example of User Limits
  //
  // Below is an example of how to set tracking constraints in a given
  // logical volume
  //
  // Sets a max step length in the tracker region, with G4StepLimiter

  G4double maxStep = 0.5*trackerWidth;
  fStepLimit = new G4UserLimits(maxStep);
  fLogicTracker->SetUserLimits(fStepLimit);

  /// Set additional contraints on the track, with G4UserSpecialCuts
  ///
  /// G4double maxLength = 2*trackerLength, maxTime = 0.1*ns, minEkin = 10*MeV;
  /// fLogicTracker->SetUserLimits(new G4UserLimits(maxStep,
  ///                                           maxLength,
  ///                                           maxTime,
  ///                                           minEkin));

  // Always return the physical world

  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  // Sensitive detectors

  /*G4String trackerSDname = "/TrackerSD";
  auto aTrackerSD = new TrackerSD(trackerSDname, "TrackerHitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector(aTrackerSD);
  // Setting aTrackerSD to all logical volumes with the same name
  // of "Tracker".
  SetSensitiveDetector("Tracker", aTrackerSD, true);*/

  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue = G4ThreeVector();
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetTargetMaterial(G4String materialName)
{
  G4NistManager* nistManager = G4NistManager::Instance();

  G4Material* pttoMaterial =
              nistManager->FindOrBuildMaterial(materialName);

  if (fTargetMaterial != pttoMaterial) {
     if ( pttoMaterial ) {
        fTargetMaterial = pttoMaterial;
        if (fLogicTarget) fLogicTarget->SetMaterial(fTargetMaterial);
        G4cout
          << G4endl
          << "----> The target is made of " << materialName << G4endl;
     } else {
        G4cout
          << G4endl
          << "-->  WARNING from SetTargetMaterial : "
          << materialName << " not found" << G4endl;
     }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetTrackerMaterial(G4String materialName)
{
  G4NistManager* nistManager = G4NistManager::Instance();

  G4Material* pttoMaterial =
              nistManager->FindOrBuildMaterial(materialName);

  if (fTrackerMaterial != pttoMaterial) {
     if ( pttoMaterial ) {
        fTrackerMaterial = pttoMaterial;
        if (fLogicTracker) fLogicTracker->SetMaterial(fTrackerMaterial);
        G4cout
          << G4endl
          << "----> The tracker is made of " << materialName << G4endl;
     } else {
        G4cout
          << G4endl
          << "-->  WARNING from SetTrackerMaterial : "
          << materialName << " not found" << G4endl;
     }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetMaxStep(G4double maxStep)
{
  if ((fStepLimit)&&(maxStep>0.)) fStepLimit->SetMaxAllowedStep(maxStep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetCheckOverlaps(G4bool checkOverlaps)
{
  fCheckOverlaps = checkOverlaps;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
