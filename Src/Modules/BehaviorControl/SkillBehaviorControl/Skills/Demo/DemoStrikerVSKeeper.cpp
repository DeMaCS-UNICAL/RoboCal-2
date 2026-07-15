/**
   * Questo file implementa lo skill DemoStrikerVSKeeper.
   * Due robot si affrontano: uno attacca e uno difende.
   * L'attaccante va sul pallone e tira in porta.
   * Il portiere para o si sposta per intercettare il tiro.
   *
   * @author angelopaldino
   */

  #include "SkillBehaviorControl.h"
  #include "Tools/BehaviorControl/Interception.h"

  option((SkillBehaviorControl) DemoStrikerVSKeeper,
         defs((bool) (true) allowDive,        /**< Il portiere si tuffa. */
              (float) (3000.f) reactRange),   /**< Distanza (mm) entro cui il portiere reagisce al pallone. */
         vars((bool) (false) saveAnnounced,   /**< Ha gia' annunciato "Parata!" in questo intervento? */
              (bool) (false) goalAnnounced))  /**< Ha gia' annunciato "Gol!"? */
  {
    // scelta del ruolo alla partenza:
    initial_state(chosenRole)
    {
      transition
      {
        if(theGameState.isGoalkeeper())
          goto keeperWait;
        else
          goto striker;
      }
    }

    // =================== ATTACCANTE ===================
    state(striker)
    {
      transition
      {
        // Appena ha calciato -> smetti (non ricalciare, non ricercare la palla).
        if(theFrameInfo.getTimeSince(theMotionInfo.lastKickTimestamp) < 2000)
          goto strikerDone;
      }
      action
      {
        // Annuncia "tiro" quando parte un calcio
        if(theFrameInfo.getTimeSince(theMotionInfo.lastKickTimestamp) < 100)
          Say({.text = "Tiro!"});
        LookAtBall();
        // Cammina verso il pallone e calcia in porta.
        KickAtGoal();
      }
    }

    // Dopo il tiro l'attaccante resta fermo.
    state(strikerDone)
    {
      action
      {
        LookAtBall();
        Stand();
      }
    }

    // =================== PORTIERE ===================
    state(keeperWait)
    {
      transition
      {
        // Reagisce se il pallone e' visto, intercettabile ed entro il raggio d'azione.
        if(theFieldBall.ballWasSeen(500) &&
           theFieldInterceptBall.interceptBall &&
           theFieldBall.positionRelative.squaredNorm() < sqr(reactRange))
        {
          saveAnnounced = false;
          goto keeperSave;
        }
      }
      action
      {
        goalAnnounced = false;
        LookAtBall();   // traccia la palla per percepirla bene
        KeyFrameArms({.motion = ArmKeyFrameRequest::keeperStand});
        Stand({.energySavingWalk = false});
      }
    }

    // interviene sul pallone (stand/walk/tuffo)
    state(keeperSave)
    {
      transition
      {
        // pallone gia' entrato in porta? annuncia goal e torna in attesa
        if(theFieldBall.ballWasSeen(300) &&
           theFieldBall.positionOnField.x() < theFieldDimensions.xPosOwnGoalLine)
          goto goal;
        // intervento concluso o pallone non piu' intercettabile -> ritorna in attesa
        if(action_done || !theFieldInterceptBall.interceptBall)
          goto keeperWait;
      }
      action
      {
        // annuncia "parata" all'inizio dell'intervento
        if(!saveAnnounced)
        {
          Say({.text = "Parata"});
          saveAnnounced = true;
        }
        // Dentro l'area: SOLO tuffi (tuffo garantito). Fuori area: fermo/passo laterale.
        unsigned methods;
        if(theLibPosition.isInOwnPenaltyArea(theRobotPose.translation))
          methods = bit(Interception::jumpLeft) | bit(Interception::jumpRight);
        else
          methods = bit(Interception::stand) | bit(Interception::walk);

        InterceptBall({.interceptionMethods = methods,
                       .allowGetUp = true,
                       .allowDive = allowDive});  // allowDive=false -> annuncia soltanto, non si tuffa
      }
    }

    // il pallone e' entrato: annuncia "goal"
    state(goal)
    {
      transition
      {
        if(state_time > 2000)
          goto keeperWait;
      }
      action
      {
        if(!goalAnnounced)
        {
          Say({.text = "Gol"});
          goalAnnounced = true;
        }
        LookAtBall();
        Stand({.energySavingWalk = false});
      }
    }
  }
