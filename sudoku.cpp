/*
Sudoku Solver Using AI
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm> //For heap construction
#include <string>
#include <utility> //For pair
#include <unordered_map>
#include <stdlib.h>//For rand
#include <time.h>
#include <unordered_set>

using namespace std;

/*
Prints the numbers in the board, as a 2d representation
Input: the 9x9 board
*/
void printBoard(const vector<vector<string>>& board)
{
    for (const auto& i : board)
    {
        for (const string& j : i)
        {
            cout << j << " ";
        }
        cout << '\n';
    }
}

/*
Prints the numbers in the domain, as a 2d representation
Input: the 9x9 domain
*/
void printDom(const vector<vector< vector<string> >>& domain)
{
    cout << "Domain size: " << domain.size() << '\n';
    for (const auto& x : domain)
    {
        //cout << x.size() << "\n";
        for (const auto& y : x)
        {
            cout << "Going Through individual domain - Size: " << y.size() << " -> ";
            for (const auto& z : y)
            //for (int i = 0; i < y.size(); ++i)
            {
                //cout << y[i] << " ";
                cout << z << " ";
            }
            cout << '\n';
        }
        cout << '\n';
    }
    return ;
}

/*
Fills out the initial legal values for the domain of 1 variable
0 has 1-9
Else the domain is specified to the number that's already there
*/
void constructDomVar(const string& var, vector<string>& domain)
{
    //cout << "In constructDomVar -> ";
    if (var == "0")
    {
        //cout << "No domain specified!\n";
        for (int i = 1; i < 10; ++i)
        {
            domain.emplace_back(to_string(i));
            //push_heap(domain.begin(), domain.end());
        }
    }
    else
    {
        //cout << "Domain Specified!\n";
        //Should only be 1 thing in the heap, and so does not need to be sorted
        domain.emplace_back(var);
        //push_heap(domain.begin(), domain.end());
    }
    make_heap(domain.begin(), domain.end(), greater<string>());
}

/*
Fills out the initial legal values for the domain of the 9x9 variables
0 has 1-9
Else the domain is specified to the number that's already there
*/
void constructDom(const vector<vector<string>>& board, vector<vector< vector<string> >>& domain)
{
    for (size_t i = 0; i < 9; ++i)
    {
        for (size_t j = 0; j < 9; ++j)
        {
            //cout << "i,j: " << i << " " << j << "\n";
            //cout << "board: " << (board[i][j]) << "\ndomain:\n";
            //cout << domain[i][j] << "\n";
            constructDomVar(board[i][j], domain[i][j]);
        }
    }
}

/*
Checks to see if complete assignment of board
Input: the 9x9 board
*/
bool isCompleteBoard(const vector<vector<string>>& board)
{
    for (const auto& i : board)
    {
        for (const string& j : i)
        {
            if (j == "0")
            {
                return false;
            }
        }
    }
    return true;
}

/*
For inserting into the map
with data type below
This maps is used to map multiple values to 1 key
These multiple values can then be retrieved with [] or at()
Unlike using regular maps or multimaps
*/
void insertToMap(unordered_map<size_t, vector< pair<size_t, size_t> > >& unOrdMap, size_t key, size_t i, size_t j)
{
    vector< pair<size_t, size_t> > temp;
    if (unOrdMap.find(key) == unOrdMap.end())
    {//Then key has not been already inserted; 1st time inserting key
        temp.emplace_back(i, j);
        unOrdMap.emplace(key, temp);
    }
    else
    {//Add the current location to the end of the vector and rewrite the map
        temp = unOrdMap.at(key);
        temp.emplace_back(i, j);
        unOrdMap.at(key) = temp;
    }
}

