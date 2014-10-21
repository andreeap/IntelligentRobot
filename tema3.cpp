/*
 *  Pirvulescu Andreea Monica
 *
 */

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include <list>

using namespace std;

// A map cell
class Place
{
    int x, y;
    char type;
    char status;
public:
    Place ();
    Place (int, int, char, char);
    char getType ()
    {
        return type;
    }
    void setType (char t)
    {
        type = t;
    }
    char getStatus ()
    {
        return status;
    }
    void setStatus (char s)
    {
        status = s;
    }
    int getX ()
    {
        return x;
    }
    int getY ()
    {
        return y;
    }
    bool operator<( const Place& place) const
    {
        return ((this->x < place.x) && (this->y < place.y));
    }
    bool operator==(const Place& place) const
    {
        if ((this->x == place.x) && (this->y == place.y) &&
                (this->type == place.type) && (this->status == place.status))
            return true;
        else return false;
    }
};

Place::Place()
{
}

Place::Place(int a, int b, char val, char st)
{
    x = a;
    y = b;
    type = val;
    status = st;
}

int compare(Place a, Place b)
{
    if((a.getX() == b.getX()) && (a.getY() == b.getY()) &&
            (a.getType() == b.getType()) && (a.getStatus() != 'U')
            && (b.getStatus()) != 'U')
        return 0;
    else return 1;
}

list<pair<int, int> > path;
map<pair<int, int>,list<Place> > nodes;

// the previous position of the robot
int prev_x = -1;
int prev_y = -1;

// the current position of the robot
int robot_x = -1;
int robot_y = -1;

// test if two adjacent are safe
// if it finds 3 adjacent cells safe and 1 unsafe
// the unsafe cell is a swamp cell
void is_safe(int i, int j, vector<vector<char> > &g, vector<vector<Place> > &m)
{

    if ((robot_x-1 >= 0 && robot_x < i-1) && (robot_y-1 >= 0 && robot_y < j-1))
    {

        if ((g.at(robot_x-1).at(robot_y) == 'M') || (g.at(robot_x).at(robot_y+1) == 'M') ||
                (g.at(robot_x+1).at(robot_y) == 'M') || (g.at(robot_x).at(robot_y-1) == 'M'))
        {

            if (m.at(robot_x-1).at(robot_y).getStatus() != 'S')
            {
                m.at(robot_x-1).at(robot_y).setStatus('U');
                if ((m.at(robot_x).at(robot_y+1).getStatus() == 'S') &&
                        (m.at(robot_x+1).at(robot_y).getStatus() == 'S') &&
                        (m.at(robot_x).at(robot_y-1).getStatus() == 'S'))
                {
                    m.at(robot_x-1).at(robot_y).setType('M');
                    m.at(robot_x).at(robot_y+1).setStatus('S');
                    m.at(robot_x+1).at(robot_y).setStatus('S');
                    m.at(robot_x).at(robot_y-1).setStatus('S');
                }
            }

            if (m.at(robot_x).at(robot_y+1).getStatus() != 'S')
            {
                m.at(robot_x).at(robot_y+1).setStatus('U');
                if ((m.at(robot_x-1).at(robot_y).getStatus() == 'S') &&
                        (m.at(robot_x+1).at(robot_y).getStatus() == 'S') &&
                        (m.at(robot_x).at(robot_y-1).getStatus() == 'S'))
                {
                    m.at(robot_x).at(robot_y+1).setType('M');
                    m.at(robot_x-1).at(robot_y).setStatus('S');
                    m.at(robot_x+1).at(robot_y).setStatus('S');
                    m.at(robot_x).at(robot_y-1).setStatus('S');
                }
            }

            if (m.at(robot_x+1).at(robot_y).getStatus() != 'S')
            {
                m.at(robot_x+1).at(robot_y).setStatus('U');
                if ((m.at(robot_x).at(robot_y+1).getStatus() == 'S') &&
                        (m.at(robot_x-1).at(robot_y).getStatus() == 'S') &&
                        (m.at(robot_x).at(robot_y-1).getStatus() == 'S'))
                {
                    m.at(robot_x+1).at(robot_y).setType('M');
                    m.at(robot_x-1).at(robot_y).setStatus('S');
                    m.at(robot_x).at(robot_y+1).setStatus('S');
                    m.at(robot_x).at(robot_y-1).setStatus('S');
                }
            }

            if (m.at(robot_x).at(robot_y-1).getStatus() != 'S')
            {
                m.at(robot_x).at(robot_y-1).setStatus('U');
                if ((m.at(robot_x).at(robot_y+1).getStatus() == 'S') &&
                        (m.at(robot_x-1).at(robot_y).getStatus() == 'S') &&
                        (m.at(robot_x+1).at(robot_y).getStatus() == 'S'))
                {
                    m.at(robot_x).at(robot_y-1).setType('M');
                    m.at(robot_x-1).at(robot_y).setStatus('S');
                    m.at(robot_x).at(robot_y+1).setStatus('S');
                    m.at(robot_x+1).at(robot_y).setStatus('S');
                }
            }

        }
        else
        {

            m.at(robot_x-1).at(robot_y).setStatus('S');
            m.at(robot_x).at(robot_y+1).setStatus('S');
            m.at(robot_x+1).at(robot_y).setStatus('S');
            m.at(robot_x).at(robot_y-1).setStatus('S');

        }
    }
}

