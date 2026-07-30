#include "SkillBehaviorControl.h"

option((SkillBehaviorControl) DemoFollowBall)
{
  initial_state(stand)
  {
    transition
    {
      if(state_time > 2000)
        goto searchBall;
    }
    action
    {
      Stand();
      LookForward();
    }
  }

  state(searchBall)
  {
    transition
    {
      if(theFieldBall.ballWasSeen())
        goto approachBall;
    }
    action
    {
      // Ruota sul posto lentamente (rallentato a 10 gradi/s per permettere a LookActive di non avere punti ciechi)
      WalkAtAbsoluteSpeed({.speed = {10_deg, 0.f, 0.f}});
      LookActive({.withBall = true}); // Scansiona l'ambiente con la testa
    }
  }

  state(approachBall)
  {
    transition
    {
      // Se perdiamo la palla per mezzo secondo (invece di 3), torniamo a cercarla!
      if(!theFieldBall.ballWasSeen(500))
        goto searchBall;
      
      // Se la distanza è minore di 300mm (30 cm), ci fermiamo
      if(theBallModel.estimate.position.norm() < 300.f)
        goto done;
    }
    action
    {
      // Calcola una velocità di rotazione sicura (ridotta a max 15 gradi al secondo per la partenza)
      float targetAngle = theBallModel.estimate.position.angle();
      float rotSpeed = targetAngle;
      if(rotSpeed > 15_deg) rotSpeed = 15_deg;
      if(rotSpeed < -15_deg) rotSpeed = -15_deg;

      // Calcola la velocità di camminata (ridotta per evitare strappi in partenza)
      float dist = theBallModel.estimate.position.norm();
      float fwdSpeed = (dist > 600.f) ? 120.f : 80.f;

      WalkAtAbsoluteSpeed({.speed = {rotSpeed, fwdSpeed, 0.f}});
      
      // Usa LookActive per seguire la palla ma anche guardarsi intorno se la perde
      LookActive({.withBall = true});
    }
  }

  state(done)
  {
    transition
    {
      // Se perdiamo la palla per più di un secondo, torniamo a cercarla
      if(!theFieldBall.ballWasSeen(1000))
        goto searchBall;
      
      // Se la palla viene allontanata a più di 400mm (40 cm), riprendiamo l'inseguimento!
      if(theBallModel.estimate.position.norm() > 400.f)
        goto approachBall;
    }
    action
    {
      // Si ferma in piedi
      Stand({.high = true});
      // Continua a fissare la palla anche mentre è fermo (invece di guardare dritto)
      LookActive({.withBall = true});
    }
  }
}
