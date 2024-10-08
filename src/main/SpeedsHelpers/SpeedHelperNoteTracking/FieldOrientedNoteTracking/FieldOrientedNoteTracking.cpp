// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "FieldOrientedNoteTracking.h"

FieldOrientedNoteTracking::FieldOrientedNoteTracking(
    Chassis *chassis, photon::PhotonCamera *noteTrackingCamera){
        this->chassis = chassis;
        this->noteTrackingCamera = noteTrackingCamera;

        this->wYPIDController.SetIZone(3);
        this->wYPIDController.SetTolerance(0.05_m);

        this->wXPIDController.SetIZone(3);
        this->wXPIDController.SetTolerance(0.01_m);
    }

void FieldOrientedNoteTracking::alterSpeed(frc::ChassisSpeeds &inputSpeed) {
    if(!noteTrackingCamera->HasTargets()){
        return;
    }

    photon::PhotonTrackedTarget trackedTarget = noteTrackingCamera->GetLatestResult().GetTargets().front();
    units::meter_t distanceToTarget = photon::PhotonUtils::CalculateDistanceToTarget(0.25_m, 0.045_m, -20_deg, units::degree_t(trackedTarget.GetPitch()));

    frc::Translation2d targetTranslation = photon::PhotonUtils::EstimateCameraToTargetTranslation(distanceToTarget, {units::degree_t(trackedTarget.GetYaw())});

    double outY = wYPIDController.Calculate(targetTranslation.Y(), 0_m);
    double outX = wXPIDController.Calculate(targetTranslation.X(), 0_m);


    if (wYPIDController.AtSetpoint() && wXPIDController.AtSetpoint()){
        outY = 0;
        outX = 0;
    }

    inputSpeed.vy = units::meters_per_second_t(outY);
    inputSpeed.vx = units::meters_per_second_t(outX);
}

void FieldOrientedNoteTracking::initialize() {}
