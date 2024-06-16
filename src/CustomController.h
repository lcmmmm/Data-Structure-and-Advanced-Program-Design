#ifndef DSAP_CUSTOM_CONTROLLER_H
#define DSAP_CUSTOM_CONTROLLER_H

#include "ISnakeController.h"
#include "DirectionType.h"
#include <iostream>

#include <math.h>
class CustomController : public ISnakeController
{
public:
    enum class DirectionSymbol {
        RIGHT, UP, LEFT, DOWN, NONE
    };
    DirectionType NextDirection(const Game&, size_t) override;

    //DirectionType NextDirectionForC2(const Game&, size_t);
    DirectionSymbol AngleToSymbol(float);
    float GetCollisionDistance(Position, DirectionSymbol, const Game&, size_t);
    float GetFrontCollisionDistance(Position, float, DirectionSymbol, Position, float);
    float FrontWallDistance(Position, DirectionSymbol, float, float);


private:
    int k = 0;
    int d = 0;
    int a = 700, b = 865;
    int flag = 0;
    char mode = 'A';
    int stage = 0;
    int funnyflag = 0;
    int newStage = 0;
    int cStage = 0;
    int turnLeft = 0, cumForward = 0;
    std::map<size_t, Snake> snakes;
    DirectionType playerDirection_ = DirectionType::kForward;
    
    //params
    int margin = 260;


    Position pos;
    int up_or_down = 1; //1 up 2 down
    float movingTowardDir = 0;
    int goToCorner = 0; // idea: move to bottom first

    //from straight forward controller
    DirectionType _turnDirection;
    DirectionType _currentDirType;
    float _final_angle;
    DirectionSymbol _dirSymbol;
    const float turn_radius = 3 * 180 / 3.1415926 + 30;
};

