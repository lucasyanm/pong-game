#include "../include/game.h"
#include <algorithm>

#pragma region Global Menu Var
GLOBALVAR MenuButton singlePlayerButton (
    -80.f, 
    -5.f, 
    .3f,
    .3f,
    "SinglePlayer", 
    true);
GLOBALVAR MenuButton localMultiplayerButton {
    10.f, 
    -5.f, 
    .3f,
    .3f,
    "Local Multiplayer"};

GLOBALVAR auto gameMode = GameMode::SINGLEPLAYER;
GLOBALVAR auto currentScreen = CurrentScreen::MAIN_MENU;
#pragma endregion

#pragma region Global Game Var
//Player 1
GLOBALVAR Player playerRight { 
    80.f, 
    2.5f,
    12.f,
    10.f, 
    arenaHalfSizeHeight - 10.f };
//Player 2
GLOBALVAR Player playerLeft {
    -80.f,
    2.5f,
    12.f,
    -10.f, 
    arenaHalfSizeHeight - 10.f};

GLOBALVAR Ball ball (
    130.f,
    0.f
);
#pragma endregion


void calculatePlayerPosition(
    Player& player, 
    const float& deltaTimeInSeconds
) {
    player.derivativeDerivativePositionY -= player.derivativePositionY * 10.f;

    player.positionY = player.positionY + player.derivativePositionY * deltaTimeInSeconds + player.derivativeDerivativePositionY * deltaTimeInSeconds * deltaTimeInSeconds * .5f;
    player.derivativePositionY = player.derivativePositionY + player.derivativeDerivativePositionY * deltaTimeInSeconds;

    if(player.positionY + player.halfHeight > arenaHalfSizeHeight) {
        player.positionY = arenaHalfSizeHeight - player.halfHeight;
        player.derivativePositionY = 0.f;
    }
    else if (player.positionY - player.halfHeight < -arenaHalfSizeHeight) {
        player.positionY = -arenaHalfSizeHeight + player.halfHeight;
        player.derivativePositionY = 0.f;
    }
}

bool checkBallCollisionWithPlayer(
    Player& player
) {
    return // checking X axis
        ball.positionX + ball.halfWidth > player.positionX - player.halfWidth 
        && ball.positionX - ball.halfWidth < player.positionX + player.halfWidth
        // checking Y axis
        && ball.positionY + ball.halfHeight > player.positionY - player.halfHeight 
        && ball.positionY - ball.halfHeight < player.positionY + player.halfHeight;
}

void calculateBallVelocityBasedOnPlayer(
    Player& player
) {
    ball.derivativePositionX *= -1.f;

    ball.derivativePositionY = 
        (ball.positionY - player.positionY) * 2 //based on ball position relative to player
        + player.derivativePositionY * .75f;
}