/*
Implements Minimum Remaining Value Heuristic for Sudoku
Inputs:
The current domain
The minimum amount of remaining values found in the domain
A map mapping remaining values to coordinates
Output:
True if single coordinate mapped to mrv
False otherwise
If false, proceed to degree heuristic

Note 1: Clears current value of min for new value

Note 2: Did not use unordered_multimap because, upon viewing the API,
find still only returns 1 element only
*/
bool mrv(const vector<vector< vector<string> >>& domain, size_t& min,
         unordered_map<size_t, vector< pair<size_t, size_t> > >& remainingVals)
{
    //Temp Count of the min number of legal val in a var
    //min = domain[0][0].size();

    //If everything only has 1 legal value left, then it should be a complete assignment,
    //which has already been checked for before calling mrv
    //There can only be nine legal values max; 1-9
    min = 10;
    //Loop through the domain
    for (size_t i = 0; i < 9; ++i)
    {
        for (size_t j = 0; j < 9; ++j)
        {
            //cerr << "i,j -> " << i << ", " << j << '\n';
            //Add the remaining value to the map
            size_t key = domain[i][j].size();
            insertToMap(remainingVals, key, i, j);

            //Update min if there is a variable with less remaining values
            //Only for keeping track of first singular coordinate of mrv
            if ((domain[i][j].size() > 1) && (domain[i][j].size() < min))
            {
                //Updated min
                //cout << "Updated MRV min\n";
                min = key;
            }
        }
    }
    /*
    cout << "End MRV looping through domain\n";
    cout << "MRV Results: Min -> " << min << " Count -> " << remainingVals.at(min).size() << "\n";
    for (const auto& x : remainingVals.at(min))
    {
        cout << x.first << ", " << x.second << "\n";
    }
    cout << '\n';
    */
    if (remainingVals.at(min).size() != 1)
    {//If more than 1 possible var, return false and use degree heuristic
        return false;
    }
    //cout << "MRV Results: Min -> " << min << "; a, b -> " << a << ", " << b << '\n';
    //There is only 1 possible var
    return true;
}

/*
Returns the updated number of unassigned neighbors there are for the specified coordinate
i,j are the original coordinates
a,b are the coordinates you are checking to see if it is unassigned
*/
void updateConstraintCount(const vector<vector<string>>& board, const size_t i, const size_t j,
    const size_t a, const size_t b, unordered_set<size_t>& visited, size_t& count)
{
    size_t key = a * 10 + b;
    if (visited.find(key) != visited.end())
    {//If you haven't seen this coordinate before
        if ((board[a][b] == "0") && (a != i) && (b != j))
        {//If that coordinate is unassigned and it isn't the original coordinate, ++count
            ++count;
        }
        visited.emplace(key);
    }
    //Else you have seen it before, and so you don't double count
}

/*
Returns the count of how many unassigned neighbors there are for the row of i,j
Visited is used to prevent double counting neighbors
*/
size_t countRowConstraint(const vector<vector<string>>& board, const size_t i, const size_t j, unordered_set<size_t>& visited)
{
    size_t count = 0;
    for (size_t a = 0; a < 9; ++a)
    {
        updateConstraintCount(board, i, j, i, a, visited, count);
    }
    return count;
}

/*
Returns the count of how many unassigned neighbors there are for the column of i,j
Visited is used to prevent double counting neighbors
*/
size_t countColConstraint(const vector<vector<string>>& board, const size_t i, const size_t j, unordered_set<size_t>& visited)
{
    size_t count = 0;
    for (size_t a = 0; a < 9; ++a)
    {
        updateConstraintCount(board, i, j, a, j, visited, count);
    }
    return count;
}

/*
Helper function for countGridConstraint
Does the actual counting in a grid
r0, r1 are used for row bounds
c0, c1 are used for col bounds
i,j are the original coordinates
*/
size_t countGridHelp(const vector<vector<string>>& board, const size_t r0, const size_t r1,
    const size_t c0, const size_t c1, const size_t i, const size_t j, unordered_set<size_t>& visited)
{
    size_t count = 0;
    //x,y used to navigate
    for (size_t x = r0; x <= r1; ++x)
    {
        for (size_t y = c0; y <= c1; ++y)
        {
            updateConstraintCount(board, i, j, x, y, visited, count);
        }
    }
    return count;
}