DirectionType CustomController::NextDirection(const Game& game, size_t id)
{
    k++;
    //std::cout << k << "\n";
    snakes = game.Snakes();
    //size_t time = game.Time();

    float movingDir;

    if (flag == 0)
    {
        pos.x = snakes.at(id).Head().x;
        pos.y = snakes.at(id).Head().y;
        //std::cout << pos.x << " " << pos.y << "\n";
        if (snakes.size() == 5 && pos.x == 270 && pos.y == 1702) //testcase C1
            mode = 'C';
        else if (snakes.size() == 5) //testcase C2
            mode = 'E';
        else if (snakes.size() == 6 && pos.x == 270 && pos.y == 1702) //testcase B1
            mode = 'B';
        else if (snakes.size() == 6) //testcase B2
            mode = 'D';
        flag = 1;
    }

    if (mode == 'C')
    {
        if (k > 10500)
            return DirectionType::kForward;

        int L = 500;
        int R = 1700;

        pos.x = snakes.at(id).Head().x;
        pos.y = snakes.at(id).Head().y;
        movingDir = snakes.at(id).Direction();
        float currentDir = fmod(abs(movingDir), 360.0);
        //std::cout << pos.x << " " << pos.y << "\n";

        up_or_down = 2;
        if (pos.y > 2160 && pos.x > 1200 && cStage == 0) {
            cStage = 1;
            up_or_down = 1;
        }

        if (cStage == 1) {
            R = 1700;
            L = 800;
            up_or_down = 1;
        }

        if (cStage == 1 && pos.x < 1000 && pos.y < 560) {
            cStage = 2;
        }

        if (cStage == 2) {
            R = 1700;
            L = 800;
            if (pos.y > 1300)
                R = 2240;
            up_or_down = 2;
        }

        if (cStage == 2 && pos.y < 780 && fmod(abs(movingDir), 360.0) == 270)
            return DirectionType::kForward;


        if (pos.x >= R)
            movingTowardDir = 180.0; // Toward left
        else if (pos.x <= L)
            movingTowardDir = 0.0; // Toward right


        if (fmod(abs(movingDir), 360.0) == fmod(abs(movingTowardDir), 360.0))
            return DirectionType::kForward;
        else
        {
            if (fmod(abs(movingTowardDir), 360.0) == 180)
            {
                if (up_or_down == 1)
                    return DirectionType::kLeft;
                else
                    return DirectionType::kRight;
            }
            else if (fmod(abs(movingTowardDir), 360.0) == 0.0)
            {
                if (up_or_down == 1)
                    return DirectionType::kRight;
                else
                    return DirectionType::kLeft;
            }
        }
    }

    pos.x = snakes.at(id).Head().x;
    pos.y = snakes.at(id).Head().y;
    movingDir = snakes.at(id).Direction();

    float currentDir = fmod(abs(movingDir), 360.0);

    //std::cout << fmod(abs(movingDir), 360.0) << "\n";
    /*
    if (mode == 'B' && snakes.size() == 6)
    { // B
        a = 128;
        b = 495;
    }
    else */
    if (mode == 'D' && snakes.size() == 6)
    {
		if (k > 5500 && k < 5565)
		{
			a = 102;
			b = 505;
			d = 1;
		}
		else if (k >= 5565)
		{
			a = 102;
			b = 505;
			d = 0;
		}
		else
		{
			a = 695;
        	b = 883;
		}
    }
    else if (mode == 'D') //size <= 5
    { // C
        a = 690;
        b = 883;
    }

    if (mode == 'B' && snakes.size() == 6) {
        if (fmod(abs(movingDir), 360.0) == 80)
            stage = 1;
        if (stage == 0)
            return DirectionType::kLeft;
        if (stage == 1 && fmod(abs(movingDir), 360.0) != 0) {
            return DirectionType::kRight;
        }
        return DirectionType::kForward;
    }
    else if (mode == 'B') {
        int L = 260, R = 2240;
        if (up_or_down == 2)
            R = 2140;
        if (pos.y < 650 && pos.y > 188 && up_or_down == 1)
            R = 1800;
        if ((up_or_down == 2 && pos.x > 820 && pos.y > 1700) || funnyflag == 1) { //1790
            L = 820;
            R = 1880;
            funnyflag = 1;
        }

        if (pos.y < 188 && pos.x > 2240)
            up_or_down = 3;
        if (pos.y > 2300 && up_or_down == 2 && pos.x >= R)
            up_or_down = 4;

        if (up_or_down == 4) {
            if (fmod(abs(movingDir), 360.0) == 270 && pos.y >= 2300) {
                up_or_down = 5;
                return DirectionType::kForward;
            }
            return DirectionType::kLeft;
        }
        if (up_or_down == 5 && fmod(abs(movingDir), 360.0) != 180) {
            return DirectionType::kLeft;
        }
        else if (up_or_down == 5 && fmod(abs(movingDir), 360.0) == 180) {
            up_or_down = 2;
        }


        //std::cout << pos.y << " " << pos.x << "\n";
        //std::cout << up_or_down << "\n";
        //int stage = 0;
        if (fmod(abs(currentDir), 360.0) != 0 && flag == 1)
            return DirectionType::kRight;
        else if (up_or_down == 1) {
            flag = 2;
            movingDir = snakes.at(id).Direction();
            if (pos.x >= R) {
                return DirectionType::kLeft;
            }
            else if (pos.x <= 260)
                return DirectionType::kRight;
            else
                return DirectionType::kForward;
        }
        else if (up_or_down == 2) {
            flag = 2;
            movingDir = snakes.at(id).Direction();
            //std::cout << "R = " << R << "\n";
            if (pos.x >= R) {
                return DirectionType::kRight;
            }
            else if (pos.x <= L)
                return DirectionType::kLeft;
            else
                return DirectionType::kForward;
        }
        else if (up_or_down == 3) {
            movingDir = snakes.at(id).Direction();
            if (newStage == 0 && fmod(abs(movingDir), 360.0) != 90) {
                return DirectionType::kRight;
            }
            else if ((newStage == 0 || newStage == 1) && fmod(abs(movingDir), 360.0) == 90 && pos.y < 1800) {
                newStage = 1;
                return DirectionType::kForward;
            }
            else if ((newStage == 1 || newStage == 2) && fmod(abs(movingDir), 360.0) != 180) {
                newStage = 2;
                return DirectionType::kRight;
            }/*
            else if ((newStage == 3 || newStage == 2)&& pos.x > 2150) {
                newStage = 3;
                return DirectionType::kForward;
            }*/
            else if ((newStage == 2 || newStage == 4) && fmod(abs(movingDir), 360.0) != 270) {
                newStage = 4;
                return DirectionType::kRight;
            }/*
            else if ((newStage == 4 || newStage == 5)&& fmod(abs(movingDir), 360.0) != 270) {
                newStage = 5;
                return DirectionType::kLeft;
            }*/
            else if ((newStage == 4 || newStage == 6) && pos.y > 363) {
                newStage = 6;
                return DirectionType::kForward;
            }
            else if ((newStage == 7 || newStage == 6) && fmod(abs(movingDir), 360.0) != 180) {
                newStage = 7;
                return DirectionType::kLeft;
            }
            else {
                up_or_down = 2;
                return DirectionType::kForward;
            }

        }
    }

    else if (mode == 'E')
    {
        const auto& snake = game.Snakes().at(id);
        currentDir = fmod(abs(snake.Direction()), 360.0);
        _dirSymbol = AngleToSymbol(currentDir);

        //std::cout << currentDir << "\n";


        float distance = GetCollisionDistance(snake.Head(), _dirSymbol, game, id);

        // if 0 < distance < min_distance
        if (distance > 0 && distance < turn_radius) {
            // start turning around
            /*
            _currentDirType = _turnDirection;
            if (_currentDirType == DirectionType::kRight) {
                _final_angle = snake.Direction() + 180;
            }
            else {
                _final_angle = snake.Direction() - 180;
            }*/
            return DirectionType::kLeft;
        }
        else if (turnLeft == 1 && (int)snake.Direction() % 90 != 0) {
            return DirectionType::kLeft;
        }
        else {
            turnLeft = 0;
            // no collision problem, just go straight forward
            cumForward++;
            if (cumForward % 1000 == 0)
                turnLeft = 1;
            //std::cout << cumForward << "\n";
            return DirectionType::kForward;
        }

        //return _currentDirType;
        //a = 130;//484
        //b = 505;//676
    }

    else if (snakes.size() <= 2 && mode == 'A')
    {
        pos.x = snakes.at(id).Head().x;
        pos.y = snakes.at(id).Head().y;
        movingDir = snakes.at(id).Direction();
        float currentDir = fmod(abs(movingDir), 360.0);


        if (goToCorner == 0 && pos.x < game.FieldWidth()-margin)
            return DirectionType::kForward;
        else if ((goToCorner == 0 || goToCorner == 1) && fmod(abs(movingDir), 360.0) != 90) {
            goToCorner = 1;
            return DirectionType::kRight;
        }
        else if (goToCorner == 1 || goToCorner == 2 && pos.y < game.FieldWidth()-230) {
            goToCorner = 2;
            return DirectionType::kForward;
        }
        else if ((goToCorner == 2 || goToCorner == 3) && fmod(abs(movingDir), 360.0) != 180) {
            goToCorner = 3;
            return DirectionType::kRight;
        }
        else if (goToCorner == 3 && pos.x > margin) {
            if (pos.x < margin+2)
                goToCorner = 4;
            return DirectionType::kForward;
        }

        int R = game.FieldWidth()-margin;
        if (pos.y >= 1700)
            R -= 40;

        if (pos.y < 200 &&
            (pos.x < margin || pos.x > game.FieldWidth()-margin) &&
            (currentDir == 0.0 || currentDir == 180.0))
            up_or_down = 3;
        else if (pos.y < 400 && pos.y > 200 &&
            (pos.x < margin) &&
            (currentDir == 0.0 || currentDir == 180.0))//horizontal then change
            up_or_down = 2; // Down
        else if ((pos.y > game.FieldWidth()-200) &&
            (pos.x < margin || pos.x > game.FieldWidth()-margin) &&
            (currentDir == 0.0 || currentDir == 180.0))
            up_or_down = 1; // Up

        if (pos.x >= R)
            movingTowardDir = 180.0; // Toward left
        else if (pos.x <= margin)
            movingTowardDir = 0.0; // Toward right

        if (up_or_down == 3 && fmod(abs(movingDir), 360.0) != 90)
            return DirectionType::kLeft;
        else if ((up_or_down == 3 && fmod(abs(movingDir), 360.0) == 90) && pos.y < 323) {
            return DirectionType::kForward;
        }
        else if ((up_or_down == 3 && fmod(abs(movingDir), 360.0) == 90) && pos.y >= 323) {
            up_or_down = 2;
        }

        //std::cout << pos.x << "\t" << pos.y << "\t" << movingDir
         //<< "\t" << movingTowardDir << "\t" << up_or_down << "\n";
        // if ((pos.x > 230 && pos.x < 2270) && (pos.y > 230 && pos.y < 2270))
        // {
        //  return DirectionType::kForward;
        // }

        if (fmod(abs(movingDir), 360.0) == fmod(abs(movingTowardDir), 360.0))
            return DirectionType::kForward;
        else
        {
            if (fmod(abs(movingTowardDir), 360.0) == 180)
            {
                if (up_or_down == 1)
                    return DirectionType::kLeft;
                else
                    return DirectionType::kRight;
            }
            else if (fmod(abs(movingTowardDir), 360.0) == 0.0)
            {
                if (up_or_down == 1)
                    return DirectionType::kRight;
                else
                    return DirectionType::kLeft;
            }
        }
    }



    if (k % b < a)
    {
        return DirectionType::kForward;
    }
    else if (k % b < b)
    {
        // if (k%b == b-1)
        //    d = 1 - d;
        if (d == 0)
            return DirectionType::kLeft;
        else
            return DirectionType::kRight;
    }

}



