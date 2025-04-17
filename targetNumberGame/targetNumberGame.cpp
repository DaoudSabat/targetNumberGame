/*
 * Name:          Daoud
 * Surname:       Sabat
 * Student ID:    U204N3640
 * Course:        COMP-211
 * Date:          2025-02-09
 * Program Title: Target 9 - Stack-based Undo/Redo Implementation
 * Program Goal:  To develop a 3x3 game using a linked stack to manage undo/redo functionality.
 */

// Include libraries
#include <iostream>
#include <iomanip>

using namespace std;

const int SIZE = 3;

// Error codes
enum ErrorCode { success, overflow, underflow };

// Move structure
struct Move {
    int row, col;
};

// Node for linked stack
struct Node {
    int grid[SIZE][SIZE];
    Node* next;
    Node(int g[SIZE][SIZE], Node* n = nullptr) {
        for (int i = 0; i < SIZE; ++i)
            for (int j = 0; j < SIZE; ++j)
                grid[i][j] = g[i][j];
        next = n;
    }
};

// Stack class
class Stack {

public:

    Stack();
    bool isEmpty();
    ErrorCode push(int grid[SIZE][SIZE]);
    ErrorCode pop(int grid[SIZE][SIZE]);
    void clear();
    ~Stack();

private:
    Node* topNode;
};

//pointer to the top of the stack
Stack::Stack() {

    topNode = nullptr;
}

//destructor to clear the stack
Stack::~Stack() {

    clear();
}

//check if the stack is empty
bool Stack::isEmpty() {

    return topNode == nullptr;
}

//clear the stack
void Stack::clear() {
    while (!isEmpty()) {
        Node* temp = topNode;
        topNode = topNode->next;
        delete temp;
    }
}

//push the grid to the stack
ErrorCode Stack::push(int grid[SIZE][SIZE]) {
    Node* newNode = new (nothrow) Node(grid, topNode);
    if (!newNode) return overflow;
    topNode = newNode;
    return success;
}

//pop the grid from the stack
ErrorCode Stack::pop(int grid[SIZE][SIZE]) {
    if (isEmpty()) return underflow;
    Node* temp = topNode;

    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            grid[i][j] = temp->grid[i][j];

    topNode = topNode->next;
    delete temp;
    return success;
}



// Game class
class MoveHandler {

public:

    void newGame(int difficulty);
    void displayGrid() const;
    void applyMove(int row, int col);
    void undo();
    void redo();
    void showHint();
    bool isSolved() const;
    

private:
    int grid[SIZE][SIZE];
    Stack undoStack, redoStack;
};

//constructor to initialize the grid
void MoveHandler::newGame(int difficulty) {
    // Initialize grid with all 9s
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            grid[i][j] = 9;


    // Clear stacks for a fresh start
    undoStack.clear();
    redoStack.clear();


    // Reverse game setup based on difficulty
    for (int i = 0; i < difficulty; i++) {
        int row = rand() % SIZE;  // Select a random row
        int col = rand() % SIZE;  // Select a random column

        // Save state before modifying the grid
        undoStack.push(grid);



        // Apply decrementing move to simulate difficulty level
        for (int j = 0; j < SIZE; j++) {
            if (j != col) grid[row][j] = (grid[row][j] - 1 + 10) % 10;
            if (j != row) grid[j][col] = (grid[j][col] - 1 + 10) % 10;
        }
        grid[row][col] = (grid[row][col] - 1 + 10) % 10;
    }
}

//display the grid
void MoveHandler::displayGrid() const {

    cout << "\nCurrent Grid:\n";
    cout << "\n    0  1  2\n\n";
    for (int i = 0; i < SIZE; i++) {
        cout << i  << " ";
        for (int j = 0; j < SIZE; j++)

            cout << setw(3) << grid[i][j];
        cout << endl;
    }
}

//apply the move
void MoveHandler::applyMove(int row, int col) {
    undoStack.push(grid);
    redoStack.clear();
    for (int i = 0; i < SIZE; i++) {
        if (i != col) grid[row][i] = (grid[row][i] + 1) % 10;
        if (i != row) grid[i][col] = (grid[i][col] + 1) % 10;
    }
    grid[row][col] = (grid[row][col] + 1) % 10;
}

//undo the move if possible
void MoveHandler::undo() {
    int tempGrid[SIZE][SIZE];
    if (undoStack.pop(tempGrid) == success) {
        redoStack.push(grid);
        for (int i = 0; i < SIZE; i++)
            for (int j = 0; j < SIZE; j++)
                grid[i][j] = tempGrid[i][j];
    }
    else {
        cout << "No moves to undo.\n";
    }
}

//redo the move if possible
void MoveHandler::redo() {
    int tempGrid[SIZE][SIZE];
    if (redoStack.pop(tempGrid) == success) {
        undoStack.push(grid);
        for (int i = 0; i < SIZE; i++)
            for (int j = 0; j < SIZE; j++)
                grid[i][j] = tempGrid[i][j];
    }
    else {
        cout << "No moves to redo.\n";
    }
}

//show the hint
void MoveHandler::showHint() {
    int bestRow = -1, bestCol = -1, minRemaining = SIZE * SIZE;
    int tempGrid[SIZE][SIZE];

    undoStack.push(grid); // Save the current state before testing moves

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            undoStack.push(grid);  // Save state before applying move
            applyMove(i, j);

            int remaining = 0;
            for (int x = 0; x < SIZE; x++)
                for (int y = 0; y < SIZE; y++)
                    if (grid[x][y] != 9) remaining++;

            if (remaining < minRemaining) {
                minRemaining = remaining;
                bestRow = i;
                bestCol = j;
            }

            undo(); // Restore the previous state
        }
    }

    undo(); // Restore the original state before showing the hint

    if (bestRow != -1)
        cout << "Hint: Try selecting cell (" << bestRow << ", " << bestCol << ") for optimal progress.\n";
    else
        cout << "No hints available. The grid might already be solved!\n";
}


//check if the game is solved
bool MoveHandler::isSolved() const {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (grid[i][j] != 9) return false;
    return true;
}

int main() {

    
    int difficulty;
    cout << "Enter difficulty level (1-9): ";
    cin >> difficulty;

	// Validate difficulty level
    MoveHandler game;  // Start with an empty grid
    game.newGame(difficulty);
    char choice;

	// Game loop
    do {
        game.displayGrid();
        cout << "\nOptions: \n1) Play move \n2) Undo \n3) Redo \n4) Show hint \n5) Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
		// Apply move
        if (choice == '1') {
            int r, c;
			// Get row and column from user
            cout << "Enter row (0-2): ";
            cin >> r;
            cout << "Enter column (0-2): ";
            cin >> c;
			// Validate row and column
            if (r >= 0 && r < SIZE && c >= 0 && c < SIZE) {
                game.applyMove(r, c);
            }
            else {
                cout << "Invalid move.\n";
            }
        }
		// Undo move
        else if (choice == '2') {
            game.undo();
        }
		// Redo move
        else if (choice == '3') {
            game.redo();
		}// Show hint
        else if (choice == '4') {
            game.showHint();
        }// Exit game
    } while (choice != '5' && !game.isSolved());

    if (game.isSolved()) cout << "Congratulations! You solved the game!\n";
    return 0;
}