/*
Returns the count of how many unassigned neighbors there are for the 3x3 grid of i,j
Visited is used to prevent double counting neighbors
*/
size_t countGridConstraint(const vector<vector<string>>& board, const size_t i, const size_t j, unordered_set<size_t>& visited)
{
    size_t count = 0;
    //BE CAREFUL NOT TO DOUBLE COUNT, SINCE ROW AND COL COUNTS ARE DONE BEFOREHAND

    if (i <= 2)
    {//Within rows 0-2
        if (j <= 2)
        {//Within cols 0-2
            count += countGridHelp(board, 0, 2, 0, 2, i, j, visited);
        }
        else if (j >= 6)
        {//Within cols 6-8
            count += countGridHelp(board, 0, 2, 6, 8, i, j, visited);
        }
        else
        {//Within cols 3-5
            count += countGridHelp(board, 0, 2, 3, 5, i, j, visited);
        }
    }
    else if (i >= 6)
    {//Within rows 6-8
        if (j <= 2)
        {//Within cols 0-2
            count += countGridHelp(board, 6, 8, 0, 2, i, j, visited);
        }
        else if (j >= 6)
        {//Within cols 6-8
            count += countGridHelp(board, 6, 8, 6, 8, i, j, visited);
        }
        else
        {//Within cols 3-5
            count += countGridHelp(board, 6, 8, 3, 5, i, j, visited);
        }
    }
    else
    {//Within rows 5-7
        if (j <= 2)
        {//Within cols 0-2
            count += countGridHelp(board, 5, 7, 0, 2, i, j, visited);
        }
        else if (j >= 6)
        {//Within cols 6-8
            count += countGridHelp(board, 5, 7, 6, 8, i, j, visited);
        }
        else
        {//Within cols 3-5
            count += countGridHelp(board, 5, 7, 3, 5, i, j, visited);
        }
    }
    return count;
}

/*
Implements degree heuristic for Sudoku
*/
pair<int, int> deg(const vector<vector<string>>& board,
    const vector< pair<size_t, size_t> >& remainingVars)
{
    //Keep track of how many unassigned neighbors the remaining variables have
    unordered_map<size_t, vector< pair<size_t, size_t> > > numNeighborsToVar;
    size_t maxUnassignedNeighbors = 0;
    //Loop through the remaining variables
    for (const auto& x : remainingVars)
    {
        //Calculate the number of unassigned neighbors
        //This is the key to the map
        size_t numUnNeighbors = 0;
        unordered_set<size_t> visited;
        numUnNeighbors += countRowConstraint(board, x.first, x.second, visited);
        numUnNeighbors += countColConstraint(board, x.first, x.second, visited);
        numUnNeighbors += countGridConstraint(board, x.first, x.second, visited);
        //Add the remaining value to the map
        insertToMap(numNeighborsToVar, numUnNeighbors, x.first, x.second);
        //cout << "numUnNeighbors: " << numUnNeighbors << '\n';

        //Update max if there is a variable with more unassigned neighbors
        if (numUnNeighbors >= maxUnassignedNeighbors)
        {
            maxUnassignedNeighbors = numUnNeighbors;
        }
    }
    //cout << "End loop\n";
    if (numNeighborsToVar.at(maxUnassignedNeighbors).size() == 1)
    {//If only 1 possible var, return it
        //cout << "1\n";
        return pair<size_t, size_t>(numNeighborsToVar.at(maxUnassignedNeighbors)[0]);
    }
    //cout << ">1\n";
    //Else, randomly choose among the vars with same maxUnassignedNeighbors
    srand(time(nullptr));
    //cout << "After srand()\n";
    size_t n = rand() % numNeighborsToVar.at(maxUnassignedNeighbors).size();
    //cout << "rand() size(): " << numNeighborsToVar.at(maxUnassignedNeighbors).size() << " n: " << n << '\n';
    return numNeighborsToVar.at(maxUnassignedNeighbors)[n];
}