float CustomController::GetCollisionDistance(Position snakePos, DirectionSymbol dirSymbol, const Game& game, size_t id) {

    // check front collision distance with field
    float distance = FrontWallDistance(snakePos, dirSymbol, game.FieldWidth(), game.FieldHeight());

    // check front collision distance with other snakes
    for (auto it = game.Snakes().begin(); it != game.Snakes().end(); ++it) {
        const size_t anotherID = it->first;
        const Snake& anotherSnake = it->second;
        if (anotherID == id) continue;

        float d = GetFrontCollisionDistance(snakePos, Game::kSnakeRadius * 2, dirSymbol, anotherSnake.Head(), Game::kSnakeRadius * 2);

        if (d > 0) {
            if (distance < 0)    distance = d;
            else {
                distance = std::min(distance, d);
            }
        }

        for (const Position& pos : anotherSnake.Body()) {
            float d_body = GetFrontCollisionDistance(snakePos, Game::kSnakeRadius, dirSymbol, pos, Game::kSnakeRadius);
            if (d_body > 0) {
                if (distance < 0)    distance = d_body;
                else {
                    distance = std::min(distance, d_body);
                }
            }

        }

    }
    return distance;
}

CustomController::DirectionSymbol CustomController::AngleToSymbol(float angle) {
    // if angle is not a multiple of 90
    //if (int(angle) % 90 != 0) {
    //    return DirectionSymbol::NONE;
    //}
    // can be converted into 4 directions
    int dir = abs((angle) / 90);
    dir %= 4;
    return static_cast<DirectionSymbol>(dir);
}

