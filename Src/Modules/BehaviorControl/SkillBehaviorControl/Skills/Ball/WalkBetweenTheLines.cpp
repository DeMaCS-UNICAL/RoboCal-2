/**
 * @file WalkBetweenTheLines.cpp 
 * 
 * Questo file implementa una skill che fa camminare un robot tra le due linee di fallo laterali 
 * (raggiunge la linea di sinistra, si gira, raggiunge quella di destra, si gira, e così via).
 * 
 * @author Giuseppe Mattia Mezzotero
 */


#include "SkillBehaviorControl.h"


option((SkillBehaviorControl) WalkBetweenTheLines, 
        vars((float)(theRobotPose.translation.x()) startX)) /** Posizione x dal quale far cominciare il cammino del robot */
{

  initial_state(walkLeft)
  {
    transition
    {
      if(theRobotPose.translation.y() >= theFieldDimensions.yPosLeftTouchline - 300.f) 
        goto turnRight;           // Raggiunto il punto dove devi arrivare, girati
    }
    action
    {
      // OUTPUT_TEXT("Walking left");       // Stampa di debug
      LookForward();
      // Il tuo target è a 90 gradi rispetto alla tua posizione e alle coordinate definite da Vector2f
      const Pose2f target(90_deg, Vector2f(startX, theFieldDimensions.yPosLeftTouchline - 300.f));
      WalkToPoint({.target = theRobotPose.inverse() * target});  // Conversione delle coordinate rispetto a quelle del robot (vedi FollowTeammate.cpp)
    }
  }

  state(turnRight)
  {
    transition
    {
      if(action_done)
        goto walkRight;           // Finito di girarti, vai verso destra
    }
    action
    {
      LookForward();
      TurnAngle({.angle = 180_deg, .margin = 5_deg});           // Girati verso destra
    }
  }

  state(walkRight)
  {
    transition
    {
      if(theRobotPose.translation.y() <= theFieldDimensions.yPosRightTouchline + 300.f)
        goto turnLeft;        // Raggiunto il punto dove devi arrivare, girati
    }   
    action
    {
      // OUTPUT_TEXT("Walking right");      // Stampa di debug
      LookForward();
      // Il tuo target è a -90 gradi rispetto alla tua posizione e alle coordinate definite da Vector2f
      const Pose2f target(-90_deg, Vector2f(startX, theFieldDimensions.yPosRightTouchline + 300.f));
      WalkToPoint({.target = theRobotPose.inverse() * target});
    }
  }

  state(turnLeft)
  {
    transition
    {
      if(action_done)
        goto walkLeft;
    }
    action
    {
      LookForward();
      TurnAngle({.angle = 180_deg, .margin = 5_deg});
    }
  }
}