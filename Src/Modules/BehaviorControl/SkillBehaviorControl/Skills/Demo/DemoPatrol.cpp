/**
 * @file DemoPatrol.cpp
 *
 * This file implements the DemoPatrol skill.
 * The robot walks back and forth in a patrol pattern.
 */

#include "SkillBehaviorControl.h"

option((SkillBehaviorControl) DemoPatrol)
{
  static Pose2f startPose;
  static bool goingForward = true;

  initial_state(initial)
  {
    transition
    {
      if(state_time > 1000)
        goto patrol;
    }
    action
    {
      startPose = theRobotPose;
      goingForward = true;
      Stand({.high = true});
      LookForward();
    }
  }

  state(patrol)
  {
    transition
    {
      if(goingForward)
      {
        // Se sta andando in avanti, controlliamo se è arrivato al traguardo (a 1.5 metri di distanza)
        if((theRobotPose.inverse() * (startPose * Vector2f(1500.f, 0.f))).norm() < 150.f)
        {
          goingForward = false;
        }
      }
      else
      {
        // Se sta tornando indietro, controlliamo se è tornato al punto di partenza
        if((theRobotPose.inverse() * startPose.translation).norm() < 150.f)
        {
          goingForward = true;
        }
      }
    }
    action
    {
      if(goingForward)
      {
        // Va verso il punto a 1.5 metri davanti, mantenendo l'orientamento originale
        WalkToPose({.target = theRobotPose.inverse() * (startPose * Pose2f(0_deg, 1500.f, 0.f))});
      }
      else
      {
        // Torna al punto di partenza, ma gli chiediamo di arrivarci guardando all'indietro (180 gradi).
        // Questo farà sì che il navigatore gestisca una rotazione fluida automatica senza fermarsi!
        WalkToPose({.target = theRobotPose.inverse() * (startPose * Pose2f(180_deg, 0.f, 0.f))});
      }
      
      LookForward();
    }
  }
}