#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <fstream>
#include <string>

//colors
#define YELLOW (255,255,0)
#define PURPLE (128,0,128)
#define RED (255,0,0)
#define GREEN (0,255,0)
#define CYAN (0,255,255)
#define ORANGE (255,165,0)
#define BLUE (0,0,255)

#define LINE_THICKNESS 2

int Matrix[12][22]; // position x=0, y=0, x=11, y = 21: borders 
int const midle_x = 6;
int const starting_y = 1;
double Piece_Size = 40.f;
int Score = 0;
int ScreanShiftLeft = Piece_Size * 7;
int ScreanShiftRight = Piece_Size * 7;
int Arena = Piece_Size * 10;
int windowHeight = (Piece_Size * 20)+ LINE_THICKNESS;
int windowWidth = Arena + ScreanShiftLeft + ScreanShiftRight;
sf::Time StartingFallTime = sf::seconds(0.5f);
sf::Time FallTime = StartingFallTime;
sf::Time FirstDelayTime = sf::milliseconds(200);
sf::Time HoldDelayTime = sf::milliseconds(50);

enum Names
{
    Squire,
    T,
    Z,
    S,
    L,
    RevL,
    I,
    NUM
};

void StartMatrix()
{
    for (int y = 0; y < 22; y++)
    {
        for (int x = 0; x < 12; x++)
        {
            if (y == 21 || y == 0)
            {
                Matrix[x][y] = 1;
                //std::cout << x << " " << y << "\n";
            }
            else if (x == 11 || x == 0)
            {
                Matrix[x][y] = 1;
            }
            else
            {
                Matrix[x][y] = 0;
            }
        }
    }
}

void Draw_Map(sf::RenderWindow* window) // Tetris map 10x20
{
    // Horizontal lines
    sf::RectangleShape line(sf::Vector2f((Piece_Size * 10) + LINE_THICKNESS, LINE_THICKNESS));

    for (double i = 0; i <= windowHeight; i = i + Piece_Size)
    {
        line.setPosition(sf::Vector2f(ScreanShiftLeft + 0.f, (float)i));
        window->draw(line);
    }

    // Vertical lines

    line.setSize(sf::Vector2f(LINE_THICKNESS, (Piece_Size * 20) + LINE_THICKNESS));

    for (double i = 0; i <= Arena; i = i + Piece_Size)
    {
        line.setPosition(sf::Vector2f(ScreanShiftLeft + (float)i, 0.f));
        window->draw(line);
    }
}

class Piece
{
public:
    int center_x, center_y;
    int position_index;
    int piece_X[4];
    int piece_Y[4];
    sf::Color color;
    bool Exist;

    Piece()
    {
        Exist = true;
    }

    void setStartingPosition()
    {
        center_x = midle_x;
        center_y = starting_y;

        position_index = 0;

        Rotate();
    }

    virtual void Position_1() = 0;
    virtual void Position_2() = 0;
    virtual void Position_3() = 0;
    virtual void Position_4() = 0;
    virtual ~Piece() 
    {
        delete piece_X;
        delete piece_Y;
    }

    void SelectPosition()
    {
        switch (position_index)
        {
        case 1:
            Position_1();
            break;
        case 2:
            Position_2();
            break;
        case 3:
            Position_3();
            break;
        case 4:
            Position_4();
            break;
        default:
            break;
        }
    }

    bool CheckPosition()
    {
        for (int i = 0; i < 4; i++)
        {
            if (Matrix[piece_X[i]][piece_Y[i]] != 0)
            {
                return false;
            }
        }
        return true;
    }

    void setColor(sf::Color new_color)
    {
        this->color = new_color;
    }

    bool CanFall()
   {
       for (int i = 0; i < 4; i++)
       {
           if (Matrix[piece_X[i]][piece_Y[i] + 1] != 0)
           {
               Exist = false;
               return false;
           }
       }
       return true;
   }

    void fall()
    {
        if (CanFall())
        {
            for (int i = 0; i < 4; i++)
            {
                piece_Y[i]++;
            }
            center_y++;
        }
    }

    bool CanMoveRight()
    {
        for (int i = 0; i < 4; i++)
        {
            if (Matrix[piece_X[i] + 1][piece_Y[i]] != 0)
            {
                return false;
            }
        }
        return true;
    }

    void MoveRight()
    {
        if (CanMoveRight())
        {
            for (int i = 0; i < 4; i++)
            {
                piece_X[i]++;
            }
            center_x++;
        }
    }

    bool CanMoveLeft()
    {
        for (int i = 0; i < 4; i++)
        {
            if (Matrix[piece_X[i] - 1][piece_Y[i]] != 0)
            {
                return false;
            }
        }
        return true;
    }

    void MoveLeft()
    {
        if (CanMoveLeft())
        {
            for (int i = 0; i < 4; i++)
            {
                piece_X[i]--;
            }
            center_x--;
        }
    }

    void Draw(sf::RenderWindow* window, sf::Texture texture)
    {
        for (int i = 0; i < 4; i++)
        {
            sf::RectangleShape tmp;
            tmp.setSize(sf::Vector2f(Piece_Size, Piece_Size));
            tmp.setTexture(&texture);
            tmp.setPosition(ScreanShiftLeft + ((piece_X[i] - 1 ) * Piece_Size), (piece_Y[i] - 1) * Piece_Size);
            tmp.setFillColor(color);

            window->draw(tmp);
        }
    }

