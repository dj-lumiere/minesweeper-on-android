//
// Created by SSAFY on 2024-10-02.
//

#include "jni.h"
#include <string>
#include "game_objects.h"

extern "C" {

// Pointer to the GameBoard instance
GameBoard* gameBoard = nullptr;

// Initialize the GameBoard
JNIEXPORT jlong JNICALL
Java_com_lumi_minesweeper_MainActivity_initGameBoard(JNIEnv* env, jobject /* this */, jint width, jint height, jint mineCount) {
    gameBoard = new GameBoard(width, height, mineCount);
    return reinterpret_cast<jlong>(gameBoard);
}

// Initialize the board after first click
JNIEXPORT void JNICALL
Java_com_lumi_minesweeper_MainActivity_initializeBoard(JNIEnv* env, jobject /* this */, jlong gameBoardPtr, jint firstClickX, jint firstClickY) {
    auto* board = reinterpret_cast<GameBoard*>(gameBoardPtr);
    if (board != nullptr) {
        board->initializeBoard(firstClickX, firstClickY);
    }
}

// Reveal a cell
JNIEXPORT void JNICALL
Java_com_lumi_minesweeper_MainActivity_revealCell(JNIEnv* env, jobject /* this */, jlong gameBoardPtr, jint x, jint y) {
    auto* board = reinterpret_cast<GameBoard*>(gameBoardPtr);
    if (board != nullptr) {
        board->revealCell(x, y);
        board->updateGameStatus();
    }
}

// Toggle a flag on a cell
JNIEXPORT void JNICALL
Java_com_lumi_minesweeper_MainActivity_toggleFlag(JNIEnv* env, jobject /* this */, jlong gameBoardPtr, jint x, jint y) {
    auto* board = reinterpret_cast<GameBoard*>(gameBoardPtr);
    if (board != nullptr) {
        board->toggleFlag(x, y);
    }
}

// Get cell data
JNIEXPORT jobject JNICALL
Java_com_lumi_minesweeper_MainActivity_getCell(JNIEnv* env, jobject /* this */, jlong gameBoardPtr, jint x, jint y) {
    if (gameBoardPtr == 0) return nullptr;

    auto* board = reinterpret_cast<GameBoard*>(gameBoardPtr);
    const Cell& cell = board->getCell(x, y);

    // Find the CellData class
    jclass cellDataClass = env->FindClass("com/lumi/minesweeper/CellData");
    if (cellDataClass == nullptr) return nullptr;

    // Get the constructor
    jmethodID constructor = env->GetMethodID(cellDataClass, "<init>", "()V");
    if (constructor == nullptr) return nullptr;

    // Create a new CellData object
    jobject cellDataObj = env->NewObject(cellDataClass, constructor);
    if (cellDataObj == nullptr) return nullptr;

    // Set fields
    jfieldID isMineField = env->GetFieldID(cellDataClass, "isMine", "Z");
    jfieldID isRevealedField = env->GetFieldID(cellDataClass, "isRevealed", "Z");
    jfieldID isFlaggedField = env->GetFieldID(cellDataClass, "isFlagged", "Z");
    jfieldID adjacentMinesField = env->GetFieldID(cellDataClass, "adjacentMines", "I");

    env->SetBooleanField(cellDataObj, isMineField, cell.isMine);
    env->SetBooleanField(cellDataObj, isRevealedField, cell.isRevealed);
    env->SetBooleanField(cellDataObj, isFlaggedField, cell.isFlagged);
    env->SetIntField(cellDataObj, adjacentMinesField, static_cast<int>(cell.adjacentMines));

    return cellDataObj;
}

// Get game state
JNIEXPORT jobject JNICALL
Java_com_lumi_minesweeper_MainActivity_getGameState(JNIEnv* env, jobject /* this */, jlong gameBoardPtr) {
    if (gameBoardPtr != 0) {
        auto* board = reinterpret_cast<GameBoard*>(gameBoardPtr);

        // Get the GameStatus class reference
        jclass gameStatusClass = env->FindClass("com/lumi/minesweeper/GameStatus");
        if (gameStatusClass == nullptr) {
            return nullptr; // Error, class not found
        }

        // Convert C++ enum to Java string name of the enum constant
        const char* enumName = nullptr;
        switch (board->state) {
            case GameStatus::ERROR:
                enumName = "ERROR";
                break;
            case GameStatus::STARTED:
                enumName = "STARTED";
                break;
            case GameStatus::ONGOING:
                enumName = "ONGOING";
                break;
            case GameStatus::STEPPED_MINE:
                enumName = "STEPPED_MINE";
                break;
            case GameStatus::VICTORY:
                enumName = "VICTORY";
                break;
            default:
                return nullptr; // Unknown state, handle as error
        }

        jstring jEnumName = env->NewStringUTF(enumName);
        if (jEnumName == nullptr) {
            return nullptr; // Error, could not create string
        }

        // Use the valueOf method to get the enum constant
        jmethodID valueOfMethod = env->GetStaticMethodID(gameStatusClass, "valueOf", "(Ljava/lang/String;)Lcom/lumi/minesweeper/GameStatus;");
        if (valueOfMethod == nullptr) {
            return nullptr; // Error, method not found
        }

        return env->CallStaticObjectMethod(gameStatusClass, valueOfMethod, jEnumName);
    }

    return nullptr; // Error state
}


// Clean up the GameBoard instance
JNIEXPORT void JNICALL
Java_com_lumi_minesweeper_MainActivity_cleanup(JNIEnv* env, jobject /* this */, jlong gameBoardPtr) {
    auto* board = reinterpret_cast<GameBoard*>(gameBoardPtr);
    if (board != nullptr) {
        delete board;
        gameBoard = nullptr;
    }
}

} // extern "C"