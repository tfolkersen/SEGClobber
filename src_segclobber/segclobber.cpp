#include <iostream>
#include "utils.h"
//#include "state.h"
#include "solver.h"
#include "options.h"
#include "database.h"
#include <cstring>
#include <string>
#include <cstdlib>
#include <cstdint>
#include <chrono>
#include <cassert>

#include "cgt_test_lib.h"

//#include "game.h"

using namespace std;

/*
bool test() {
    return false;

    Database db;
    db.load();

    uint8_t board[] = {1,1,2};
    size_t len = sizeof(board);
    
    uint8_t *entry = db.get(board, len);
    assert(entry);

    cout << "Outcome is: ";
    cout << (int) *db_get_outcome(entry) << endl;


    cout << "Entry is:" << endl;
    cout << (uint64_t) *db_get_outcome(entry) << endl;
    cout << (uint64_t) db_get_dominance(entry)[0] << endl;
    cout << (uint64_t) db_get_dominance(entry)[1] << endl;
    cout << (uint64_t) db_get_bounds(entry)[0] << endl;
    cout << (uint64_t) db_get_bounds(entry)[1] << endl;
    cout << (uint64_t) *db_get_metric(entry) << endl;
    cout << (uint64_t) *db_get_link(entry) << endl;
    cout << (uint64_t) *db_get_shape(entry) << endl;
    cout << (uint64_t) *db_get_number(entry) << endl;
        

    cout << endl;

    uint8_t board1[] = {1,1,1,1};
    uint8_t board2[] = {2,1,1,1};
    uint8_t *ptr1 = db.get(board1, 4);
    uint8_t *ptr2 = db.get(board2, 4);

    assert(ptr1);
    assert(ptr2);
    cout << "Diff: " << (uint64_t) (ptr2 - ptr1) << endl;



    return true;
}
*/

string solverDBName;

void printUsage(const char *execName) {
    cout << "Usage:" << endl;
    cout << execName << " <board> <toPlay>" << endl;
    cout << "or:" << endl;
    cout << execName << " [--persist --altmove]" << endl;
    cout << endl;
    cout << "Legal board characters: 'B', 'W', '.'" << endl;
    cout << "Legal players: 'B', 'W'" << endl; 
}

inline uint8_t segclobber_color_from_ctl_color(ctl_color_t ctl_color) {
    if (ctl_color == CTL_COLOR_BLACK)
        return BLACK;
    if (ctl_color == CTL_COLOR_WHITE)
        return WHITE;
    if (ctl_color == CTL_COLOR_EMPTY)
        return EMPTY;
    assert(false);
}

std::vector<uint8_t> getCTLBoard() {
    std::vector<uint8_t> board;

    auto push_tile = [&](uint8_t tile) -> void {
        const bool back_is_empty = board.empty() || board.back() == EMPTY;

        if (back_is_empty && tile == EMPTY)
            return;

        board.push_back(tile);
    };

    const ctl_size_t n_subgames = ctl_test_get_subgame_count();

    for (ctl_size_t i = 0; i < n_subgames; i++) {
        const ctl_size_t board_len = ctl_test_get_subgame_board_len(i);
        const ctl_color_t* subgame_board = ctl_test_get_subgame_color_board(i);

        if (i > 0)
            push_tile(EMPTY);

        for (ctl_size_t j = 0; j < board_len; j++)
        {
            const uint8_t tile = segclobber_color_from_ctl_color(subgame_board[j]);
            push_tile(tile);
        }
    }

    if (board.empty())
        board.push_back(EMPTY);

    return board;
}