/*
Returns a selected unassigned variable to branch off by setting values later on
*/
pair<int, int> selectUnassignedVar(const vector<vector<string>>& board, const vector<vector< vector<string> >>& domain)
{
    //Run Minimum Remaining Value Heuristic
    size_t min = 69;//Max only 9 vals left, 1-9
    unordered_map<size_t, vector< pair<size_t, size_t> > > remainingVals;
    if (mrv(domain, min, remainingVals))
    {
        return remainingVals.at(min)[0];
    }
    //cout << "Running degree heuristic in selectUnassignedVar\n";
    //cout << "min: " << min << '\n';
    //If mrv fails, then run degree heuristic
    return deg(board, remainingVals.at(min));
}

/*
Helper function for checking consistency
Loops over used
If a number is used more than once, then it returns false
Otherwise, it is locally consistent and returns true
0 can be used more than once, since it indicates there is no value assigned there
*/
bool tallyUsed(const vector<int>& used)
{
    bool result = true;
    /*
    cout << "  Printing USED vector\n";

    for (const auto x : used)
    {
        cout << x << " ";
    }
    cout << '\n';
    */
    for (size_t i = 1; i < 10; ++i)
    {
        int x = used[i];
        if ((x != 1) && (x != 0))
        {//If this int is used more than once, we need to return false
            //cout << "\t!!! Inconsistency found: " << x << " !!! \n";
            result = (result && false);
        }
    }
    //cout << "Result from tallyUsed: " << result << '\n';
    return result;
}

/*
Checks to see if all rows of an assignment of the board is consistent
Returns true if it is consistent
False if not consistent
*/
bool checkConsistentRows(const vector<vector<string>>& board)
{
    //cout << "CHECKING CONSISTENT ROWS\n----------------\n";
    bool result = true;
    for (size_t i = 0; i < 9; ++i)
    {
        //The index is a number
        //[] returns how many times the number has been used in the row
        vector<int> used(10, 0);
        //cout << " Used --- Row: " << i;
        for (size_t j = 0; j < 9; ++j)
        {
            //cout << " Column: " << j << " ";
            //cout << stoi(board[i][j]) << "\n";
            ++used[stoi(board[i][j])];
        }
        result = result && tallyUsed(used);
        if (!result)
        {//If result is false in this row, return false immediately
            return result;
        }
    }
    return result;
}

/*
Checks to see if all columns of an assignment of the board is consistent
Returns true if it is consistent
False if not consistent
*/
bool checkConsistentCols(const vector<vector<string>>& board)
{
    //cout << "CHECKING CONSISTENT COLS\n----------------\n";
    bool result = true;
    for (size_t j = 0; j < 9; ++j)
    {
        //The index is a number
        //[] returns how many times the number has been used in the col
        vector<int> used(10, 0);
        for (size_t i = 0; i < 9; ++i)
        {
            ++used[stoi(board[i][j])];
        }
        result = result && tallyUsed(used);
        if (!result)
        {//If result is false in this col, return false immediately
            return result;
        }
    }
    return result;
}