// test if a cell is a wall
void is_wall(int x, int y, vector<vector<Place> > &m)
{
    if (robot_x == prev_x && robot_y == prev_y)
    {
        m.at(x).at(y).setStatus('S');
        m.at(x).at(y).setType('%');
    }
}

// the robot goes up if is a land cell
void up(vector<vector<char> > &g, vector<vector<Place> > &m)
{
    prev_x = robot_x;
    prev_y = robot_y;

    if (g.at(robot_x-1).at(robot_y) != '%')
    {
        robot_x = robot_x - 1;
        m.at(robot_x).at(robot_y).setType('-');
        m.at(robot_x).at(robot_y).setStatus('S');
        path.push_back(make_pair(robot_x,robot_y));
    }

    is_wall(robot_x-1,robot_y,m);
}

// the robot goes right if is a land cell
void right(vector<vector<char> > &g, vector<vector<Place> > &m)
{
    prev_x = robot_x;
    prev_y = robot_y;

    if (g.at(robot_x).at(robot_y+1) != '%')
    {
        robot_y = robot_y + 1;
        m.at(robot_x).at(robot_y).setType('-');
        m.at(robot_x).at(robot_y).setStatus('S');
        path.push_back(make_pair(robot_x,robot_y));
    }

    is_wall(robot_x,robot_y+1,m);
}

// the robot goes down if is a land cell
void down(vector<vector<char> > &g, vector<vector<Place> > &m)
{
    prev_x = robot_x;
    prev_y = robot_y;

    if (g.at(robot_x+1).at(robot_y) != '%')
    {
        robot_x = robot_x + 1;
        m.at(robot_x).at(robot_y).setType('-');
        m.at(robot_x).at(robot_y).setStatus('S');
        path.push_back(make_pair(robot_x,robot_y));
    }

    is_wall(robot_x+1,robot_y,m);
}

// the robot goes left if is a land cell
void left(vector<vector<char> > &g, vector<vector<Place> > &m)
{
    prev_x = robot_x;
    prev_y = robot_y;

    if (g.at(robot_x).at(robot_y-1) != '%')
    {
        robot_y = robot_y - 1;
        m.at(robot_x).at(robot_y).setType('-');
        m.at(robot_x).at(robot_y).setStatus('S');
        path.push_back(make_pair(robot_x,robot_y));
    }

    is_wall(robot_x,robot_y-1,m);
}

// expand the current cell by adding the possible neighbors
// in a list for next exploration
void expand(Place next_p, vector<vector<Place> > &m)
{

    if ((next_p.getType() == 'X') || (next_p.getType() == 'S'))
    {
        if(nodes.find(make_pair(robot_x,robot_y)) == nodes.end())
        {
            list<Place> neigh;
            nodes.insert(make_pair(make_pair(robot_x, robot_y), neigh));
        }
        list<Place> neigh = nodes.at(make_pair(robot_x,robot_y));
        list<Place>::iterator itb = neigh.begin();
        list<Place>::iterator ite =neigh.end();
        Place p (next_p.getX(), next_p.getY(), next_p.getType(), '?');
        nodes.at(make_pair(robot_x,robot_y)).remove(p);
        if (find(itb, ite, next_p) == ite)
        {
            nodes.at(make_pair(robot_x,robot_y)).push_back(next_p);

        }
    }
    else return;
}

