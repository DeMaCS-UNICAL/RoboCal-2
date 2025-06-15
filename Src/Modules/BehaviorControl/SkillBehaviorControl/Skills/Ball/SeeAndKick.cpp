/**
 * @file FollowAndKickBall.cpp
 *
 * Questo file implementa una skill che fa seguire al robot la palla e poi la calcia.
 *
 * @author Giuseppe Mattia Mezzotero
 */

#include "SkillBehaviorControl.h"

option((SkillBehaviorControl) FollowAndKickBall)
{
  initial_state(followBall)
  {
    transition
    {
      if(theFieldBall.positionRelative.norm() < 300.f) // se la palla è abbatanza vicina, calciala
        goto kick;
    }
    action
    {
      WalkToPoint({.target = theFieldBall.positionRelative});       // cammina verso la palla
      LookAtBall();     // Guarda la palla
      
    }
  }

  target_state(kick)
  {
    transition
    {
      if(state_time > 1000) // dopo 1 secondo va a riseguire la palla
        goto followBall;
    }
    action
    {
      LookAtBall();             
      WalkToBallAndKick({.targetDirection = 0_deg,
                         .kickType = KickInfo::walkForwardsRight,
                         .alignPrecisely = KickPrecision::notPrecise});         // cammina verso la palla e calciala
    }
  }
}