    void NextPositionIndex()
    {
        if (position_index == 4)
        {
            position_index = 1;
        }
        else
        {
            position_index++;
        }
    }

    void PrevPositionIndex()
    {
        if (position_index == 1)
        {
            position_index = 4;
        }
        else
        {
            position_index--;
        }
    }

    void Rotate() // rotate right
    {
        NextPositionIndex();
        SelectPosition();

        if (!CheckPosition())
        {
            if (CanMoveLeft())
            {
                MoveLeft();
            }
            else if (CanMoveRight())
            {
                MoveRight();
            }
            else
            {
                PrevPositionIndex();
                SelectPosition();
            }

        }
    }

    void RotateLeft() // rotate left
    {
        PrevPositionIndex();
        SelectPosition();

        if (!CheckPosition())
        {
            if (CanMoveLeft())
            {
                MoveLeft();
            }
            else if (CanMoveRight())
            {
                MoveRight();
            }
            else
            {
                PrevPositionIndex();
                SelectPosition();
            }

        }
    }
};

class Squire_Piece : public Piece
{
public:

    void Position_1() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x + 1; // right 
        piece_Y[1] = center_y;

        piece_X[2] = center_x + 1; // left + down
        piece_Y[2] = center_y + 1;

        piece_X[3] = center_x; // down
        piece_Y[3] = center_y + 1;
    }

    void Position_2() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x + 1; // right 
        piece_Y[1] = center_y;

        piece_X[2] = center_x + 1; // left + down
        piece_Y[2] = center_y + 1;

        piece_X[3] = center_x; // down
        piece_Y[3] = center_y + 1;
    }

    void Position_3() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x + 1; // right 
        piece_Y[1] = center_y;

        piece_X[2] = center_x + 1; // left + down
        piece_Y[2] = center_y + 1;

        piece_X[3] = center_x; // down
        piece_Y[3] = center_y + 1;
    }

    void Position_4() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x + 1; // right 
        piece_Y[1] = center_y;

        piece_X[2] = center_x + 1; // left + down
        piece_Y[2] = center_y + 1;

        piece_X[3] = center_x; // down
        piece_Y[3] = center_y + 1;
    }

    Squire_Piece()
    {
        sf::Color new_color YELLOW;
        setColor(new_color);

        center_x = midle_x - 1;
        center_y = starting_y;

        position_index = 0;

        Rotate();
    }

};

class T_Piece : public Piece
{
public:

    void Position_1() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x + 1; // right 
        piece_Y[1] = center_y;

        piece_X[2] = center_x - 1; // left
        piece_Y[2] = center_y;

        piece_X[3] = center_x; // down
        piece_Y[3] = center_y + 1;
    }

    void Position_2() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x; // UP 
        piece_Y[1] = center_y - 1;

        piece_X[2] = center_x - 1; // left
        piece_Y[2] = center_y;

        piece_X[3] = center_x; // down
        piece_Y[3] = center_y + 1;
    }

    void Position_3() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x; // UP 
        piece_Y[1] = center_y - 1;

        piece_X[2] = center_x - 1; // left
        piece_Y[2] = center_y;

        piece_X[3] = center_x + 1; // right
        piece_Y[3] = center_y;
    }

    void Position_4() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x; // UP 
        piece_Y[1] = center_y - 1;

        piece_X[2] = center_x + 1; // right
        piece_Y[2] = center_y;

        piece_X[3] = center_x; // down
        piece_Y[3] = center_y + 1;
    }

    T_Piece()
    {
        sf::Color new_color PURPLE;
        setColor(new_color);

        center_x = midle_x;
        center_y = starting_y;
        
        position_index = 0;

        Rotate();
    }

};

class Z_Piece : public Piece
{
public:

    void Position_1() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x - 1; // left 
        piece_Y[1] = center_y;

        piece_X[2] = center_x; // down
        piece_Y[2] = center_y + 1;

        piece_X[3] = center_x + 1; // down and Right
        piece_Y[3] = center_y + 1;
    }

    void Position_2() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x; // UP 
        piece_Y[1] = center_y - 1;

        piece_X[2] = center_x - 1; // left
        piece_Y[2] = center_y;

        piece_X[3] = center_x - 1; // left + down
        piece_Y[3] = center_y + 1;
    }

    void Position_3() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x; // UP 
        piece_Y[1] = center_y - 1;

        piece_X[2] = center_x - 1; // left + UP
        piece_Y[2] = center_y - 1;

        piece_X[3] = center_x + 1; // right
        piece_Y[3] = center_y;
    }

    void Position_4() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x + 1; // UP + right
        piece_Y[1] = center_y - 1;

        piece_X[2] = center_x + 1; // right
        piece_Y[2] = center_y;

        piece_X[3] = center_x; // down
        piece_Y[3] = center_y + 1;
    }

    Z_Piece()
    {
        sf::Color new_color RED;
        setColor(new_color);

        center_x = midle_x;
        center_y = starting_y;

        position_index = 0;

        Rotate();
    }

};

class S_Piece : public Piece
{
public:

    void Position_1() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x + 1; // right 
        piece_Y[1] = center_y;

        piece_X[2] = center_x; // down
        piece_Y[2] = center_y + 1;

