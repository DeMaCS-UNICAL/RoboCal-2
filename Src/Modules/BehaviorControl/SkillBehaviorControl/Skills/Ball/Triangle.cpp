/**
 * @file Triangle._cpp
 * 
 * Questo file implementa una skill che fa passare la palla in maniera triangolare 
 * tra i giocatori della stessa squadra.
 * 
 * @author Giuseppe Mattia Mezzotero
 */

#include "SkillBehaviorControl.h"

option((SkillBehaviorControl) Triangle){
    initial_state(pass){
        transition{
            if(theFieldBall.recentBallPositionOnField().x() > theFieldDimensions.xPosOpponentPenaltyArea && std::abs(theFieldBall.recentBallPositionOnField().y()) < theFieldDimensions.yPosLeftPenaltyArea)
                goto kick;  // Se la palla è nell'area di rigore, calciala
        }
        action{
            //   OUTPUT_TEXT("Entered");        // Stampa di debug

            // theTeamData.teammates è un vettore che salva tutti i compagni di squadra con il quale un robot
            // sta comunicando al momento. mates serve allo stesso scopo, ma è usato perché theTeamData.teammates non è
            // ordinato, mentre mates lo sarà più avanti
            std::vector<const Teammate*> mates;
            for(const Teammate& t : theTeamData.teammates)
                if(t.number != theGameState.playerNumber) // Escludi me stesso
                    mates.push_back(&t);

            // Ordina i giocatori in modo crescente in base alla distanza dalla palla
            std::sort(mates.begin(), mates.end(), [&](const Teammate* a, const Teammate* b){
                return (a->theRobotPose.translation - theFieldBall.recentBallPositionOnField()).squaredNorm() <
                    (b->theRobotPose.translation - theFieldBall.recentBallPositionOnField()).squaredNorm();
            });

            // Prendi i giocatori più vicini, e calcola a chi passarla in base alla distanza maggiore verso a dove ti trovi
            const Teammate* mate1 = mates[0];
            const Teammate* mate2 = mates[1];
            const int target = mate1->theRobotPose.translation.x() > mate2->theRobotPose.translation.x() ? mate1->number : mate2->number;

            // OUTPUT_TEXT("I'm robot "<< theGameState.playerNumber <<" and I'm passing to teammate "<< target);     // Stampa di debug
            PassToTeammate({.playerNumber = target});     // Passa la palla al target calcolato prima
        }
    }

    target_state(kick){
        action{
            // OUTPUT_TEXT("I'm robot "<< theGameState.playerNumber <<" and I'm kicking at goal");     // Stampa di debug
            KickAtGoal();
        }
    }
}