void filesMain(const std::string& inputDir, const std::string& outputCSVPath) {
    Database db;
    db.loadFrom(solverDBName.c_str());

    Solver solver(70, &db);

    ctl_init(inputDir.c_str(), outputCSVPath.c_str());

    uint64_t test_number = 0;
    for (; ctl_has_test(); ctl_next_test()) {
        cout << "Starting test " << test_number << endl;
        test_number++;

        const ctl_size_t nSubgames = ctl_test_get_subgame_count();

        assert(ctl_test_get_kind() == CTL_TEST_KIND_BW_SOLVE);
        for (ctl_size_t i = 0; i < nSubgames; i++) {
            const char* subgameType = ctl_test_get_subgame_type(i);
            assert(strcmp(subgameType, "clobber_1xn") == 0);
        }

        const int toPlay = 
            segclobber_color_from_ctl_color(ctl_test_get_to_play());
        assert(toPlay == BLACK || toPlay == WHITE);

        const std::vector<uint8_t> boardConst = getCTLBoard();

        const size_t boardLen = boardConst.size();
        uint8_t board[boardLen];
        memcpy(board, boardConst.data(), boardLen);

        for (size_t i = 0; i < boardLen; i++)
            assert(board[i] == boardConst[i]);

        node_count = 0;
        best_from = -1;
        best_to = -1;

        ctl_begin_test();
        const int result = solver.solveID(board, boardLen, toPlay);
        ctl_stop_test();

        ctl_report_node_count(node_count);

        const ctl_outcome_t ctl_outcome =
            (result == toPlay) ? CTL_OUTCOME_WIN : CTL_OUTCOME_LOSS;
        ctl_finalize_test(ctl_outcome);
    }

    ctl_finalize();
}

void persistMain() {
    Database db;
    db.loadFrom(solverDBName.c_str());

    string boardStr;
    string toPlayStr;

    //int rootPlayer = charToPlayerNumber(*argv[2]);

    Solver solver(70, &db); // boardLen is only used to pick TT hash length

    while (cin) {
        cin >> boardStr;

        if (cin.fail()) {
            if (!cin.eof()) {
                cerr << "Some input error happened reading board" << endl;
                exit(-1);
            }
            return;
        }

        cin >> toPlayStr;

        if (cin.fail()) {
            cerr << "Some input error happened reading player" << endl;
            exit(-1);
        }

        // Validate input
        for (int i = 0; i < boardStr.size(); i++) {
            const char &c = boardStr[i];
            if (c != '.' && c != 'B' && c != 'W') {
                cerr << "Invalid board character. Use 'B', 'W', and '.'" << endl;
                exit(-1);
            }
        }

        if (toPlayStr.size() != 1 || (toPlayStr[0] != 'B' && toPlayStr[0] != 'W')) {
            cerr << "Bad player character. Use 'B' or 'W'" << endl;
            exit(-1);
        }

        size_t boardLen = strlen(boardStr.c_str());
        uint8_t board[boardLen];

        memcpy(board, boardStr.c_str(), boardLen);

        // sanity checks
        if (boardLen != boardStr.size()) { // string size() excludes null
            cerr << "Wrong board size" << endl;
            exit(-1);
        }

        for (size_t i = 0; i < boardLen; i++) {
            if (board[i] != boardStr[i]) {
                cerr << "Bad board copy" << endl;
                exit(-1);
            }
        }

        // Convert board format
        for (size_t i = 0; i < boardLen; i++) {
            board[i] = charToPlayerNumber(board[i]);
        }

        int rootPlayer = charToPlayerNumber(toPlayStr[0]);

        // Reset these
        node_count = 0;
        best_from = -1;
        best_to = -1;

        int result = solver.solveID(board, boardLen, rootPlayer);

        if (best_from == -1) {
            cout << playerNumberToChar(result) << " None" << " " << node_count;
        } else {
            cout << playerNumberToChar(result) << " " << best_from << "-" << best_to << " " << node_count;
        }

        cout << endl;
    }
}

enum main_mode_enum
{
    MAIN_MODE_CLI = 0,
    MAIN_MODE_PERSIST,
    MAIN_MODE_FROM_FILES,
};