        piece_X[3] = center_x - 1; // down and left
        piece_Y[3] = center_y + 1;
    }

    void Position_2() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x - 1; // UP + left
        piece_Y[1] = center_y - 1;

        piece_X[2] = center_x - 1; // left
        piece_Y[2] = center_y;

        piece_X[3] = center_x; // down
        piece_Y[3] = center_y + 1;
    }

    void Position_3() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x; // UP 
        piece_Y[1] = center_y - 1;

        piece_X[2] = center_x + 1; // right + up
        piece_Y[2] = center_y - 1;

        piece_X[3] = center_x - 1; // left
        piece_Y[3] = center_y;
    }

    void Position_4() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x; // UP 
        piece_Y[1] = center_y - 1;

        piece_X[2] = center_x + 1; // right
        piece_Y[2] = center_y;

        piece_X[3] = center_x + 1; // right + down
        piece_Y[3] = center_y + 1;
    }

    S_Piece()
    {
        sf::Color new_color GREEN;
        setColor(new_color);

        center_x = midle_x;
        center_y = starting_y;

        position_index = 0;

        Rotate();
    }

};

class L_Piece : public Piece
{
public:

    void Position_1() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x + 1; // right 
        piece_Y[1] = center_y;

        piece_X[2] = center_x - 1; // left
        piece_Y[2] = center_y;

        piece_X[3] = center_x - 1; // down and left
        piece_Y[3] = center_y + 1;
    }

    void Position_2() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x - 1; // UP + left
        piece_Y[1] = center_y - 1;

        piece_X[2] = center_x; // UP
        piece_Y[2] = center_y - 1;

        piece_X[3] = center_x; // down
        piece_Y[3] = center_y + 1;
    }

    void Position_3() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x + 1; // right 
        piece_Y[1] = center_y;

        piece_X[2] = center_x - 1; // left
        piece_Y[2] = center_y;

        piece_X[3] = center_x + 1; // up + right
        piece_Y[3] = center_y - 1;
    }

    void Position_4() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x + 1; // down + right
        piece_Y[1] = center_y + 1;

        piece_X[2] = center_x; // UP
        piece_Y[2] = center_y - 1;

        piece_X[3] = center_x; // down
        piece_Y[3] = center_y + 1;
    }

    L_Piece()
    {
        sf::Color new_color ORANGE;
        setColor(new_color);

        center_x = midle_x;
        center_y = starting_y;

        position_index = 0;

        Rotate();
    }

};

class RevL_Piece : public Piece
{
public:

    void Position_1() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x + 1; // right 
        piece_Y[1] = center_y;

        piece_X[2] = center_x - 1; // left
        piece_Y[2] = center_y;

        piece_X[3] = center_x + 1; // down and right
        piece_Y[3] = center_y + 1;
    }

    void Position_2() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x - 1; // down + left
        piece_Y[1] = center_y + 1;

        piece_X[2] = center_x; // UP
        piece_Y[2] = center_y - 1;

        piece_X[3] = center_x; // down
        piece_Y[3] = center_y + 1;
    }

    void Position_3() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x + 1; // right 
        piece_Y[1] = center_y;

        piece_X[2] = center_x - 1; // left
        piece_Y[2] = center_y;

        piece_X[3] = center_x - 1; // up + left
        piece_Y[3] = center_y - 1;
    }

    void Position_4() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x + 1; // up + right
        piece_Y[1] = center_y - 1;

        piece_X[2] = center_x; // UP
        piece_Y[2] = center_y - 1;

        piece_X[3] = center_x; // down
        piece_Y[3] = center_y + 1;
    }

    RevL_Piece()
    {
        sf::Color new_color BLUE;
        setColor(new_color);

        center_x = midle_x;
        center_y = starting_y;

        position_index = 0;

        Rotate();
    }

};

class I_Piece : public Piece
{
public:

    void Position_1() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x + 1; // right 
        piece_Y[1] = center_y;

        piece_X[2] = center_x - 1; // left
        piece_Y[2] = center_y;

        piece_X[3] = center_x - 2; // left + left
        piece_Y[3] = center_y;
    }

    void Position_2() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x; // UP
        piece_Y[1] = center_y - 1;

        piece_X[2] = center_x; // down
        piece_Y[2] = center_y + 1;

        piece_X[3] = center_x; // down + down
        piece_Y[3] = center_y + 2;
    }

    void Position_3() override
    {
        piece_X[0] = center_x;
        piece_Y[0] = center_y;

        piece_X[1] = center_x + 1; // right 
        piece_Y[1] = center_y;

        piece_X[2] = center_x - 1; // left
        piece_Y[2] = center_y;

        piece_X[3] = center_x - 2; // left + left
        piece_Y[3] = center_y;
    }

    void Position_4() override
    {
        piece_X[0] = center_x - 1; //left
        piece_Y[0] = center_y;

        piece_X[1] = center_x - 1; // UP + left
        piece_Y[1] = center_y - 1;

        piece_X[2] = center_x - 1; // down + left
        piece_Y[2] = center_y + 1;

        piece_X[3] = center_x - 1; // down + down + left
        piece_Y[3] = center_y + 2;
    }

    I_Piece()
    {
        sf::Color new_color CYAN;
        setColor(new_color);

        center_x = midle_x;
        center_y = starting_y;

        position_index = 0;

        Rotate();
    }

};

