/**
 *  @file FollowTeammate.cpp
 * 
 * Questo file implementa una skill che, dato un numero di maglia di un giocatore, viene fatto seguire.
 * 
 * @author Giuseppe Mattia Mezzotero
 */

#include "SkillBehaviorControl.h"

option((SkillBehaviorControl) FollowTeammate, 
        vars((const GlobalTeammatesModel::TeammateEstimate*)(nullptr) teammate,     // Utilizza le variabili teammate (il compagno di squadra)
             (Vector2f)(Vector2f::Zero()) teammatePosition)){       // e teammatePosition (la posizione del compagno di squadra)


    // Con questa funzione, assegna alle variabili teammate e teammatePosition, rispettivamente, 
    // il compagno di squadra e la sua posizione
    // La funzione è quasi identica a quella in PassToTeammate.cpp alla riga 101
    const auto setTeammate = [&]
    {
    teammate = nullptr;
    teammatePosition = Vector2f::Zero();
    for(const auto& t : theGlobalTeammatesModel.teammates)
        {
            if(t.playerNumber == 2)         // Cambiare il numero di maglia del giocatore da seguire qui
            {
                teammate = &t;
                teammatePosition = teammate->pose.translation;
                break;
            }
        }
    };

    
    initial_state(follow){
        action{
            setTeammate();      // Assegna i valori alle variabili

            // Stampe di debug
            // if(teammate)
            //     OUTPUT_TEXT("Teammate found. I'm robot 5 and I'm looking at robot "<< teammate->playerNumber);
            // else
            //     OUTPUT_TEXT("Teammate NOT found");


            // Essendo teammatePosition in coordinate globali (cioè, le cordinate del campo), bisogna trasformarle
            // in coordinate relative al robot. Quindi, .inverse() trasforma la posizione del robot 
            // e usa l'operatore * con la posizione del compagno, in modo tale da calcolare
            // la teammatePosition rispetto a dove si trova il robot di partenza.
            Vector2f target = theRobotPose.inverse() * teammatePosition;
            const Vector3f target3D = (Vector3f() << target, 0.f).finished();       // Coordinate in 3D per far capire al robot dove guardare

            // Stampe di debug
            // OUTPUT_TEXT("My pose: x = " << theRobotPose.translation.x()<< ", y = " << theRobotPose.translation.y());
            // OUTPUT_TEXT("Ball relative: x = " << theFieldBall.positionRelative.x()<< ", y = " << theFieldBall.positionRelative.y());
            // OUTPUT_TEXT("Walking towards position (x=" << target3D.x() << ", y=" << target3D.y() << ", z=" << target3D.z() << ")");


            LookAtPoint({.target = target3D});      // Guarda il tuo compagno
            WalkToPoint({.target = target});        // Cammina verso il tuo compagno
        }
    }
}