// BUG: Delay to start player movement in 50% keyboard
// BUG: Not capturing properly keyboard press only
void simulateGame(
    const Input& input, 
    const float& deltaTimeInSeconds,
    RenderState& renderState
) {
    //background
    clearScreen(renderState, MAIN_COLOR);
    renderRect(
        renderState, 
        0, 0, 
        arenaHalfSizeWidth, 
        arenaHalfSizeHeight, 
        SECONDARY_COLOR);

    switch (currentScreen)
    {
        case MAIN_MENU: {
            if(isPressed(KeyboardButton::RIGHT)) {
                gameMode = LOCAL_MULTIPLAYER;
            }
            else if(isPressed(KeyboardButton::LEFT)) {
                gameMode = SINGLEPLAYER;
            }
            // HACK: Not recozing enter press only
            else if (isHold(KeyboardButton::ENTER))
            {  
                currentScreen = GAME;
            }

            renderLetterCharacter(
                renderState,
                "CLASSIC PONG GAME",
                -55.f,
                30.f,
                .5f,
                .5f,
                MAIN_COLOR);

            renderLetterCharacter(
                renderState, 
                singlePlayerButton.text, 
                singlePlayerButton.positionX,
                singlePlayerButton.positionY, 
                singlePlayerButton.halfWidth, 
                singlePlayerButton.halfHeight, 
                gameMode == GameMode::SINGLEPLAYER 
                     ? MAIN_COLOR
                     : MENU_NOT_SELECTED_COLOR);   
            
            renderLetterCharacter(
                renderState, 
                localMultiplayerButton.text, 
                localMultiplayerButton.positionX, 
                localMultiplayerButton.positionY, 
                localMultiplayerButton.halfWidth, 
                localMultiplayerButton.halfHeight, 
                gameMode == GameMode::LOCAL_MULTIPLAYER
                     ? MAIN_COLOR
                     : MENU_NOT_SELECTED_COLOR);   
        }
            break;
        case GAME: {
            #pragma region Movment
            switch (gameMode)
            {
                case SINGLEPLAYER:
                    playerRight.derivativeDerivativePositionY = std::max(
                    -1300.f,
                    std::min(
                        1300.f, 
                        (ball.positionY - playerRight.positionY) * 100.f)
                    );
                    break;
                case LOCAL_MULTIPLAYER:
                    playerRight.derivativeDerivativePositionY = 0.f;
                    if (isHold(KeyboardButton::DOWN))
                    {
                        playerRight.derivativeDerivativePositionY -= 2000;
                    };
                    if (isHold(KeyboardButton::UP)) {
                        playerRight.derivativeDerivativePositionY += 2000;
                    };
                    break;
                default:
                    break;
            }

            playerLeft.derivativeDerivativePositionY = 0.f;
            if (isHold(KeyboardButton::S))
            {
                playerLeft.derivativeDerivativePositionY -= 2000;
            };
            if (isHold(KeyboardButton::W)) {
                playerLeft.derivativeDerivativePositionY += 2000;
            };

            calculatePlayerPosition(playerRight, deltaTimeInSeconds);
            calculatePlayerPosition(playerLeft, deltaTimeInSeconds);
            #pragma endregion

            #pragma region Calculate Ball Pos
            {
                ball.positionY += ball.derivativePositionY * deltaTimeInSeconds;
                ball.positionX += ball.derivativePositionX * deltaTimeInSeconds;

                #pragma region Player

                //checking player right collision
                if (checkBallCollisionWithPlayer(playerRight)) {
                    ball.positionX = playerRight.positionX - playerRight.halfWidth - ball.halfWidth;
                    calculateBallVelocityBasedOnPlayer(playerRight);
                } 
                //checking player left collision
                else if (checkBallCollisionWithPlayer(playerLeft)) {
                    ball.positionX = playerLeft.positionX + playerLeft.halfWidth + ball.halfWidth;
                    calculateBallVelocityBasedOnPlayer(playerLeft);
                }

                #pragma endregion

                #pragma region Arena

                //checking arena axis Y collision
                if (ball.positionY + ball.halfHeight > arenaHalfSizeHeight) {
                    ball.positionY = arenaHalfSizeHeight - ball.halfHeight;
                    ball.derivativePositionY *= -1.f;
                }
                else if (ball.positionY - ball.halfHeight < -arenaHalfSizeHeight) {
                    ball.positionY = -arenaHalfSizeHeight + ball.halfHeight;
                    ball.derivativePositionY *= -1.f;
                }

                //checking arena axis X collision

                //right collision
                if(ball.positionX + ball.halfWidth > arenaHalfSizeWidth) {
                    ball.positionX = 0.f;
                    ball.positionY = 0.f;
                    ball.derivativePositionX *= -1.f;
                    ball.derivativePositionY = 0.f;

                    playerLeft.score.points++;
                }
                //left collision
                else if (ball.positionX - ball.halfWidth < -arenaHalfSizeWidth) {
                    ball.positionX = 0.f;
                    ball.positionY = 0.f;
                    ball.derivativePositionX *= -1.f;
                    ball.derivativePositionY = 0.f;

                    playerRight.score.points++;
                }

                #pragma endregion
            }
            #pragma endregion

            #pragma region Render Objects
            renderNumberCharacter(
                renderState, 
                playerLeft.score.points,
                playerLeft.score.positionX, 
                playerLeft.score.positionY, 
                playerLeft.score.halfWidth * 2, 
                playerLeft.score.halfHeight * 2, 
                MAIN_COLOR
            );
            renderNumberCharacter(
                renderState, 
                playerRight.score.points,
                playerRight.score.positionX, 
                playerRight.score.positionY, 
                playerRight.score.halfWidth * 2, 
                playerRight.score.halfHeight * 2, 
                MAIN_COLOR
            );

            renderRect(
                renderState, 
                ball.positionX, 
                ball.positionY, 
                ball.halfWidth, 
                ball.halfHeight, 
                MAIN_COLOR);
            renderRect(
                renderState, 
                playerLeft.positionX, 
                playerLeft.positionY, 
                playerLeft.halfWidth, 
                playerLeft.halfHeight, 
                MAIN_COLOR);
            renderRect(
                renderState, 
                playerRight.positionX, 
                playerRight.positionY, 
                playerRight.halfWidth, 
                playerLeft.halfHeight, 
                MAIN_COLOR);
            #pragma endregion
        }
            break;
        default:
            break;
    }


       
}


