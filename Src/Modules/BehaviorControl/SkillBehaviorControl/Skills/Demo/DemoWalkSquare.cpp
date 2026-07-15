/**
 * @file DemoWalkSquare.cpp
 *
 * Questo file implementa la skill DemoWalkSquare.
 * Il robot percorre un quadrato: cammina dritto per una distanza fissa,
 * poi ruota di 90 gradi e ripete il processo fino a completare il quadrato.
 *
 * @author angelopaldino
 */

#include "SkillBehaviorControl.h"

option((SkillBehaviorControl) DemoWalkSquare,
       defs((float)(1000.f) sideLength,  /**< Lunghezza del lato in mm. */
            (Angle)(90_deg) turnAngle),  /**< Rotazione tra un lato e l'altro. */
       vars((int)(0) sidesDone,          /**< Numero di lati gia' percorsi. */
            (Vector2f)(theOdometryData.translation) startPosition)) /**< Odometria a inizio lato. */
{
  // Cammina dritto finche' non ha percorso 'sideLength' mm.
  initial_state(walkForward)
  {
    transition
    {
      if((theOdometryData.translation - startPosition).norm() >= sideLength)
        goto turn;
    }
    action
    {
      LookForward();
      // Posa relativa al robot: rotazione 0, avanti 'sideLength' mm, nessuno spostamento laterale.
      WalkToPose({.target = {0_deg, sideLength, 0.f}});
    }
  }

  // Ruota sul posto di 'turnAngle' rispetto alla rotazione di inizio stato.
  state(turn)
  {
    transition
    {
      if(action_done)
      {
        // Ogni volta che finiamo una rotazione abbiamo completato un lato.
        ++sidesDone;
        if(sidesDone >= 4)
          goto done;
        // Reset dell'odometria di riferimento per il lato successivo.
        startPosition = theOdometryData.translation;
        goto walkForward;
      }
    }
    action
    {
      LookForward();
      TurnAngle({.angle = turnAngle, .margin = 5_deg});
    }
  }

  target_state(done)
  {
    action
    {
      LookForward();
      Stand();
    }
  }
}
