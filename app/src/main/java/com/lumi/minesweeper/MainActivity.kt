package com.lumi.minesweeper

import android.os.Bundle
import android.view.View
import android.widget.Button
import android.widget.GridLayout
import android.widget.Toast
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.lifecycleScope
import com.lumi.minesweeper.databinding.ActivityMainBinding
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext

class MainActivity : AppCompatActivity() {
    private lateinit var binding: ActivityMainBinding

    companion object {
        init {
            System.loadLibrary("minesweeper") // Ensure this matches your C++ library name
        }

        const val STEPPED_MINE = 2
        const val VICTORY = 3
    }

    // Declare native methods
    external fun initGameBoard(width: Int, height: Int, mineCount: Int): Long
    external fun initializeBoard(gameBoardPtr: Long, firstClickX: Int, firstClickY: Int)
    external fun revealCell(gameBoardPtr: Long, x: Int, y: Int): Int
    external fun toggleFlag(gameBoardPtr: Long, x: Int, y: Int)
    external fun getCell(gameBoardPtr: Long, x: Int, y: Int): CellData
    external fun getGameState(gameBoardPtr: Long): Int
    external fun cleanup(gameBoardPtr: Long)

    private lateinit var gameBoardLayout: GridLayout
    private var gameBoardPtr = 0L
    private val gridWidth = 10
    private val gridHeight = 10
    private val mineCount = 20
    private var isFirstClickFlag = true

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Initialize the game board
        gameBoardPtr = initGameBoard(gridWidth, gridHeight, mineCount)
        createBoardUI()
    }

    override fun onDestroy() {
        super.onDestroy()
        // Clean up native resources
        cleanup(gameBoardPtr)
    }

    private fun createBoardUI() {
        gameBoardLayout = binding.gameBoard
        gameBoardLayout.columnCount = gridWidth
        gameBoardLayout.rowCount = gridHeight
        gameBoardLayout.removeAllViews()

        for (y in 0 until gridHeight) {
            for (x in 0 until gridWidth) {
                val button = Button(this)
                val params = GridLayout.LayoutParams().apply {
                    width = 0
                    height = 0
                    columnSpec = GridLayout.spec(x, 1f)
                    rowSpec = GridLayout.spec(y, 1f)
                    setMargins(2, 2, 2, 2)
                }
                button.layoutParams = params
                button.text = ""
                button.setBackgroundColor(getColor(android.R.color.holo_blue_light))

                // Set click listeners
                button.setOnClickListener {
                    onCellClicked(x, y, button)
                }

                button.setOnLongClickListener {
                    onCellLongClicked(x, y, button)
                    true
                }

                gameBoardLayout.addView(button)
            }
        }
    }

    private fun onCellClicked(x: Int, y: Int, button: Button) {
        if (isFirstClickFlag) {
            isFirstClickFlag = false
            lifecycleScope.launch {
                withContext(Dispatchers.Default) {
                    initializeBoard(gameBoardPtr, x, y)
                }
                button.isEnabled = false
                updateCellUI(x, y, button)
            }
        } else {
            lifecycleScope.launch {
                val gameState = withContext(Dispatchers.Default) {
                    revealCell(gameBoardPtr, x, y)
                }
                updateCellUI(x, y, button)

                when (gameState) {
                    STEPPED_MINE -> {
                        Toast.makeText(this@MainActivity, "Game Over!", Toast.LENGTH_SHORT).show()
                        revealAllMines()
                        disableAllButtons()
                    }

                    VICTORY -> {
                        Toast.makeText(this@MainActivity, "You Win!", Toast.LENGTH_SHORT).show()
                        revealAllMines()
                        disableAllButtons()
                    }
                }
            }
        }
    }

    private fun onCellLongClicked(x: Int, y: Int, button: Button) {
        lifecycleScope.launch {
            withContext(Dispatchers.Default) {
                toggleFlag(gameBoardPtr, x, y)
            }
            updateCellUI(x, y, button)

            val gameState = withContext(Dispatchers.Default) {
                getGameState(gameBoardPtr)
            }

            if (gameState == VICTORY) {
                Toast.makeText(this@MainActivity, "You Win!", Toast.LENGTH_SHORT).show()
                revealAllMines()
                disableAllButtons()
            }
        }
    }

    private fun updateCellUI(x: Int, y: Int, button: Button) {
        val cellData = getCell(gameBoardPtr, x, y)
        if (cellData.isRevealed) {
            button.isEnabled = false
            if (cellData.isMine) {
                button.text = "💣"
                button.setBackgroundColor(getColor(android.R.color.holo_red_dark))
            } else if (cellData.adjacentMines > 0) {
                button.text = cellData.adjacentMines.toString()
                button.setBackgroundColor(getColor(android.R.color.darker_gray))
            } else {
                button.text = ""
                button.setBackgroundColor(getColor(android.R.color.darker_gray))
            }
        } else if (cellData.isFlagged) {
            button.text = "🚩"
            button.setBackgroundColor(getColor(android.R.color.holo_orange_light))
        } else {
            button.text = ""
            button.setBackgroundColor(getColor(android.R.color.holo_blue_light))
        }
    }

    private fun revealAllMines() {
        for (y in 0 until gridHeight) {
            for (x in 0 until gridWidth) {
                val cellData = getCell(gameBoardPtr, x, y)
                if (cellData.isMine) {
                    val button = gameBoardLayout.getChildAt(y * gridWidth + x) as Button
                    button.text = "💣"
                    button.isEnabled = false
                    button.setBackgroundColor(getColor(android.R.color.holo_red_dark))
                }
            }
        }
    }

    private fun disableAllButtons() {
        for (i in 0 until gridWidth * gridHeight) {
            val button = gameBoardLayout.getChildAt(i) as Button
            button.isEnabled = false
        }
    }
}