int main(int argc, char **argv) {
    //if (test()) {
    //    return 0;
    //}

    if (argc < 2) {
        printUsage(argv[0]);
        return -1;
    }

    main_mode_enum main_mode = MAIN_MODE_CLI;
    bool altDB = false;

    int additionalArgs = 0;

    std::string testInputDir;
    std::string outputCSVPath;

    // --persist, --altmove, --no-id, --altdb, --no-links
    int _argIdx;
    for (_argIdx = 1; _argIdx < argc; _argIdx++) {
        const char *arg = argv[_argIdx];
        additionalArgs++;

        if (strcmp(arg, "--persist") == 0) {
            main_mode = MAIN_MODE_PERSIST;
            continue;
        }

        if (strcmp(arg, "--altmove") == 0) {
            Solver::useBWMoveOrder = true;
            continue;
        }

        if (strcmp(arg, "--no-id") == 0) {
            Solver::useID = false;
            continue;
        }

        if (strcmp(arg, "--altdb") == 0) {
            altDB = true;
            continue;
        }

        if (strcmp(arg, "--no-links") == 0) {
            Solver::useLinks = false;
            continue;
        }

        if (strcmp(arg, "--no-delete-subgames") == 0) {
            Solver::deleteGames = false;
            continue;
        }

        if (strcmp(arg, "--no-delete-dominated") == 0) {
            Solver::deleteDominated = false;
            continue;
        }

        if (strcmp(arg, "--run-tests") == 0) {
            main_mode = MAIN_MODE_FROM_FILES;

            if (!(_argIdx + 2 < argc))
            {
                std::cerr << "Error: got --run-tests but invalid input/output "
                    "paths. Usage: --run-tests <input dir> <output CSV path>"
                    << std::endl;

                return -1;
            }

            testInputDir = argv[_argIdx + 1];
            outputCSVPath = argv[_argIdx + 2];
            _argIdx += 2;

            continue;
        }
        
        additionalArgs--;
        break;
    }

    if (altDB)
        solverDBName = "database3_alt.bin";
    else
        solverDBName = "database3.bin";


    if (main_mode == MAIN_MODE_PERSIST) {
        if (_argIdx != argc) {
            printUsage(argv[0]);
            return -1;
        }

        persistMain();
        return 0;
    }

    if (main_mode == MAIN_MODE_FROM_FILES)
    {
        if (_argIdx != argc) {
            printUsage(argv[0]);
            return -1;
        }

        filesMain(testInputDir, outputCSVPath);
        return 0;
    }

    if (argc < 3 || additionalArgs + 3 != argc) {
        printUsage(argv[0]);
        return -1;
    }

    int boardArgIdx = 1 + additionalArgs;
    int playerArgIdx = 2 + additionalArgs;

    assert(boardArgIdx < argc && playerArgIdx < argc);

    // Board and player args
    const char *boardArg = argv[boardArgIdx];
    const size_t boardArgLen = strlen(boardArg);

    const char *playerArg = argv[playerArgIdx];
    const size_t playerArgLen = strlen(playerArg);

    // Validate input
    {
        bool isValid = true;

        for (size_t i = 0; i < boardArgLen; i++) {
            const char &c = boardArg[i];

            if (c != 'B' && c != 'W' && c != '.') {
                isValid = false;
                break;
            }
        }

        // Player
        if (playerArgLen != 1)
            isValid = false;

        const char &playerChar = playerArg[0];

        if (playerChar != 'B' && playerChar != 'W')
            isValid = false;

        if (!isValid) {
            printUsage(argv[0]);
            return -1;
        }
    }


    Database db;
    db.loadFrom(solverDBName.c_str());
    //db.init();

    const size_t boardLen = boardArgLen;
    uint8_t board[boardLen];

    for (size_t i = 0; i < boardLen; i++)
        board[i] = charToPlayerNumber(boardArg[i]);

    int rootPlayer = charToPlayerNumber(*playerArg);

    Solver solver(boardLen, &db);

    auto startTime = std::chrono::steady_clock::now();
    int result = solver.solveID(board, boardLen, rootPlayer);

    //Print output
    auto endTime = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - startTime).count();

    if (best_from == -1) {
        cout << playerNumberToChar(result) << " None" << " " << elapsed << " " << node_count;
    } else {
        cout << playerNumberToChar(result) << " " << best_from << "-" << best_to << " " << elapsed << " " << node_count;
    }

    cout << endl;

    return 0;
}