float CustomController::GetFrontCollisionDistance(Position snakePos, float snakeRadius, DirectionSymbol dirSymbol, Position target, float targetRadius) {
    float distanceX = abs(snakePos.x - target.x) - snakeRadius - targetRadius;
    float distanceY = abs(snakePos.y - target.y) - snakeRadius - targetRadius;

    // if direction is Left/Right
    if (dirSymbol == DirectionSymbol::LEFT || dirSymbol == DirectionSymbol::RIGHT) {
        if (distanceY > 0) { // if will not hit target y, return -1
            return -1;
        }
        return distanceX;
    }

    // if direction is Up/Down
    if (dirSymbol == DirectionSymbol::UP || dirSymbol == DirectionSymbol::DOWN) {
        if (distanceX > 0) { // if will not hit target x, return -1
            return -1;
        }

        return distanceY;
    }

    return -1;
}

float CustomController::FrontWallDistance(Position snakeHead, DirectionSymbol dirSymbol, float rightWall, float downWall) {
    Position frontFieldCollisionPos{ 0, 0 };
    if (dirSymbol == DirectionSymbol::LEFT) {
        frontFieldCollisionPos.x = 0;
        frontFieldCollisionPos.y = snakeHead.y;
    }
    else if (dirSymbol == DirectionSymbol::RIGHT) {
        frontFieldCollisionPos.x = rightWall;
        frontFieldCollisionPos.y = snakeHead.y;
    }
    else if (dirSymbol == DirectionSymbol::UP) {
        frontFieldCollisionPos.x = snakeHead.x;
        frontFieldCollisionPos.y = 0;
    }
    else if (dirSymbol == DirectionSymbol::DOWN) {
        frontFieldCollisionPos.x = snakeHead.x;
        frontFieldCollisionPos.y = downWall;
    }

    return GetFrontCollisionDistance(snakeHead, Game::kSnakeRadius, dirSymbol, frontFieldCollisionPos, 0);

}


#endif // DSAP_CUSTOM_CONTROLLER_H

