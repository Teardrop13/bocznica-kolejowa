#include <ncurses.h>
#include <unistd.h> // sleep function
#include <vector>
#include <iostream>
#include <stack>
#include <list>

// draw wagons for hints displayed on the end of the tracks
// optional_wagons - array of all possible wagons to draw
std::vector<char> draw_set(int number, char optional_wagons[])
{
    std::vector<char> set;

    for (int i = 0; i < number; i++)
    {
        set.push_back(optional_wagons[rand() % 4]);
    }

    return set;
}

// adds wagons to the front of the list until <int number> is reached
void fill_awaiting_wagons_list(std::list<char> &awaiting_wagons_list, int number, char optional_wagons[])
{
    while (awaiting_wagons_list.size() < number)
    {
        awaiting_wagons_list.push_front(optional_wagons[rand() % 4]);
    }
}

void print_grid(std::vector<std::string> &grid)
{
    clear();
    for (std::string line : grid)
    {
        printw(line.c_str());
        printw("\n");
    }
    refresh();
}

void put_wagons_on_grid(std::vector<std::string> &grid,
                        std::list<char> &awaiting_wagons_list,
                        std::vector<std::vector<char>> &wagon_sets,
                        int points)
{
    std::list<char>::iterator it = awaiting_wagons_list.begin();
    for (int i = 0; i < awaiting_wagons_list.size(); i++)
    {
        grid[0].replace(i, 1, std::string(1, *it));
        it++;
    }

    for (int i = 0, track = 0; i < wagon_sets.size(); i++, track += 2)
    {
        for (int j = 0; j < 4; j++)
        {
            grid[track].replace(74 + j, 1, std::string(1, wagon_sets[i][j]));
        }
    }

    std::string points_string = "points: " + std::to_string(points);
    if (points == 0)
        points_string.append("0");
    grid[grid.size() - 1].replace(1, 10, points_string);
}

// animation of moving wagons
void animate(int start, int target_track, char wagon, std::vector<std::string> &grid)
{
    std::string w = std::string() + '=' + wagon;
    int i = start;

    while (i < 18)
    {
        grid[0].replace(i, 2, w);
        print_grid(grid);
        i++;
        usleep(20000);
    }
    grid[0].replace(i, 1, "=");
    while (grid[target_track][i] != '=') i++;

    while (grid[target_track][i + 1] == '=')
    {
        grid[target_track].replace(i, 2, w);
        print_grid(grid);
        i++;
        usleep(20000);
    }
}

void display_end_screen(int points)
{
    clear();
    if (points >= 50)
    {
        mvprintw(4, 40, "You win");
    }
    else if (points < 0)
    {
        mvprintw(4, 40, "You lose");
    }
    refresh();
}

int main()
{
    const int TRACKS_NUMBER = 5;
    const int AWAITING_WAGONS_LIST_SIZE = 8;
    const int MAX_SET_SIZE = 4;

    int points = 0;
    int target_track;
    char optional_wagons[] = {'A', 'B', 'C', 'D'};

    initscr();
    curs_set(0); // invisible cursor
    srand(time(NULL));

    std::list<char> awaiting_wagons_list;
    std::vector<std::vector<char>> wagon_sets(TRACKS_NUMBER);
    std::vector<std::stack<char>> ready_wagons(TRACKS_NUMBER);
    std::vector<std::string> grid(9);

    for (int i = 0; i < TRACKS_NUMBER; i++)
    {
        wagon_sets[i] = draw_set(MAX_SET_SIZE, optional_wagons);
    }

    grid[0] = "12345678========================================================= Tor 1: [1234]";
    grid[1] = "                 \\\\ (2)                                                        ";
    grid[2] = "                  \\\\============================================= Tor 2: [1234]";
    grid[3] = "                   \\\\ (3)                                                      ";
    grid[4] = "                    \\\\=========================================== Tor 3: [1234]";
    grid[5] = "                     \\\\ (4)                                                    ";
    grid[6] = "                      \\\\========================================= Tor 4: [1234]";
    grid[7] = "                       \\\\ (5)                                                  ";
    grid[8] = "                        ========================================= Tor 5: [1234]";

    fill_awaiting_wagons_list(awaiting_wagons_list, AWAITING_WAGONS_LIST_SIZE, optional_wagons);

    put_wagons_on_grid(grid, awaiting_wagons_list, wagon_sets, points);
    print_grid(grid);

    char released_wagon;
    char pressed_button;

    while (points < 50 && points >= 0)
    {
        pressed_button = getchar();

        switch (pressed_button)
        {
        case '2':
            target_track = 1;
            break;
        case '3':
            target_track = 2;
            break;
        case '4':
            target_track = 3;
            break;
        case '5':
            target_track = 4;
            break;
        default:
            target_track = 0;
        }

        if (pressed_button != 'w')
        {
            while (getchar() != 'w');
        }

        released_wagon = awaiting_wagons_list.back();
        awaiting_wagons_list.pop_back();

        fill_awaiting_wagons_list(awaiting_wagons_list, AWAITING_WAGONS_LIST_SIZE, optional_wagons);
        put_wagons_on_grid(grid, awaiting_wagons_list, wagon_sets, points);
        ready_wagons[target_track].push(released_wagon);

        animate(AWAITING_WAGONS_LIST_SIZE, target_track * 2, released_wagon, grid);

        if (ready_wagons[target_track].size() == MAX_SET_SIZE)
        {
            bool correct_set = true;
            for (int i = 0; i < MAX_SET_SIZE; i++)
            {
                if (ready_wagons[target_track].top() != wagon_sets[target_track][i])
                    correct_set = false;
                ready_wagons[target_track].pop();
            }
            if (correct_set == true)
            {
                points += 10;
            }
            else
            {
                points -= 10;
            }
            ready_wagons[target_track] = {};
            wagon_sets[target_track] = draw_set(4, optional_wagons);
            grid[target_track * 2].replace(61, 4, "====");
            put_wagons_on_grid(grid, awaiting_wagons_list, wagon_sets, points);
            print_grid(grid);
        }
    }

    display_end_screen(points);

    getchar();
    endwin();

    return 0;
}