Piece* Create_Piece()
{
    Piece* piece;
    int random = rand() % NUM;

    //std::cout << random << std::endl;

    switch (random)
    {
        case Squire:
            piece = new Squire_Piece;
            return piece;
        case T:
            piece = new T_Piece;
            return piece;
        case Z:
            piece = new Z_Piece;
            return piece;
        case S:
            piece = new S_Piece;
            return piece;
        case I:
            piece = new I_Piece;
            return piece;
        case L:
            piece = new L_Piece;
            return piece;
        case RevL:
            piece = new RevL_Piece;
            return piece;
    
    default:
        break;
    }
}

class Memory
{
public:
    int x, y;
    sf::Color color;
    Memory* next;

    Memory()
    {
        next = NULL;
    }

    void SetMemory(int new_x, int new_y, sf::Color new_color)
    {
        x = new_x;
        y = new_y;
        color = new_color;
    }

    Memory* ChangeMemory(Memory* first, int cor_x, int cor_y, int change_Y_By)
    {
        Memory* tmp;
        tmp = first;
        while (tmp != NULL)
        {
            if(tmp->x == cor_x && tmp->y == cor_y)
            {
                tmp->y = tmp->y + change_Y_By;
                return first;
            }
            else
            {
                tmp = tmp->next;
            }
        }
        return first;
    }

    Memory* RemoveFromMemory(Memory* first, int cor_x, int cor_y)
    {
        Memory* tmp_prev;
        Memory* tmp;
        tmp = first;
        tmp_prev = first;
        
        if (first->x == cor_x && first->y == cor_y)
        {
            first = first->next;
            delete tmp_prev;
            return first;
        }
        else
        {
            tmp = tmp->next;
            
            while (tmp != NULL) // midle and last
            {
                if (tmp->x == cor_x && tmp->y == cor_y)
                {
                    tmp_prev->next = tmp->next;
                    delete tmp;
                    return first;
                }
                else
                {
                    tmp_prev = tmp;
                    tmp = tmp->next;
                }
            }
        }
    }

    void DrawMemory(sf::RenderWindow* window, Memory* first, sf::Texture texture)
    {
        Memory* pointer;
        pointer = first;

        while (pointer != NULL)
        {
            sf::RectangleShape tmp;
            tmp.setSize(sf::Vector2f(Piece_Size, Piece_Size));
            tmp.setTexture(&texture);
            tmp.setPosition(ScreanShiftLeft + ((pointer->x - 1) * Piece_Size), (pointer->y - 1) * Piece_Size);
            tmp.setFillColor(pointer->color);
            pointer = pointer->next;

            window->draw(tmp);
        }
    }

    Memory* DeleteAllMomory(Memory* first)
    {
        while (first != NULL)
        {
            Memory* tmp;
            tmp = first;
            first = first->next;
            delete tmp;
        }
        return first = NULL;
    }
};

int RowFull()
{
    for (int y = 20; y > 0; y--)
    {
        int sum = 0;
        for (int x = 1; x <= 10; x++)
        {
            sum = sum + Matrix[x][y];
        }
        if (sum == 10)
        {
            return y; //true
        }
    }
    return 0; //false
}

Memory* AllElementsFall(Memory* first, int fallBy, int Start_y)
{
    for (int x = 1; x <= 10; x++)
    {
        for (int y = Start_y; y >= 1; y--)
        {
            if (Matrix[x][y] != 0)
            {
                first = first->ChangeMemory(first, x, y, fallBy);

                Matrix[x][y] = 0;
                Matrix[x][y + fallBy] = 1;
            }
        }
    }
    return first;
}

Memory* RemoveRow(Memory* first)
{
    int scoreMultiplier = 0;
    int y = RowFull();
    //int FirstFullRow = y;
    while (y != 0)
    {
        scoreMultiplier++;
        
        for (int x = 1; x <= 10; x++)
        {
            Matrix[x][y] = 0;
            first = first->RemoveFromMemory(first, x, y);
        }

        first = AllElementsFall(first, 1, y - 1); // <---

        y = RowFull();
    }
    if (scoreMultiplier != 0)
    {
        //first = AllElementsFall(first, scoreMultiplier, FirstFullRow - scoreMultiplier); bug with fall
        if (FallTime >= sf::seconds(0.1f))
        {
            sf::Time tmp = FallTime;
            FallTime = tmp - sf::seconds(0.01f);
            //std::cout << FallTime.asSeconds() << std::endl;
        }
    }
    
    Score = Score + 100 * scoreMultiplier * scoreMultiplier;
    //std::cout << Score << std::endl;
    return first;
}

void GameOver(sf::RenderWindow* window)
{
    std::cout << "Game over\n";
    window->close();
}

class SmallBox
{

public:
    sf::RectangleShape box;
    sf::Text Header;
    int x, y;
    int box_width, box_hight;
    sf::Texture texture;

    void SetSmallBox(int width, int hight, int x_cor, int y_cor, sf::Color color, sf::Texture New_texture)
    {
        //box.setTexture(&texture);
        texture = New_texture;
        box.setSize(sf::Vector2f(width, hight));
        x = x_cor;
        y = y_cor;
        box_width = width;
        box_hight = hight;
        box.setPosition(x, y);
        box.setFillColor(color);
    }

    void SetHeader(sf::Font& font, sf::String HeaderText, int CharSize, sf::Color color)
    {
        Header.setFont(font);
        Header.setString(HeaderText);
        Header.setCharacterSize(CharSize);
        Header.setFillColor(color);
        sf::FloatRect tmp;
        tmp = Header.getLocalBounds();
        Header.setOrigin(tmp.width / 2, tmp.height / 2);
        //Header.setStyle(sf::Text::Bold | sf::Text::Underlined);
        Header.setPosition((box_width / 2) + x, (Piece_Size / 2) + y);
    }