/*
Helper function for checkConsistentGrids
Does the actual checking in a grid
r0, r1 are used for row bounds
c0, c1 are used for col bounds
Returns true if local grid constraint met
False if inconsistent
*/
bool checkConsistentGridsHelp(const vector<vector<string>>& board,
    const size_t r0, const size_t r1, const size_t c0, const size_t c1)
{
    bool result = true;
    //The index is a number
    //[] returns how many times the number has been used in the col
    vector<int> used(10, 0);
    //x,y used to navigate
    //cout << "Checking Consistent Grid: " << r0 << ", " << r1 << " " << c0 << " " << c1 << '\n';
    for (size_t x = r0; x <= r1; ++x)
    {
        for (size_t y = c0; y <= c1; ++y)
        {//Update the frequency of the number being used
            //cout << "Updating Freq " << x << " " << y << " of " << stoi(board[x][y]) << '\n';
            //cout << "\tCurrent Freq BEFORE Update: " << used[stoi(board[x][y])];
            used[stoi(board[x][y])] = 1 + used[stoi(board[x][y])];
            /*
            cout << " Freq After update: " << used[stoi(board[x][y])] << '\n';
            cout << "\t\tPrinting USED vector IN LOOP of test\n\t\t";
            for (const auto x : used)
            {
                cout << x << " ";
            }
            cout << '\n';
            */
        }
    }
    /*
    cout << "Printing USED vector IN TEST\n";
    for (const auto x : used)
    {
        cout << x << " ";
    }
    cout << '\n';
    */
    result = result && tallyUsed(used);
    return result;
}

/*
Checks to see if all nine 3x3 grids of an assignment of the board is consistent
Returns true if it is consistent
False if not consistent
*/
bool checkConsistentGrids(const vector<vector<string>>& board)
{
    //cout << "CHECKING CONSISTENT GRIDS\n----------------\n";

    return checkConsistentGridsHelp(board, 0, 2, 0, 2) && checkConsistentGridsHelp(board, 0, 2, 3, 5) &&
    checkConsistentGridsHelp(board, 0, 2, 6, 8) && checkConsistentGridsHelp(board, 3, 5, 0, 2) &&
    checkConsistentGridsHelp(board, 3, 5, 3, 5) && checkConsistentGridsHelp(board, 3, 5, 6, 8) &&
    checkConsistentGridsHelp(board, 6, 8, 0, 2) && checkConsistentGridsHelp(board, 6, 8, 3, 5) &&
    checkConsistentGridsHelp(board, 6, 8, 6, 8);

    /*
    vector<vector<string>> testX{vector<string>{"94","4","9","5","95","96","97","98","99"},
                                vector<string>{"90","2","8","9","14","15","16","17","18"},
                                vector<string>{"70","1","3","7","23","24","25","26","27"}};
    cout << "In test checkGrids\n";
    return checkConsistentGridsHelp(testX, 0, 2, 1, 3);
    */
}

/*
Checks to see if an assignment of the board is consistent
Returns true if it is consistent
False if not consistent
*/
bool isConsistent(const vector<vector<string>>& board)
{
    //cout << "\nRUNNING ISCONSISTENT()!!!!!!!!!\n-------------\n";
    //cout << "\tconsistentRows: " << boolalpha << checkConsistentRows(board);
    //cout << "\tconsistentCols: " << boolalpha << checkConsistentCols(board);
    bool temp = checkConsistentGrids(board);
    //cout << "\tconsistentGrids: " << boolalpha << temp << '\n';
    //cout << '\n';
    //cout << "Running and Returning isConsistent()";
    //return false;
    return checkConsistentRows(board) && checkConsistentCols(board) && temp;
}

