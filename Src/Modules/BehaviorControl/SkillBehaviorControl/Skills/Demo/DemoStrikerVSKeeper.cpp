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
              (bool) (false) goalAnnounced,   /**< Ha gia' annunciato "Gol!"? */
              (unsigned) (theMotionInfo.lastKickTimestamp) lastKickSeen))  /**< Ultimo calcio gia' visto (per rilevare un calcio NUOVO). */
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
    // Tira in porta; ma NON dentro l'area avversaria (prima porta la palla fuori).
    state(striker)
    {
      transition
      {
        // Palla dentro l'area di rigore avversaria: non si puo' tirare -> portala fuori.
        if(theLibPosition.isInOpponentPenaltyArea(theFieldBall.positionOnField))
          goto strikerClear;
        // Calcio NUOVO (il timestamp cambia) -> smetti e aspetta il ritorno palla.
        if(theMotionInfo.lastKickTimestamp != lastKickSeen)
        {
          lastKickSeen = theMotionInfo.lastKickTimestamp;
          goto strikerWait;
        }
      }
      action
      {
        LookAtBall();
        // Cammina verso il pallone e calcia nella porta avversaria.
        KickAtGoal();
      }
    }

    // Dentro l'area avversaria: dribbla la palla fuori (verso la propria meta'), poi torna a tirare.
    state(strikerClear)
    {
      transition
      {
        // Palla uscita dall'area -> riprendi a tirare (risincronizza per ignorare i dribble-kick).
        if(!theLibPosition.isInOpponentPenaltyArea(theFieldBall.positionOnField))
        {
          lastKickSeen = theMotionInfo.lastKickTimestamp;
          goto striker;
        }
      }
      action
      {
        LookAtBall();
        // Dribbla verso la propria meta' campo: angolo assoluto 180deg convertito in relativo al robot.
        GoToBallAndDribble({.targetDirection = Angle::normalize(180_deg - theRobotPose.rotation)});
      }
    }

    // Aspetta fermo finche' la palla e' nella zona del portiere.
    state(strikerWait)
    {
      transition
      {
        // Palla fuori dall'area di rigore -> il portiere ha finito di passare:
        // vai a recuperarla e ritira.
        if(state_time > 1000 &&
          theFieldBall.ballWasSeen(1000) &&
           !theLibPosition.isInOpponentPenaltyArea(theFieldBall.positionOnField))
          goto striker;
        }
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
        // pallone entrato nella porta difesa (quella avversaria, +x)? annuncia goal
        if(theFieldBall.ballWasSeen(300) &&
           theFieldBall.positionOnField.x() < theFieldDimensions.xPosOwnGoalLine)
          goto goal;
        // intervento concluso o pallone non piu' intercettabile -> va a rispedire la palla
        if(action_done)
          goto keeperReturn;
        
        if(theFieldBall.ballWasSeen(500) &&
           theFieldBall.positionRelative.norm() < 700.f)
        {
          goto keeperReturn;
        }
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

    // Dopo l'intervento: va alla palla e la rispedisce su verso l'attaccante.
    state(keeperReturn)
    {
      transition
      {
        // Palla uscita dall'area di rigore -> passaggio riuscito, torna tra i pali.
        // Se il calcio non la fa uscire, resta qui e RICALCIA (anche piu' passaggi).
        if(state_time > 1000 &&
           theFieldBall.ballWasSeen(1000) &&
           !theLibPosition.isInOwnPenaltyArea(theFieldBall.positionOnField))
          goto keeperGoHome;
        }
      action
      {
        LookAtBall();
        // Bersaglio: posizione del compagno attaccante (#5) dal modello compagni;
        // se non noto, fallback verso la propria meta' campo (-x), dove sta l'attaccante.
      Vector2f target(theFieldDimensions.xPosOpponentGoalLine, 0.f);
      /**for(const auto& mate : theGlobalTeammatesModel.teammates)
          if(mate.playerNumber == 5)
          {
            target = mate.pose.translation;
            break;
          }*/
        // Direzione assoluta verso il bersaglio, convertita in relativa al robot.
        const Angle passDir =
            Angle::normalize((target - theFieldBall.positionOnField).angle()
                              - theRobotPose.rotation);

        const bool useLeft = passDir > 0_deg;
        // Vai a palla, allineati verso il compagno e calcia (calcio lungo).
        GoToBallAndKick({
            .targetDirection = passDir,
            .kickType = useLeft ? KickInfo::walkForwardsLeftLong
                                : KickInfo::walkForwardsRightLong
        });
      }
    }

    // Torna tra i pali della porta difesa (+x), poi ridiventa pronto.
    state(keeperGoHome)
    {
      transition
      {
        // Arrivato vicino alla porta E rivolto verso il campo -> pronto ad aspettare il prossimo tiro.
        const Vector2f goalCenter(theFieldDimensions.xPosOwnGoalLine, 0.f);
        if((theRobotPose.translation - goalCenter).squaredNorm() < sqr(600.f) &&
           std::abs(theRobotPose.rotation) < 20_deg)
          goto keeperWait;
      }
      action
      {
        LookAtBall();
        const Pose2f goalPose(0_deg, theFieldDimensions.xPosOwnGoalLine + 200.f, 0.f);
        WalkToPose({.target = theRobotPose.inverse() * goalPose});
      }
    }

    // il pallone e' entrato: annuncia "goal"
    state(goal)
    {
      transition
      {
        // dopo l'annuncio va a recuperare la palla e la rispedisce su
        if(state_time > 1000)
          goto keeperGoHome;
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