// the robot is exploring as long as it has a safe path
void explore(int i, int j, vector<vector<char> > &g, vector<vector<Place> > &m)
{
    while(!path.empty())
    {

        Place up_p, right_p, down_p, left_p;

        if ((robot_x - 1) >= 0)
            up_p = m.at(robot_x-1).at(robot_y);
        else
            up_p = Place (-1, -1, '/', '?');

        if (robot_y < (j - 1))
            right_p = m.at(robot_x).at(robot_y+1);
        else
            right_p = Place (-1, -1, '/', '?');

        if (robot_x < (i - 1))
            down_p = m.at(robot_x+1).at(robot_y);
        else
            down_p = Place (-1, -1, '/', '?');

        if ((robot_y - 1) >= 0)
            left_p = m.at(robot_x).at(robot_y-1);
        else
            left_p = Place (-1, -1, '/', '?');

        is_safe(i,j,g,m);

        if ((robot_x-1>=0 && robot_x<i-1) && (robot_y-1>=0 && robot_y < j-1))
        {

            if(up_p.getStatus() != 'U')
                expand(up_p, m);
            if(right_p.getStatus() != 'U')
                expand(right_p, m);
            if(down_p.getStatus() != 'U')
                expand(down_p, m);
            if(left_p.getStatus() != 'U')
                expand(left_p, m);
        }
        list<Place> neigh = nodes.at(make_pair(robot_x, robot_y));
        Place p;

        if(!neigh.empty())
        {
            p = neigh.front();
            neigh.pop_front();
            if ((p.getType() != 'X') && (neigh.empty()))
                p = Place (-1, -1, '/', '?');
            else
                while(!neigh.empty() && (p.getType() != 'U'))
                {
                    p = neigh.front();
                    neigh.pop_front();
                }
        }
        nodes.at(make_pair(robot_x, robot_y)) = neigh;

        if ((p.getX() != -1) && (p.getY() != -1))
        {
            if (compare(p, m.at(robot_x-1).at(robot_y)) == 0)
            {
                up(g, m);
            }
            else if (compare(p, m.at(robot_x).at(robot_y+1)) == 0)
            {
                right(g, m);
            }
            else if (compare(p, m.at(robot_x+1).at(robot_y)) == 0)
            {
                down(g, m);
            }
            else if (compare(p, m.at(robot_x).at(robot_y-1)) == 0)
            {
                left(g, m);
            }
            else
            {
                path.pop_back();
                robot_x = path.back().first;
                robot_y = path.back().second;
            }
        }

    }

}

int main(int argc, char* argv[])
{

    ifstream fin(argv[1]);
    int i, j, grid_i, grid_j;

    if (!fin.good())
    {
        cout << "Couldn't open the file.\n";
        return 1;
    }

    fin>>robot_x;
    fin>>robot_y;
    fin>>grid_i;
    fin>>grid_j;

    vector<vector<char> > grid(grid_i);
    for (i = 0; i < grid_i; i++)
        grid[i].resize(grid_j);

    for(i = 0; i < grid_i; i++)
        for(j = 0; j < grid_j; j++)
            fin>>grid[i][j];

    cout<<"%=PERETE"<<endl;
    cout<<"M=MLASTINA"<<endl;
    cout<<"-=PAMANT"<<endl;
    cout<<"X=NEEXPLORAT"<<endl;
    cout<<endl;

    cout<<"Grid"<<endl;
    for(i = 0; i < grid_i; i++)
    {
        for(j = 0; j < grid_j; j++)
            cout<<grid[i][j];
        cout<<endl;
    }

    cout<<endl;

    vector<vector<Place> > robot_map(grid_i);
    for (i = 0; i < grid_i; i++)
        robot_map[i].resize(grid_j);

    cout<<"Robot Map"<<endl;
    for(i =0; i < grid_i; i++)
    {
        for(j = 0; j < grid_j; j++)
        {
            Place p (i, j, 'X', '?');
            robot_map.at(i).at(j) = p;
        }
    }

    Place robot (robot_x, robot_y, '-', 'S');
    robot_map.at(robot_x).at(robot_y) = robot;

    path.push_back(make_pair(robot_x, robot_y));

    explore(grid_i, grid_j, grid, robot_map);

    for(i =0; i < grid_i; i++)
    {
        for(j = 0; j < grid_j; j++)
            cout<<robot_map.at(i).at(j).getType();
        cout<<endl;
    }
}