    void DrawBox(sf::RenderWindow* window)
    {
        window->draw(box);
    }

    void DrawHeader(sf::RenderWindow* window)
    {
        window->draw(Header);
    }

};

class ScoreBox : public SmallBox
{
public:
    sf::Text Score_text;

    void SetScore(sf::Font& font, int CharSize, sf::Color color)
    {
        std::string str;
        sf::String sf_str;
        str = std::to_string(Score);
        sf_str = str;
        //std::cout << str <<" <-- here \n";

        Score_text.setFont(font);
        Score_text.setString(sf_str);
        Score_text.setCharacterSize(CharSize);
        Score_text.setFillColor(color);
        sf::FloatRect tmp;
        tmp = Score_text.getLocalBounds();
        Score_text.setOrigin(tmp.width / 2, tmp.height / 2);
        //Header.setStyle(sf::Text::Bold | sf::Text::Underlined);
        Score_text.setPosition((box_width / 2) + x, (box_hight / 2) + y);
    }

    void DrawAll(sf::RenderWindow* window)
    {
        window->draw(box);
        window->draw(Header);
        window->draw(Score_text);
    }
};

class NextBox : public SmallBox
{
public:
    Piece* nextPiece;
    sf::RectangleShape tmp[4];

    void setPiece()
    {
        nextPiece = Create_Piece();

        for (int i = 0; i < 4; i++)
        {
            tmp[i].setTexture(&texture);
            tmp[i].setSize(sf::Vector2f(Piece_Size, Piece_Size));
            int new_x_center = (nextPiece->piece_X[i] - (nextPiece->center_x));
            int new_y_center = (nextPiece->piece_Y[i] - (nextPiece->center_y));
            tmp[i].setPosition
            (
                ((nextPiece->piece_X[i] - (nextPiece->center_x)) * Piece_Size) + (x + (box_width / 2)),
                ((nextPiece->piece_Y[i] - (nextPiece->center_y)) * Piece_Size) + (y + (box_hight / 2))
            );

            tmp[i].setFillColor(nextPiece->color);
        }
    }

    Piece* getPiece()
    {
        return nextPiece;
    }

    void DrawPiece(sf::RenderWindow *window)
    {
        for (int i = 0; i < 4; i++)
        {
            window->draw(tmp[i]);
        }
    }
};

class HoldBox : public SmallBox // almost the same as NextBox ...
{
public:
    Piece* holdPiece = NULL;
    sf::RectangleShape tmp[4];

    void setPiece(Piece *piece)
    {
        holdPiece = piece;

        for (int i = 0; i < 4; i++)
        {
            tmp[i].setTexture(&texture);
            tmp[i].setSize(sf::Vector2f(Piece_Size, Piece_Size));
            int new_x_center = (holdPiece->piece_X[i] - (holdPiece->center_x));
            int new_y_center = (holdPiece->piece_Y[i] - (holdPiece->center_y));
            tmp[i].setPosition
            (
                ((holdPiece->piece_X[i] - (holdPiece->center_x)) * Piece_Size) + (x + (box_width / 2)),
                ((holdPiece->piece_Y[i] - (holdPiece->center_y)) * Piece_Size) + (y + (box_hight / 2))
            );

            tmp[i].setFillColor(holdPiece->color);
        }
    }

    Piece* getPiece()
    {
        return holdPiece;
    }

    void DrawPiece(sf::RenderWindow* window)
    {
        if (holdPiece != 0)
        {
            for (int i = 0; i < 4; i++)
            {
                window->draw(tmp[i]);
            }
        }
    }
};

void PutInCorrectOrder(int Arr[])
{
    for (int num = 4; num >= 0; num--)
    {
        for (int i = 0; i < num; i++)
        {
            if (!(Arr[i] > Arr[i + 1]))
            {
                //std::cout << "OK\n";
                int tmp = Arr[i];
                Arr[i] = Arr[i + 1];
                Arr[i + 1] = tmp;
            }
        }
    }
}

