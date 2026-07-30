#include "SkillBehaviorControl.h"

option((SkillBehaviorControl) DemoFigureEight)
{
  static Pose2f startPose;

  initial_state(stand)
  {
    transition
    {
      if(state_time > 2000)
        goto p1;
    }
    action
    {
      startPose = theRobotPose;
      Stand();
      LookForward();
    }
  }

  // Segmento 1: Va al vertice in alto a sinistra dell'8 (guardando indietro)
  state(p1)
  {
    transition
    {
      if((theRobotPose.inverse() * (startPose * Vector2f(800.f, 800.f))).norm() < 250.f)
        goto p2;
    }
    action
    {
      WalkToPose({.target = theRobotPose.inverse() * (startPose * Pose2f(180_deg, 800.f, 800.f)), .speed = Pose2f(0.5f, 0.8f, 0.5f)});
      LookForward();
    }
  }

  // Segmento 2: Torna al centro incrociando
  state(p2)
  {
    transition
    {
      if((theRobotPose.inverse() * startPose.translation).norm() < 250.f)
        goto p3;
    }
    action
    {
      WalkToPose({.target = theRobotPose.inverse() * (startPose * Pose2f(-45_deg, 0.f, 0.f)), .speed = Pose2f(0.5f, 0.8f, 0.5f)});
      LookForward();
    }
  }

  // Segmento 3: Va al vertice in alto a destra dell'8 (guardando indietro)
  state(p3)
  {
    transition
    {
      if((theRobotPose.inverse() * (startPose * Vector2f(800.f, -800.f))).norm() < 250.f)
        goto p4;
    }
    action
    {
      WalkToPose({.target = theRobotPose.inverse() * (startPose * Pose2f(180_deg, 800.f, -800.f)), .speed = Pose2f(0.5f, 0.8f, 0.5f)});
      LookForward();
    }
  }

  // Segmento 4: Torna al centro incrociando di nuovo
  state(p4)
  {
    transition
    {
      if((theRobotPose.inverse() * startPose.translation).norm() < 250.f)
        goto p1;
    }
    action
    {
      WalkToPose({.target = theRobotPose.inverse() * (startPose * Pose2f(45_deg, 0.f, 0.f)), .speed = Pose2f(0.5f, 0.8f, 0.5f)});
      LookForward();
    }
  }
}