/*
Does the actual backtracking algorithm for the Sudoku board
*/
bool backtrack(vector<vector<string>>& board, vector<vector< vector<string> >>& domain)
{
    if (isCompleteBoard(board))
    {
        //cout << "Complete Board Assignment!!";
        return true;
    }
    //Indices to traverse the board
    //cout << "Selecting Var\n";
    pair<size_t, size_t> var = selectUnassignedVar(board, domain);
    size_t i = var.first;
    size_t j = var.second;
    //cout << "Backtracking on i, j: " << i << ", " << j << " Domain size: " << domain[i][j].size() << "\n";
    //cout << "Domain values:\n";
    /*
    for (const auto& x : domain[i][j])
    {
        cout << x << " ";
    }
    cout << '\n';
    */
    //Values in the domain are sorted in increasing order, due to heap properties
    for (auto& value : domain[i][j])
    {
        vector<vector<string>> newBoard = board;
        newBoard[i][j] = value;
        //cout << "\tChecking isConsistent( " << value << " ): " << boolalpha << isConsistent(newBoard) << '\n';
        if (isConsistent(newBoard))
        {//If value is consistent with assignment
            //Update the board with new value
            board[i][j] = value;
            //Update the domain with new value
            auto save = domain[i][j];
            vector<string> tempVal;
            tempVal.emplace_back(value);
            domain[i][j] = tempVal;
            //Recurse
            bool result = backtrack(board, domain);
            if (result)
            {//If the recursive call is successful, we also return true
                return true;
            }
            //Else, this branch is dead, do not use this value
            board[i][j] = "0";
            domain[i][j] = save;
        }
    }
    return false;
}

int main()
{
    //Set up the 9x9 board
    vector<vector<string>> board(9);
    for (auto& i : board)
    {
        for (int a = 0; a < 9; ++a)
        {
            i.push_back("0");
        }
    }
    //printBoard(board);

    //cout << "Opening input\n";
    /*
    Read the board state from the input file
    */
    ifstream ifs;
    ifs.open("Input1.txt");
    //ifs.open("int.txt");
    if (!ifs)
    {
        cerr << "Could not open input file\n";
        exit(1);
    }
    string line{};
    size_t i = 0;
    size_t j = 0;
    while (ifs >> line)
    {
        if (i == 8 && j == 9)
        {
            cerr << "Error: board overflow\n";
            exit(2);
        }
        board[i][j] = line;
        //cout << i << " " << j << "  ";
        ++j;
        if (j == 9)
        {
            j = 0;
            ++i;
            //cout << "endline \n";
        }
    }
    cout << "Finished Board Setup\n";
    //printBoard(board);
    ifs.close();
    ifs.clear();

    //cout << "\nConstructing Domain\n";
    //Construct 9x9 grid of domain of each variable
    //vector<vector<priority_queue<greater<string>, vector<string>>>> domain(9);
    vector<vector< vector<string> >> domain(9);
    for (auto& x : domain)
    {
        for (int a = 0; a < 9; ++a)
        {
            x.push_back({});
        }
    }
    //constructDomVar(board[0][0], domain[0][0]);

    //cout << "Domain Before Construct\n";
    //printDom(domain);

    cout << "Domain After Construct\n";
    constructDom(board, domain);
    //printDom(domain);

    /*
    cout << "Testing MRV: \n";
    size_t minTemp;
    unordered_map<size_t, vector< pair<size_t, size_t> > > remTemp;
    bool mrvTest = mrv(domain, minTemp, remTemp);
    //cout << mrvTest.first << ", " << mrvTest.second << '\n';
    cout << boolalpha << mrvTest << '\n';
    */

    cout << "Testing Degree\n";
    //cout << "CountColConstraint 0,2: " << countColConstraint(board, 0, 2);

    //Use backtracking to solve Sudoku puzzle
    cout << "\nBacktracking\n";
    cout << "Result: " << std::boolalpha << backtrack(board, domain) << '\n';
    //cout << "TEST!! " << checkConsistentGrids(board) << '\n';
    printBoard(board);
    //printDom(domain);

    //Output results to an output file
    ofstream ofs("Output1.txt");
    if (!ofs)
    {
        cerr << "Could not open output file\n";
        exit(3);
    }
    line = "";
    for (const auto& i : board)
    {
        for (const string& j : i)
        {
            line += j;
            line += " ";
        }
        //If more than 1 tile, remove the ending space of the line
        line = line.substr(0, 17);
        line += "\n";
        //Add the line to output file and then clear line
        ofs << line;
        line = "";
    }
    ofs.close();

}