void StartingWindow(sf::RenderWindow* window, bool &Open, bool ContinuePlaying, sf::Texture MainMenuTexture, sf::Texture buttonTexture,
                    int main_box_position_x = Piece_Size * 8 + LINE_THICKNESS,
                    int main_box_position_y = Piece_Size * 2 + LINE_THICKNESS)
{
    if (Open == false)
    {
        return;
    }

    std::fstream scoreFile;
    scoreFile.open("Best Score.txt");
    if (!scoreFile.is_open())
    {
        std::cout << "Failed to open Score file\n";
    }

    int BestScore[5];

    bool CorrectOrder = true;
    for (int i = 0; i < 5; i++)
    {
        scoreFile >> BestScore[i];
        if (i != 0 || CorrectOrder == false)
        {
            if (BestScore[i] > BestScore[i - 1])
            {
                CorrectOrder = false;
            }
        }
    }
    scoreFile.close();

    if (!CorrectOrder)
    {
        PutInCorrectOrder(BestScore);
    }

    if (BestScore[4] < Score)
    {
        BestScore[4] = Score;
        PutInCorrectOrder(BestScore);
        scoreFile.open("Best Score.txt", std::ios::out | std::ios::trunc);

        for (int i = 0; i < 5; i++)
        {
            scoreFile << BestScore[i];
            scoreFile << "\n";
        }
    }

    

    sf::Font font;
    if (!font.loadFromFile("Roots Regular 400.ttf"))
    {
        std::cout << "Error Font\n";
    }


    int main_box_width = Piece_Size * 8 - LINE_THICKNESS;
    int main_box_hight = Piece_Size * 17 - LINE_THICKNESS;

    sf::RectangleShape Main_box, Play_box, HighScore_box, HowToPlay_box, Exit_box;

    Main_box.setSize(sf::Vector2f(main_box_width, main_box_hight));
    Main_box.setPosition(main_box_position_x, main_box_position_y);
    Main_box.setFillColor(sf::Color::Red);
    Main_box.setTexture(&MainMenuTexture);

    // Play box
    int play_box_width = Piece_Size * 5;
    int play_box_hight = Piece_Size * 3;
    int play_box_position_x = main_box_position_x + main_box_width / 2 - play_box_width / 2;
    int play_box_position_y = main_box_position_y + Piece_Size * 2;

    Play_box.setSize(sf::Vector2f(play_box_width, play_box_hight));
    Play_box.setPosition(play_box_position_x, play_box_position_y);
    Play_box.setFillColor(sf::Color::White);
    Play_box.setTexture(&buttonTexture);
    sf::Text play;
    play.setFont(font);
    play.setCharacterSize(Piece_Size);
    if (ContinuePlaying)
    {
        play.setCharacterSize(Piece_Size - 5);
        play.setString("CONTINUE");
    }
    else
    {
        play.setString("PLAY");
    }
    play.setFillColor(sf::Color::White);
    int tmp_x, tmp_y;
    sf::FloatRect tmp = play.getLocalBounds();
    tmp_x = tmp.width / 2;
    tmp_y = tmp.height; // Okey?
    play.setOrigin(tmp_x, tmp_y);
    play.setPosition(play_box_position_x + play_box_width / 2, play_box_position_y + play_box_hight / 2);

    // Exit box
    int exit_box_width = Piece_Size * 3;
    int exit_box_hight = Piece_Size * 2;
    int exit_box_position_x = main_box_position_x + main_box_width / 2 - exit_box_width / 2;
    int exit_box_position_y = main_box_hight + main_box_position_y - (Piece_Size * 2 + exit_box_hight);

    Exit_box.setSize(sf::Vector2f(exit_box_width, exit_box_hight));
    Exit_box.setPosition(exit_box_position_x, exit_box_position_y);
    Exit_box.setFillColor(sf::Color::White);
    Exit_box.setTexture(&buttonTexture);
    sf::Text exit;
    exit.setFont(font);
    exit.setCharacterSize(Piece_Size);
    exit.setString("Exit");
    exit.setFillColor(sf::Color::White);
    //int tmp_x, tmp_y;
    //sf::FloatRect tmp = play.getLocalBounds();
    tmp = exit.getLocalBounds();
    tmp_x = tmp.width / 2;
    tmp_y = tmp.height + 5; // Okey?
    exit.setOrigin(tmp_x, tmp_y);
    exit.setPosition(exit_box_position_x + exit_box_width / 2, exit_box_position_y + exit_box_hight / 2);

    // Score box
    int HScore_box_width = Piece_Size * 5;
    int HScore_box_hight = Piece_Size * 6;
    int HScore_box_position_x = main_box_position_x + main_box_width / 2 - HScore_box_width / 2;
    int HScore_box_position_y = play_box_hight + play_box_position_y + Piece_Size;

    HighScore_box.setSize(sf::Vector2f(HScore_box_width, HScore_box_hight));
    HighScore_box.setPosition(HScore_box_position_x, HScore_box_position_y);
    HighScore_box.setFillColor(sf::Color::Black);
    sf::Text HScoreText;
    HScoreText.setFont(font);
    HScoreText.setCharacterSize(Piece_Size / 2);
    HScoreText.setString("Your highest score:");
    HScoreText.setFillColor(sf::Color::White);
    //int tmp_x, tmp_y;
    //sf::FloatRect tmp = play.getLocalBounds();
    tmp = HScoreText.getLocalBounds();
    tmp_x = tmp.width / 2;
    tmp_y = tmp.height; // Okey?
    HScoreText.setOrigin(tmp_x, tmp_y);
    HScoreText.setPosition(HScore_box_position_x + HScore_box_width / 2, HScore_box_position_y + tmp_y);

    sf::Text printScore[5];
    for (int i = 0; i < 5; i++)
    {
        std::string  std_tmp;
        sf::String sf_tmp;
        std_tmp = std::to_string(BestScore[i]);
        sf_tmp = std_tmp;
        printScore[i].setString(sf_tmp);

        printScore[i].setFont(font);
        printScore[i].setCharacterSize(Piece_Size / 2);
        printScore[i].setFillColor(sf::Color::White);

        int tmp_x, tmp_y;
        sf::FloatRect tmp = printScore[i].getLocalBounds();
        //tmp = HScoreText.getLocalBounds();
        tmp_x = tmp.width / 2;
        tmp_y = tmp.height; // Okey?
        printScore[i].setOrigin(tmp_x, tmp_y);
        printScore[i].setPosition(HScore_box_position_x + HScore_box_width / 2, HScore_box_position_y + tmp_y + Piece_Size * (i + 1));
    }

    window->draw(Main_box);

    window->draw(Play_box);
    window->draw(play);

    window->draw(HighScore_box);
    window->draw(HScoreText);
    for (int i = 0; i < 5; i++)
    {
        window->draw(printScore[i]);
    }

    window->draw(Exit_box);
    window->draw(exit);

    // end the current frame
    window->display();

    // run the program as long as the window is open
    while (window->isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window->waitEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window->close();
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                //std::cout << "Mause presed\n";
                sf::Vector2i localPosition = sf::Mouse::getPosition(*window);
                sf::FloatRect box_position = Play_box.getGlobalBounds();
                int mause_x = localPosition.x;
                int mause_y = localPosition.y;
                int box_x = box_position.left;
                int box_width = box_position.width + box_position.left;
                int box_y = box_position.top;
                int box_height = box_position.height + box_position.top;
                if (mause_x >= box_x && mause_x <= box_width &&
                    mause_y >= box_y && mause_y <= box_height)
                {
                    Open = false;
                    return;
                }
                //sf::Vector2i localPosition = sf::Mouse::getPosition(*window);
                //sf::FloatRect box_position = Play_box.getGlobalBounds();
                localPosition = sf::Mouse::getPosition(*window);
                box_position = Exit_box.getGlobalBounds();
                mause_x = localPosition.x;
                mause_y = localPosition.y;
                box_x = box_position.left;
                box_width = box_position.width + box_position.left;
                box_y = box_position.top;
                box_height = box_position.height + box_position.top;

                if (mause_x >= box_x && mause_x <= box_width &&
                    mause_y >= box_y && mause_y <= box_height)
                {
                    window->close();
                }
            }
        }
    }

}

