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

#ifndef B2aDetectorConstruction_h
#define B2aDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "tls.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;
class G4UserLimits;
class G4GlobalMagFieldMessenger;

namespace SEP
{

//class DetectorMessenger;

/// Detector construction class to define materials, geometry
/// and global uniform magnetic field.

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction() = default;
    ~DetectorConstruction() override;

  public:
    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

    // Set methods
    void SetTargetMaterial (G4String );
    void SetTrackerMaterial(G4String );
    void SetMaxStep (G4double );
    void SetCheckOverlaps(G4bool );
    const G4VPhysicalVolume* GetTrackerPV() const;
    const G4VPhysicalVolume* GetTargetPV() const;

  private:
    // methods
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();

    // static data members
    static G4ThreadLocal G4GlobalMagFieldMessenger*  fMagFieldMessenger;
                                         // magnetic field messenger
                                         
    // data members

    G4LogicalVolume*  fLogicTarget = nullptr;  // pointer to the logical Target
    G4LogicalVolume*  fLogicTracker = nullptr; // pointer to the logical Tracker
    
    G4VPhysicalVolume* fTargetPV = nullptr;  // pointer to the physical Target
    G4VPhysicalVolume* fTrackerPV = nullptr;  // pointer to the physical Tracker

    G4Material*       fTargetMaterial = nullptr;  // pointer to the target  material
    G4Material*       fTrackerMaterial = nullptr; // pointer to the tracker material

    G4UserLimits* fStepLimit = nullptr; // pointer to user step limits

    //DetectorMessenger* fMessenger = nullptr; // messenger

    G4bool fCheckOverlaps = true; // option to activate checking of volumes overlaps
};

inline const G4VPhysicalVolume* DetectorConstruction::GetTrackerPV() const {
  return fTrackerPV;
}

inline const G4VPhysicalVolume* DetectorConstruction::GetTargetPV() const {
  return fTargetPV;
}

}

#endif
