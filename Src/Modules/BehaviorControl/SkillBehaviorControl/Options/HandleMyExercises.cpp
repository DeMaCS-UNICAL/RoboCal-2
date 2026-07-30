#include "SkillBehaviorControl.h"

option((SkillBehaviorControl) HandleMyExercises)
{
  initial_state(start)
  {
    transition
    {
      goto executeExercise;
    }
    action {}
  }

  state(executeExercise)
  {
    action
    {
      // Scegli quale esercizio testare decommentandolo:

      // DemoFigureEight();
      // DemoFollowBall();
      // DemoSearchForBall();
      DemoPatrol();
      // DemoGoToBallAndKick();
    }
  }
}