int main()
{
    srand(time(0));

    sf::Music music;
    if (!music.openFromFile("Tetris_theme_2.ogg"))
    {
        return -1;
    }
    music.setLoop(true);
    music.setVolume(50);
    music.setPlayingOffset(sf::seconds(0.7f));
    bool musicPlaying = true;

    sf::Font font;
    if (!font.loadFromFile("Roots Regular 400.ttf"))
    {
        std::cout << "Font Error\n";
    }

    sf::Texture PieceTexture, MainMenuTexture, buttonTexture;
    //if (!PieceTexture.loadFromFile("Piece_texture.png"))
    if (!PieceTexture.loadFromFile("TetrisPiece_2.png"))
    {
        std::cout << "Piece texture Error\n";
    }
    if (!MainMenuTexture.loadFromFile("MainMenu.png"))
    {
        std::cout << "Main menu texture Error\n";
    }
    if (!buttonTexture.loadFromFile("ButtonTexture.png"))
    {
        std::cout << "Button texture Error\n";
    }


    ScoreBox* scoreBox;
    HoldBox* holdBox;
    NextBox* nextBox;

    scoreBox = new ScoreBox;
    holdBox = new HoldBox;
    nextBox = new NextBox;

    scoreBox->SetSmallBox(Piece_Size * 5, Piece_Size * 4, Piece_Size * 1, windowHeight - Piece_Size * 6, sf::Color::Transparent, PieceTexture);
    scoreBox->SetHeader(font, "Score:", Piece_Size, sf::Color::White);
    scoreBox->SetScore(font, Piece_Size, sf::Color::White);

    holdBox->SetSmallBox(Piece_Size * 5, Piece_Size * 5, Piece_Size * 1, Piece_Size * 1, sf::Color::Transparent, PieceTexture);
    holdBox->SetHeader(font, "Hold", Piece_Size, sf::Color::White);

    nextBox->SetSmallBox(Piece_Size * 5, Piece_Size * 5, windowWidth - Piece_Size * 6, Piece_Size * 1, sf::Color::Transparent, PieceTexture);
    nextBox->SetHeader(font, "Next Piece", Piece_Size, sf::Color::White);
    nextBox->setPiece();

    sf::Clock clock;

    StartMatrix();

    Memory* first;
    first = NULL;

    sf::RenderWindow* window;
    Piece* piece;
    piece = new S_Piece; // for some reazon I have to inisilaze "piece"
    

    //piece = Create_Piece();
    piece = nextBox->getPiece();
    nextBox->setPiece();

    window = new sf::RenderWindow(sf::VideoMode(windowWidth, windowHeight), "Tetris", sf::Style::Close);

    bool CanHold = true;

    bool OpenStartingWindow = true;
    StartingWindow(window, OpenStartingWindow, false, MainMenuTexture, buttonTexture);

    music.play();

    sf::Clock MoveLeftButtonPressTime, FallButtonPressTime, MoveRightButtonPressTime;

    window->setKeyRepeatEnabled(false);
    bool DelayR = false;
    bool DelayL = false;
    bool DelayFall = false;

    while (window->isOpen())
    {   
        if (OpenStartingWindow)
        {
            StartingWindow(window, OpenStartingWindow, false, MainMenuTexture, buttonTexture);
            Score = 0;
            scoreBox->SetScore(font, Piece_Size, sf::Color::White);
            FallTime = StartingFallTime;
            if (musicPlaying)
            {
                music.setPlayingOffset(sf::seconds(0.7f));
                music.play();
            }
            window->clear(sf::Color::Black);
        }

        if (!piece->Exist)
        {
            CanHold = true;

            for (int i = 0; i < 4; i++) // Add to memory
            {
                if (first == NULL)
                {
                    first = new Memory;
                    first->SetMemory(piece->piece_X[i], piece->piece_Y[i], piece->color);
                    //std::cout << first->x <<" "<< first->y << "\n";
                    first->next = NULL;
                }
                else
                {
                    Memory* tmp;
                    tmp = new Memory;
                    tmp->SetMemory(piece->piece_X[i], piece->piece_Y[i], piece->color);
                    tmp->next = first;
                    first = tmp;
                    //std::cout << first->x << " " << first->y << "\n";
                }

                Matrix[piece->piece_X[i]][piece->piece_Y[i]] = 1;
            }

            first = RemoveRow(first);

            //piece = Create_Piece();
            piece = nextBox->getPiece();
            nextBox->setPiece();

            for (int i = 0; i < 4; i++)
            {
                if (Matrix[piece->piece_X[i]][piece->piece_Y[i]] != 0)
                {
                    first = first->DeleteAllMomory(first);
                    StartMatrix();
                    CanHold = true;
                    nextBox->setPiece();
                    piece = nextBox->getPiece();
                    nextBox->setPiece();
                    holdBox->holdPiece = NULL;
                    OpenStartingWindow = true;
                    if (musicPlaying)
                    {
                        music.stop();
                    }
                    break;
                }
            }

            if (OpenStartingWindow)
            {
                continue;
            }

            scoreBox->SetScore(font, Piece_Size, sf::Color::White);
        }

        sf::Event event;
        while (window->pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window->close();
                break;

            case sf::Event::KeyPressed:
                switch (event.key.code)
                {
                case sf::Keyboard::Escape:
                    if (musicPlaying)
                    {
                        music.pause();
                    }

                    OpenStartingWindow = true;
                    StartingWindow(window, OpenStartingWindow, true, MainMenuTexture, buttonTexture);
                    window->clear(sf::Color::Black);

                    if (musicPlaying)
                    {
                        music.play();
                    }
                    break;

                case sf::Keyboard::D:
                case sf::Keyboard::Right:
                    DelayR = true;
                    MoveRightButtonPressTime.restart();
                    piece->MoveRight();
                    break;

                case sf::Keyboard::A:
                case sf::Keyboard::Left:
                    DelayL = true;
                    MoveLeftButtonPressTime.restart();
                    piece->MoveLeft();
                    break;

                case sf::Keyboard::S:
                case sf::Keyboard::Down:
                    piece->fall();
                    FallButtonPressTime.restart();
                    clock.restart();
                    break;

                case sf::Keyboard::W:
                case sf::Keyboard::Up:
                    piece->Rotate();
                    break;

                case sf::Keyboard::L:
                    piece->RotateLeft();
                    break;

                case sf::Keyboard::H:

                    if (CanHold)
                    {
                        CanHold = false;
                        piece->setStartingPosition();

                        if (holdBox->getPiece() == NULL)
                        {
                            holdBox->setPiece(piece);
                            piece = nextBox->getPiece();
                            nextBox->setPiece();
                        }
                        else
                        {
                            Piece* tmp;
                            tmp = piece;
                            piece = holdBox->getPiece();
                            holdBox->setPiece(tmp);
                        }
                    }
                    break;

                case sf::Keyboard::M:
                    if (musicPlaying)
                    {
                        music.pause();
                        musicPlaying = false;
                    }
                    else
                    {
                        music.play();
                        musicPlaying = true;
                    }
                    break;

                default:
                    break;
                }
                break;

            default:
                break;
            }
        }

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || (sf::Keyboard::isKeyPressed(sf::Keyboard::D)))
           && (MoveRightButtonPressTime.getElapsedTime() > HoldDelayTime))
        {
            if (DelayR)
            {
                if (MoveRightButtonPressTime.getElapsedTime() > FirstDelayTime)
                {
                    DelayR = false;
                }
            }
            else
            {
                piece->MoveRight();
                MoveRightButtonPressTime.restart();
            }
        }

        else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || (sf::Keyboard::isKeyPressed(sf::Keyboard::A)))
                && (MoveLeftButtonPressTime.getElapsedTime() > HoldDelayTime))
        {
            if (DelayL)
            {
                if (MoveLeftButtonPressTime.getElapsedTime() > FirstDelayTime)
                {
                    DelayL = false;
                }
            }
            else
            {
                piece->MoveLeft();
                MoveLeftButtonPressTime.restart();
            }
        }

        else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || (sf::Keyboard::isKeyPressed(sf::Keyboard::S)))
                && (FallButtonPressTime.getElapsedTime() > HoldDelayTime))
        {
            if (DelayFall)
            {
                if (FallButtonPressTime.getElapsedTime() > FirstDelayTime)
                {
                    DelayFall = false;
                }
            }
            else
            {
                piece->fall();
                FallButtonPressTime.restart();
                clock.restart();
            }
        }
        
        if (clock.getElapsedTime() >= FallTime)
        {
            clock.restart();
            piece->fall();
        }

        window->clear();

        scoreBox->DrawAll(window);

        holdBox->DrawBox(window);
        holdBox->DrawHeader(window);
        holdBox->DrawPiece(window);

        nextBox->DrawBox(window);
        nextBox->DrawHeader(window);
        nextBox->DrawPiece(window);

        piece->Draw(window, PieceTexture);
        first->DrawMemory(window, first, PieceTexture);
        Draw_Map(window);


        window->display();
    }

    first = first->DeleteAllMomory(first);

    return 0;
}